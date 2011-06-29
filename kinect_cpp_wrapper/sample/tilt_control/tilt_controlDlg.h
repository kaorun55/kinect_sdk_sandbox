
// tilt_controlDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxcmn.h"


// Ctilt_controlDlg ダイアログ
class Ctilt_controlDlg : public CDialogEx
{
// コンストラクション
public:
	Ctilt_controlDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_TILT_CONTROL_DIALOG };

    enum {
        TIMER_ANGLE,
        TIMER_UPDATE,
    };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

private:

    kinect::nui::Kinect kinect_;

// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    CSliderCtrl m_sliderTilt;
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    int m_angle;
};
