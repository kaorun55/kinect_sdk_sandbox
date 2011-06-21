#include "gtest\gtest.h"
#include "kinect\nui\Kinect.h"

//#define ANGLE_TEST

// インスタンスの生成
TEST( KinectTest, CreateInstance )
{
	kinect::nui::Kinect kinect;
}

// アクティブなKinect数の取得
TEST( KinectTest, GetActiveCount )
{
	EXPECT_EQ( 1, kinect::nui::Kinect::GetActiveCount() );
}

// デバイスの初期化
TEST( KinectTest, Initialize )
{
	kinect::nui::Kinect kinect;
	kinect.Initialize( NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX );
}

// 初期化なしのシャットダウン
TEST( KinectTest, NonInitializeShutDown )
{
	kinect::nui::Kinect kinect;
	kinect.Shutdown();
}

// 首ふり
TEST( KinectTest, CameraElevationAngle )
{
	// SetAngle のあとに少し待たないと、GetAngleでエラーになる
	const int SLEEP_TIME = 1000;

#ifdef ANGLE_TEST
	kinect::nui::Kinect kinect;
	kinect.Initialize( NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX );

	// SetとGetの値は多少ずれる
	kinect.SetAngle( NUI_CAMERA_ELEVATION_MAXIMUM );
	::Sleep( SLEEP_TIME );
	EXPECT_EQ( NUI_CAMERA_ELEVATION_MAXIMUM, kinect.GetAngle() );

	kinect.SetAngle( NUI_CAMERA_ELEVATION_MINIMUM );
	::Sleep( SLEEP_TIME );
	EXPECT_EQ( NUI_CAMERA_ELEVATION_MINIMUM, kinect.GetAngle() );

	kinect.SetAngle( 0 );
	::Sleep( SLEEP_TIME );
	EXPECT_EQ( 0, kinect.GetAngle() );
#endif
}
