#include "kinect\nui\ImageFrame.h"
#include "kinect\nui\ImageStream.h"

#include "win32/Win32Exception.h"

namespace kinect {
	namespace nui {
        ImageFrame::ImageFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait /*= 0*/ )
            : imageStream_( imageStream )
            , imageFrame_( imageStream.GetNextFrame( dwMillisecondsToWait ) )
        {
            imageFrame_->pFrameTexture->LockRect( 0, &lockedRect_, 0, 0 );
        }

        ImageFrame::~ImageFrame()
        {
            imageStream_.ReleaseFrame( imageFrame_ );
        }
 
        UINT ImageFrame::Width() const
        {
            KINECT_SURFACE_DESC desc = { 0 };
            imageFrame_->pFrameTexture->GetLevelDesc( 0, &desc );
            return desc.Width;
        }

        UINT ImageFrame::Height() const
        {
            KINECT_SURFACE_DESC desc = { 0 };
            imageFrame_->pFrameTexture->GetLevelDesc( 0, &desc );
            return desc.Height;
        }
 

        VideoFrame::VideoFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait /*= 0*/ )
            : ImageFrame( imageStream, dwMillisecondsToWait )
        {
        }

        VideoFrame::~VideoFrame()
        {
        }

        UINT VideoFrame::operator () ( UINT x, UINT y )
        {
            UINT* video = (UINT*)lockedRect_.pBits;
            return video[(Width() * y) + x];
        }

    
        DepthFrame::DepthFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait /*= 0*/ )
            : ImageFrame( imageStream, dwMillisecondsToWait )
        {
        }

        DepthFrame::~DepthFrame()
        {
        }

        USHORT DepthFrame::operator () ( UINT x, UINT y )
        {
            USHORT* depth = (USHORT*)lockedRect_.pBits;
            UCHAR* d = (UCHAR*)&depth[(Width() * y) + x];
            return  (USHORT)(d[0] | d[1] << 8 );
        }
    }
}