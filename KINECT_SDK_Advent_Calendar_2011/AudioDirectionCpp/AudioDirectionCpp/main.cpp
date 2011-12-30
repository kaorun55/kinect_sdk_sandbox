#include <Windows.h>
#include <conio.h>

#include <iostream>
#include <stdexcept>
#include <vector>


#include <dmo.h>
// IMediaObject


#include <mfapi.h>
// IPropertyStore

#include <wmcodecdsp.h>
// MFPKEY_WMAAECMA_SYSTEM_MODE

#include <MMDeviceApi.h>
// GetMicArrayDeviceIndex‚Ì’†

#include <devicetopology.h>
// GetJackSubtypeForEndpoint‚Ì’†

#include <uuids.h>
// DShowRecord‚Ì’†

#include <atlbase.h>

#include <MSRKinectAudio.h>

#include "WaveAudio.h"

#define CHECKHR(x) hr = x; if (FAILED(hr)) {printf("%d: %08X\n", __LINE__, hr); throw std::exception();}
#define CHECK_RET(hr, message) if (FAILED(hr)) { printf("%s: %08X\n", message, hr); throw std::exception();}


#pragma comment( lib, "Msdmo.lib" )
#pragma comment( lib, "dmoguids.lib" ) // IMediaObject
#pragma comment( lib, "amstrmid.lib" )
//#pragma comment( lib, "avrt.lib" )

// Kinect SDK‚ÌƒTƒ“ƒvƒ‹‚©‚ç
class CStaticMediaBuffer : public IMediaBuffer {
public:
   CStaticMediaBuffer() {}
   CStaticMediaBuffer(BYTE *pData, ULONG ulSize, ULONG ulData) :
      m_pData(pData), m_ulSize(ulSize), m_ulData(ulData), m_cRef(1) {}
   STDMETHODIMP_(ULONG) AddRef() { return 2; }
   STDMETHODIMP_(ULONG) Release() { return 1; }
   STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {
      if (riid == IID_IUnknown) {
         AddRef();
         *ppv = (IUnknown*)this;
         return NOERROR;
      }
      else if (riid == IID_IMediaBuffer) {
         AddRef();
         *ppv = (IMediaBuffer*)this;
         return NOERROR;
      }
      else
         return E_NOINTERFACE;
   }
   STDMETHODIMP SetLength(DWORD ulLength) {m_ulData = ulLength; return NOERROR;}
   STDMETHODIMP GetMaxLength(DWORD *pcbMaxLength) {*pcbMaxLength = m_ulSize; return NOERROR;}
   STDMETHODIMP GetBufferAndLength(BYTE **ppBuffer, DWORD *pcbLength) {
      if (ppBuffer) *ppBuffer = m_pData;
      if (pcbLength) *pcbLength = m_ulData;
      return NOERROR;
   }
   void Init(BYTE *pData, ULONG ulSize, ULONG ulData) {
        m_pData = pData;
        m_ulSize = ulSize;
        m_ulData = ulData;
    }
protected:
   BYTE *m_pData;
   ULONG m_ulSize;
   ULONG m_ulData;
   ULONG m_cRef;
};


///////////////////////////////////////////////////////////////////////////////
// GetJackSubtypeForEndpoint
//
// Gets the subtype of the jack that the specified endpoint device is plugged
// into.  E.g. if the endpoint is for an array mic, then we would expect the
// subtype of the jack to be KSNODETYPE_MICROPHONE_ARRAY
//
///////////////////////////////////////////////////////////////////////////////
HRESULT GetJackSubtypeForEndpoint(IMMDevice* pEndpoint, GUID* pgSubtype)
{
    HRESULT hr = S_OK;

    try {
        if (pEndpoint == NULL)
            return E_POINTER;
   
        CComPtr<IDeviceTopology>    spEndpointTopology;
        CComPtr<IConnector>         spPlug;
        CComPtr<IConnector>         spJack;
        CComPtr<IPart>              spJackAsPart;

        // Get the Device Topology interface
        CHECKHR( pEndpoint->Activate(__uuidof(IDeviceTopology), CLSCTX_INPROC_SERVER, 
                                NULL, (void**)&spEndpointTopology) );

        CHECKHR( spEndpointTopology->GetConnector(0, &spPlug) );
        CHECKHR( spPlug->GetConnectedTo( &spJack ) );
        CHECKHR( spJack.QueryInterface( &spJackAsPart ) );

        hr = spJackAsPart->GetSubType( pgSubtype );
    }
    catch ( std::exception& /*ex*/ ) {
    }

    return hr;
}//GetJackSubtypeForEndpoint()

