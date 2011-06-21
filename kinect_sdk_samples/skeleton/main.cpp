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

void DrawSkeletonSegment( IplImage* Skeleton, CvPoint StartPoint, CvPoint EndPoint)
{
    cvLine( Skeleton, StartPoint, EndPoint, cvScalar( 0, 0, 255 ), 4 );
}

void DrawSkeleton( IplImage* Skeleton, kinect::nui::SkeletonData& skeleton )
{
    int width = 640;
    int height = 480;

    int scaleX = width; //scaling up to image coordinates
    int scaleY = height;
    float fx=0,fy=0;

    std::vector< CvPoint > points;
    for ( int i = 0; i < kinect::nui::SkeletonData::POSITION_COUNT; i++)
    {
        kinect::nui::SkeletonData::Point p = skeleton.TransformSkeletonToDepthImage( i );
        points.push_back( cvPoint( (p.x * scaleX) + 0.5f, (p.y * scaleY) + 0.5f ) );
    }

    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_HIP_CENTER], points[NUI_SKELETON_POSITION_SPINE] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_SPINE], points[NUI_SKELETON_POSITION_SHOULDER_CENTER] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_SHOULDER_CENTER], points[NUI_SKELETON_POSITION_HEAD] );

    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_SHOULDER_CENTER], points[NUI_SKELETON_POSITION_SHOULDER_LEFT] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_SHOULDER_LEFT], points[NUI_SKELETON_POSITION_ELBOW_LEFT] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_ELBOW_LEFT], points[NUI_SKELETON_POSITION_WRIST_LEFT] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_WRIST_LEFT], points[NUI_SKELETON_POSITION_HAND_LEFT] );

    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_SHOULDER_CENTER], points[NUI_SKELETON_POSITION_SHOULDER_RIGHT] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_SHOULDER_RIGHT], points[NUI_SKELETON_POSITION_ELBOW_RIGHT] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_ELBOW_RIGHT], points[NUI_SKELETON_POSITION_WRIST_RIGHT] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_WRIST_RIGHT], points[NUI_SKELETON_POSITION_HAND_RIGHT] );

    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_HIP_CENTER], points[NUI_SKELETON_POSITION_HIP_LEFT] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_HIP_LEFT], points[NUI_SKELETON_POSITION_KNEE_LEFT] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_KNEE_LEFT], points[NUI_SKELETON_POSITION_ANKLE_LEFT] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_ANKLE_LEFT], points[NUI_SKELETON_POSITION_FOOT_LEFT] );

    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_HIP_CENTER], points[NUI_SKELETON_POSITION_HIP_RIGHT] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_HIP_RIGHT], points[NUI_SKELETON_POSITION_KNEE_RIGHT] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_KNEE_RIGHT], points[NUI_SKELETON_POSITION_ANKLE_RIGHT] );
    DrawSkeletonSegment(Skeleton, points[NUI_SKELETON_POSITION_ANKLE_RIGHT], points[NUI_SKELETON_POSITION_FOOT_RIGHT] );

    // Draw the joints in a different color
    for ( int i = 0; i < kinect::nui::SkeletonData::POSITION_COUNT; ++i ) {
        cvCircle(Skeleton, points[i], 5,  cvScalar( 255, 0, 0 ), -1 );
    }
}

void main()
{
    try {
        kinect::nui::Kinect kinect;
        kinect.Initialize( NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON );

        kinect::nui::ImageStream& video = kinect.VideoStream();
        video.Open( NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480 );

        kinect::nui::ImageStream& depth = kinect.DepthStream();
        depth.Open( NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_320x240 );

        kinect::nui::SkeletonEngine& skeleton = kinect.Skeleton();
        skeleton.Enable();

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
                    int player = depthMD( x / 2, y / 2 ) & 0x7;
                    int depth = depthMD( x / 2, y / 2 ) >> 3;

                    // 1.5m以内でプレーヤがいなかった場合は白くする
                    // ちらかった家の対策
                    if ( (depth >= 1500) && (player == 0) ) {
                        *img = 0xFFFFFFFF;
                    }
                    // そうでなければ描画する
                    else {
                        *img = videoMD( x, y ) & colors[player];
                    }
                }
            }

            kinect::nui::SkeletonFrame skeletonMD = skeleton.GetNextFrame();
            if ( skeletonMD.IsFoundSkeleton() ) {
                skeletonMD.TransformSmooth();

                for ( int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i ) {
                    if ( skeletonMD[i].TrackingState() == NUI_SKELETON_TRACKED ) {
                        DrawSkeleton( playerImg, skeletonMD[i] );
                    }
                }
            }

            ::cvShowImage( windowName, playerImg );

            // 1画面分の書込
            std::stringstream ss;
            ss << "jpg\\" << ::GetTickCount() << ".jpg";
            cvSaveImage( ss.str().c_str(), playerImg );
     
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
