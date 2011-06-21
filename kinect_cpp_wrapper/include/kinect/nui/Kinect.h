#pragma once

#include <memory>

#include <Windows.h>
#include <MSR_NuiApi.h>

#include "kinect\nui\ImageStream.h"
#include "kinect\nui\SkeletonEngine.h"

namespace std {
	using namespace std::tr1;
}

namespace kinect {
	namespace nui {
		class Kinect
		{
		public:

            typedef std::shared_ptr< INuiInstance > NuiInstance;

			Kinect( int index = 0 );
			~Kinect();

			// アクティブなKinectの数を取得する
			static int GetActiveCount();

			// デバイスの初期化と終了
			void Initialize( DWORD dwFlags );
			void Shutdown();

            void WaitAndUpdateAll();

			void SetAngle( LONG lAngleDegrees );
			LONG GetAngle() const;

            int GetInstanceIndex() const { return index_; }

            ImageStream& VideoStream() { return video_; }
            ImageStream& DepthStream() { return depth_; }
            SkeletonEngine& Skeleton() { return skeleton_; }

            static const LONG CAMERA_ELEVATION_MAXIMUM = NUI_CAMERA_ELEVATION_MAXIMUM;
            static const LONG CAMERA_ELEVATION_MINIMUM = NUI_CAMERA_ELEVATION_MINIMUM;

        private:

            NuiInstance Create( int index );

		private:

			NuiInstance instance_;

            ImageStream video_;
            ImageStream depth_;
            SkeletonEngine  skeleton_;

            int index_;
		};
	}
}
