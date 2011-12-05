using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using Microsoft.Research.Kinect.Nui;

namespace DepthWPF
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            // Depthのみを使う設定で初期化
            Runtime kinect = Runtime.Kinects[0];
            kinect.Initialize( RuntimeOptions.UseDepth );
            kinect.DepthStream.Open( ImageStreamType.Depth, 2,
                ImageResolution.Resolution640x480, ImageType.Depth );
            kinect.DepthFrameReady +=
                new EventHandler<ImageFrameReadyEventArgs>( kinect_DepthFrameReady );
        }

        void kinect_DepthFrameReady( object sender, ImageFrameReadyEventArgs e )
        {
            var source = e.ImageFrame.Image;
            image1.Source = BitmapSource.Create( source.Width, source.Height, 96, 96,
                    PixelFormats.Gray16, null, ConvertGrayScale( source ).Bits,
                    source.Width * source.BytesPerPixel );
        }

        // 距離データをグレースケールに変換する
        private static PlanarImage ConvertGrayScale( PlanarImage source )
        {
            // thanks : http://www.atmarkit.co.jp/fdotnet/kinectsdkbeta/kinectsdkbeta02/kinectsdkbeta02_02.html
            for ( int i = 0; i < source.Bits.Length; i += 2 ) {
                // Depthのみ(ユーザーデータなし)のデータ取得
                ushort depth = (ushort)(source.Bits[i] | (source.Bits[i+1] << 8));

                // 0x0fff(Depthの最大値)を0-0xffffの間に変換する
                depth = (ushort)(0xffff - (0xffff * depth / 0x0fff));

                // 戻す
                source.Bits[i] = (byte)(depth & 0xff);
                source.Bits[i+1] = (byte)((depth >> 8) & 0xff);
            }

            return source;
        }
    }
}
