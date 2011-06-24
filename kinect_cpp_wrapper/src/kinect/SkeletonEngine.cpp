#include "kinect\nui\SkeletonEngine.h"

#include "win32/Win32Exception.h"

namespace kinect {
	namespace nui {
        SkeletonEngine::SkeletonEngine( std::shared_ptr< INuiInstance >& instance )
            : instance_( instance )
            , event_( 0 )
            , isEnabled_( false )
        {
        }

        SkeletonEngine::~SkeletonEngine()
        {
        }

        void SkeletonEngine::Enable( DWORD dwFlags /*= 0*/ )
        {
            HRESULT ret = instance_->NuiSkeletonTrackingEnable( event_.get(), dwFlags );
			if ( ret != S_OK ) {
				throw win32::Win32Exception( ret );
			}

            isEnabled_ = true;
        }

        void SkeletonEngine::Disable()
        {
            HRESULT ret = instance_->NuiSkeletonTrackingDisable();
			if ( ret != S_OK ) {
				throw win32::Win32Exception( ret );
			}

            isEnabled_ = false;
        }
 
        SkeletonFrame SkeletonEngine::GetNextFrame( DWORD dwMillisecondsToWait /*= 0*/ )
        {
            return SkeletonFrame( instance_, dwMillisecondsToWait );
        }
    }
}