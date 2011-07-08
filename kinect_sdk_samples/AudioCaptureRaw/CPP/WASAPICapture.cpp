/***********************************************************************
*                                                                      *
*   WASAPICapture.cpp -- This module provides sample code used to      *
*                        demonstrate capturing raw audio streams from  *
*                        the Kinect 4-microphone array.                *
*                                                                      *
*   Copyright (c) Microsoft Corp. All rights reserved.                 *
*                                                                      *
*  This code is licensed under the terms of the                        *
*  Microsoft Kinect for Windows SDK (Beta) from Microsoft Research     *
*  License Agreement: http://research.microsoft.com/KinectSDK-ToU      *
*                                                                      *
************************************************************************/

#include "StdAfx.h"
#include <assert.h>
#include <avrt.h>
#include "WASAPICapture.h"

// 追加のインクルードディレクトリに $(DXSDK_DIR)\Include を追加する
#include <xaudio2.h>
#include <iostream>

// 追加のライブラリディレクトリに $(DXSDK_DIR)\Lib\x86 を追加する
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "dxerr.lib" )

//
//  A simple WASAPI Capture client.
//

CWASAPICapture::CWASAPICapture(IMMDevice *Endpoint, ERole EndpointRole) : 
    _Endpoint(Endpoint),
    _AudioClient(NULL),
    _CaptureClient(NULL),
    _CaptureThread(NULL),
    _ShutdownEvent(NULL),
    _MixFormat(NULL),
    _CurrentCaptureIndex(0),
    _EndpointRole(EndpointRole),    
    _AudioSessionControl(NULL),
    _DeviceEnumerator(NULL) 
{
    _Endpoint->AddRef();    // Since we're holding a copy of the endpoint, take a reference to it.  It'll be released in Shutdown();
}

//
//  Empty destructor - everything should be released in the Shutdown() call.
//
CWASAPICapture::~CWASAPICapture(void) 
{
}


//
//  Initialize WASAPI in timer driven mode, and retrieve 
//  a capture client for the transport
//
bool CWASAPICapture::InitializeAudioEngine()
{
    HRESULT hr = _AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_NOPERSIST, _EngineLatencyInMS*10000, 0, _MixFormat, NULL);

    if (FAILED(hr))
    {
        printf_s("Unable to initialize audio client: %x.\n", hr);
        return false;
    }


    hr = _AudioClient->GetService(IID_PPV_ARGS(&_CaptureClient));
    if (FAILED(hr))
	{
        printf_s("Unable to get new capture client: %x.\n", hr);
        return false;
    }

    return true;
}

//
//  Retrieve the format we'll use to capture samples.
//
//  We use the Mix format since we're capturing in shared mode.
//
bool CWASAPICapture::LoadFormat()
{
    HRESULT hr = _AudioClient->GetMixFormat(&_MixFormat);
    if (FAILED(hr))
    {
        printf_s("Unable to get mix format on audio client: %x.\n", hr);
        return false;
    }

    _FrameSize = (_MixFormat->wBitsPerSample / 8) * _MixFormat->nChannels;
    return true;
}

//
//  Initialize the capturer.
//
bool CWASAPICapture::Initialize(UINT32 EngineLatency)
{
    //
    //  Create our shutdown event - we want auto reset events that start in the not-signaled state.
    //
    _ShutdownEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
    if (_ShutdownEvent == NULL)
    {
        printf_s("Unable to create shutdown event: %d.\n", GetLastError());
        return false;
    }    

    //
    //  Now activate an IAudioClient object on our preferred endpoint and retrieve the mix format for that endpoint.
    //
    HRESULT hr = _Endpoint->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<void **>(&_AudioClient));
    if (FAILED(hr))
    {
        printf_s("Unable to activate audio client: %x.\n", hr);
        return false;
    }

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_DeviceEnumerator));
    if (FAILED(hr))
    {
        printf_s("Unable to instantiate device enumerator: %x\n", hr);
        return false;
    }

    //
    // Load the MixFormat.  This may differ depending on the shared mode used
    //
    if (!LoadFormat())
    {
        printf_s("Failed to load the mix format \n");
        return false;
    }

    //
    //  Remember our configured latency
    //
    _EngineLatencyInMS = EngineLatency;

    if (!InitializeAudioEngine())
    {
        return false;
    }

    return true;
}

