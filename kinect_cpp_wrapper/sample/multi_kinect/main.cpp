// 複数のKinectのカメラ画像を表示する
#include <iostream>
#include <sstream>
#include <vector>

#include "kinect\nui\kinect.h"
#include "kinect\nui\ImageFrame.h"

#include <opencv2/opencv.hpp>

struct Runtime
{
    std::shared_ptr< kinect::nui::Kinect > kinect;
    std::string         windowName;
    cv::Ptr< IplImage > image;

    Runtime( int index, const std::string& name )
        : kinect( new kinect::nui::Kinect( index ) )
        , windowName( name )
    {
    }
};

void main()
{
    try {
        std::vector< Runtime > runtime;
        for ( int i = 0; i < kinect::nui::Kinect::GetActiveCount(); ++i ) {
            std::stringstream ss;
            ss << "malti_kinect " << (i + 1);
            Runtime instance( i, ss.str().c_str() );

            instance.kinect->Initialize( NUI_INITIALIZE_FLAG_USES_COLOR );
            instance.kinect->VideoStream().Open( NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480 );

            instance.image = ::cvCreateImage( cvSize(instance.kinect->VideoStream().Width(), 
                                                     instance.kinect->VideoStream().Height()),
                                              IPL_DEPTH_8U, 4 );
            ::cvNamedWindow( instance.windowName.c_str() );

            runtime.push_back( instance );
        }

        bool continue_ = true;
        while ( continue_ ) {
            for ( std::vector< Runtime >::iterator it = runtime.begin(); it != runtime.end(); ++it ) {
                // データの更新を待つ
                it->kinect->WaitAndUpdateAll();

                // 次のフレームのデータを取得して表示する
                kinect::nui::VideoFrame videoMD( it->kinect->VideoStream() );
                memcpy( it->image->imageData, (BYTE*)videoMD.Bits(), videoMD.Pitch() * videoMD.Height() );
                ::cvShowImage( it->windowName.c_str(), it->image );

                int key = ::cvWaitKey( 10 );
                if ( key == 'q' ) {
                    continue_ = false;
                }
            }
        }

        ::cvDestroyAllWindows();
    }
    catch ( std::exception& ex ) {
        std::cout << ex.what() << std::endl;
    }
}
