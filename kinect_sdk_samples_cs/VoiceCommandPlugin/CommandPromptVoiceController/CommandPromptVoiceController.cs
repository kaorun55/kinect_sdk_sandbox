using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace kaorun55
{
    public class CommandPromptVoiceController
    {
        private IntPtr window = IntPtr.Zero;

        const string defaultWindowsTitile = @"C:\Windows\system32\cmd.exe";

        public CommandPromptVoiceController()
        {
            WindowsTitile = defaultWindowsTitile;
        }

        public string WindowsTitile
        {
            get;
            set;
        }

        public void FindPrompt()
        {
            window = IntPtr.Zero;

            // PPTとスライドショーを探す
            EnumWindows( new EnumWindowsDelegate( delegate( IntPtr hWnd, int lParam )
            {
                StringBuilder sb = new StringBuilder( 0x1024 );
                if ( (IsWindowVisible( hWnd ) != 0) && (GetWindowText( hWnd, sb, sb.Capacity ) != 0) ) {
                    string title = sb.ToString();
                    Debug.Print( title );
                    if ( string.Compare( title, WindowsTitile, true ) == 0 ) {
                        window = hWnd;
                    }
                }
                return 1;
            } ), 0 );

            if ( window == IntPtr.Zero ) {
                throw new Exception( "コマンドプロンプト が見つかりませんでした" );
            }
        }

        public void Exit()
        {
            SendKey( window, "{ENTER}" );
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
