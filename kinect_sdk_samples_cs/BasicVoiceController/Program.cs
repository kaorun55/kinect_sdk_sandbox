using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Research.Kinect.Audio;
using Microsoft.Speech.Recognition;
using kaorun55;
using System.IO;
using Microsoft.Speech.AudioFormat;
using System.Reflection;
using System.Diagnostics;

namespace BasicVoiceController
{
    class Program
    {
        static PowerPointController ppt = new PowerPointController();
        static VisualStudioVoiceController vs = new VisualStudioVoiceController();
        static CommandPromptVoiceController prompt = new CommandPromptVoiceController();

        static void Main( string[] args )
        {
            try {
                string path = @"file:\\\" + Assembly.GetEntryAssembly().Location;
                Debug.WriteLine( path );
                path.Replace( '\\', '/' );
                prompt.WindowsTitile = path;
                Debug.WriteLine( prompt.WindowsTitile );

                using ( var source = new KinectAudioSource() ) {
                    source.FeatureMode = true;
                    source.AutomaticGainControl = false; //Important to turn this off for speech recognition
                    source.SystemMode = SystemMode.OptibeamArrayOnly; //No AEC for this sample

                    var colors = new Choices();
                    foreach ( var command in new string[] { "かいし", "しゅうりょう", "つぎ", "まえ",
                                                            "びじゅあるすたじお", "びるど", "でばっぐ",
                                                            "ぷろんぷと", "いぐじっと"} ) {
                        Console.WriteLine( command );
                        colors.Add( command );
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
                switch ( e.Result.Text ) {
                case "かいし":
                    ppt.FindSlideShow();
                    break;
                case "しゅうりょう":
                    ppt.End();
                    break;
                case "つぎ":
                    ppt.Next();
                    break;
                case "まえ":
                    ppt.Prev();
                    break;

                case "びじゅあるすたじお":
                    vs.FindVisualStudio();
                    break;
                case "びるど":
                    vs.Build();
                    break;
                case "でばっぐ":
                    vs.Debug();
                    break;

                case "ぷろんぷと":
                    prompt.FindPrompt();
                    break;
                case "いぐじっと":
                    prompt.Exit();
                    break;
                }
            }
            catch ( Exception ex ) {
                Console.WriteLine( ex.Message );
            }
        }
    }
}
