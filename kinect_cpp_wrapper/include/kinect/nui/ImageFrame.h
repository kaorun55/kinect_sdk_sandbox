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

		/// 1フレームのデータ
        class ImageFrame
        {
        public:

            ImageFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait = 0 );
            virtual ~ImageFrame();

			/// 1ラインのバイト数を取得する
            INT Pitch(){ return lockedRect_.Pitch; };

			/// データの先頭アドレスを取得する
            void* Bits() { return lockedRect_.pBits; };

			/// 幅(解像度)を取得する
            UINT Width() const;
			
			/// 高さ(解像度)を取得する
            UINT Height() const;

        protected:

            ImageStream& imageStream_;

            const NUI_IMAGE_FRAME *imageFrame_;	///< フレームデータ
            KINECT_LOCKED_RECT lockedRect_;		///< 矩形データ
        };

		/// カメラ画像の1フレームデータ
        class VideoFrame : public ImageFrame
        {
        public:

            VideoFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait = 0 );
            virtual ~VideoFrame();

			// 指定座標のデータ
            UINT operator () ( UINT x, UINT y );
        };

		/// 距離画像の1フレームデータ
        class DepthFrame : public ImageFrame
        {
        public:
            
            DepthFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait = 0 );
            virtual ~DepthFrame();

			// 指定座標のデータ
            USHORT operator () ( UINT x, UINT y );
        };
    }
}
