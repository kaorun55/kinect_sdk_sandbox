/** @file
 * @brief Win32API例外クラスの定義
 *
 * @author 中村  薫
 * $Date: 2004/11/20 05:21:49 $
 * $Revision: 1.3 $
 */
#include "win32/Win32Exception.h"
#include <sstream>

/// Win32ラッパークラス群
namespace win32
{
	/**
	 * コンストラクタ
	 *
	 * @param dwResult エラーコード
	 */
	Win32Exception::Win32Exception( DWORD errorCode )
		: std::runtime_error( "" )
		, errorCode_( errorCode )
		, errorMessage_( getErrorMessage( errorCode ) )
	{
	}

	Win32Exception::Win32Exception( DWORD errorCode, const std::string& fileName, int lineNo )
		: std::runtime_error( "" )
		, errorCode_( errorCode )
	{
		std::stringstream str;
		str << fileName << "(" << lineNo << "):" << getErrorMessage( errorCode );
		errorMessage_ = str.str();
	}

	/**
	 * デストラクタ
	 */
	Win32Exception::~Win32Exception()
	{
	}

	/**
	 * エラーコードの取得
	 *
	 * @return エラーコード
	 */
	DWORD Win32Exception::getErrorCode() const
	{
		return errorCode_;
	}

	/**
	 * エラーメッセージの取得
	 *
	 * @return エラーメッセージ
	 */
	const std::string& Win32Exception::getErrorMessage() const
	{
		return errorMessage_;
	}

	/**
	 * エラーメッセージの取得
	 *
	 * @return エラーメッセージ
	 */
	const char* Win32Exception::what() const
	{
		return errorMessage_.c_str();
	}

	/**
	 * エラーコードからエラーメッセージの取得(クラス関数)
	 *
	 * @param dwErrCode エラーコード
	 *
	 * @return エラーメッセージ
	 */
	std::string Win32Exception::getErrorMessage( DWORD errorCode )
	{
		// エラーメッセージ用クラス
		class ErrorMessage
		{
		public:

			ErrorMessage() : message_(0) {}
			~ErrorMessage(){ if( message_ != 0 ) ::LocalFree( message_ ); }

			char** operator & (){ return &message_; }
			char* get(){ return message_; }

		private:

			char* message_;
		};

		ErrorMessage message; // メッセージバッファ

		// エラーメッセージの取得
		BOOL ret = ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								0, errorCode, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPTSTR)&message, 0, 0 );

		return ret ? message.get() : "未定義エラー";
	}
}
// EOF
