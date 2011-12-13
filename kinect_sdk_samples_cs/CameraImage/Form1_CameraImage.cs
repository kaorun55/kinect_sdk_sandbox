using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
using Microsoft.Research.Kinect.Nui;
using System.Runtime.InteropServices;

namespace CameraImage
{
    // ƒAƒvƒŠƒP[ƒVƒ‡ƒ“ŒÅ—L‚Ìˆ—‚ğ‹Lq
    partial class Form1
    {
        private Runtime runtime;

        // ‰Šú‰»
        private void xnInitialize()
        {
<<<<<<< HEAD
            // ãƒ©ãƒ³ã‚¿ã‚¤ãƒ ã®åˆæœŸåŒ– ... (1)
            runtime = Runtime.Kinects[0];
=======
            // ƒ‰ƒ“ƒ^ƒCƒ€‚Ì‰Šú‰» ... (1)
            runtime = new Runtime();
>>>>>>> wpf_sample
            runtime.Initialize( RuntimeOptions.UseColor );

            // ƒrƒfƒIAƒfƒvƒXƒXƒgƒŠ[ƒ€‚Ìì¬
            runtime.VideoStream.Open( ImageStreamType.Video, 2, ImageResolution.Resolution640x480, ImageType.Color );
        }

        // •`‰æ
        private void xnDraw()
        {
            // ƒrƒfƒIƒCƒ[ƒW‚ÌXV‚ğ‘Ò‚¿A‰æ‘œƒf[ƒ^‚ğæ“¾‚·‚é ... (4)
            var video = runtime.VideoStream.GetNextFrame( 0 );
            if ( video == null ) {
                return;
            }

            // ‰æ‘œ‚Ìì¬ ... (5)
            lock ( this ) {
                // ‘‚«‚İ—p‚Ìƒrƒbƒgƒ}ƒbƒvƒf[ƒ^‚ğì¬(32bit bitmap)
                Rectangle rect = new Rectangle( 0, 0, bitmap.Width, bitmap.Height );
                BitmapData data = bitmap.LockBits( rect, ImageLockMode.WriteOnly,
                                        System.Drawing.Imaging.PixelFormat.Format32bppRgb );
                Marshal.Copy( video.Image.Bits, 0, data.Scan0, video.Image.Bits.Length );
                bitmap.UnlockBits( data );
            }
        }

        // ƒL[ƒCƒxƒ“ƒg
        private void xnKeyDown( Keys key )
        {
        }
    }
}
