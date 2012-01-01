#pragma once

#include <Windows.h>
#include <atlbase.h>

#include <stdexcept>
#include <vector>

#include <mfapi.h>          // IPropertyStore
#include <wmcodecdsp.h>     // MFPKEY_WMAAECMA_SYSTEM_MODE
#include <MMDeviceApi.h>    // GetMicArrayDeviceIndex‚Ì’†
#include <devicetopology.h> // GetJackSubtypeForEndpoint‚Ì’†
#include <uuids.h>          // DShowRecord‚Ì’†

#include <MSRKinectAudio.h>

#include "StaticMediaBuffer.h"

#pragma comment( lib, "Msdmo.lib" )
#pragma comment( lib, "dmoguids.lib" ) // IMediaObject
#pragma comment( lib, "amstrmid.lib" )
//#pragma comment( lib, "avrt.lib" )


class KinectAudioSource
{
public:

    void Initialize();

    void SetSystemMode( LONG mode );

    void Start();

    std::vector< BYTE > Read();


    const WAVEFORMATEX& GetWaveFormat()
    {
        static const WAVEFORMATEX wfxOut = {WAVE_FORMAT_PCM, 1, 16000, 32000, 2, 16, 0};
        return wfxOut;
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

private:
    
    HRESULT GetJackSubtypeForEndpoint(IMMDevice* pEndpoint, GUID* pgSubtype);
    HRESULT GetMicArrayDeviceIndex(int *piDevice);

private:

    CComPtr<IMediaObject> pDMO;  
    CComPtr<IPropertyStore> pPS;
	CComPtr<ISoundSourceLocalizer> pSC;

    CStaticMediaBuffer mediaBuffer;
    DMO_OUTPUT_DATA_BUFFER OutputBufferStruct;

    double dBeamAngle;
    double dAngle;
    double dConf;	
};

