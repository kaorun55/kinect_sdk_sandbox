#include <vector>

#include "kinect/nui/Kinect.h"

#include "win32/Win32Exception.h"

namespace kinect {
	namespace nui {
		/**
		 * コンストラクタ
		 *
		 * @param	index	生成するKinectインスタンスのインデックス
		 */
		Kinect::Kinect( int index /*= 0*/ )
            : instance_( Create( index ) )
            , video_( instance_ )
            , depth_( instance_ )
            , skeleton_( instance_ )
            , index_( index )
		{
		}

		/**
		 * デストラクタ
		 */
		Kinect::~Kinect()
		{
			Shutdown();
		}

		/**
		 * インスタンスを生成する
		 *
		 * @param	index	生成するKinectインスタンスのインデックス
		 */
        Kinect::NuiInstance Kinect::Create( int index )
        {
			INuiInstance* instance = 0;
			HRESULT ret = ::MSR_NuiCreateInstanceByIndex( index, &instance );
			if ( ret != S_OK ) {
				throw win32::Win32Exception( ret );
			}

			return NuiInstance( instance, ::MSR_NuiDestroyInstance );
        }

		/**
		 * アクティブなKinectの数を取得する
		 *
		 * @return	アクティブなKinectの数
		 */
		/*static*/ int Kinect::GetActiveCount()
		{
			int count = 0;
			HRESULT ret = ::MSR_NUIGetDeviceCount( &count );
			if ( ret != S_OK ) {
				throw win32::Win32Exception( ret );
			}

			return count;
		}

		/**
		 * Kinectの初期化
		 *
		 * @param	dwFlags	初期化フラグ
		 *	@arg	NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX	距離とユーザーを取得する
		 *	@arg	NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX	距離とユーザーを取得する
		 *	@arg	NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX	距離とユーザーを取得する
		 *	@arg	NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX	距離とユーザーを取得する
		 */
		void Kinect::Initialize( DWORD dwFlags )
		{
			HRESULT ret = instance_->NuiInitialize( dwFlags );
			if ( ret != S_OK ) {
				throw win32::Win32Exception( ret );
			}
		}

		void Kinect::Shutdown()
		{
			instance_->NuiShutdown();
		}

        void Kinect::WaitAndUpdateAll()
        {
            std::vector< HANDLE >   handle;
            if ( VideoStream().hStream_ != 0 ) {
                handle.push_back( VideoStream().event_.get() );
            }

            if ( DepthStream().hStream_ != 0 ) {
                handle.push_back( DepthStream().event_.get() );
            }

            if ( Skeleton().IsEnabled() ) {
                handle.push_back( Skeleton().event_.get() );
            }

            DWORD ret = ::WaitForMultipleObjects( handle.size(), &handle[0], TRUE, INFINITE );
            if ( ret == WAIT_FAILED ) {
                throw win32::Win32Exception( ::GetLastError() );
            }
        }

		void Kinect::SetAngle( LONG lAngleDegrees )
		{
			HRESULT ret = instance_->NuiCameraElevationSetAngle( lAngleDegrees );
			if ( ret != S_OK ) {
				throw win32::Win32Exception( ret );
			}
		}

		LONG Kinect::GetAngle() const
		{
			LONG angle = 0;
			HRESULT ret = instance_->NuiCameraElevationGetAngle( &angle );
			//if ( ret != S_OK ) {
			//	throw win32::Win32Exception( ret );
			//}

			return angle;
		}
	}
}
