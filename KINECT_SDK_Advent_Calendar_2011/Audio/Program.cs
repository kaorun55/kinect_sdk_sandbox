using System;
using System.IO;
using Microsoft.Research.Kinect.Audio;

namespace Audio
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
                // SingleChannelNsAgc:???
                source.SystemMode = SystemMode.SingleChannelAec;

                using ( Stream audioStream = source.Start() ) {
                    Console.WriteLine( "Start... Press any key" );

                    byte[] buffer = new byte[4096];
                    Win32.StreamingWavePlayer player = new Win32.StreamingWavePlayer( 16000, 16, 1, 100 );
                    while ( !Console.KeyAvailable ) {
                        int count = audioStream.Read( buffer, 0, buffer.Length );
                        player.Output( buffer );
                    }
                }
            }
        }
    }
}
