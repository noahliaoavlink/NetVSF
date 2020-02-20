#include "stdafx.h"
#include "StreamingCtrl.h"
#include "../../../Include/sstring.h"
#include "Mmsystem.h"

DWORD WINAPI ImageThread(LPVOID lpParam);

StreamingCtrl::StreamingCtrl()
{
	m_iFPS = 10;

	m_pVideoDecoder = new VideoDecoder;

#ifdef _ENABLE_GL_RENDER
	//m_pTmpBuf = 0;
	m_pglRender = new glRender;
	m_bFinished = true;
	m_dwLastTime = 0; 
#endif

	InitializeCriticalSection(&m_CriticalSection);
}

StreamingCtrl::~StreamingCtrl()
{
	for (int i = 0; i < 4; i++)
		delete m_pImageFile[i];

	delete m_pServerManager;

	if (m_pVideoDecoder)
		delete m_pVideoDecoder;

#ifdef _ENABLE_GL_RENDER
	if (m_pglRender)
		delete m_pglRender;

	//	if (m_pTmpBuf)
		//	delete m_pTmpBuf;
#endif

	DeleteCriticalSection(&m_CriticalSection);
}

void StreamingCtrl::Init(HWND hWnd)
{
	char szAPPath[256];
	char szFileName[512];

	m_hWnd = hWnd;

	GetExecutionPath(NULL, szAPPath);

	for (int i = 0; i < 4; i++)
		m_pImageFile[i] = new ImageFile;

	sprintf(szFileName, "%s\\data\\2012-01-07_15-54-57_327.bmp", szAPPath);
	m_pImageFile[0]->Load(szFileName);
	m_pImageFile[0]->RGB2BGR(m_pImageFile[0]->GetBuffer(), m_pImageFile[0]->GetWidth(), m_pImageFile[0]->GetHeight());
	sprintf(szFileName, "%s\\data\\2012-01-07_15-55-13_140.bmp", szAPPath);
	m_pImageFile[1]->Load(szFileName);
	m_pImageFile[1]->RGB2BGR(m_pImageFile[1]->GetBuffer(), m_pImageFile[1]->GetWidth(), m_pImageFile[1]->GetHeight());
	sprintf(szFileName, "%s\\data\\2012-01-07_15-56-08_766.bmp", szAPPath);
	m_pImageFile[2]->Load(szFileName);
	m_pImageFile[2]->RGB2BGR(m_pImageFile[2]->GetBuffer(), m_pImageFile[2]->GetWidth(), m_pImageFile[2]->GetHeight());
	sprintf(szFileName, "%s\\data\\2012-01-07_15-56-41_596.bmp", szAPPath);
	m_pImageFile[3]->Load(szFileName);
	m_pImageFile[3]->RGB2BGR(m_pImageFile[3]->GetBuffer(), m_pImageFile[3]->GetWidth(), m_pImageFile[3]->GetHeight());

	//m_ImageFile[0].GetWidth();
	//m_ImageFile[0].GetHeight();

	m_pServerManager = new ServerManager;
	m_pServerManager->Init(CT_JPEG, m_pImageFile[0]->GetWidth(), m_pImageFile[0]->GetHeight());

	m_pServerManager->SetEventCallback(this);

#ifdef _ENABLE_GL_RENDER
	m_pglRender->Init(hWnd);
#endif

	m_iFrameCount = 0;
}

void StreamingCtrl::Start()
{
	m_bFinished = true;
	m_pServerManager->Open(2019);

#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)
#else
	m_bIsAlive = true;
	DWORD nThreadID;
	m_hImageThread = CreateThread(0, 0, ImageThread, (void *)this, 0, &nThreadID);
#endif
}

void StreamingCtrl::Stop()
{
	m_pServerManager->Close();

#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)
#else
	m_bIsAlive = false;
	::WaitForSingleObject(m_hImageThread, 10000);
#endif
}

void StreamingCtrl::SetQuality(int iValue)
{
	m_pServerManager->SetQuality(iValue);
}

void StreamingCtrl::SetFPS(int iValue)
{
	m_iFPS = iValue;
}

