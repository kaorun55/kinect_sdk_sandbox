#pragma once
#include <memory>

#include <Windows.h>
#include <MSR_NuiApi.h>

#include "win32/Event.h"

namespace std {
	using namespace std::tr1;
}

namespace kinect {
	namespace nui {
        class Kinect;

        class ImageStream
        {
            friend class Kinect;

        public:
            ~ImageStream();

            void Open( NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eResolution, DWORD dwImageFrameFlags_NotUsed = 0,
								DWORD dwFrameLimit = 2 );

    		const NUI_IMAGE_FRAME* GetNextFrame( DWORD dwMillisecondsToWait = 0 );
            void ReleaseFrame( CONST NUI_IMAGE_FRAME *pImageFrame );

            bool Wait( DWORD dwMilliseconds = INFINITE ) { return event_.Wait( dwMilliseconds ); }

            UINT Width() const { return width_; }
            UINT Height() const { return height_; }

        private:

            ImageStream( std::shared_ptr< INuiInstance >& instance );

            ImageStream( const ImageStream& rhs );
            ImageStream& operator = ( const ImageStream& rhs );

        private:

            std::shared_ptr< INuiInstance > instance_;
            HANDLE hStream_;
            win32::Event    event_;

            UINT    width_;
            UINT    height_;
        };

    }
}
