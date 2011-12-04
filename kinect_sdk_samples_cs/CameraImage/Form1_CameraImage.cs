using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
using Microsoft.Research.Kinect.Nui;
using System.Runtime.InteropServices;

namespace CameraImage
{
    // アプリケーション固有の処理を記述
    partial class Form1
    {
        private Runtime runtime;

        // 初期化
        private void xnInitialize()
        {
            // ランタイムの初期化 ... (1)
            runtime = new Runtime();
            runtime.Initialize( RuntimeOptions.UseColor );

            // ビデオ、デプスストリームの作成
            runtime.VideoStream.Open( ImageStreamType.Video, 2, ImageResolution.Resolution640x480, ImageType.Color );
        }

        // 描画
        private void xnDraw()
        {
            // ビデオイメージの更新を待ち、画像データを取得する ... (4)
            var video = runtime.VideoStream.GetNextFrame( 0 );
            if ( video == null ) {
                return;
            }

            // 画像の作成 ... (5)
            lock ( this ) {
                // 書き込み用のビットマップデータを作成(32bit bitmap)
                Rectangle rect = new Rectangle( 0, 0, bitmap.Width, bitmap.Height );
                BitmapData data = bitmap.LockBits( rect, ImageLockMode.WriteOnly,
                                        System.Drawing.Imaging.PixelFormat.Format32bppRgb );
                Marshal.Copy( video.Image.Bits, 0, data.Scan0, video.Image.Bits.Length );
                bitmap.UnlockBits( data );
            }
        }

        // キーイベント
        private void xnKeyDown( Keys key )
        {
        }
    }
}