void StreamingCtrl::UpdateVideo()
{
	m_bFinished = false;
	EnterCriticalSection(&m_CriticalSection);

	int iIndex = m_iFrameCount % 4;

#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)

	m_pglRender->UpdateTexture(m_pImageFile[iIndex]->GetBuffer(), m_pImageFile[iIndex]->GetWidth(), m_pImageFile[iIndex]->GetHeight());
	m_pglRender->Update();
	m_pglRender->CopyTexture2Image();
	m_pglRender->GetOutputImage();

	//img.SaveBmpFile("D:\\temp\\gl_tex2bmp.bmp", m_pglRender->GetOutputImage(), m_pglRender->GetWidth(), m_pglRender->GetHeight());

	int iRet = m_pServerManager->UpdateVideo(m_pglRender->GetOutputImage(), m_pglRender->GetWidth(), m_pglRender->GetHeight());
#else
	int iRet = m_pServerManager->UpdateVideo(m_pImageFile[iIndex]->GetBuffer(), m_pImageFile[iIndex]->GetWidth(), m_pImageFile[iIndex]->GetHeight());
#endif

	if (iRet > 0)
	{
		
#ifdef _ENABLE_GL_RENDER
		m_pglRender->UpdateTexture(m_pImageFile[iIndex]->GetBuffer(), m_pImageFile[iIndex]->GetWidth(), m_pImageFile[iIndex]->GetHeight());
		//m_pglRender->Draw();
#else
		RECT rect;
		GetClientRect(m_hWnd, &rect);
		DrawBMP(m_pImageFile[iIndex]->GetBuffer(), 0, 0, rect.right, rect.bottom, m_pImageFile[iIndex]->GetWidth(), m_pImageFile[iIndex]->GetHeight());
#endif	
		m_iFrameCount++;
	}
	else
	{
		char szMsg[512];
		sprintf(szMsg, "VSFError - CtestVSFServerDlg::UpdateVideo() Error!! [%d]\n", iRet);
		OutputDebugStringA(szMsg);
	}

	int iWaitTime = 1000.0 / m_iFPS;

	Sleep(iWaitTime);
	LeaveCriticalSection(&m_CriticalSection);

	m_bFinished = true;
}

void StreamingCtrl::UpdateVideo2()
{
	EnterCriticalSection(&m_CriticalSection);

	DWORD dwCurTime = timeGetTime();

	if (m_dwLastTime == 0)
		m_dwLastTime = dwCurTime;
	else
	{
		int iWaitTime = 1000.0 / m_iFPS;

		if (dwCurTime >= m_dwLastTime + iWaitTime)
		{
			m_dwLastTime = dwCurTime;
			int iIndex = m_iFrameCount % 4;

#if (_ENABLE_GL_RENDER && _ENABLE_GL_TEXTURE_2_IMG)

			m_pglRender->UpdateTexture(m_pImageFile[iIndex]->GetBuffer(), m_pImageFile[iIndex]->GetWidth(), m_pImageFile[iIndex]->GetHeight());
			m_pglRender->Update();
			m_pglRender->CopyTexture2Image();
			m_pglRender->GetOutputImage();

			//img.SaveBmpFile("D:\\temp\\gl_tex2bmp.bmp", m_pglRender->GetOutputImage(), m_pglRender->GetWidth(), m_pglRender->GetHeight());

			int iRet = m_pServerManager->UpdateVideo(m_pglRender->GetOutputImage(), m_pglRender->GetWidth(), m_pglRender->GetHeight());
#else
			int iRet = m_pServerManager->UpdateVideo(m_pImageFile[iIndex]->GetBuffer(), m_pImageFile[iIndex]->GetWidth(), m_pImageFile[iIndex]->GetHeight());
#endif

			if (iRet > 0)
			{

#ifdef _ENABLE_GL_RENDER
				m_pglRender->UpdateTexture(m_pImageFile[iIndex]->GetBuffer(), m_pImageFile[iIndex]->GetWidth(), m_pImageFile[iIndex]->GetHeight());
				//m_pglRender->Draw();
#else
				RECT rect;
				GetClientRect(m_hWnd, &rect);
				DrawBMP(m_pImageFile[iIndex]->GetBuffer(), 0, 0, rect.right, rect.bottom, m_pImageFile[iIndex]->GetWidth(), m_pImageFile[iIndex]->GetHeight());
#endif	
				m_iFrameCount++;
			}
			else
			{
				char szMsg[512];
				sprintf(szMsg, "VSFError - CtestVSFServerDlg::UpdateVideo() Error!! [%d]\n", iRet);
				OutputDebugStringA(szMsg);
			}
		}
	}
	LeaveCriticalSection(&m_CriticalSection);
}

