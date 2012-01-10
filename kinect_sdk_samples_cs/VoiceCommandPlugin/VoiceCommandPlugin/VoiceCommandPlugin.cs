using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace kaorun55
{
    public abstract class VoiceCommandPlugin
    {
        public VoiceCommandPlugin( string name )
        {
            Name = name;
        }

        public string Name
        {
            get;
            protected set;
        }

        public abstract void Command( string command );

        public abstract IEnumerator<string> GetEnumerator();
    }
}
