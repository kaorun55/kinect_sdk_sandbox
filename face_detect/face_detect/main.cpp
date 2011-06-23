// 顔を検出して、カメラの角度を動かすサンプル
#include <iostream>
#include <sstream>

#include "kinect\nui\Kinect.h"
#include "kinect\nui\ImageFrame.h"

#include <opencv2/opencv.hpp>

const char* FACE_CASCADE_PATH = "C:/OpenCV2.2/data/haarcascades/haarcascade_frontalface_alt.xml";

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
        char* windowName = "camera_elevation";
        ::cvNamedWindow( windowName );
        cv::Ptr< IplImage > videoImg = ::cvCreateImage( cvSize(video.Width(), video.Height()), IPL_DEPTH_8U, 4 );

        // 画像を読み込んでKinectにあわせて変換する
        cv::Ptr< IplImage > face = ::cvLoadImage( "lena.jpg" );
        cv::Ptr< IplImage > faceImg = ::cvCreateImage( cvSize(face->width, face->height), IPL_DEPTH_8U, 4 );
        ::cvCvtColor( face, faceImg, CV_RGB2RGBA ); 

        // 検出器のロード
        CvHaarClassifierCascade* faceCascade = (CvHaarClassifierCascade*)::cvLoad( FACE_CASCADE_PATH, 0, 0, 0 );
        if( !faceCascade ) {
            throw std::runtime_error("error : cvLoad");
        }

        // 顔検出用のストレージ
        CvMemStorage* storage = ::cvCreateMemStorage();
        bool isDetected = true;

        // 前回の検出時間
        DWORD prevTime = ::GetTickCount();

        while ( 1 ) {
            // データの更新を待って、次のフレームを取得する
            kinect.WaitAndUpdateAll();
            kinect::nui::VideoFrame videoMD( video );

            // データのコピー
            memcpy( videoImg->imageData, (BYTE*)videoMD.Bits(), videoImg->widthStep * videoImg->height );

            // 顔の検出
            if (isDetected) {
                // モータを回しすぎるといかんので、1秒に一回
                if ( (::GetTickCount() - prevTime) > 1000 ) {
                    prevTime = ::GetTickCount();

                    // 顔の検出
                    cvClearMemStorage(storage);
                    CvSeq* faces = ::cvHaarDetectObjects( videoImg, faceCascade, storage );
                    for ( int i = 0; i < faces->total; ++i ) {
                        // 検出した座標の取得
                        CvRect rect = *(CvRect*)::cvGetSeqElem( faces, i );

                        // 検出した顔に画像を貼り付ける
                        ::cvSetImageROI( videoImg, rect );
                        cv::Ptr< IplImage >  resizeImg = ::cvCreateImage( cvSize( rect.width, rect.height), faceImg->depth, faceImg->nChannels );
                        ::cvResize( faceImg, resizeImg );
                        ::cvCopy( resizeImg, videoImg );
                        ::cvResetImageROI( videoImg );

                        // 中心点と、表示領域を三分割したときの座標
                        POINT c = { rect.x + (rect.width / 2), rect.y + (rect.height / 2) };
                        RECT place[] = {
                            { 0, 0, video.Width(), video.Height() / 3 },
                            { 0, video.Height() / 3, video.Width(), video.Height() / 3 * 2 },
                            { 0, video.Height() / 3 * 2, video.Width(), video.Height() },
                        };

                        // 顔の位置によって、Kinectの首を動かす
                        // 上1/3にいれば、上に動かす
                        LONG angle = kinect.GetAngle();
                        if ( ::PtInRect( &place[0], c ) ) {
                            angle += 5;
                            if ( angle < kinect.CAMERA_ELEVATION_MAXIMUM ) {
                                kinect.SetAngle( angle );
                            }
                        }
                        // 中1/3にいれば、何もしない
                        else if ( ::PtInRect( &place[1], c ) ) {
                        }
                        // 下1/3にいれば、下に動かす
                        else if ( ::PtInRect( &place[2], c ) ) {
                            angle -= 5;
                            if ( angle > kinect.CAMERA_ELEVATION_MINIMUM ) {
                                kinect.SetAngle( angle );
                            }
                        }
                    }
                }
            }

            ::cvShowImage( windowName, videoImg );

            int key = ::cvWaitKey( 10 );
            if ( key == 'q' ) {
                break;
            }
            else if (key == 'd') {
                isDetected = !isDetected;
            }
        }

        ::cvDestroyAllWindows();
    }
    catch ( std::exception& ex ) {
        std::cout << ex.what() << std::endl;
    }
}
