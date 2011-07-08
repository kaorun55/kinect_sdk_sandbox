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
using Kinect.Toolkit;
using System.Diagnostics;

namespace kinect_toolkit
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        Runtime kinectRuntime;
        readonly ColorStreamManager colorStreamManager = new ColorStreamManager();          // カメラ画像の描画サポート
        readonly GestureDetector rightHandGestureRecognizer = new SwipeGestureDetector();   // ジェスチャーの検出器
        readonly BarycenterHelper barycenterHelper = new BarycenterHelper(30);              // 重心
        readonly PostureDetector postureRecognizer = new PostureDetector();                 // ポーズの検出器

        SkeletonDisplayManager skeletonDisplayManager;                                      // ジェスチャーの描画サポート


        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // Init Kinect
            try
            {
                kinectRuntime = new Runtime();
                kinectRuntime.Initialize(RuntimeOptions.UseDepthAndPlayerIndex | RuntimeOptions.UseSkeletalTracking | RuntimeOptions.UseColor);
                kinectRuntime.VideoStream.Open(ImageStreamType.Video, 2, ImageResolution.Resolution640x480, ImageType.Color);
                kinectRuntime.DepthStream.Open(ImageStreamType.Depth, 2, ImageResolution.Resolution320x240, ImageType.DepthAndPlayerIndex);

                kinectRuntime.SkeletonFrameReady += kinectRuntime_SkeletonFrameReady;
                kinectRuntime.VideoFrameReady += kinectRuntime_ColorFrameReady;

                kinectRuntime.SkeletonEngine.TransformSmooth = true;

                var parameters = new TransformSmoothParameters
                {
                    Smoothing = 0.7f,
                    Correction = 0.3f,
                    Prediction = 0.4f,
                    JitterRadius = 1.0f,
                    MaxDeviationRadius = 0.5f
                };
                kinectRuntime.SkeletonEngine.SmoothParameters = parameters;

                skeletonDisplayManager = new SkeletonDisplayManager(kinectRuntime.SkeletonEngine, skeletonCanvas);

                // Events
                rightHandGestureRecognizer.OnGestureDetected += rightHandGestureRecognizer_OnGestureDetected;
            }
            catch(Exception ex )
            {
                MessageBox.Show(ex.Message);
                Close();
                kinectRuntime = null;
            }
        }

        void rightHandGestureRecognizer_OnGestureDetected(SupportedGesture gesture)
        {
            switch (gesture)
            {
                case SupportedGesture.SwipeToLeft:
                    Gesture.Content = "GestureDetected:SwipeToLeft";
                    break;
                case SupportedGesture.SwipeToRight:
                    Gesture.Content = "GestureDetected:SwipeToRight";
                    break;
            }
        }

        void kinectRuntime_ColorFrameReady(object sender, ImageFrameReadyEventArgs e)
        {
            // カメラ画像の描画
            ColorImage.Source = colorStreamManager.Update(e);
        }

        void kinectRuntime_SkeletonFrameReady(object sender, SkeletonFrameReadyEventArgs e)
        {
            SkeletonFrame skeletonFrame = e.SkeletonFrame;

            foreach (SkeletonData data in skeletonFrame.Skeletons)
            {
                if (data.TrackingState != SkeletonTrackingState.Tracked)
                    continue;

                barycenterHelper.Add(data.Position.ToVector3(),data.TrackingID);
                if (!barycenterHelper.IsStable(data.TrackingID))
                    continue;

                foreach (Joint joint in data.Joints)
                {
                    if (joint.Position.W < 0.8f || joint.TrackingState != JointTrackingState.Tracked)
                        continue;

                    if (joint.ID == JointID.HandRight)
                    {
                        rightHandGestureRecognizer.Add(joint.Position, kinectRuntime.SkeletonEngine);
                        Trace.WriteLine(Gesture.Content = "HandRight.Add");
                    }
                }

                // ポーズの検出
                postureRecognizer.TrackPostures(data);
            }

            // スケルトンの描画
            skeletonDisplayManager.Draw(e.SkeletonFrame);

            labelPose.Content = "Pose: " + postureRecognizer.CurrentPosture.ToString();
        }
    }
}
