
// tilt_controlDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "tilt_control.h"
#include "tilt_controlDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ctilt_controlDlg ダイアログ




Ctilt_controlDlg::Ctilt_controlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Ctilt_controlDlg::IDD, pParent)
    , m_angle(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ctilt_controlDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SLIDER_TILT, m_sliderTilt);
    DDX_Slider(pDX, IDC_SLIDER_TILT, m_angle);
}

BEGIN_MESSAGE_MAP(Ctilt_controlDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDOK, &Ctilt_controlDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &Ctilt_controlDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// Ctilt_controlDlg メッセージ ハンドラー

BOOL Ctilt_controlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	ShowWindow(SW_MINIMIZE);

	// TODO: 初期化をここに追加します。
	try {
		kinect_.Initialize( NUI_INITIALIZE_FLAG_USES_COLOR );
		kinect_.VideoStream().Open( NUI_IMAGE_TYPE::NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION::NUI_IMAGE_RESOLUTION_640x480 );

		m_sliderTilt.SetRange( kinect_.CAMERA_ELEVATION_MINIMUM, kinect_.CAMERA_ELEVATION_MAXIMUM );
		m_angle = kinect_.GetAngle();

		SetTimer( TIMER_ANGLE, 1000, 0 );
		SetTimer( TIMER_UPDATE, 100, 0 );

		UpdateData( FALSE );
	}
	catch ( std::exception& ex ) {
		MessageBox( ex.what() );
		OnCancel();
	}

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void Ctilt_controlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
        CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR Ctilt_controlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ctilt_controlDlg::OnTimer(UINT_PTR nIDEvent)
{
    if ( nIDEvent == TIMER_ANGLE ) {
        UpdateData( TRUE );

        // スクロールバーの上下と正負が逆なので、値を逆にする
        kinect_.SetAngle( -m_angle );
    }
    else if ( nIDEvent == TIMER_UPDATE ) {
        kinect_.WaitAndUpdateAll();
        kinect::nui::VideoFrame videoMD( kinect_.VideoStream() );

        CDC* dc = GetDC();
        CDC MemoryDC;
        MemoryDC.CreateCompatibleDC( dc );

        CBitmap Bitmap;
        Bitmap.CreateCompatibleBitmap( dc , videoMD.Width(), videoMD.Height() );
        CBitmap* pOldBitmap = MemoryDC.SelectObject(&Bitmap);
        Bitmap.SetBitmapBits( videoMD.Pitch() * videoMD.Height(), videoMD.Bits() );

        dc->BitBlt( 0, 0, videoMD.Width(), videoMD.Height(), &MemoryDC, 0, 0, SRCCOPY );

        MemoryDC.SelectObject( pOldBitmap );
        MemoryDC.DeleteDC();
    }

    CDialogEx::OnTimer(nIDEvent);
}


void Ctilt_controlDlg::OnBnClickedOk()
{
    CDialogEx::OnOK();
}


void Ctilt_controlDlg::OnBnClickedCancel()
{
    CDialogEx::OnCancel();
}
