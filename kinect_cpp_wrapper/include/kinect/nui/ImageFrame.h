#pragma once
#include <memory>

#include <Windows.h>
#include <MSR_NuiApi.h>

namespace std {
	using namespace std::tr1;
}

namespace kinect {
	namespace nui {
        class ImageStream;

        class ImageFrame
        {
        public:

            ImageFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait = 0 );
            virtual ~ImageFrame();

            INT Pitch(){ return lockedRect_.Pitch; };
            void* Bits() { return lockedRect_.pBits; };

            UINT Width() const;
            UINT Height() const;

        protected:

            ImageStream& imageStream_;

            const NUI_IMAGE_FRAME *imageFrame_;
            KINECT_LOCKED_RECT lockedRect_;
        };

        class VideoFrame : public ImageFrame
        {
        public:

            VideoFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait = 0 );
            virtual ~VideoFrame();

            UINT operator () ( UINT x, UINT y );
        };

        class DepthFrame : public ImageFrame
        {
        public:
            
            DepthFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait = 0 );
            virtual ~DepthFrame();

            USHORT operator () ( UINT x, UINT y );
        };
    }
}
