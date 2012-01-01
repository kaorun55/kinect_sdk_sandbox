#include "KinectAudioSource.h"


#define CHECKHR( x ) { HRESULT hr = x; if (FAILED(hr)) { char buf[256]; sprintf( buf, "%d: %08X\n", __LINE__, hr ); throw std::runtime_error( buf );} }

    void KinectAudioSource::Initialize()
    {
        CHECKHR( mediaObject_.CoCreateInstance(CLSID_CMSRKinectAudio, NULL, CLSCTX_INPROC_SERVER ) );
        CHECKHR( mediaObject_.QueryInterface( &propertyStore_ ) );
	    CHECKHR( mediaObject_->QueryInterface( IID_ISoundSourceLocalizer, (void**)&soundSource_ ) );

        // Tell DMO which capture device to use (we're using whichever device is a microphone array).
        // Default rendering device (speaker) will be used.
        int  iMicDevIdx = GetMicArrayDeviceIndex();
    
        PROPVARIANT pvDeviceId;
        PropVariantInit(&pvDeviceId);
        pvDeviceId.vt = VT_I4;

        //Speaker index is the two high order bytes and the mic index the two low order ones
        int  iSpkDevIdx = 0;  //Asume default speakers
        pvDeviceId.lVal = (unsigned long)(iSpkDevIdx<<16) | (unsigned long)(0x0000ffff & iMicDevIdx);
        CHECKHR(propertyStore_->SetValue(MFPKEY_WMAAECMA_DEVICE_INDEXES, pvDeviceId));
        PropVariantClear(&pvDeviceId);
    }

    //   SINGLE_CHANNEL_AEC = 0
    //   OPTIBEAM_ARRAY_ONLY = 2
    //   OPTIBEAM_ARRAY_AND_AEC = 4
    //   SINGLE_CHANNEL_NSAGC = 5
    void KinectAudioSource::SetSystemMode( LONG mode )
    {
        // Set AEC-MicArray DMO system mode.
        // This must be set for the DMO to work properly
        PROPVARIANT pvSysMode;
        PropVariantInit(&pvSysMode);
        pvSysMode.vt = VT_I4;
        pvSysMode.lVal = mode;
        CHECKHR(propertyStore_->SetValue(MFPKEY_WMAAECMA_SYSTEM_MODE, pvSysMode));
        PropVariantClear(&pvSysMode);
    }

    void KinectAudioSource::Start()
    {

        DMO_MEDIA_TYPE mt = {0};

        ULONG cbProduced = 0;

        memset( &outputBufferStruct_, 0, sizeof(outputBufferStruct_) );
        outputBufferStruct_.pBuffer = &mediaBuffer_;

        // Set DMO output format
        CHECKHR( MoInitMediaType(&mt, sizeof(WAVEFORMATEX)) );

        mt.majortype = MEDIATYPE_Audio;
        mt.subtype = MEDIASUBTYPE_PCM;
        mt.lSampleSize = 0;
        mt.bFixedSizeSamples = TRUE;
        mt.bTemporalCompression = FALSE;
        mt.formattype = FORMAT_WaveFormatEx;
        memcpy(mt.pbFormat, &GetWaveFormat(), sizeof(WAVEFORMATEX));
    
        CHECKHR( mediaObject_->SetOutputType(0, &mt, 0) );
        MoFreeMediaType(&mt);

        // Allocate streaming resources. This step is optional. If it is not called here, it
        // will be called when first time ProcessInput() is called. However, if you want to 
        // get the actual frame size being used, it should be called explicitly here.
        CHECKHR( mediaObject_->AllocateStreamingResources() );
    
        // Get actually frame size being used in the DMO. (optional, do as you need)
        int iFrameSize;
        PROPVARIANT pvFrameSize;
        PropVariantInit(&pvFrameSize);
        CHECKHR(propertyStore_->GetValue(MFPKEY_WMAAECMA_FEATR_FRAME_SIZE, &pvFrameSize));
        iFrameSize = pvFrameSize.lVal;
        PropVariantClear(&pvFrameSize);

        // allocate output buffer
        mediaBuffer_.SetBufferLength( GetWaveFormat().nSamplesPerSec * GetWaveFormat().nBlockAlign );
    }

    std::vector< BYTE > KinectAudioSource::Read()
    {
        mediaBuffer_.Clear();

        do{
            // 音声データを取得する
            DWORD dwStatus;
            CHECKHR( mediaObject_->ProcessOutput(0, 1, &outputBufferStruct_, &dwStatus) );

            // ビームと音声の方向を取得する
			CHECKHR( soundSource_->GetBeam(&beamAngle_) );
			CHECKHR( soundSource_->GetPosition(&soundSourcePosition_, &soundSourcePositionConfidence_) );
        } while ( outputBufferStruct_.dwStatus & DMO_OUTPUT_DATA_BUFFERF_INCOMPLETE );

        return mediaBuffer_.Clone();
    }

    ///////////////////////////////////////////////////////////////////////////////
    // GetJackSubtypeForEndpoint
    //
    // Gets the subtype of the jack that the specified endpoint device is plugged
    // into.  E.g. if the endpoint is for an array mic, then we would expect the
    // subtype of the jack to be KSNODETYPE_MICROPHONE_ARRAY
    //
    ///////////////////////////////////////////////////////////////////////////////
    GUID KinectAudioSource::GetJackSubtypeForEndpoint( IMMDevice* pEndpoint )
    {
        if ( pEndpoint == 0 ) {
            throw std::invalid_argument( "生成されていないインスタンスが渡されました" );
        }
   
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

        GUID subtype;
        CHECKHR( spJackAsPart->GetSubType( &subtype ) );

        return subtype;
    }

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
    int KinectAudioSource::GetMicArrayDeviceIndex()
    {
        CComPtr<IMMDeviceEnumerator> spEnumerator;
        CHECKHR( spEnumerator.CoCreateInstance( __uuidof(MMDeviceEnumerator),  NULL, CLSCTX_ALL ) );

        CComPtr<IMMDeviceCollection> spEndpoints;
        CHECKHR( spEnumerator->EnumAudioEndpoints( eCapture, DEVICE_STATE_ACTIVE, &spEndpoints ) );

        UINT dwCount = 0;
        CHECKHR(spEndpoints->GetCount(&dwCount));

        // Iterate over all capture devices until finding one that is a microphone array
        for ( UINT index = 0; index < dwCount; index++) {
            IMMDevice* spDevice;
            CHECKHR( spEndpoints->Item( index, &spDevice ) );
        
            GUID subType = GetJackSubtypeForEndpoint( spDevice );
            if ( subType == KSNODETYPE_MICROPHONE_ARRAY ) {
                return index;
            }
        }

        throw std::runtime_error( "デバイスが見つかりません" );
    }
