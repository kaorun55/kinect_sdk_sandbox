#pragma once
#include <memory>

#include <Windows.h>
#include <MSR_NuiApi.h>

#include "SkeletonFrame.h"

#include "win32/Event.h"

namespace std {
	using namespace std::tr1;
}

namespace kinect {
	namespace nui {
        class Kinect;

        class SkeletonEngine
        {
            friend class Kinect;

        public:
            ~SkeletonEngine();

            void Enable( DWORD dwFlags = 0 );
            void Disable();

            SkeletonFrame GetNextFrame( DWORD dwMillisecondsToWait = 0 );

            bool IsEnabled() const { return isEnabled_; }

            bool Wait( DWORD dwMilliseconds = INFINITE ) { return event_.Wait( dwMilliseconds ); }

        private:

            SkeletonEngine( std::shared_ptr< INuiInstance >& instance );

            SkeletonEngine( const SkeletonEngine& rhs );
            SkeletonEngine& operator = ( const SkeletonEngine& rhs );

        private:

            std::shared_ptr< INuiInstance > instance_;
            win32::Event    event_;

            bool isEnabled_;
        };

    }
}