///////////////////////////////////////////////////////////////////////////
// GetMicArrayDeviceIndex
//
// Obtain device index corresponding to microphone array device.
//
// Parameters: piDevice: [out] Index of microphone array device.
//
// Return: S_OK if successful
//         Failure code otherwise (e.g.: if microphone array device is not found).
//
///////////////////////////////////////////////////////////////////////////////
HRESULT GetMicArrayDeviceIndex(int *piDevice)
{
    HRESULT hr = S_OK;

    try {

        CComPtr<IMMDeviceEnumerator> spEnumerator;
        CHECKHR( spEnumerator.CoCreateInstance( __uuidof(MMDeviceEnumerator),  NULL, CLSCTX_ALL ) );

        CComPtr<IMMDeviceCollection> spEndpoints;
        CHECKHR( spEnumerator->EnumAudioEndpoints( eCapture, DEVICE_STATE_ACTIVE, &spEndpoints ) );

        UINT dwCount = 0;
        CHECKHR(spEndpoints->GetCount(&dwCount));

        // Iterate over all capture devices until finding one that is a microphone array
        *piDevice = -1;
        for ( UINT index = 0; index < dwCount; index++) {
            IMMDevice* spDevice;
            CHECKHR( spEndpoints->Item( index, &spDevice ) );
        
            GUID subType = {0};
            CHECKHR( GetJackSubtypeForEndpoint( spDevice, &subType ) );
            if ( subType == KSNODETYPE_MICROPHONE_ARRAY ) {
                *piDevice = index;
                break;
            }
        }

        hr = (*piDevice >= 0) ? S_OK : E_FAIL;
    }
    catch ( std::exception& /*ex*/ ) {
    }

    return hr;
}

///////////////////////////////////////////////////////////////////////////
// DShowRecord
//
// Uses the DMO in source mode to retrieve clean audio samples and record
// them to a .wav file.
//
///////////////////////////////////////////////////////////////////////////
HRESULT DShowRecord(IMediaObject* pDMO, IPropertyStore* pPS )
{
	printf(("\nRecording using DMO\n"));

	CComPtr<ISoundSourceLocalizer> pSC;
	HRESULT hr;

    WAVEFORMATEX wfxOut = {WAVE_FORMAT_PCM, 1, 16000, 32000, 2, 16, 0};
    WaveAudio wave;
    wave.Initialize( wfxOut.nSamplesPerSec, wfxOut.wBitsPerSample, wfxOut.nChannels );

    DMO_OUTPUT_DATA_BUFFER OutputBufferStruct = {0};
    DMO_MEDIA_TYPE mt = {0};

    ULONG cbProduced = 0;
    DWORD dwStatus;

    CStaticMediaBuffer mediaBuffer;
    OutputBufferStruct.pBuffer = &mediaBuffer;


    // Set DMO output format
    hr = MoInitMediaType(&mt, sizeof(WAVEFORMATEX));
    CHECK_RET(hr, "MoInitMediaType failed");
    
    mt.majortype = MEDIATYPE_Audio;
    mt.subtype = MEDIASUBTYPE_PCM;
    mt.lSampleSize = 0;
    mt.bFixedSizeSamples = TRUE;
    mt.bTemporalCompression = FALSE;
    mt.formattype = FORMAT_WaveFormatEx;	
    memcpy(mt.pbFormat, &wfxOut, sizeof(WAVEFORMATEX));
    
    hr = pDMO->SetOutputType(0, &mt, 0); 
    CHECK_RET(hr, "SetOutputType failed");
    MoFreeMediaType(&mt);

    // Allocate streaming resources. This step is optional. If it is not called here, it
    // will be called when first time ProcessInput() is called. However, if you want to 
    // get the actual frame size being used, it should be called explicitly here.
    hr = pDMO->AllocateStreamingResources();
    CHECK_RET(hr, "AllocateStreamingResources failed");
    
    // Get actually frame size being used in the DMO. (optional, do as you need)
    int iFrameSize;
    PROPVARIANT pvFrameSize;
    PropVariantInit(&pvFrameSize);
    CHECKHR(pPS->GetValue(MFPKEY_WMAAECMA_FEATR_FRAME_SIZE, &pvFrameSize));
    iFrameSize = pvFrameSize.lVal;
    PropVariantClear(&pvFrameSize);

    // allocate output buffer
    DWORD cOutputBufLen = wfxOut.nSamplesPerSec * wfxOut.nBlockAlign;
    std::vector< BYTE > buffer( cOutputBufLen );

    // number of frames to record
	DWORD written = 0;
	int totalBytes = 0;
	
	hr = pDMO->QueryInterface(IID_ISoundSourceLocalizer, (void**)&pSC);
	CHECK_RET (hr, "QueryInterface for IID_ISoundSourceLocalizer failed");

    // main loop to get mic output from the DMO
    puts("\nAEC-MicArray is running ... Press \"s\" to stop\n");

    while ( !_kbhit() )
    {
        Sleep(10); //sleep 10ms

        do{
            mediaBuffer.Init( &buffer[0], buffer.size(), 0 );

            hr = pDMO->ProcessOutput(0, 1, &OutputBufferStruct, &dwStatus);
            CHECK_RET (hr, "ProcessOutput failed. You must be rendering sound through the speakers before you start recording in order to perform echo cancellation.");

            // o—Í
            DWORD written = 0;
            hr = mediaBuffer.GetBufferAndLength(NULL, &written);
            CHECK_RET (hr, "GetBufferAndLength failed");

            wave.Output( &buffer[0], written );

            // Obtain beam angle from ISoundSourceLocalizer afforded by microphone array
			//Use a moving average to smooth this out
        	double dBeamAngle, dAngle, dConf;	
			hr = pSC->GetBeam(&dBeamAngle);
			hr = pSC->GetPosition(&dAngle, &dConf);
			if ( SUCCEEDED( hr ) ) {
				if( dConf > 0.9 ) {
					printf( "Position: %f\t\tBeam Angle = %f\r", dAngle, dBeamAngle );					
				}
			}

        } while (OutputBufferStruct.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE);
    }

    return hr;
}

