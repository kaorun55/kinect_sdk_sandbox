using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using Microsoft.Research.Kinect.Nui;
using System.Threading;
using System.Windows.Threading;

namespace SkeletonWPF
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
                kinect.Initialize( RuntimeOptions.UseColor | RuntimeOptions.UseDepthAndPlayerIndex |
                                RuntimeOptions.UseSkeletalTracking );
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
            Colors.Magenta,
            Colors.Pink
        };

        void RenderThread()
        {
            while ( shouldRun ) {
                Runtime kinect = Runtime.Kinects[0];
                var video = kinect.VideoStream.GetNextFrame( 100 );
                var depth = kinect.DepthStream.GetNextFrame( 100 );
                var skeleton = kinect.SkeletonEngine.GetNextFrame( 100 );

                this.Dispatcher.BeginInvoke( DispatcherPriority.Background, new Action( () =>
                {
                    DrawingVisual drawingVisual = new DrawingVisual();
                    using ( DrawingContext drawingContext = drawingVisual.RenderOpen() ) {
                        drawingContext.DrawImage( DrawPixels( kinect, video, depth ),
                                    new Rect( 0, 0, video.Image.Width, video.Image.Height ) );

                        foreach ( var s in skeleton.Skeletons ) {
                            if ( s.TrackingState == SkeletonTrackingState.Tracked ) {
                                foreach ( Joint j in s.Joints ) {
                                    var point = GetVideoPoint( j );

                                    // 円を書く
                                    drawingContext.DrawEllipse( new SolidColorBrush( Colors.Red ),
                                        new Pen( Brushes.Red, 1 ), new Point( point.X, point.Y ), 5, 5 );
                                }
                            }
                        }
                    }

                    // 描画可能なビットマップを作る
                    // http://stackoverflow.com/questions/831860/generate-bitmapsource-from-uielement
                    RenderTargetBitmap bitmap =
                    new RenderTargetBitmap( video.Image.Width, video.Image.Height, 96, 96, PixelFormats.Default );
                    bitmap.Render( drawingVisual );

                    image1.Source = bitmap;
                } ) );
            }
        }

        private WriteableBitmap DrawPixels( Runtime kinect, ImageFrame video, ImageFrame depth )
        {
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
                        video.Image.Bits[videoIndex] *= userColor[playerIndex].R;
                        video.Image.Bits[videoIndex + 1] *= userColor[playerIndex].G;
                        video.Image.Bits[videoIndex + 2] *= userColor[playerIndex].B;
                    }
                }
            }

            //バイト列をビットマップに展開
            //描画可能なビットマップを作る
            //http://msdn.microsoft.com/ja-jp/magazine/cc534995.aspx
            WriteableBitmap bitmap = new WriteableBitmap( video.Image.Width, video.Image.Height, 96, 96,
                                                          PixelFormats.Bgr32, null );
            bitmap.WritePixels( new Int32Rect( 0, 0, video.Image.Width, video.Image.Height ), video.Image.Bits,
                                video.Image.Width * video.Image.BytesPerPixel, 0 );

            return bitmap;
        }


        private Point GetVideoPoint( Joint joint )
        {
            Runtime kinect = Runtime.Kinects[0];
            float depthX = 0, depthY = 0;
            kinect.SkeletonEngine.SkeletonToDepthImage( joint.Position, out depthX, out depthY );
            depthX = Math.Min( depthX * kinect.DepthStream.Width, kinect.DepthStream.Width );
            depthY = Math.Min( depthY * kinect.DepthStream.Height, kinect.DepthStream.Height );

            int videoX = 0, videoY = 0;
            kinect.NuiCamera.GetColorPixelCoordinatesFromDepthPixel( ImageResolution.Resolution640x480,
                new ImageViewArea(), (int)depthX, (int)depthY, 0, out videoX, out videoY );

            return new Point( Math.Min( videoX, kinect.VideoStream.Width ), Math.Min( videoY, kinect.VideoStream.Height ) );
        }

        private void Window_Closing( object sender, System.ComponentModel.CancelEventArgs e )
        {
            shouldRun = false;
        }
    }
}
