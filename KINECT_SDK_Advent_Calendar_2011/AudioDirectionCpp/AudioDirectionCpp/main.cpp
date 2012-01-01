#include <conio.h>

#include <iostream>


#include "WaveAudio.h"
#include "KinectAudioSource.h"

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
