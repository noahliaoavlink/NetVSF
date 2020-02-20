#include "stdafx.h"
#include "ClientCtrl.h"

#include "ImageFile.h"

#ifdef _ENABLE_GL
#endif

ClientCtrl::ClientCtrl()
{
	m_hWnd = 0;

	m_pClient = 0;//new Client;
//	m_pClient->Startup();

#ifdef _ENABLE_GL_RENDER
	//m_pTmpBuf = 0;
	m_pglRender = new glRender;
#endif
}

ClientCtrl::~ClientCtrl()
{
	if (m_pClient)
		delete m_pClient;

#ifdef _ENABLE_GL_RENDER
	if (m_pglRender)
		delete m_pglRender;

//	if (m_pTmpBuf)
	//	delete m_pTmpBuf;
#endif
}

void ClientCtrl::Init(HWND hWnd)
{
	m_hWnd = hWnd;

	//m_pClient->SetEventCallback(this);

#ifdef _ENABLE_GL_RENDER
	m_pglRender->Init(hWnd);
#endif
}

void ClientCtrl::Connect(char* szIP, int iPort)
{
	
	m_pClient = new Client;
	m_pClient->SetEventCallback(this);
	m_pClient->Startup();
	
	m_pClient->Connect(szIP,iPort);
}

void ClientCtrl::Close()
{
	if (m_pClient)
	{
		m_pClient->Close();

		m_pClient->Cleanup();
		if (m_pClient)
			delete m_pClient;
	}
	m_pClient = 0;
}

void ClientCtrl::DisplayVideoFrame(unsigned char* pBuf,int iLen)
{
	RECT rect;
	int w = m_pClient->GetWidth();
	int h = m_pClient->GetHeight();

	GetClientRect(m_hWnd, &rect);

#ifdef _ENABLE_GL_RENDER
	m_pglRender->UpdateTexture(pBuf, w, h);
	//m_pglRender->Draw();
#else
	DrawBMP(pBuf, 0, 0, rect.right, rect.bottom, w,h);
#endif
}

void ClientCtrl::TimeEvent()
{
#ifdef _ENABLE_GL_RENDER
	m_pglRender->Update();
#endif
}

void* ClientCtrl::Event(int iMsg, void* pParameter1, void* pParameter2)
{
	switch (iMsg)
	{
		case CCE_DISPLAY_VIDEO_FRAME:
			{
				int* iLen = (int*)pParameter2;
				DisplayVideoFrame((unsigned char*)pParameter1,*iLen);
			}
			break;
	}
	return 0;
}

void ClientCtrl::DrawBMP(unsigned char* pBuffer, int x, int y, int w, int h, int iSrcW, int iSrcH)
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

void ClientCtrl::Test()
{
#ifdef _ENABLE_GL_RENDER
	m_pglRender->Test();
#else
	RECT rect;
	//int w = m_pClient->GetWidth();
	//int h = m_pClient->GetHeight();

	GetClientRect(m_hWnd, &rect);

	char szFileName[512] = "D:\\noah_src\\android_mapping\\NetVSF\\app\\testVSFServer\\Debug\\data\\2012-01-07_15-54-57_327.bmp";
	ImageFile img_file;
	img_file.Load(szFileName);

	int w = img_file.GetWidth();
	int h = img_file.GetHeight();

	DrawBMP(img_file.GetBuffer(), 0, 0, rect.right, rect.bottom, w, h);
#endif
}