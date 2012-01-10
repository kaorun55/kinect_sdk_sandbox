using System;
using System.IO;
using System.Linq;
using Microsoft.Speech.AudioFormat;
using Microsoft.Speech.Recognition;

namespace SpeechRecognizer
{
    public class Recognizer
    {
        SpeechRecognitionEngine engine;
        RecognizerInfo info;
        GrammarBuilder builder;
        Grammar grammar;

        public event EventHandler<SpeechRecognitionRejectedEventArgs> SpeechRecognitionRejected;
        public event EventHandler<SpeechHypothesizedEventArgs> SpeechHypothesized;
        public event EventHandler<SpeechRecognizedEventArgs> SpeechRecognized;

        public string Name
        {
            get
            {
                return info.Name;
            }
        }

        public Recognizer( string name, Choices alternateChoices )
        {
            info = GetRecognizer( name );
            engine = new SpeechRecognitionEngine( info.Id );

            builder = new GrammarBuilder();
            builder.Culture = info.Culture;
            builder.Append( alternateChoices );

            grammar = new Grammar( builder );
            engine.LoadGrammar( grammar );
            engine.SpeechRecognized += Recognizer_SpeechRecognized;
            engine.SpeechHypothesized += Recognizer_SpeechHypothesized;
            engine.SpeechRecognitionRejected += Recognizer_SpeechRecognitionRejected;
        }

        public void SetInputToAudioStream( Stream audioSource, SpeechAudioFormatInfo audioFormat )
        {
            engine.SetInputToAudioStream( audioSource, audioFormat );
        }

        public void RecognizeAsync( RecognizeMode mode )
        {
            engine.RecognizeAsync( mode );
        }

        public void RecognizeAsyncStop()
        {
            engine.RecognizeAsyncStop();
        }


        private void Recognizer_SpeechRecognitionRejected( object sender, SpeechRecognitionRejectedEventArgs e )
        {
            if ( SpeechRecognitionRejected != null ) {
                SpeechRecognitionRejected( sender, e );
            }
        }

        private void Recognizer_SpeechHypothesized( object sender, SpeechHypothesizedEventArgs e )
        {
            if ( SpeechHypothesized != null ) {
                SpeechHypothesized( sender, e );
            }
        }

        private void Recognizer_SpeechRecognized( object sender, SpeechRecognizedEventArgs e )
        {
            if ( SpeechRecognized != null ) {
                SpeechRecognized( sender, e );
            }
        }


        private static RecognizerInfo GetRecognizer( string name )
        {
            Func<RecognizerInfo, bool> matchingFunc = r =>
            {
                return name.Equals( r.Culture.Name, StringComparison.InvariantCultureIgnoreCase );
            };
            return SpeechRecognitionEngine.InstalledRecognizers().Where( matchingFunc ).FirstOrDefault();
        }
    }
}
