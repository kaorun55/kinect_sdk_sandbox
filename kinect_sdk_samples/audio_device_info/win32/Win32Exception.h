/** @file
 * @brief Win32API例外クラスの宣言
 *
 * @author 中村  薫
 * $Date: 2004/11/20 05:21:49 $
 * $Revision: 1.3 $
 */
#if !defined( WINEXCEPTION_H_INCLUDE )
#define WINEXCEPTION_H_INCLUDE

// MFC使用時用にはwindows.hはインクルードしない
#if !defined( __AFXWIN_H__ )
 #include <windows.h>
#endif  // !defined( __AFXWIN_H__ )

#include <string>
#include <stdexcept>

/// Win32ラッパークラス群
namespace win32
{
#define Throw_Win32Exception() \
	throw win32::Win32Exception( ::GetLastError(), __FILE__, __LINE__ )

    /// Win32例外クラス
    class Win32Exception : public std::runtime_error
    {
    public:

        // コンストラクタ
        explicit Win32Exception( DWORD errorCode );
		Win32Exception( DWORD errorCode, const std::string& fileName, int lineNo );

        // デストラクタ
        ~Win32Exception();

        // エラーコードの取得
        DWORD GetErrorCode() const;
        // エラーメッセージの取得
        const std::string& GetErrorMessage() const;

        // エラーメッセージの取得
        const char* what() const;

        // エラーコードからエラーメッセージの取得
        static std::string GetErrorMessage( DWORD errorCode );

    private:

        DWORD       errorCode_;     ///< エラーコード
        std::string errorMessage_;  ///< エラーメッセージ
    };
} // namespace win32

#endif  // !defined( WINEXCEPTION_H_INCLUDE )
// EOF