//
//  Shut down the capture code and free all the resources.
//
void CWASAPICapture::Shutdown()
{
    if (_CaptureThread)
    {
        SetEvent(_ShutdownEvent);
        WaitForSingleObject(_CaptureThread, INFINITE);
        CloseHandle(_CaptureThread);
        _CaptureThread = NULL;
    }

    if (_ShutdownEvent)
    {
        CloseHandle(_ShutdownEvent);
        _ShutdownEvent = NULL;
    }

    SafeRelease(&_Endpoint);
    SafeRelease(&_AudioClient);
    SafeRelease(&_CaptureClient);

    if (_MixFormat)
    {
        CoTaskMemFree(_MixFormat);
        _MixFormat = NULL;
    }
}


//
//  Start capturing...
//
bool CWASAPICapture::Start(BYTE *CaptureBuffer, size_t CaptureBufferSize)
{
    HRESULT hr;

    _CaptureBuffer = CaptureBuffer;
    _CaptureBufferSize = CaptureBufferSize;

    //
    //  Now create the thread which is going to drive the capture.
    //
    _CaptureThread = CreateThread(NULL, 0, WASAPICaptureThread, this, 0, NULL);
    if (_CaptureThread == NULL)
    {
        printf_s("Unable to create transport thread: %x.", GetLastError());
        return false;
    }

    //
    //  We're ready to go, start capturing!
    //
    hr = _AudioClient->Start();
    if (FAILED(hr))
    {
        printf_s("Unable to start capture client: %x.\n", hr);
        return false;
    }

    return true;
}

//
//  Stop the capturer.
//
void CWASAPICapture::Stop()
{
    HRESULT hr;

    //
    //  Tell the capture thread to shut down, wait for the thread to complete then clean up all the stuff we 
    //  allocated in Start().
    //
    if (_ShutdownEvent)
    {
        SetEvent(_ShutdownEvent);
    }

    hr = _AudioClient->Stop();
    if (FAILED(hr))
    {
        printf_s("Unable to stop audio client: %x\n", hr);
    }

    if (_CaptureThread)
    {
        WaitForSingleObject(_CaptureThread, INFINITE);

        CloseHandle(_CaptureThread);
        _CaptureThread = NULL;
    }
}


//
//  Capture thread - processes samples from the audio engine
//
DWORD CWASAPICapture::WASAPICaptureThread(LPVOID Context)
{
    CWASAPICapture *capturer = static_cast<CWASAPICapture *>(Context);
    return capturer->DoCaptureThread();
}

