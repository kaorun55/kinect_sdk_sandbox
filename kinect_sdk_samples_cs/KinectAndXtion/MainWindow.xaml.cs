using System;
using System.Windows;
using Coding4Fun.Kinect.Wpf;
using Microsoft.Research.Kinect.Nui;
using System.Threading;
using OpenNI;
using System.Windows.Threading;
using System.Windows.Media.Imaging;
using System.Windows.Media;


namespace KinectAndXtion
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        private Context context;
        private ImageGenerator image;

        private Thread readerThread;
        private bool shouldRun;

        public MainWindow()
        {
            InitializeComponent();

            try {
                InitKinectSDK();
                InitOpenNI();

            }
            catch ( Exception ex ) {
                MessageBox.Show( ex.Message );
                Close();
            }
        }

        private void InitOpenNI()
        {
            // ContextとImageGeneratorの作成
            ScriptNode node;
            context = Context.CreateFromXmlFile( "SamplesConfig.xml", out node );
            context.GlobalMirror = false;
            image = context.FindExistingNode( NodeType.Image ) as ImageGenerator;

            // 画像更新のためのスレッドを作成
            shouldRun = true;
            readerThread = new Thread( new ThreadStart( () =>
            {
                while ( shouldRun ) {
                    context.WaitAndUpdateAll();
                    ImageMetaData imageMD = image.GetMetaData();

                    // ImageMetaDataをBitmapSourceに変換する(unsafeにしなくてもOK!!)
                    this.Dispatcher.BeginInvoke( DispatcherPriority.Background, new Action( () =>
                    {
                        imageOpenNI.Source = BitmapSource.Create( imageMD.XRes, imageMD.YRes,
                            96, 96, PixelFormats.Rgb24, null, imageMD.ImageMapPtr,
                            imageMD.DataSize, imageMD.XRes * imageMD.BytesPerPixel );
                    } ) );
                }
            } ) );
            readerThread.Start();
        }

        private void InitKinectSDK()
        {
            // Kinectインスタンスを取得する
            Runtime kinect = Runtime.Kinects[0];
            kinect.Initialize( RuntimeOptions.UseColor );

            // RGBカメラの初期化と、イベントを登録する
            kinect.VideoStream.Open( ImageStreamType.Video, 2,
                ImageResolution.Resolution640x480, ImageType.Color );
            kinect.VideoFrameReady +=
                    new EventHandler<ImageFrameReadyEventArgs>( kinect_VideoFrameReady );
        }

        // RGBカメラのフレーム更新イベント
        void kinect_VideoFrameReady( object sender, ImageFrameReadyEventArgs e )
        {
            // RGBカメラの画像を、Imageに表示する
            // ToBitmapSource()は、Coding4Fun.Kinect.Wpfにある拡張メソッド
            imageKinectSDK.Source = e.ImageFrame.ToBitmapSource();
        }

        private void Window_Closing( object sender, System.ComponentModel.CancelEventArgs e )
        {
            shouldRun = false;
        }
    }
}
