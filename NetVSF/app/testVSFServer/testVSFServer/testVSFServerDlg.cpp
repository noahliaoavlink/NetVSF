
// testVSFServerDlg.cpp: 實作檔案
//

#include "stdafx.h"
#include "testVSFServer.h"
#include "testVSFServerDlg.h"
#include "afxdialogex.h"

#include "../../../Include/sstring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)
DWORD WINAPI DlgImageThread(LPVOID lpParam);
#endif

// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CtestVSFServerDlg 對話方塊



CtestVSFServerDlg::CtestVSFServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTVSFSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestVSFServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtestVSFServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CtestVSFServerDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CtestVSFServerDlg::OnBnClickedStop)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_UPDATE, &CtestVSFServerDlg::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_TEST, &CtestVSFServerDlg::OnBnClickedTest)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CtestVSFServerDlg 訊息處理常式

BOOL CtestVSFServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	CWnd* pWnd = GetDlgItem(IDC_VIDEO);
	pWnd->GetSafeHwnd();
	m_pStreamingCtrl = new StreamingCtrl;
	m_pStreamingCtrl->Init(pWnd->GetSafeHwnd());
	m_pStreamingCtrl->SetQuality(85);

#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)
	m_pStreamingCtrl->SetFPS(1);
#else
	m_pStreamingCtrl->SetFPS(10);
#endif

	wchar_t wszItemText[16];

	CComboBox* pQualty = (CComboBox*)GetDlgItem(IDC_QUALTY);
	for (int i = 1; i < 21; i++)
	{
		swprintf(wszItemText, L"%d", i * 5);
		pQualty->AddString(wszItemText);
	}
	pQualty->SetCurSel(17);

	CComboBox* pFPS = (CComboBox*)GetDlgItem(IDC_FPS);
	for (int i = 1; i < 31; i++)
	{
		swprintf(wszItemText, L"%d", i);
		pFPS->AddString(wszItemText);
	}
	pFPS->SetCurSel(9);

#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)
	SetTimer(101, 100, NULL);
#else
	SetTimer(101, 10, NULL);
#endif
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CtestVSFServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CtestVSFServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CtestVSFServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CtestVSFServerDlg::OnBnClickedStart()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	m_pStreamingCtrl->Start();

#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)
	/*
	m_bIsAlive = true;
	DWORD nThreadID;
	m_hImageThread = CreateThread(0, 0, DlgImageThread, (void *)this, 0, &nThreadID);
	*/
#endif
}


void CtestVSFServerDlg::OnBnClickedStop()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	m_pStreamingCtrl->Stop();

#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)
	/*
	m_bIsAlive = false;
	::WaitForSingleObject(m_hImageThread, 10000);
	*/
#endif
}

void CtestVSFServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此加入您的訊息處理常式程式碼

	if (m_pStreamingCtrl)
		delete m_pStreamingCtrl;
}

void CtestVSFServerDlg::OnBnClickedUpdate()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	wchar_t wszQualtyText[16];
	wchar_t wszFPSText[16];
	int iQualty;
	int iFPS;

	CComboBox* pQualty = (CComboBox*)GetDlgItem(IDC_QUALTY);
	pQualty->GetLBText(pQualty->GetCurSel(), wszQualtyText);
	iQualty = _wtoi(wszQualtyText);

	CComboBox* pFPS = (CComboBox*)GetDlgItem(IDC_FPS);
	pFPS->GetLBText(pFPS->GetCurSel(), wszFPSText);
	iFPS = _wtoi(wszFPSText);

	m_pStreamingCtrl->SetQuality(iQualty);
	m_pStreamingCtrl->SetFPS(iFPS);
}

void CtestVSFServerDlg::OnBnClickedTest()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	m_pStreamingCtrl->Test();
}


void CtestVSFServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)
	if(IsAlive())
		UpdateVideo();
#else
	m_pStreamingCtrl->TimeEvent();
#endif

	CDialogEx::OnTimer(nIDEvent);
}

#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)
void CtestVSFServerDlg::UpdateVideo()
{
	m_pStreamingCtrl->UpdateVideo2();
}

bool CtestVSFServerDlg::IsAlive()
{
	return m_bIsAlive;
}

DWORD WINAPI DlgImageThread(LPVOID lpParam)
{
	
	CtestVSFServerDlg* pObj = (CtestVSFServerDlg*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->UpdateVideo();
	}
	
	return 0;
}
#endif