void main()
{
    try {
        HRESULT hr = S_OK;
        ::CoInitialize( NULL );

        CComPtr<IMediaObject> pDMO;  
        CHECKHR( pDMO.CoCreateInstance(CLSID_CMSRKinectAudio, NULL, CLSCTX_INPROC_SERVER ) );

        CComPtr<IPropertyStore> pPS;
        CHECKHR( pDMO.QueryInterface( &pPS ) );

        // Set AEC-MicArray DMO system mode.
        // This must be set for the DMO to work properly
        PROPVARIANT pvSysMode;
        PropVariantInit(&pvSysMode);
        pvSysMode.vt = VT_I4;
        //   SINGLE_CHANNEL_AEC = 0
        //   OPTIBEAM_ARRAY_ONLY = 2
        //   OPTIBEAM_ARRAY_AND_AEC = 4
        //   SINGLE_CHANNEL_NSAGC = 5
        pvSysMode.lVal = (LONG)(4);
        CHECKHR(pPS->SetValue(MFPKEY_WMAAECMA_SYSTEM_MODE, pvSysMode));
        PropVariantClear(&pvSysMode);

        // Tell DMO which capture device to use (we're using whichever device is a microphone array).
        // Default rendering device (speaker) will be used.
        int  iMicDevIdx = -1; 
        hr = GetMicArrayDeviceIndex(&iMicDevIdx);
        CHECK_RET(hr, "Failed to find microphone array device. Make sure microphone array is properly installed.");
    
        PROPVARIANT pvDeviceId;
        PropVariantInit(&pvDeviceId);
        pvDeviceId.vt = VT_I4;

        //Speaker index is the two high order bytes and the mic index the two low order ones
        int  iSpkDevIdx = 0;  //Asume default speakers
        pvDeviceId.lVal = (unsigned long)(iSpkDevIdx<<16) | (unsigned long)(0x0000ffff & iMicDevIdx);
        CHECKHR(pPS->SetValue(MFPKEY_WMAAECMA_DEVICE_INDEXES, pvDeviceId));
        PropVariantClear(&pvDeviceId);

        DShowRecord( pDMO, pPS );

        std::cout << std::endl;
        std::cout << "success!!" << std::endl;
    }
    catch ( std::exception& ex ) {
        std::cout << ex.what() << std::endl;
    }
}
