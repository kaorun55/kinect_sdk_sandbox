/** @file
 * @brief DLL動的ロードサポートクラスの宣言
 *
 * @author 中村  薫
 * $Date: 2004/11/20 05:21:49 $
 * $Revision: 1.2 $
 */
#if !defined( DLLLOADER_H_INCLUDE )
#define DLLLOADER_H_INCLUDE

// MFC使用時用にはwindows.hはインクルードしない
#if !defined( __AFXWIN_H__ )
 #include <windows.h>
#endif  // !defined( __AFXWIN_H__ )

#include <memory>

namespace std {
	using namespace std::tr1;
}

#include "SharedType.h"
#include "Win32Exception.h"

/// Win32ラッパークラス群
namespace win32
{
	/**
	 *
	 * @brief DLLの動的ロード制御
	 * @note
	 *  DllLoader  dll( "DLL名" );\n
	 *  typedef void (*FUNC)();\n
	 *  FUNC func = 0;\n
	 *    .\n
	 *    .\n
	 *    .\n
	 *\n
	 *  // VC++ 6.0の場合はメンバ関数テンプレートのテンプレート引数での\n
	 *  // 解決ができないので戻したい関数型を引数で明示する。\n
	 *  func = dll.GetProcAddress( "関数名", func );\n
	 *\n
	 *  // 一時オブジェクトで渡してみる。\n
	 *  func = dll.GetProcAddress( "関数名", FUNC() );\n
	 *\n
	 *  // VC++ 7.1(.NET 2003)の場合はテンプレート引数で解決できるので関数型引数は\n
	 *  // 指定しなくてもよい。\n
	 *  func = dll.GetProcAddress< FUNC >( "関数名" );\n
	 */
	class DllLoader
	{
	public:

		// コンストラクタ
		DllLoader();
		DllLoader( const win32::tstring& dllName );

		// デストラクタ
		~DllLoader();
		
		// DLLのオープン
		void Open( const win32::tstring& dllName );

		// DLLのクローズ
		void Close();

		// DLLのオープン状態を取得する
		bool IsOpen() const;

		/**
		 * DLL内の関数のアドレスを取得
		 *
		 * @param lpProcName 関数名
		 *
		 * @return 関数アドレス
		 *
		 * @exception Win32Exception
		 *
		 * @note
		 *  VC++ 6.0の場合メンバ関数テンプレートはクラス宣言内に記述しないと
		 * コンパイルできない。
		 */
		template< typename FuncPtr >
		FuncPtr GetProcAddress( const std::string& procName, FuncPtr = 0 )
		{
			FuncPtr func = reinterpret_cast< FuncPtr >( ::GetProcAddress( dll_.get(), procName.c_str() ) );
			if( func == 0 ) {
				Throw_Win32Exception();
			}

			return func;
		}

	private:

		std::shared_ptr< HINSTANCE__ >    dll_;   ///< DLLハンドル
	};
} // namespace win32

#endif  // !defined( DLLLOADER_H_INCLUDE )
// EOF
