// カメラ画像を表示するサンプル
#include <iostream>

// MSR_NuiApi.hの前にWindows.hをインクルードする
#include <Windows.h>
#include <MSR_NuiApi.h>

#include <opencv2/opencv.hpp>

#define ERROR_CHECK( ret )  \
    if ( ret != S_OK ) {    \
        std::cout << "failed " #ret " " << ret << std::endl;    \
        exit( 1 );          \
    }


void main()
{
    // 初期化
    ERROR_CHECK( ::NuiInitialize( NUI_INITIALIZE_FLAG_USES_COLOR ) );

    // カメラハンドルの取得
    HANDLE imageEvent = ::CreateEvent( 0, TRUE, FALSE, 0 );
    HANDLE streamHandle = 0;
    NUI_IMAGE_RESOLUTION resolution = NUI_IMAGE_RESOLUTION_640x480;
    ERROR_CHECK( ::NuiImageStreamOpen( NUI_IMAGE_TYPE_COLOR, resolution,
                    0, 2, imageEvent, &streamHandle ) );

    // 画面サイズを取得
    DWORD x = 0, y = 0;
    ::NuiImageResolutionToSize( resolution, x, y );

    // OpenCVの初期設定
    char* windowName = "camera_image";
    ::cvNamedWindow( windowName );
    cv::Ptr< IplImage > videoImg = ::cvCreateImage( cvSize( x, y ), IPL_DEPTH_8U, 4 );

    // メインループ
    while ( 1 ) {
        // データの更新を待つ
        ::WaitForSingleObject( imageEvent, INFINITE );

        // カメラデータの取得
        CONST NUI_IMAGE_FRAME *imageFrame = 0;
        ERROR_CHECK( ::NuiImageStreamGetNextFrame( streamHandle, 0, &imageFrame ) );

        // 画像データの取得
        KINECT_LOCKED_RECT rect;
        imageFrame->pFrameTexture->LockRect( 0, &rect, 0, 0 );

        // データのコピーと表示
        memcpy( videoImg->imageData, (BYTE*)rect.pBits, videoImg->widthStep * videoImg->height );
        ::cvShowImage( windowName, videoImg );

        // カメラデータの解放
        ERROR_CHECK( ::NuiImageStreamReleaseFrame( streamHandle, imageFrame ) );

        int key = ::cvWaitKey( 10 );
        if ( key == 'q' ) {
            break;
        }
    }

    // 終了処理
    NuiShutdown();

    ::cvDestroyAllWindows();
}
