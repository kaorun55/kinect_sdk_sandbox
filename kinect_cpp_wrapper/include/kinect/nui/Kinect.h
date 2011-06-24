#pragma once

#include <memory>

#include <Windows.h>
#include <MSR_NuiApi.h>

#include "kinect\nui\ImageStream.h"
#include "kinect\nui\SkeletonEngine.h"

/// tr1をstdにする
namespace std {
	using namespace std::tr1;
}

/// MS公式SDKの名前空間
namespace kinect {
	/// NUI(カメラ、画像、距離)の名前空間
	namespace nui {
		/// Kinectの全体的な動作
		class Kinect
		{
		public:

			/// インスタンスのポインタ
            typedef std::shared_ptr< INuiInstance > NuiInstance;

			Kinect( int index = 0 );
			~Kinect();

			static int GetActiveCount();

			/**
			 * Kinectの初期化
			 *
			 * @param	dwFlags	初期化フラグ
			 *	@arg	NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX	距離データとユーザーを取得する
			 *	@arg	NUI_INITIALIZE_FLAG_USES_COLOR					カメラ画像を取得する
			 *	@arg	NUI_INITIALIZE_FLAG_USES_SKELETON				骨格追跡を行う
			 *	@arg	NUI_INITIALIZE_FLAG_USES_DEPTH					距離データを取得する
			 */
			void Initialize( DWORD dwFlags );
			void Shutdown();

            void WaitAndUpdateAll();

			void SetAngle( LONG lAngleDegrees );
			LONG GetAngle() const;

			/**
			 * このKinectのインスタンスインデックスを取得する
			 * 
			 * @return 0から始まるインスタンスのインデックス
			 */
            int GetInstanceIndex() const { return index_; }

			/**
			 * このKinectのカメライメージを取得する
			 * 
			 * @return ImageStreamの参照
			 */
            ImageStream& VideoStream() { return video_; }

			/**
			 * このKinectの距離イメージを取得する
			 * 
			 * @return ImageStreamの参照
			 */
			ImageStream& DepthStream() { return depth_; }

			/**
			 * このKinectのスケルトンエンジンを取得する
			 * 
			 * @return SkeletonEngineの参照
			 */
			SkeletonEngine& Skeleton() { return skeleton_; }

            static const LONG CAMERA_ELEVATION_MAXIMUM = NUI_CAMERA_ELEVATION_MAXIMUM;	///< カメラの首ふり最大値
            static const LONG CAMERA_ELEVATION_MINIMUM = NUI_CAMERA_ELEVATION_MINIMUM;	///< カメラの首ふり最小値

        private:

            NuiInstance Create( int index );

		private:

			NuiInstance instance_;			///< インスタンス

            ImageStream video_;				///< カメライメージ
            ImageStream depth_;				///< 距離イメージ
            SkeletonEngine  skeleton_;		///< 骨格トラッキング

            int index_;						///< このKinectのインデックス
		};
	}
}
