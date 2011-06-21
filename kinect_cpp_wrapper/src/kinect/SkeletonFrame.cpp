#include "kinect\nui\SkeletonFrame.h"
#include "kinect\nui\SkeletonEngine.h"

#include "win32/Win32Exception.h"

namespace kinect {
	namespace nui {
        SkeletonFrame::SkeletonFrame( std::shared_ptr< INuiInstance >& instance, DWORD dwMillisecondsToWait /*= 0*/ )
            : instance_( instance )
        {
            GetNextFrame( dwMillisecondsToWait );
        }

        SkeletonFrame::~SkeletonFrame()
        {
        }

        void SkeletonFrame::TransformSmooth( const NUI_TRANSFORM_SMOOTH_PARAMETERS *pSmoothingParams /*= 0*/ )
        {
            HRESULT ret = instance_->NuiTransformSmooth( &skeletonFrame_, pSmoothingParams );
			if ( ret != S_OK ) {
				throw win32::Win32Exception( ret );
			}
        }

        bool SkeletonFrame::IsFoundSkeleton() const
        {
            for( int i = 0; i < NUI_SKELETON_COUNT; ++i ) {
                if( skeletonFrame_.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED ) {
                    return true;
                }
            }

            return false;
        }
 
        void SkeletonFrame::GetNextFrame( DWORD dwMillisecondsToWait /*= 0*/ )
        {
            HRESULT ret = instance_->NuiSkeletonGetNextFrame( dwMillisecondsToWait, &skeletonFrame_ );
			if ( ret != S_OK ) {
				throw win32::Win32Exception( ret );
			}
        }
    }
}
