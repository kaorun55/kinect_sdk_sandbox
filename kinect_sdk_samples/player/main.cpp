// 中心の距離を表示するサンプル
#include <iostream>
#include <sstream>

#include "kinect\nui\Kinect.h"
#include "kinect\nui\ImageFrame.h"

#include <opencv2/opencv.hpp>

// ユーザーの色づけ
const UINT colors[] =
{
    0xFFFFFFFF,    // ユーザーなし
    0xFF00FFFF,
    0xFFFF00FF,
    0xFFFFFF00,
    0xFF00FF00,
    0xFF0000FF,
    0xFFFF0000,
};

void main()
{
    try {
        kinect::nui::Kinect kinect;
        kinect.Initialize( NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON );

        kinect::nui::ImageStream& video = kinect.VideoStream();
        video.Open( NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480 );

        kinect::nui::ImageStream& depth = kinect.DepthStream();
        depth.Open( NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240 );

        // OpenCVの初期設定
        char* windowName = "player";
        ::cvNamedWindow( windowName );
        cv::Ptr< IplImage > playerImg = ::cvCreateImage( cvSize(video.Width(), video.Height()), IPL_DEPTH_8U, 4 );

        while ( 1 ) {
            // データの更新を待つ
            kinect.WaitAndUpdateAll();

            // 次のフレームのデータを取得する(OpenNIっぽく)
            kinect::nui::VideoFrame videoMD( video );
            kinect::nui::DepthFrame depthMD( depth );

            // データのコピーと表示
            UINT* img = (UINT*)playerImg->imageData;
            for ( int y = 0; y < videoMD.Height(); ++y ) {
                for ( int x = 0; x < videoMD.Width(); ++x, ++img ) {
                    *img = videoMD( x, y ) & colors[depthMD( x / 2, y / 2 ) & 0x7];
                }
            }

            ::cvShowImage( windowName, playerImg );

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
