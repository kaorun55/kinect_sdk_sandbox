/** @file
 * @brief 同期オブジェクト・イベントの定義
 *
 * @author 中村  薫
 * $Date: 2004/10/21 11:25:09 $
 * $Revision: 1.1.1.1 $
 */
#include "win32/Event.h"
#include "win32/Win32Exception.h"

namespace win32 {
    Event::Event( LPCTSTR lpName, BOOL bManualReset /*= TRUE*/, BOOL bInitialState /*= FALSE*/,
                    LPSECURITY_ATTRIBUTES lpEventAttributes /*= 0*/ )
    {
        HANDLE handle = ::CreateEvent( lpEventAttributes, bManualReset, bInitialState, lpName );
        if ( handle == 0 ) {
			Throw_Win32Exception();
        }

        handle_ = Handle( handle, &::CloseHandle );

        // すでに作成済みのイベントかどうか
        exists_ = GetLastError() == ERROR_ALREADY_EXISTS;
    }

    Event::~Event()
    {
    }

    bool Event::Wait( DWORD dwMilliseconds /*= INFINITE*/ )
    {
        // データの更新を待つ
        DWORD ret = ::WaitForSingleObject( handle_.get(), dwMilliseconds );
        if ( (ret == WAIT_FAILED) || (ret == WAIT_ABANDONED) ) {
			Throw_Win32Exception();
        }

        // シグナル状態になったか、タイムアウトしたか
        return ret == WAIT_OBJECT_0;
    }
}