void StreamingCtrl::CheckFPS()
{
#if _ENABLE_QOS
	float fFPS = m_pServerManager->GetAvgFPS(0);

	if (fFPS > 0)
	{
		int iOldFPS = m_iFPS;
		//m_iFPS = fFPS * 0.7;

		if (fFPS > 30)
			m_iFPS = 30;
		else
			m_iFPS = fFPS;

		if (iOldFPS != m_iFPS)
		{
			char szMsg[512];
			sprintf(szMsg, "TCS - StreamingCtrl::CheckFPS() fps [%d -> %d]\n", iOldFPS, m_iFPS);
			OutputDebugStringA(szMsg);
		}
	}
#endif
}

bool StreamingCtrl::IsAlive()
{
	return m_bIsAlive;
}

bool StreamingCtrl::IsFinished()
{
	return m_bFinished;
}

void StreamingCtrl::DrawBMP(unsigned char* pBuffer, int x, int y, int w, int h, int iSrcW, int iSrcH)
{
	BITMAPINFOHEADER bmpInfo;
	memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
	bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.biWidth = iSrcW;
	bmpInfo.biHeight = iSrcH;
	bmpInfo.biPlanes = 1;
	bmpInfo.biBitCount = 24;

	bmpInfo.biCompression = BI_RGB;
	bmpInfo.biSizeImage = iSrcW * iSrcH * 3;
	bmpInfo.biSizeImage = 0;

	HDC hdc;
	RECT rect;
	hdc = ::GetDC(m_hWnd);

	if (x >= 0 && y >= 0)
	{
		::SetStretchBltMode(hdc, COLORONCOLOR);
		/*
		StretchDIBits(hdc,
			x,
			y,
			w,
			h,
			0,
			iSrcH,
			iSrcW,
			-iSrcH,//iSrcH - 1,
			pBuffer,
			(BITMAPINFO*)&bmpInfo,
			DIB_RGB_COLORS,
			SRCCOPY);
			*/
		StretchDIBits(hdc,
			x,
			y,
			w,
			h,
			0,
			0,
			iSrcW,
			iSrcH,//iSrcH - 1,
			pBuffer,
			(BITMAPINFO*)&bmpInfo,
			DIB_RGB_COLORS,
			SRCCOPY);
	}

	::ReleaseDC(m_hWnd, hdc);
}

void StreamingCtrl::DisplayVideoFrame(unsigned char* pBuf,int iLen)
{
	if (!m_pVideoDecoder->IsOpened())
	{
		m_pVideoDecoder->Open(CT_JPEG,1920,1080);
	}

	int iRet = m_pVideoDecoder->Decode(pBuf, iLen);

	if (iRet >= 1)
	{
		int iLen = m_pVideoDecoder->GetBufLen();

		RECT rect;
		GetClientRect(m_hWnd, &rect);

		DrawBMP(m_pVideoDecoder->GetOutBuffer(), 0, 0, rect.right, rect.bottom, m_pVideoDecoder->GetWidth(), m_pVideoDecoder->GetHeight());
	}
}

void* StreamingCtrl::Event(int iMsg, void* pParameter1, void* pParameter2)
{
	switch (iMsg)
	{
		case CCE_DISPLAY_VIDEO_FRAME:
		{
			int* iLen = (int*)pParameter2;
			DisplayVideoFrame((unsigned char*)pParameter1, *iLen);
		}
		break;
	}
	return 0;
}

void StreamingCtrl::TimeEvent()
{
#ifdef _ENABLE_GL_RENDER
	m_pglRender->Update();
#endif

	CheckFPS();
}

void StreamingCtrl::Test()
{
	//int iIndex = m_iFrameCount % 4;

#ifdef _ENABLE_GL_RENDER

	ImageFile img;

	DWORD dwTime1 = timeGetTime();

	m_pglRender->CopyTexture2Image();

	DWORD dwTime2 = timeGetTime();

	m_pglRender->GetOutputImage();

	DWORD dwTime3 = timeGetTime();

	char szMsg[512];
	sprintf(szMsg, "VSFInfo - CtestVSFServerDlg::Test() spend time:[%d , %d]\n", dwTime2- dwTime1, dwTime3 - dwTime2);
	OutputDebugStringA(szMsg);

	img.SaveBmpFile("D:\\temp\\gl_tex2bmp.bmp", m_pglRender->GetOutputImage(), m_pglRender->GetWidth(), m_pglRender->GetHeight());
#endif

	//m_pImageFile[iIndex]->GetBuffer(), m_pImageFile[iIndex]->GetWidth(), m_pImageFile[iIndex]->GetHeight();
}

DWORD WINAPI ImageThread(LPVOID lpParam)
{
	StreamingCtrl* pObj = (StreamingCtrl*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->UpdateVideo2();
	}

	return 0;
}