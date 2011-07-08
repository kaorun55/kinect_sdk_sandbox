/***********************************************************************
*                                                                      *
*   WASAPICapture.h -- Common includes used in AudioCaptureRaw sample. *
*                                                                      *
*   Copyright (c) Microsoft Corp. All rights reserved.                 *
*                                                                      *
*  This code is licensed under the terms of the                        *
*  Microsoft Kinect for Windows SDK (Beta) from Microsoft Research     *
*  License Agreement: http://research.microsoft.com/KinectSDK-ToU      *
*                                                                      *
************************************************************************/
#pragma once
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>

//
//  WASAPI Capture class.
class CWASAPICapture
{
public:
    //  Public interface to CWASAPICapture.
    CWASAPICapture(IMMDevice *Endpoint, ERole EndpointRole);
    bool Initialize(UINT32 EngineLatency);
    void Shutdown();
    bool Start(BYTE *CaptureBuffer, size_t BufferSize);
    void Stop();
    WORD ChannelCount() { return _MixFormat->nChannels; }
    UINT32 SamplesPerSecond() { return _MixFormat->nSamplesPerSec; }
    UINT32 BytesPerSample() { return _MixFormat->wBitsPerSample / 8; }
    size_t FrameSize() { return _FrameSize; }
    WAVEFORMATEX *MixFormat() { return _MixFormat; }
    size_t BytesCaptured() { return _CurrentCaptureIndex; }
	~CWASAPICapture(void);

private:
    //
    //  Core Audio Capture member variables.
    //
    IMMDevice *         _Endpoint;
    IAudioClient *      _AudioClient;
    IAudioCaptureClient *_CaptureClient;

    HANDLE              _CaptureThread;
    HANDLE              _ShutdownEvent;
    WAVEFORMATEX *      _MixFormat;
    size_t              _FrameSize;    

    //
    //  Capture buffer management.
    //
    BYTE *_CaptureBuffer;
    size_t _CaptureBufferSize;
    size_t _CurrentCaptureIndex;

    static DWORD __stdcall WASAPICaptureThread(LPVOID Context);
    DWORD CWASAPICapture::DoCaptureThread();
    
    ERole                   _EndpointRole;    
    IAudioSessionControl *  _AudioSessionControl;
    IMMDeviceEnumerator *   _DeviceEnumerator;
    LONG                    _EngineLatencyInMS;
        
    //
    //  Utility functions.
    //
    bool InitializeAudioEngine();
    bool LoadFormat();
};
