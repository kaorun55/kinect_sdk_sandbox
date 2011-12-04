using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
using Microsoft.Research.Kinect.Nui;
using System.Runtime.InteropServices;

namespace User
{
    // アプリケーション固有の処理を記述
    partial class Form1
    {
        private Runtime runtime;

        private Color[] color = new Color[7] {
             Color.Red, Color.Blue, Color.Green, Color.Yellow, Color.Pink,  Color.Black, Color.Gold
        };

        // 初期化
        private void xnInitialize()
        {
            // ランタイムの初期化
            runtime = Runtime.Kinects[0];
            runtime.Initialize( RuntimeOptions.UseColor | RuntimeOptions.UseDepthAndPlayerIndex );

            // ビデオ、デプスストリームの作成
            runtime.VideoStream.Open( ImageStreamType.Video, 2, ImageResolution.Resolution640x480, ImageType.Color );
            runtime.DepthStream.Open( ImageStreamType.Depth, 2, ImageResolution.Resolution320x240, ImageType.DepthAndPlayerIndex );
        }

        // 描画
        private void xnDraw()
        {
            // ビデオ、デプスの更新を待ち、データを取得する
            var video = runtime.VideoStream.GetNextFrame( 50 );
            var depth = runtime.DepthStream.GetNextFrame( 50 );
            if ( (video == null) || (depth == null) ) {
                return;
            }

            // 画像の作成
            lock ( this ) {
                // ピクセルごとのユーザーIDを取得する
                for ( int y = 0; y < depth.Image.Height; y++ ) {
                    for ( int x = 0; x < depth.Image.Width; x++ ) {
                        int index = (x + (y * depth.Image.Width)) * 2;
                        byte byte0 = depth.Image.Bits[index];
                        byte byte1 = depth.Image.Bits[index + 1];

                        // ユーザーIDと距離を取得する
                        int playerIndex = byte0 & 0x7;
                        int distance = byte1 << 5 | byte0 >> 3;
                        if ( playerIndex != 0 ) {
                            // 距離座標をカメラ座標に変換する
                            int videoX = 0, videoY = 0;
                            runtime.NuiCamera.GetColorPixelCoordinatesFromDepthPixel( ImageResolution.Resolution640x480,
                                   new ImageViewArea(), x, y, 0, out videoX, out videoY ); 

                            int videoIndex = (videoX + (videoY * video.Image.Width)) * 4;
                            videoIndex =Math.Min( videoIndex, video.Image.Bits.Length - 4 );
                            video.Image.Bits[videoIndex] = color[playerIndex].B;
                            video.Image.Bits[videoIndex + 1] = color[playerIndex].G;
                            video.Image.Bits[videoIndex + 2] = color[playerIndex].R;
                        }
                    }
                }

                // 書き込み用のビットマップデータを作成
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
