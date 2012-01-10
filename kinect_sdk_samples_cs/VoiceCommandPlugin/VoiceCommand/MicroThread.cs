using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace kaorun55
{
    class MicroThread
    {
        public int x = 0;
        public int y = 0;
        IEnumerator<Command> thread;

        public List<VoiceCommandPlugin> Apps
        {
            get;
            set;
        }

        public MicroThread()
        {
            Apps = new List<VoiceCommandPlugin>();

            init();
        }

        public void AddApp( VoiceCommandPlugin app )
        {
            Apps.Add( app );
        }

        void init()
        {
            thread = get();
            thread.MoveNext();
        }

        public void DoWork( string command )
        {
            if ( thread != null ) {
                thread.Current( command );
                if ( !thread.MoveNext() ) {
                    init();
                }
            }
        }

        delegate void Command( string command );
        IEnumerator<Command> get()
        {
            string name = "";
            yield return ( app ) => name = app;

            var c = Apps.Where( ( a ) => a.Name == name );
            if ( c.Count() != 0 ) {
                yield return c.First().Command;
            }
            else {
                Console.WriteLine( "invalid application." );
            }
        }
    }
}
