using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace kaorun55
{
    public class VisualStudioVoiceController
    {
        private IntPtr window = IntPtr.Zero;

        const string defaultVisualStudioPpostfix = "Microsoft Visual Studio";

        public VisualStudioVoiceController()
        {
            VisualStudioPpostfix = defaultVisualStudioPpostfix;
        }

        public string VisualStudioPpostfix
        {
            get;
            set;
        }

        public void FindVisualStudio()
        {
            window = IntPtr.Zero;

            // PPTとスライドショーを探す
            EnumWindows( new EnumWindowsDelegate( delegate( IntPtr hWnd, int lParam )
            {
                StringBuilder sb = new StringBuilder( 0x1024 );
                if ( (IsWindowVisible( hWnd ) != 0) && (GetWindowText( hWnd, sb, sb.Capacity ) != 0) ) {
                    string title = sb.ToString();
                    System.Diagnostics.Debug.Print( title );
                    if ( title.EndsWith( VisualStudioPpostfix ) ) {
                        window = hWnd;
                    }
                }
                return 1;
            } ), 0 );

            if ( window == IntPtr.Zero ) {
                throw new Exception( "Visual Studio が見つかりませんでした" );
            }
        }

        public void Build()
        {
            SendKey( window, "^+B" );
        }

        public void Debug()
        {
            SendKey( window, "{F5}" );
        }

        private void SendKey( IntPtr window, string key )
        {
            if ( window != IntPtr.Zero ) {
                bool ret = SetForegroundWindow( window );
                if ( !ret ) {
                    throw new Exception( "ウィンドウを前面に出せません" );
                }

                System.Windows.Forms.SendKeys.SendWait( key );
            }
        }



        private delegate int EnumWindowsDelegate( IntPtr hWnd, int lParam );

        [DllImport( "user32.DLL" )]
        public static extern bool SetForegroundWindow( IntPtr hWnd );
        [DllImport( "user32.dll" )]
        private static extern int EnumWindows( EnumWindowsDelegate lpEnumFunc, int lParam );
        [DllImport( "user32.dll" )]
        private static extern int IsWindowVisible( IntPtr hWnd );
        [DllImport( "user32.dll", CharSet = CharSet.Auto )]
        private static extern int GetWindowText( IntPtr hWnd, StringBuilder lpString, int nMaxCount );
    }
}
