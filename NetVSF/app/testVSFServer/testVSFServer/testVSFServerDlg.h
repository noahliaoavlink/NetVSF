
// testVSFServerDlg.h: 標頭檔
//

#pragma once

#include "StreamingCtrl.h"

// CtestVSFServerDlg 對話方塊
class CtestVSFServerDlg : public CDialogEx
{
	StreamingCtrl* m_pStreamingCtrl;

#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)
	bool m_bIsAlive;
	HANDLE m_hImageThread;
	CRITICAL_SECTION m_CriticalSection;
#endif

// 建構
public:
	CtestVSFServerDlg(CWnd* pParent = nullptr);	// 標準建構函式

#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)
	void UpdateVideo();
	bool IsAlive();
#endif

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTVSFSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedUpdate();
	afx_msg void OnBnClickedTest();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
