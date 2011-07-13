/////////////////////////////////////////////////////////////////////////
//
// This module provides sample code used to demonstrate the use
// of the KinectAudioSource for speech recognition
//
// Copyright © Microsoft Corporation.  All rights reserved.  
// This code is licensed under the terms of the 
// Microsoft Kinect for Windows SDK (Beta) from Microsoft Research 
// License Agreement: http://research.microsoft.com/KinectSDK-ToU
//
/////////////////////////////////////////////////////////////////////////

/*
 * IMPORTANT: This sample requires the following components to be installed:
 * 
 * Speech Platform Runtime (v10.2) x86. Even on x64 platforms the x86 needs to be used because the MSR Kinect SDK runtime is x86
 * http://www.microsoft.com/downloads/en/details.aspx?FamilyID=bb0f72cb-b86b-46d1-bf06-665895a313c7
 * 
 * Kinect English Language Pack: MSKinectLangPack_enUS.msi (available in the same location as the Kinect For
 * Windows SDK)
 *
 * Speech Platform SDK (v10.2) 
 * http://www.microsoft.com/downloads/en/details.aspx?FamilyID=1b1604d3-4f66-4241-9a21-90a294a5c9a4&displaylang=en
 * */

using System;
using System.IO;
using System.Linq;
using Microsoft.Research.Kinect.Audio;
using Microsoft.Speech.AudioFormat;
using Microsoft.Speech.Recognition;

namespace Speech
{
    class Program
    {
        private const string RecognizerId = "SR_MS_en-US_Kinect_10.0";

        static void Main(string[] args)
        {
            try
            {
                using (var source = new KinectAudioSource())
                {
                    source.FeatureMode = true;
                    source.AutomaticGainControl = false; //Important to turn this off for speech recognition
                    source.SystemMode = SystemMode.OptibeamArrayOnly; //No AEC for this sample

                    RecognizerInfo ri = SpeechRecognitionEngine.InstalledRecognizers().Where(r => r.Id == RecognizerId).FirstOrDefault();

                    if (ri == null)
                    {
                        Console.WriteLine("Could not find speech recognizer: {0}. Please refer to the sample requirements.", RecognizerId);
                        return;
                    }

                    Console.WriteLine("Using: {0}", ri.Name);

                    using (var sre = new SpeechRecognitionEngine(ri.Id))
                    {
                        var colors = new Choices();
                        colors.Add( "red" );
                        colors.Add( "green" );
                        colors.Add( "blue" );

                        var gb = new GrammarBuilder();
                        //Specify the culture to match the recognizer in case we are running in a different culture.                                 
                        gb.Culture = ri.Culture;
                        gb.Append(colors);


                        // Create the actual Grammar instance, and then load it into the speech recognizer.
                        var g = new Grammar(gb);

                        sre.LoadGrammar( g );
                        sre.SpeechRecognized += SreSpeechRecognized;
                        sre.SpeechHypothesized += SreSpeechHypothesized;
                        sre.SpeechRecognitionRejected += SreSpeechRecognitionRejected;

                        using (Stream s = source.Start())
                        {
                            sre.SetInputToAudioStream(s,
                                                      new SpeechAudioFormatInfo(
                                                          EncodingFormat.Pcm, 16000, 16, 1,
                                                          32000, 2, null));

                            Console.WriteLine("Recognizing. Say: 'red', 'green' or 'blue'. Press ENTER to stop");

                            sre.RecognizeAsync(RecognizeMode.Multiple);
                            Console.ReadLine();
                            Console.WriteLine("Stopping recognizer ...");
                            sre.RecognizeAsyncStop();
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        static void SreSpeechRecognitionRejected(object sender, SpeechRecognitionRejectedEventArgs e)
        {
			Console.WriteLine("\nSpeech Rejected");
            if (e.Result != null)
                DumpRecordedAudio(e.Result.Audio);
        }

        static void SreSpeechHypothesized(object sender, SpeechHypothesizedEventArgs e)
        {
            Console.Write( "\rSpeech Hypothesized: \t{0}", e.Result.Text );
        }

        static void SreSpeechRecognized(object sender, SpeechRecognizedEventArgs e)
        {
			//This first release of the Kinect language pack doesn't have a reliable confidence model, so 
			//we don't use e.Result.Confidence here.
            Console.WriteLine( "\nSpeech Recognized: \t{0}", e.Result.Text );
        }

        private static void DumpRecordedAudio(RecognizedAudio audio)
        {
            if (audio == null) return;

            int fileId = 0;
            string filename;
            while (File.Exists((filename = "RetainedAudio_" + fileId + ".wav")))
                fileId++;

            Console.WriteLine("\nWriting file: {0}", filename);
            using (var file = new FileStream(filename, System.IO.FileMode.CreateNew))
                audio.WriteToWaveStream(file);
        }

    }
}
