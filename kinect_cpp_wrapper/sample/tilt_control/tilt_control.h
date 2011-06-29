
// tilt_control.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// Ctilt_controlApp:
// このクラスの実装については、tilt_control.cpp を参照してください。
//

class Ctilt_controlApp : public CWinApp
{
public:
	Ctilt_controlApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern Ctilt_controlApp theApp;