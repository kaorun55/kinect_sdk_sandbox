// 中心の距離を表示するサンプル
#include <iostream>
#include <sstream>

#include "kinect\nui\Kinect.h"
#include "kinect\nui\ImageFrame.h"

#include <opencv2/opencv.hpp>

void main()
{
    try {
        // NUI_INITIALIZE_FLAG_USES_DEPTHの表示のさせ方がわからん
        kinect::nui::Kinect kinect;
        kinect.Initialize( NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX );

        kinect::nui::ImageStream& depth = kinect.DepthStream();
        depth.Open( NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240 );

        // OpenCVの初期設定
        char* windowName = "depth_map";
        ::cvNamedWindow( windowName );
        cv::Ptr< IplImage > depthImg = ::cvCreateImage( cvSize(depth.Width(), depth.Height()), IPL_DEPTH_16U, 1 );

        while ( 1 ) {
            // データの更新を待つ
            kinect.WaitAndUpdateAll();

            // 次のフレームのデータを取得する(OpenNIっぽく)
            kinect::nui::DepthFrame depthMD( depth );

            // データのコピーと表示
            memcpy( depthImg->imageData, (BYTE*)depthMD.Bits(), depthImg->widthStep * depthImg->height );

            ::cvShowImage( windowName, depthImg );

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
