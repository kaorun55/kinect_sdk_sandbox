// GetKinectDeviceInfoのサンプル
#include <iostream>

// MSRKinectAudio.hの前にWindows.hをインクルードする
#include <Windows.h>
#include <MSRKinectAudio.h>

#include "win32\DllLoader.h"

typedef HRESULT (WINAPI *GETKINECTDEVICEINFO)(PKINECT_AUDIO_INFO pDeviceInfo, int *piDeviceCount);

void main()
{
    try {
        HRESULT hr = ::CoInitialize( 0 );
        if ( hr != S_OK ) {
            throw win32::Win32Exception( hr );
        }

        // インポートライブラリがないので、DLLから直にロードする
        win32::DllLoader dll( "MSRKinectAudio.dll" );
        GETKINECTDEVICEINFO GetKinectDeviceInfo = dll.GetProcAddress<GETKINECTDEVICEINFO>( "GetKinectDeviceInfo" );

        KINECT_AUDIO_INFO DeviceInfo = { 0 };
        int DeviceCount = 0;
        hr = GetKinectDeviceInfo( &DeviceInfo, &DeviceCount );
        if ( hr != S_OK ) {
            throw win32::Win32Exception( hr );
        }

        std::wcout << "DeviceCount:" << DeviceCount << " " <<
                     "DeviceIndex:" << DeviceInfo.iDeviceIndex << " " <<
                     "DeviceName :" << DeviceInfo.szDeviceName << " " <<
                     "DeviceID   :" << DeviceInfo.szDeviceID << std::endl;
    }
    catch ( std::exception& ex ) {
        std::cout << ex.what() << std::endl;
    }
}
