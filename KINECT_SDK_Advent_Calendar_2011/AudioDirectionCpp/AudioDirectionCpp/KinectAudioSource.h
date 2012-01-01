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

#include "StaticmediaBuffer.h"

#pragma comment( lib, "Msdmo.lib" )
#pragma comment( lib, "dmoguids.lib" ) // IMediaObject
#pragma comment( lib, "amstrmid.lib" )

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
        return soundSourcePositionConfidence_;
    }

    double GetSoundSourcePosition() const
    {
        return soundSourcePosition_;
    }
    
    double GetMicArrayBeamAngle() const
    {
        return beamAngle_;
    }

private:
    
    GUID GetJackSubtypeForEndpoint( IMMDevice* pEndpoint );
    int GetMicArrayDeviceIndex();

private:

    CComPtr<IMediaObject> mediaObject_;  
    CComPtr<IPropertyStore> propertyStore_;
	CComPtr<ISoundSourceLocalizer> soundSource_;

    CStaticmediaBuffer mediaBuffer_;
    DMO_OUTPUT_DATA_BUFFER outputBufferStruct_;

    double beamAngle_;
    double soundSourcePosition_;
    double soundSourcePositionConfidence_;	
};

