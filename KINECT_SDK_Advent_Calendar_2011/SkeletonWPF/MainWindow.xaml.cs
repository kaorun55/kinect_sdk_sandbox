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
            InitializeComponent();

            Runtime kinect = Runtime.Kinects[0];
            kinect.Initialize( RuntimeOptions.UseColor | RuntimeOptions.UseDepthAndPlayerIndex |
                                RuntimeOptions.UseSkeletalTracking );
            kinect.VideoStream.Open( ImageStreamType.Video, 2,
                ImageResolution.Resolution640x480, ImageType.Color );
            kinect.DepthStream.Open( ImageStreamType.Depth, 2,
                ImageResolution.Resolution640x480, ImageType.DepthAndPlayerIndex );

            shouldRun = true;
            readerThread = new Thread( new ThreadStart( RenderThread ) );
            readerThread.Start();
        }

        // ユーザーにつける色
        static Color[] userColor= new Color[] {
            Color.FromRgb( 0, 0, 0 ),   // ユーザーなし
            Color.FromRgb( 1, 0, 0 ),
            Color.FromRgb( 0, 1, 0 ),
            Color.FromRgb( 0, 0, 1 ),
            Color.FromRgb( 1, 1, 0 ),
            Color.FromRgb( 1, 0, 1 ),
            Color.FromRgb( 0, 1, 1 ),
        };

        void RenderThread()
        {
            while ( shouldRun ) {
                Runtime kinect = Runtime.Kinects[0];
                var image = kinect.VideoStream.GetNextFrame( 0 );
                var depth = kinect.DepthStream.GetNextFrame( 0 );
                var skeleton = kinect.SkeletonEngine.GetNextFrame( 0 );

                var size  =image.Image.Width * image.Image.Height;
                for ( int i = 0; i < size; i++ ) {
                    var imageIndex = i * image.Image.BytesPerPixel;
                    var depthIndex = i * depth.Image.BytesPerPixel;

                    var depthVal = depth.Image.Bits[depthIndex] >> 3 | depth.Image.Bits[depthIndex + 1] << 5;
                    var userId = depth.Image.Bits[depthIndex] & 3;

                    image.Image.Bits[imageIndex] *= userColor[userId].R;
                    image.Image.Bits[imageIndex + 1] *= userColor[userId].G;
                    image.Image.Bits[imageIndex + 2] *= userColor[userId].B;
                }

                foreach ( var s in skeleton.Skeletons ) {
                    if ( s.TrackingState == SkeletonTrackingState.Tracked ) {
                        foreach ( Joint j in s.Joints ) {
                            var point = GetVideoPoint( j );

                            // 円を書く
                        }
                    }
                }

                this.Dispatcher.BeginInvoke( DispatcherPriority.Background, new Action( () =>
                {
                    var bitmap = image.Image;
                    image1.Source = BitmapImage.Create( bitmap.Width, bitmap.Height, 96, 96,
                                                        PixelFormats.Bgr32, null, bitmap.Bits,
                                                        bitmap.Width * bitmap.BytesPerPixel );
                } ) );
            }
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
