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
        private Brush brush = new SolidBrush( Color.Black );
        private Font font = new Font( "ＭＳ ゴシック", 30 );

        // 初期化
        private void xnInitialize()
        {
            // ランタイムの初期化
            runtime.Initialize( RuntimeOptions.UseColor | RuntimeOptions.UseSkeletalTracking );

            // ビデオストリームの作成
            runtime.VideoStream.Open( ImageStreamType.Video, 2, ImageResolution.Resolution640x480, ImageType.Color );
        }

        // 描画
        private void xnDraw()
        {
            // ビデオ、デプスの更新を待ち、データを取得する
            var video = runtime.VideoStream.GetNextFrame( 0 );
            var skeleton = runtime.SkeletonEngine.GetNextFrame( 0 );

            // 画像の作成
            lock ( this ) {
                // 書き込み用のビットマップデータを作成
                Rectangle rect = new Rectangle( 0, 0, bitmap.Width, bitmap.Height );
                BitmapData data = bitmap.LockBits( rect, ImageLockMode.WriteOnly,
                                    System.Drawing.Imaging.PixelFormat.Format24bppRgb );
                Marshal.Copy( video.Image.Bits, 0, data.Scan0, video.Image.Bits.Length );
                bitmap.UnlockBits( data );

                // パーツの座標を取得して描画する
            }
        }

        // キーイベント
        private void xnKeyDown( Keys key )
        {
        }
    }
}
