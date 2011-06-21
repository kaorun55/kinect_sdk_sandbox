// 中心の距離を表示するサンプル
#include <iostream>
#include <sstream>

#include "kinect\nui\Kinect.h"
#include "kinect\nui\ImageFrame.h"

#include <opencv2/opencv.hpp>

void main()
{
    try {
        kinect::nui::Kinect kinect;
        kinect.Initialize( NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH );

        kinect::nui::ImageStream& video = kinect.VideoStream();
        video.Open( NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480 );

        kinect::nui::ImageStream& depth = kinect.DepthStream();
        depth.Open( NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480 );

        // OpenCVの初期設定
        char* windowName = "depth";
        ::cvNamedWindow( windowName );
        cv::Ptr< IplImage > videoImg = ::cvCreateImage( cvSize(video.Width(), video.Height()), IPL_DEPTH_8U, 4 );

        // 描画用フォントの作成
        CvFont font;
        ::cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 10);

        while ( 1 ) {
            // データの更新を待つ
            kinect.WaitAndUpdateAll();

            // 次のフレームのデータを取得する(OpenNIっぽく)
            kinect::nui::ImageFrame imageMD( video );
            kinect::nui::DepthFrame depthMD( depth );

            // データのコピーと表示
            memcpy( videoImg->imageData, (BYTE*)imageMD.Bits(), videoImg->widthStep * videoImg->height );

            // 中心にある物体までの距離を測って表示する
            CvPoint point = cvPoint( depthMD.Width() / 2, depthMD.Height() / 2);
            ::cvCircle( videoImg, point, 10, cvScalar(0, 0, 0), -1 );

            std::stringstream ss;
            ss << depthMD( point.x, point.y ) << "mm";
            ::cvPutText( videoImg, ss.str().c_str(), point, &font, cvScalar(0, 0, 255) );

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
