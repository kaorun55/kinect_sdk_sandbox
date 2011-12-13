using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
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
            InitializeComponent();

            Runtime kinect = Runtime.Kinects[0];
            kinect.Initialize( RuntimeOptions.UseColor | RuntimeOptions.UseDepthAndPlayerIndex );
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

                this.Dispatcher.BeginInvoke( DispatcherPriority.Background, new Action( () =>
                    {
                        var bitmap = image.Image;
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
