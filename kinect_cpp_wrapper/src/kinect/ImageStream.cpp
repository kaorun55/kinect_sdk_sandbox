#include "kinect\nui\ImageStream.h"

#include "win32/Win32Exception.h"

namespace kinect {
	namespace nui {
        ImageStream::ImageStream( std::shared_ptr< INuiInstance >& instance )
            : instance_( instance )
            , hStream_( 0 )
            , event_( 0 )
        {
        }

        ImageStream::~ImageStream()
        {
        }
 
		void ImageStream::Open( NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eResolution, DWORD dwImageFrameFlags_NotUsed /*= 0*/,
								DWORD dwFrameLimit /*= 2*/ )
		{
			HRESULT ret = instance_->NuiImageStreamOpen( eImageType, eResolution, dwImageFrameFlags_NotUsed,
														dwFrameLimit, event_.get(), &hStream_ );
			if ( ret != S_OK ) {
				throw win32::Win32Exception( ret );
			}

            ::NuiImageResolutionToSize( eResolution, (DWORD&)width_, (DWORD&)height_ );
		}

		const NUI_IMAGE_FRAME* ImageStream::GetNextFrame( DWORD dwMillisecondsToWait /*= 0*/ )
		{
			const NUI_IMAGE_FRAME* pImageFrame = 0;
			HRESULT ret = instance_->NuiImageStreamGetNextFrame( hStream_, dwMillisecondsToWait, &pImageFrame );
			if ( ret != S_OK ) {
				throw win32::Win32Exception( ret );
			}

			return pImageFrame;
		}
 
        void ImageStream::ReleaseFrame( CONST NUI_IMAGE_FRAME *pImageFrame )
        {
            HRESULT ret = instance_->NuiImageStreamReleaseFrame( hStream_, pImageFrame );
			if ( ret != S_OK ) {
				throw win32::Win32Exception( ret );
			}
        }
    }
}