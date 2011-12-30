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
// GetMicArrayDeviceIndexの中

#include <devicetopology.h>
// GetJackSubtypeForEndpointの中

#include <uuids.h>
// DShowRecordの中

#include <atlbase.h>

#include <MSRKinectAudio.h>

#include "WaveAudio.h"

#define CHECKHR( x ) { HRESULT hr = x; if (FAILED(hr)) { char buf[256]; sprintf( buf, "%d: %08X\n", __LINE__, hr ); throw std::runtime_error( buf );} }


#pragma comment( lib, "Msdmo.lib" )
#pragma comment( lib, "dmoguids.lib" ) // IMediaObject
#pragma comment( lib, "amstrmid.lib" )
//#pragma comment( lib, "avrt.lib" )

// Kinect SDKのサンプルから
class CStaticMediaBuffer : public IMediaBuffer {
public:
   CStaticMediaBuffer() {}
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

   STDMETHODIMP SetLength( DWORD ulLength ) {
       m_ulData = ulLength;
       return NOERROR;
   }

   STDMETHODIMP GetMaxLength(DWORD *pcbMaxLength) {
       *pcbMaxLength = buffer_.size();
       return NOERROR;
   }

   STDMETHODIMP GetBufferAndLength(BYTE **ppBuffer, DWORD *pcbLength) {
        if ( ppBuffer )
            *ppBuffer = &buffer_[0];

        if ( pcbLength )
            *pcbLength = m_ulData;

        return NOERROR;
   }

   void Clear()
   {
        m_ulData = 0;
   }

   ULONG GetDataLength() const
   {
       return m_ulData;
   }

   void SetBufferLength( ULONG length )
   {
       buffer_.resize( length );
   }

   std::vector< BYTE > Clone() const
   {
       return std::vector< BYTE >( buffer_.begin(), buffer_.begin() + GetDataLength() );
   }

protected:
    std::vector< BYTE > buffer_;
    ULONG m_ulData;
};

class KinectAudioSource
{
public:

    void Initialize()
    {
        CHECKHR( pDMO.CoCreateInstance(CLSID_CMSRKinectAudio, NULL, CLSCTX_INPROC_SERVER ) );
        CHECKHR( pDMO.QueryInterface( &pPS ) );
	    CHECKHR( pDMO->QueryInterface( IID_ISoundSourceLocalizer, (void**)&pSC ) );

        // Tell DMO which capture device to use (we're using whichever device is a microphone array).
        // Default rendering device (speaker) will be used.
        int  iMicDevIdx = -1; 
        CHECKHR( GetMicArrayDeviceIndex(&iMicDevIdx) );
    
        PROPVARIANT pvDeviceId;
        PropVariantInit(&pvDeviceId);
        pvDeviceId.vt = VT_I4;

        //Speaker index is the two high order bytes and the mic index the two low order ones
        int  iSpkDevIdx = 0;  //Asume default speakers
        pvDeviceId.lVal = (unsigned long)(iSpkDevIdx<<16) | (unsigned long)(0x0000ffff & iMicDevIdx);
        CHECKHR(pPS->SetValue(MFPKEY_WMAAECMA_DEVICE_INDEXES, pvDeviceId));
        PropVariantClear(&pvDeviceId);
    }

    void SetSystemMode( LONG mode )
    {
        // Set AEC-MicArray DMO system mode.
        // This must be set for the DMO to work properly
        PROPVARIANT pvSysMode;
        PropVariantInit(&pvSysMode);
        pvSysMode.vt = VT_I4;
        //   SINGLE_CHANNEL_AEC = 0
        //   OPTIBEAM_ARRAY_ONLY = 2
        //   OPTIBEAM_ARRAY_AND_AEC = 4
        //   SINGLE_CHANNEL_NSAGC = 5
        pvSysMode.lVal = mode;
        CHECKHR(pPS->SetValue(MFPKEY_WMAAECMA_SYSTEM_MODE, pvSysMode));
        PropVariantClear(&pvSysMode);
    }

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
            if ( pEndpoint == NULL )
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

    const WAVEFORMATEX& GetWaveFormat()
    {
        static const WAVEFORMATEX wfxOut = {WAVE_FORMAT_PCM, 1, 16000, 32000, 2, 16, 0};
        return wfxOut;
    }

