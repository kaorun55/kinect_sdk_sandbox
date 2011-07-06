#pragma once
#include <memory>
#include <tuple>

#include <Windows.h>
#include <MSR_NuiApi.h>

namespace std {
	using namespace std::tr1;
}

namespace kinect {
	namespace nui {
        class SkeletonFrame;

        class SkeletonData
        {
            friend class SkeletonFrame;

        public:

            struct Point {
                float   x;
                float   y;
                USHORT  depth;

                Point() : x( 0 ), y( 0 ) {}
            };

            static const int POSITION_COUNT = NUI_SKELETON_POSITION_COUNT;

            Point TransformSkeletonToDepthImage( int index )
            {
                Point p;
                NuiTransformSkeletonToDepthImageF( skeletonData_.SkeletonPositions[index], &p.x, &p.y, &p.depth );
                return p;
            }


            NUI_SKELETON_TRACKING_STATE TrackingState() const { return skeletonData_.eTrackingState; }

			Vector4 operator [] ( int index )
			{
				return skeletonData_.SkeletonPositions[index];
			}

            DWORD GetUserIndex() const { return skeletonData_.dwUserIndex; }

        private:

            SkeletonData( NUI_SKELETON_DATA& skeletonData )
                : skeletonData_( skeletonData )
            {
            }

        private:

            NUI_SKELETON_DATA&   skeletonData_;
        };


        class SkeletonEngine;

        class SkeletonFrame
        {
            friend class SkeletonEngine;

        public:

            static const int SKELETON_COUNT = NUI_SKELETON_COUNT;

            virtual ~SkeletonFrame();

            void TransformSmooth( const NUI_TRANSFORM_SMOOTH_PARAMETERS *pSmoothingParams = 0 );

            bool IsFoundSkeleton() const;

            SkeletonData operator [] ( int index ) { return SkeletonData( skeletonFrame_.SkeletonData[index] ); }

        private:

            SkeletonFrame( std::shared_ptr< INuiInstance >& instance, DWORD dwMillisecondsToWait = 0 );

            void GetNextFrame( DWORD dwMillisecondsToWait = 0 );

        protected:

            std::shared_ptr< INuiInstance > instance_;

            NUI_SKELETON_FRAME skeletonFrame_;
        };
    }
}
