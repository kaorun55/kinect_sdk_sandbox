// 複数のKinectのカメラ画像を表示する
#include <iostream>
#include <sstream>
#include <vector>

// MSR_NuiApi.hの前にWindows.hをインクルードする
#include <Windows.h>
#include <MSR_NuiApi.h>

#include <opencv2/opencv.hpp>

#define ERROR_CHECK( ret )  \
    if ( ret != S_OK ) {    \
        std::cout << "failed " #ret " " << ret << std::endl;    \
        exit( 1 );          \
    }

struct Runtime
{
    INuiInstance*       kinect;         // Kinectのインスタンス
    HANDLE              imageEvent;     // データの更新イベントハンドル
    HANDLE              streamHandle;   // 画像データのハンドル
    std::string         windowName;     // 表示するウィンドウの名前
    cv::Ptr< IplImage > image;          // 表示データ
};

void main()
{
    try {
        const NUI_IMAGE_RESOLUTION resolution = NUI_IMAGE_RESOLUTION_640x480;

        // アクティブなKinectの数を取得する
        int kinectCount = 0;
        ERROR_CHECK( ::MSR_NUIGetDeviceCount( &kinectCount ) );

        // Kinectのインスタンスを生成する
        typedef std::vector< Runtime > Runtimes;
        Runtimes runtime( kinectCount );
        for ( int i = 0; i < runtime.size(); ++i ) {
            // Kinectのインスタンス生成と初期化
            ERROR_CHECK( ::MSR_NuiCreateInstanceByIndex( i, &runtime[i].kinect ) );

            runtime[i].kinect->NuiInitialize( NUI_INITIALIZE_FLAG_USES_COLOR );

            runtime[i].imageEvent = ::CreateEvent( 0, TRUE, FALSE, 0 );
            ERROR_CHECK( runtime[i].kinect->NuiImageStreamOpen( NUI_IMAGE_TYPE_COLOR, resolution,
                            0, 2, runtime[i].imageEvent, &runtime[i].streamHandle ) );
 
            // ウィンドウ名を作成
            std::stringstream ss;
            ss << "malti_kinect " << (i + 1);
            runtime[i].windowName = ss.str();

            // 画面サイズを取得
            DWORD x = 0, y = 0;
            ::NuiImageResolutionToSize( resolution, x, y );

            // OpenCVの初期設定
            runtime[i].image = ::cvCreateImage( cvSize( x, y ), IPL_DEPTH_8U, 4 );
            ::cvNamedWindow( runtime[i].windowName.c_str() );
        }

        bool continue_ = true;
        while ( continue_ ) {
            for ( Runtimes::iterator it = runtime.begin(); it != runtime.end(); ++it ) {
                // データの更新を待つ
                ::WaitForSingleObject( it->imageEvent, INFINITE );

                // カメラデータの取得
                CONST NUI_IMAGE_FRAME *imageFrame = 0;
                ERROR_CHECK( it->kinect->NuiImageStreamGetNextFrame( it->streamHandle, 0, &imageFrame ) );

                // 画像データの取得
                KINECT_LOCKED_RECT rect;
                imageFrame->pFrameTexture->LockRect( 0, &rect, 0, 0 );

                // データのコピーと表示
                memcpy( it->image->imageData, (BYTE*)rect.pBits, it->image->widthStep * it->image->height );
                ::cvShowImage( it->windowName.c_str(), it->image );

                // カメラデータの解放
                ERROR_CHECK( it->kinect->NuiImageStreamReleaseFrame( it->streamHandle, imageFrame ) );

                int key = ::cvWaitKey( 10 );
                if ( key == 'q' ) {
                    continue_ = false;
                }
            }
        }

        // 終了処理
        for ( Runtimes::iterator it = runtime.begin(); it != runtime.end(); ++it ) {
            it->kinect->NuiShutdown();
        }

        ::cvDestroyAllWindows();
    }
    catch ( std::exception& ex ) {
        std::cout << ex.what() << std::endl;
    }
}
