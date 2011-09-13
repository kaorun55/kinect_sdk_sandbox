using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
using Microsoft.Research.Kinect.Nui;
using System.Runtime.InteropServices;

namespace Skeleton
{
    // アプリケーション固有の処理を記述
    partial class Form1
    {
        private Runtime runtime;

        // 描画に必要なオブジェクト
        private Brush brush = new SolidBrush( Color.Red );
        private Font font = new Font( "ＭＳ ゴシック", 30 );

        // 初期化
        private void xnInitialize()
        {
            // ランタイムの初期化
            runtime = new Runtime();
            runtime.Initialize( RuntimeOptions.UseColor | RuntimeOptions.UseDepthAndPlayerIndex );

            // ビデオ、デプスストリームの作成
            runtime.VideoStream.Open( ImageStreamType.Video, 2, ImageResolution.Resolution640x480, ImageType.Color );
            runtime.DepthStream.Open( ImageStreamType.Depth, 2, ImageResolution.Resolution320x240, ImageType.DepthAndPlayerIndex );
        }

        // 描画
        private void xnDraw()
        {
            // ビデオ、デプスの更新を待ち、データを取得する
            var video = runtime.VideoStream.GetNextFrame( 0 );
            var skeleton = runtime.SkeletonEngine.GetNextFrame( 50 );   // 0だとメモリがどんどん増える
            if ( (video == null) || (skeleton == null) ) {
                return;
            }

            // 画像の作成
            lock ( this ) {
                // 書き込み用のビットマップデータを作成
                Rectangle rect = new Rectangle( 0, 0, bitmap.Width, bitmap.Height );
                BitmapData data = bitmap.LockBits( rect, ImageLockMode.WriteOnly,
                                    System.Drawing.Imaging.PixelFormat.Format32bppRgb );
                Marshal.Copy( video.Image.Bits, 0, data.Scan0, video.Image.Bits.Length );
                bitmap.UnlockBits( data );

                // パーツの座標を取得して描画する
                if ( skeleton == null ) {
                    return;
                }

                // 中心点の距離を表示
                Graphics g = Graphics.FromImage( bitmap );

                foreach ( SkeletonData skeletonData in skeleton.Skeletons ) {
                    if ( skeletonData.TrackingState != SkeletonTrackingState.Tracked ) {
                        continue;
                    }

                    foreach ( Joint joint in skeletonData.Joints ) {
                        Point videoPoint = getVideoPoint( joint );
                        g.FillEllipse( brush, videoPoint.X, videoPoint.Y, 10, 10 );
                    }
                }
            }
        }

        private Point getVideoPoint( Joint joint )
        {
            float depthX = 0, depthY = 0;
            runtime.SkeletonEngine.SkeletonToDepthImage( joint.Position, out depthX, out depthY );
            depthX = Math.Min( depthX * runtime.DepthStream.Width, runtime.DepthStream.Width );
            depthY = Math.Min( depthY * runtime.DepthStream.Height, runtime.DepthStream.Height );

            int videoX = 0, videoY = 0;
            runtime.NuiCamera.GetColorPixelCoordinatesFromDepthPixel( ImageResolution.Resolution640x480,
                new ImageViewArea(), (int)depthX, (int)depthY, 0, out videoX, out videoY );

            return new Point( Math.Min( videoX, runtime.VideoStream.Width ), Math.Min( videoY, runtime.VideoStream.Height ) );
        }

        // キーイベント
        private void xnKeyDown( Keys key )
        {
        }
    }
}
