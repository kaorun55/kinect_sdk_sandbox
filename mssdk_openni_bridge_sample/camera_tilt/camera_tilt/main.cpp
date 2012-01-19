#include <iostream>
#include <stdexcept>

// MSR_NuiApi.hの前にWindows.hをインクルードする
#include <Windows.h>
#include <MSR_NuiApi.h>

#include <opencv2/opencv.hpp>

#include <XnCppWrapper.h>

// 設定ファイルのパス(環境に合わせて変更してください)
const char* CONFIG_XML_PATH = "SamplesConfig.xml";

int main (int argc, char * argv[])
{
    IplImage* camera = 0;

    try {
        // MS公式SDKを初期化する
        HRESULT ret = ::NuiInitialize( NUI_INITIALIZE_FLAG_USES_COLOR );
        if ( ret != S_OK ) {
            throw std::runtime_error("NuiInitialize failed");
        }
 
        // OpenNIを初期化する
        xn::Context context;
        XnStatus rc = context.InitFromXmlFile(CONFIG_XML_PATH);
        if (rc != XN_STATUS_OK) {
            throw std::runtime_error(xnGetStatusString(rc));
        }

        // OpenNIのイメージジェネレータを作成する
        xn::ImageGenerator image;
        rc = context.FindExistingNode(XN_NODE_TYPE_IMAGE, image);
        if (rc != XN_STATUS_OK) {
            throw std::runtime_error(xnGetStatusString(rc));
        }

        // カメラサイズのイメージを作成(8bitのRGB)
        XnMapOutputMode outputMode;
        image.GetMapOutputMode(outputMode);
        camera = ::cvCreateImage(cvSize(outputMode.nXRes, outputMode.nYRes),
            IPL_DEPTH_8U, 3);
        if (!camera) {
            throw std::runtime_error("error : cvCreateImage");
        }

        // 起動からの時間
        DWORD tick = ::GetTickCount();

        // メインループ
        while (1) {
            // OpenNIでカメライメージの更新を待ち、画像データを取得する
            context.WaitOneUpdateAll(image);
            xn::ImageMetaData imageMD;
            image.GetMetaData(imageMD);

            // カメラ画像の表示
            //  Kinectからの入力がRGBであるため、BGRに変換して表示する
            memcpy(camera->imageData, imageMD.RGB24Data(), camera->imageSize);
            ::cvCvtColor(camera, camera, CV_RGB2BGR);
            ::cvShowImage("KinectImage", camera);

            // キーの取得
            char key = cvWaitKey(10);
            // 終了する
            if (key == 'q') {
                break;
            }

            // 1秒に一回、公式SDKでKinectの首を動かす
            if ( (::GetTickCount() - tick) > 1000 ) {
                tick =::GetTickCount();

                LONG angle = 0;
                ::NuiCameraElevationGetAngle( &angle );
                if ( angle < NUI_CAMERA_ELEVATION_MAXIMUM ) {
                    ::NuiCameraElevationSetAngle( angle + 5 );
                }
                else {
                    ::NuiCameraElevationSetAngle( 0 );
                }
            }
        }

        ::NuiCameraElevationSetAngle( 0 );
        ::NuiShutdown();
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }

    ::cvReleaseImage(&camera);

    return 0;
}
