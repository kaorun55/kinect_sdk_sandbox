// Thanks : http://muumoo.jp/news/2008/03/26/0enumwindows.html
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace SpeechRecognizer
{
    public class PowerPointController
    {
        private IntPtr slideWindow = IntPtr.Zero;
        private IntPtr pptWindow = IntPtr.Zero;

        const string defaultSlideShowPrefix = "PowerPoint スライド ショー";
        const string defaultPowerPointPpostfix = "Microsoft PowerPoint";

        public PowerPointController()
        {
            SlideShowPrefix = defaultSlideShowPrefix;
            PowerPointPpostfix = defaultPowerPointPpostfix;
        }

        public string SlideShowPrefix
        {
            get;
            set;
        }

        public string PowerPointPpostfix
        {
            get;
            set;
        }

        public void FindSlideShow()
        {
            slideWindow = IntPtr.Zero;

            // PPTとスライドショーを探す
            EnumWindows( new EnumWindowsDelegate( delegate( IntPtr hWnd, int lParam )
            {
                StringBuilder sb = new StringBuilder( 0x1024 );
                if ( (IsWindowVisible( hWnd ) != 0) && (GetWindowText( hWnd, sb, sb.Capacity ) != 0) ) {
                    string title = sb.ToString();
                    Debug.Print( title );
                    if ( title.StartsWith( SlideShowPrefix ) ) {
                        slideWindow = hWnd;
                    }
                    else if ( title.EndsWith( PowerPointPpostfix ) ) {
                        pptWindow = hWnd;
                    }
                }
                return 1;
            } ), 0 );

            if ( slideWindow == IntPtr.Zero ) {
                if ( pptWindow == IntPtr.Zero ) {
                    throw new Exception( "PowerPointまたはスライドショーが見つかりませんでした" );
                }

                // PPT があれば、スライドショーを開始すして、もう一度探す
                SendKey( pptWindow, "{F5}" );
                FindSlideShow();
            }
        }

        public void Next()
        {
            SendKey( slideWindow, "{Right}" );
        }

        public void Prev()
        {
            SendKey( slideWindow, "{Left}" );
        }

        public void End()
        {
            SendKey( slideWindow, "{Escape}" );
        }

        public void Move( int page )
        {
            SendKey( slideWindow, page.ToString() + "{Enter}" );
        }

        public void MoveFirst()
        {
            Move( 1 );
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