DWORD CWASAPICapture::DoCaptureThread()
{
    HANDLE mmcssHandle = NULL;

    IXAudio2* xaudio = 0;
    IXAudio2MasteringVoice* mastering_voice = 0;

    IXAudio2SourceVoice* source_voice = 0;

    try {

        bool stillPlaying = true;
        DWORD mmcssTaskIndex = 0;

        HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        if (FAILED(hr))
        {
            printf_s("Unable to initialize COM in render thread: %x\n", hr);
            return hr;
        }

    
        mmcssHandle = AvSetMmThreadCharacteristics(L"Audio", &mmcssTaskIndex);
        if (mmcssHandle == NULL)
        {
            printf_s("Unable to enable MMCSS on capture thread: %d\n", GetLastError());
        }

        //
        //  XAudioの初期化
        //
        {
            UINT32 flags = 0;
#ifdef _DEBUG
            flags |= XAUDIO2_DEBUG_ENGINE;
#endif
            if( FAILED( hr = XAudio2Create( &xaudio, flags ) ) )
                throw "XAudio2Create";

            //  Create a mastering voice
            if( FAILED( hr = xaudio->CreateMasteringVoice( &mastering_voice ) ) )
                throw "CreateMasteringVoice";

            //  WAVファイルのWAVEFORMATEXを使ってSourceVoiceを作成
            if( FAILED( xaudio->CreateSourceVoice( &source_voice, MixFormat() ) ) )
                throw "CreateSourceVoice";

            //  再生
            source_voice->Start();

        }

        while (stillPlaying)
        {
            HRESULT hr;
            //
            //  In Timer Driven mode, we want to wait for half the desired latency in milliseconds.
            //
            //  That way we'll wake up half way through the processing period to pull the 
            //  next set of samples from the engine.
            //
		    DWORD waitResult = WaitForSingleObject(_ShutdownEvent, _EngineLatencyInMS / 2);
            switch (waitResult)
            {
            case WAIT_OBJECT_0 + 0:     // _ShutdownEvent
                stillPlaying = false;       // We're done, exit the loop.
                break;        
            case WAIT_TIMEOUT:          // Timeout
                //
                //  We need to retrieve the next buffer of samples from the audio capturer.
                //
                BYTE *pData;
                UINT32 framesAvailable;
                DWORD  flags;

                //
                //  Find out how much capture data is available.  We need to make sure we don't run over the length
                //  of our capture buffer.  We'll discard any samples that don't fit in the buffer.
                //
                hr = _CaptureClient->GetBuffer(&pData, &framesAvailable, &flags, NULL, NULL);
                if (SUCCEEDED(hr))
                {
                    UINT32 framesToCopy = min(framesAvailable, static_cast<UINT32>((_CaptureBufferSize - _CurrentCaptureIndex) / _FrameSize));
                    if (framesToCopy != 0)
                    {
                        //
                        //  The flags on capture tell us information about the data.
                        //
                        //  We only really care about the silent flag since we want to put frames of silence into the buffer
                        //  when we receive silence.  We rely on the fact that a logical bit 0 is silence for both float and int formats.
                        //
                        if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
                        {
                            //
                            //  Fill 0s from the capture buffer to the output buffer.
                            //
                            ZeroMemory(&_CaptureBuffer[_CurrentCaptureIndex], framesToCopy*_FrameSize);
                        }
                        else
                        {
                            //
                            //  Copy data from the audio engine buffer to the output buffer.
                            //
                            CopyMemory(&_CaptureBuffer[_CurrentCaptureIndex], pData, framesToCopy*_FrameSize);

                            // SourceVoiceにデータを送信
                            XAUDIO2_BUFFER buffer = { 0 };
                            buffer.AudioBytes = framesToCopy * _FrameSize;  //バッファのバイト数
                            buffer.pAudioData = &pData[ 0 ];                //バッファの先頭アドレス
                            source_voice->SubmitSourceBuffer( &buffer );
                        }
                        //
                        //  Bump the capture buffer pointer.
                        //
                        _CurrentCaptureIndex += framesToCopy*_FrameSize;
                    }
                    hr = _CaptureClient->ReleaseBuffer(framesAvailable);
                    if (FAILED(hr))
                    {
                        printf_s("Unable to release capture buffer: %x!\n", hr);
                    }
                }
                break;
            }
        }
    }
    catch( const char* e )
    {
        std::cout << e << std::endl;
    }

    //  Cleanup XAudio2
    if( mastering_voice != 0 ) {
        // ここで落ちる
        //mastering_voice->DestroyVoice();
        mastering_voice = 0;
    }

    if( xaudio != 0 ) {
        // ここでも落ちる
        //xaudio->Release();
        xaudio = 0;
    }

    AvRevertMmThreadCharacteristics(mmcssHandle);
    
    CoUninitialize();
    return 0;
}
