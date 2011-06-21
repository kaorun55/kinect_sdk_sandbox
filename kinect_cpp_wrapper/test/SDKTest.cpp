#include "gtest\gtest.h"

#include <Windows.h>
#include <MSR_NuiApi.h>

TEST( SDKTest, NuiGetPropsType )
{
	MsrNui::NUI_PROPSTYPE actual = ::MSR_NuiGetPropsType( MsrNui::_NUI_PROPSINDEX::INDEX_UNIQUE_DEVICE_NAME );
	EXPECT_EQ( MsrNui::NUI_PROPSTYPE::PROPTYPE_BSTR, actual );
}

TEST( SDKTest, NuiGetPropsBlob )
{
	BSTR deviceName = 0;
	DWORD dwInOutSize = 0;

	bool condition = ::MSR_NuiGetPropsBlob( MsrNui::_NUI_PROPSINDEX::INDEX_UNIQUE_DEVICE_NAME, &deviceName, &dwInOutSize );
	SysFreeString( deviceName );

	EXPECT_TRUE( condition );
}
