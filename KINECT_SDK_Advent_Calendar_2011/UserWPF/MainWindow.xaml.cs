using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using Microsoft.Research.Kinect.Nui;
using System.Threading;
using System.Windows.Threading;

namespace UserWPF
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        private Thread readerThread;
        private bool shouldRun;

        public MainWindow()
        {
            try {
                InitializeComponent();

                Runtime kinect = Runtime.Kinects[0];
                kinect.Initialize( RuntimeOptions.UseColor | RuntimeOptions.UseDepthAndPlayerIndex );
                kinect.VideoStream.Open( ImageStreamType.Video, 2,
                    ImageResolution.Resolution640x480, ImageType.Color );
                kinect.DepthStream.Open( ImageStreamType.Depth, 2,
                    ImageResolution.Resolution320x240, ImageType.DepthAndPlayerIndex );

                shouldRun = true;
                readerThread = new Thread( new ThreadStart( RenderThread ) );
                readerThread.Start();
            }
            catch ( Exception ex ) {
                MessageBox.Show( ex.Message );
            }
        }

        // ユーザーにつける色
        static Color[] userColor= new Color[] {
            Color.FromRgb( 0, 0, 0 ),   // ユーザーなし
            Colors.Red,
            Colors.Green,
            Colors.Blue,
            Colors.Yellow,
            Colors.Magenta
        };

        void RenderThread()
        {
            while ( shouldRun ) {
                Runtime kinect = Runtime.Kinects[0];
                var video = kinect.VideoStream.GetNextFrame( 100 );
                var depth = kinect.DepthStream.GetNextFrame( 100 );

                // ピクセルごとのユーザーIDを取得する
                for ( int y = 0; y < depth.Image.Height; y++ ) {
                    for ( int x = 0; x < depth.Image.Width; x++ ) {
                        int index = (x + (y * depth.Image.Width)) * 2;
                        byte byte0 = depth.Image.Bits[index];
                        byte byte1 = depth.Image.Bits[index + 1];

                        // ユーザーIDと距離を取得する
                        int playerIndex = byte0 & 0x7;
                        int distance = byte1 << 5 | byte0 >> 3;

                        if ( playerIndex != 0 ) {
                            // 距離座標をカメラ座標に変換する
                            int videoX = 0, videoY = 0;
                            kinect.NuiCamera.GetColorPixelCoordinatesFromDepthPixel( ImageResolution.Resolution640x480,
                                   new ImageViewArea(), x, y, 0, out videoX, out videoY );

                            int videoIndex = (videoX + (videoY * video.Image.Width)) * video.Image.BytesPerPixel;
                            videoIndex = Math.Min( videoIndex, video.Image.Bits.Length - video.Image.BytesPerPixel );
                            video.Image.Bits[videoIndex] = userColor[playerIndex].R;
                            video.Image.Bits[videoIndex + 1] = userColor[playerIndex].G;
                            video.Image.Bits[videoIndex + 2] = userColor[playerIndex].B;
                        }
                    }
                }

                this.Dispatcher.BeginInvoke( DispatcherPriority.Background, new Action( () =>
                    {
                        var bitmap = video.Image;
                        image1.Source = BitmapImage.Create( bitmap.Width, bitmap.Height, 96, 96,
                                                            PixelFormats.Bgr32, null, bitmap.Bits,
                                                            bitmap.Width * bitmap.BytesPerPixel );
                    } ) );
            }
        }

        private void Window_Closing( object sender, System.ComponentModel.CancelEventArgs e )
        {
            shouldRun = false;
        }
    }
}
