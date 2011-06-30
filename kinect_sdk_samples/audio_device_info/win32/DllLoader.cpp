/** @file
 * @brief DLL動的ロードサポートクラスの定義
 *
 * @author 中村  薫
 * $Date: 2004/11/20 05:21:49 $
 * $Revision: 1.2 $
 */
#include "DllLoader.h"
#include "Win32Exception.h"

/// Win32ラッパークラス群
namespace win32
{

/**
 *コンストラクタ
 */
DllLoader::DllLoader()
    : dll_()
{
}

/**
 * コンストラクタ
 *
 * @param dllName DLLファイル名
 */
DllLoader::DllLoader( const win32::tstring& dllName )
    : dll_()
{
    // DLLを開く
    Open( dllName );
}

/**
 * デストラクタ
 */
DllLoader::~DllLoader()
{
    Close();
}

/**
 * DLLのオープン
 *
 * @param dllName DLLファイル名
 */
// DLLのオープン
void DllLoader::Open( const win32::tstring& dllName )
{
	dll_ = std::shared_ptr< HINSTANCE__ >( ::LoadLibrary( dllName.c_str() ), &::FreeLibrary );
    if( dll_.get() == 0 ) {
		Throw_Win32Exception();
	}
}

/// DLLのクローズ
void DllLoader::Close()
{
    dll_.reset();
}

/// DLLのオープン状態
bool DllLoader::IsOpen() const
{
    return dll_.get() != 0;
}

} // namespace win32
// EOF
