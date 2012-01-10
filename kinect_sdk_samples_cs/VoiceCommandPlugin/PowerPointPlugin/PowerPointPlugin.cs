using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SpeechRecognizer;

namespace VoiceCommand
{
    public class PowerPointPlugin : VoiceCommandPlugin
    {
        const string Start = "かいし";
        const string End = "しゅうりょう";

        const string Next = "つぎ";
        const string Prev = "まえ";

        PowerPointController ppt = new PowerPointController();

        public PowerPointPlugin()
            : base( "powerpoint" )
        {
        }

        public override void Command( string command )
        {
            if ( command == Start ) {
                Console.WriteLine( Next );
                ppt.FindSlideShow();
            }
            else if ( command == End ) {
                Console.WriteLine( Next );
                ppt.End();
            }
            else if ( command == Next ) {
                Console.WriteLine( Next );
                ppt.Next();
            }
            else if ( command == Prev ) {
                Console.WriteLine( Prev );
                ppt.Prev();
            }
            else {
                Console.WriteLine( "invalid command." );
            }
        }

        public override IEnumerator<string> GetEnumerator()
        {
            yield return Name;
            yield return Start;
            yield return End;
            yield return Next;
            yield return Prev;
        }
    }
}
