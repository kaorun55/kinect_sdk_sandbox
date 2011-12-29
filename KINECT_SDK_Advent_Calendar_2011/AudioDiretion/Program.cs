using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Research.Kinect.Audio;
using System.IO;

namespace AudioDiretion
{
    class Program
    {
        static void Main( string[] args )
        {
            // 音源を取得するインスタンスを生成する
            using ( KinectAudioSource source = new KinectAudioSource() ) {
                // SingleChannelAec:シングルチャネルのマイクで、エコーキャンセルを使用する
                // OptibeamArrayOnly:マルチチャネルのマイクのみを使用する(エコーキャンセルを使用しない)
                // OptibeamArrayAndAec:マルチチャネルのマイクと、エコーキャンセルを使用する)
                // SingleChannelNsAgc:シングルチャネルのマイクのみを使用する(エコーキャンセルを使用しない)
                source.SystemMode = SystemMode.OptibeamArrayOnly;
                source.BeamChanged += new EventHandler<BeamChangedEventArgs>( source_BeamChanged );

                using ( Stream audioStream = source.Start() ) {
                    Console.WriteLine( "Start... Press any key" );

                    byte[] buffer = new byte[4096];
                    Win32.StreamingWavePlayer player = new Win32.StreamingWavePlayer( 16000, 16, 1, 100 );
                    while ( !Console.KeyAvailable ) {
                        int count = audioStream.Read( buffer, 0, buffer.Length );
                        player.Output( buffer );

                        // 詳細な音源方向の信頼性がある場合、音源方向を更新する
                        if ( source.SoundSourcePositionConfidence > 0.9 ) {
                            Console.Write( "詳細な音源方向(推定) : {0}\t\tビーム方向 : {1}\r",
                                source.SoundSourcePosition, source.MicArrayBeamAngle );
                        }
                    }
                }
            }
        }

        static void source_BeamChanged( object sender, BeamChangedEventArgs e )
        {
            Console.WriteLine( "ビーム方向が変わった:{0}", e.Angle );
        }
    }
}
