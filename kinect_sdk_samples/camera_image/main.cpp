// カメラ画像を表示するサンプル
#include <iostream>

// MSR_NuiApi.hの前にWindows.hをインクルードする
#include <Windows.h>
#include <MSR_NuiApi.h>

#include <opencv2/opencv.hpp>

void main()
{
    try {
        // 初期化
        HRESULT ret = ::NuiInitialize( NUI_INITIALIZE_FLAG_USES_COLOR );
        if ( ret != S_OK ) {
            throw std::runtime_error( "NuiInitialize failed" );
        }

        // カメラハンドルの取得
        HANDLE imageEvent = ::CreateEvent( 0, TRUE, FALSE, 0 );
        HANDLE streamHandle = 0;
        NUI_IMAGE_RESOLUTION resolution = NUI_IMAGE_RESOLUTION_640x480;
        ret = ::NuiImageStreamOpen( NUI_IMAGE_TYPE_COLOR, resolution,
                                        0, 2, imageEvent, &streamHandle );
        if ( ret != S_OK ) {
            throw std::runtime_error( "NuiImageStreamOpen failed" );
        }

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
            ::NuiImageStreamGetNextFrame( streamHandle, 0, &imageFrame );

            // 画像データの取得
            KINECT_LOCKED_RECT rect;
            imageFrame->pFrameTexture->LockRect( 0, &rect, 0, 0 );

            // データのコピーと表示
            memcpy( videoImg->imageData, (BYTE*)rect.pBits, videoImg->widthStep * videoImg->height );
            ::cvShowImage( windowName, videoImg );

            // カメラデータの解放
            ::NuiImageStreamReleaseFrame( streamHandle, imageFrame );

            int key = ::cvWaitKey( 10 );
            if ( key == 'q' ) {
                break;
            }
        }
    }
    catch ( std::exception& ex ) {
        std::cout << ex.what() << std::endl;
    }

    // 終了処理
    NuiShutdown();

    ::cvDestroyAllWindows();
}