    void Start()
    {

        DMO_MEDIA_TYPE mt = {0};

        ULONG cbProduced = 0;

        memset( &OutputBufferStruct, 0, sizeof(OutputBufferStruct) );
        OutputBufferStruct.pBuffer = &mediaBuffer;

        // Set DMO output format
        CHECKHR( MoInitMediaType(&mt, sizeof(WAVEFORMATEX)) );

        mt.majortype = MEDIATYPE_Audio;
        mt.subtype = MEDIASUBTYPE_PCM;
        mt.lSampleSize = 0;
        mt.bFixedSizeSamples = TRUE;
        mt.bTemporalCompression = FALSE;
        mt.formattype = FORMAT_WaveFormatEx;
        memcpy(mt.pbFormat, &GetWaveFormat(), sizeof(WAVEFORMATEX));
    
        CHECKHR( pDMO->SetOutputType(0, &mt, 0) );
        MoFreeMediaType(&mt);

        // Allocate streaming resources. This step is optional. If it is not called here, it
        // will be called when first time ProcessInput() is called. However, if you want to 
        // get the actual frame size being used, it should be called explicitly here.
        CHECKHR( pDMO->AllocateStreamingResources() );
    
        // Get actually frame size being used in the DMO. (optional, do as you need)
        int iFrameSize;
        PROPVARIANT pvFrameSize;
        PropVariantInit(&pvFrameSize);
        CHECKHR(pPS->GetValue(MFPKEY_WMAAECMA_FEATR_FRAME_SIZE, &pvFrameSize));
        iFrameSize = pvFrameSize.lVal;
        PropVariantClear(&pvFrameSize);

        // allocate output buffer
        mediaBuffer.SetBufferLength( GetWaveFormat().nSamplesPerSec * GetWaveFormat().nBlockAlign );
    }

    std::vector< BYTE > Read()
    {
        mediaBuffer.Clear();

        do{
            // 音声データを取得する
            DWORD dwStatus;
            CHECKHR( pDMO->ProcessOutput(0, 1, &OutputBufferStruct, &dwStatus) );

            // ビームと音声の方向を取得する
			CHECKHR( pSC->GetBeam(&dBeamAngle) );
			CHECKHR( pSC->GetPosition(&dAngle, &dConf) );
        } while ( OutputBufferStruct.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE );

        return mediaBuffer.Clone();
    }

    double GetSoundSourcePositionConfidence() const
    {
        return dConf;
    }

    double GetSoundSourcePosition() const
    {
        return dAngle;
    }
    
    double GetMicArrayBeamAngle() const
    {
        return dBeamAngle;
    }

    CComPtr<IMediaObject> pDMO;  
    CComPtr<IPropertyStore> pPS;
	CComPtr<ISoundSourceLocalizer> pSC;

    CStaticMediaBuffer mediaBuffer;
    DMO_OUTPUT_DATA_BUFFER OutputBufferStruct;

    double dBeamAngle, dAngle, dConf;	

private:


};

void main()
{
    try {
        ::CoInitialize( NULL );

        KinectAudioSource source;
        source.Initialize();
        source.SetSystemMode( 4 );

        source.Start();
	    printf(("\nRecording using DMO\n"));

        WaveAudio wave;
        wave.Initialize( source.GetWaveFormat().nSamplesPerSec, source.GetWaveFormat().wBitsPerSample,
            source.GetWaveFormat().nChannels );

        puts("\nAEC-MicArray is running ... Press any key to stop\n");

        while ( !_kbhit() ) {
            std::vector< BYTE > buffer = source.Read();
            if ( buffer.size() != 0 ) {
                wave.Output( &buffer[0], buffer.size() );
            }

            if( source.GetSoundSourcePositionConfidence() > 0.9 ) {
                printf( "Position: %f\t\tBeam Angle = %f\r", source.GetSoundSourcePosition(),
                    source.GetMicArrayBeamAngle() );					
			}
        }

        std::cout << std::endl;
        std::cout << "success!!" << std::endl;
    }
    catch ( std::exception& ex ) {
        std::cout << ex.what() << std::endl;
    }
}
