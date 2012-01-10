using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;

namespace VoiceCommand
{
    public class VoiceCommandPluginHost
    {
        public static IEnumerable<VoiceCommandPlugin> GetInstance( string pluginName )
        {
            Assembly asm = Assembly.LoadFrom( pluginName );
            foreach ( Type t in asm.GetTypes() ) {
                if ( t.IsClass && t.IsPublic && !t.IsAbstract && t.IsSubclassOf( typeof( VoiceCommandPlugin ) ) ) {
                    yield return asm.CreateInstance( t.FullName ) as VoiceCommandPlugin;
                }
            }
        }
    }
}
