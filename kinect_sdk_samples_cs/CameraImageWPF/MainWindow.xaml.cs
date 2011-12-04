using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

using Microsoft.Research.Kinect.Nui;

namespace CameraImageWPF
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        Runtime kinect;

        public MainWindow()
        {
            InitializeComponent();

            kinect = Runtime.Kinects[0];
            kinect.Initialize( RuntimeOptions.UseColor );
            kinect.VideoStream.Open( ImageStreamType.Video, 2,
                ImageResolution.Resolution640x480, ImageType.Color );
            kinect.VideoFrameReady +=
                new EventHandler<ImageFrameReadyEventArgs>( kinect_VideoFrameReady );
        }

        void kinect_VideoFrameReady( object sender, ImageFrameReadyEventArgs e )
        {
            PlanarImage image = e.ImageFrame.Image;
            kinectImage.Source = BitmapSource.Create( image.Width, image.Height, 96, 96,
                PixelFormats.Bgr32, null, image.Bits, image.Width * image.BytesPerPixel );
        }

    }
}
