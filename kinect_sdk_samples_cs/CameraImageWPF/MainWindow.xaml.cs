using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

using Microsoft.Research.Kinect.Nui;
using System.Windows.Controls;

namespace CameraImageWPF
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            images = new Image[] {
                image1, image2, image3, image4
            };

            // 挿抜状態の変化を通知する
            Runtime.Kinects.StatusChanged +=
                new EventHandler<StatusChangedEventArgs>( Kinects_StatusChanged );

            ShowKinectCount();

            // 最初に刺されている分を初期化する
            foreach ( Runtime kinect in Runtime.Kinects ) {
                InitKinect( kinect );
            }
        }

        private void InitKinect( Runtime kinect )
        {
            kinect.Initialize( RuntimeOptions.UseColor );
            kinect.VideoStream.Open( ImageStreamType.Video, 2,
                ImageResolution.Resolution640x480, ImageType.Color );
            kinect.VideoFrameReady +=
                    new EventHandler<ImageFrameReadyEventArgs>( kinect_VideoFrameReady );
        }

        // KINECTの状態が変わった
        void Kinects_StatusChanged( object sender, StatusChangedEventArgs e )
        {
            // KINECTの数を表示
            ShowKinectCount();

            // KINECTが刺された
            if ( e.Status == KinectStatus.Connected ) {
                InitKinect( e.KinectRuntime );
            }
            // KINECTが抜かれた
            else if ( e.Status == KinectStatus.Disconnected ) {
                // インスタンス数が切りつめられるので、インスタンス数の最後のImageをnullにする
                // 一度に何個も抜かれるかもなので、最後までnullにしちゃう
                for ( int i = Runtime.Kinects.Count; i < images.Length; i++ ) {
                    images[i].Source = null;
                }

                // 抜かれたKINECTのイベント削除と、インスタンスの破棄
                e.KinectRuntime.VideoFrameReady -=
                    new EventHandler<ImageFrameReadyEventArgs>( kinect_VideoFrameReady );
                e.KinectRuntime.Uninitialize();
            }
        }

        private void ShowKinectCount()
        {
            kinectCount.Text = Runtime.Kinects.Count + "台のKINECTが有効です";
        }

        void kinect_VideoFrameReady( object sender, ImageFrameReadyEventArgs e )
        {
            // 抜かれた瞬間のKINECTは、InstanceIndex が 0 になる
            Runtime kinect = sender as Runtime;
            if ( (kinect != null) && (kinect.InstanceIndex >= 0) ) {
                PlanarImage srouce = e.ImageFrame.Image;
                Image dest = images[kinect.InstanceIndex];
                dest.Source = BitmapSource.Create( srouce.Width, srouce.Height, 96, 96,
                    PixelFormats.Bgr32, null, srouce.Bits, srouce.Width * srouce.BytesPerPixel );
            }
        }

        Image[] images;
    }
}
