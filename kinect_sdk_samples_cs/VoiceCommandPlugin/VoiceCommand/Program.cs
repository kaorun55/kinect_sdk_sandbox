using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using System.IO;
using System.Diagnostics;
using Microsoft.Research.Kinect.Audio;
using Microsoft.Speech.Recognition;
using SpeechRecognizer;
using Microsoft.Speech.AudioFormat;

namespace VoiceCommand
{
    class Program
    {
        static MicroThread mt = new MicroThread();

        static void Main( string[] args )
        {
            try {
                var dlls = Directory.EnumerateFiles( @"../../../plugin", @"*.dll" );
                foreach ( var dll in dlls ) {
                    Console.WriteLine( Path.GetFileName( dll ) );
                    foreach ( var app in VoiceCommandPluginHost.GetInstance( dll ) ) {
                        mt.AddApp( app );
                    }
                }


                #region Test
#if false
                mt.DoWork( "powerpoint" );
                mt.DoWork( "next" );

                mt.DoWork( "powerpoint" );
                mt.DoWork( "close" );

                mt.DoWork( "powerpoint" );
                mt.DoWork( "prev" );

                mt.DoWork( "powerpoint" );

                mt.DoWork( "explorer" );
                mt.DoWork( "close" );


                mt.DoWork( "powerpoint" );
                mt.DoWork( "prev" );
#endif
                #endregion

                using ( var source = new KinectAudioSource() ) {
                    source.FeatureMode = true;
                    source.AutomaticGainControl = false; //Important to turn this off for speech recognition
                    source.SystemMode = SystemMode.OptibeamArrayOnly; //No AEC for this sample

                    var colors = new Choices();
                    foreach ( var app in mt.Apps ) {
                        foreach ( var command in app ) {
                            Console.WriteLine( command );
                            colors.Add( command );
                        }
                    }

                    Recognizer r = new Recognizer( "ja-JP", colors );
                    r.SpeechRecognized += SreSpeechRecognized;
                    r.SpeechHypothesized += SreSpeechHypothesized;
                    r.SpeechRecognitionRejected += SreSpeechRecognitionRejected;
                    Console.WriteLine( "Using: {0}", r.Name );

                    using ( Stream s = source.Start() ) {
                        r.SetInputToAudioStream( s, new SpeechAudioFormatInfo(
                                                        EncodingFormat.Pcm, 16000, 16, 1,
                                                        32000, 2, null ) );

                        r.RecognizeAsync( RecognizeMode.Multiple );
                        Console.ReadLine();
                        Console.WriteLine( "Stopping recognizer ..." );
                        r.RecognizeAsyncStop();
                    }
                }

            }
            catch ( Exception ex ) {
                Console.WriteLine( ex.Message );
            }
        }

        static void SreSpeechRecognitionRejected( object sender, SpeechRecognitionRejectedEventArgs e )
        {
            Console.WriteLine( "\nSpeech Rejected" );
        }

        static void SreSpeechHypothesized( object sender, SpeechHypothesizedEventArgs e )
        {
            Console.Write( "\rSpeech Hypothesized: \t{0}", e.Result.Text );
        }

        static void SreSpeechRecognized( object sender, SpeechRecognizedEventArgs e )
        {
            try {
                Console.WriteLine( "\nSpeech Recognized: \t{0}", e.Result.Text );
                mt.DoWork( e.Result.Text );
            }
            catch ( Exception ex ) {
                Console.WriteLine( ex.Message );
            }
        }
}
}
