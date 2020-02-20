
// testVSFClientDlg.cpp: 實作檔案
//

#include "stdafx.h"
#include "testVSFClient.h"
#include "testVSFClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
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


// CtestVSFClientDlg 對話方塊



CtestVSFClientDlg::CtestVSFClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTVSFCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestVSFClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtestVSFClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT, &CtestVSFClientDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_STOP, &CtestVSFClientDlg::OnBnClickedStop)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CtestVSFClientDlg 訊息處理常式

BOOL CtestVSFClientDlg::OnInitDialog()
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
	
	m_pClientCtrl = new ClientCtrl;
	m_pClientCtrl->Init(pWnd->GetSafeHwnd());  //pWnd->GetSafeHwnd()

	SetDlgItemText(IDC_SERVER_IP,L"192.168.0.103");
	SetDlgItemText(IDC_SERVER_PORT, L"2019");

	SetTimer(101,10,NULL);
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CtestVSFClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CtestVSFClientDlg::OnPaint()
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
HCURSOR CtestVSFClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CtestVSFClientDlg::OnBnClickedConnect()
{
	// TODO: 在此加入控制項告知處理常式程式碼
#if 1	
	char szIP[32] = "127.0.0.1";// "192.168.0.103";
	int iPort = 2019;

	GetDlgItemTextA(this->GetSafeHwnd(),IDC_SERVER_IP, szIP,16);
	iPort = GetDlgItemInt(IDC_SERVER_PORT);

	m_pClientCtrl->Connect(szIP, iPort);
#else
	m_pClientCtrl->Test();
#endif
}


void CtestVSFClientDlg::OnBnClickedStop()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	m_pClientCtrl->Close();
}


void CtestVSFClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此加入您的訊息處理常式程式碼
	delete m_pClientCtrl;
}


void CtestVSFClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if(m_pClientCtrl)
		m_pClientCtrl->TimeEvent();

	CDialogEx::OnTimer(nIDEvent);
}
