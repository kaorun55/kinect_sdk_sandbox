// アクティブなKinectの数を取得するサンプル
#include <iostream>

// MSR_NuiApi.hの前にWindows.hをインクルードする
#include <Windows.h>
#include <MSR_NuiApi.h>

void main()
{
	int count = 0;
	::MSR_NUIGetDeviceCount( &count );
	std::cout << count << " kinects" << std::endl;;
}
