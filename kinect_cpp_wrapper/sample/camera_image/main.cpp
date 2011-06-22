// OpenCVを使ってカメラ画像を表示するサンプル
#include <iostream>

#include "kinect\nui\Kinect.h"
#include "kinect\nui\ImageFrame.h"

#include <opencv2/opencv.hpp>

void main()
{
    try {
        kinect::nui::Kinect kinect;
        kinect.Initialize( NUI_INITIALIZE_FLAG_USES_COLOR );

        kinect::nui::ImageStream& video = kinect.VideoStream();
        video.Open( NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480 );

        // OpenCVの初期設定
        char* windowName = "camera_image";
        ::cvNamedWindow( windowName );
        cv::Ptr< IplImage > videoImg = ::cvCreateImage( cvSize(video.Width(), video.Height()), IPL_DEPTH_8U, 4 );

        while ( 1 ) {
            // データの更新を待つ
            video.Wait();

            // 次のフレームのイメージデータを取得する
            kinect::nui::ImageFrame image( video );
            if( image.Pitch() == 0 ) {
                std::cout << "Buffer length of received texture is bogus" << std::endl;;
                continue;
            }

            // データのコピーと表示
            memcpy( videoImg->imageData, (BYTE*)image.Bits(), videoImg->widthStep * videoImg->height );

            ::cvShowImage( windowName, videoImg );

            int key = ::cvWaitKey( 10 );
            if ( key == 'q' ) {
                break;
            }
        }

        ::cvDestroyAllWindows();
    }
    catch ( std::exception& ex ) {
        std::cout << ex.what() << std::endl;
    }
}
