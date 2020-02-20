#pragma once

#ifndef _StreamingCtrl_H_
#define _StreamingCtrl_H_

#include "ImageFile.h"
#include "ServerManager.h"

#include "..\..\..\Include\MediaFileCommon.h"
#include "..\..\..\Include\NetCommon.h"
#include "VideoDecoder.h"

#define _ENABLE_GL_RENDER 1
//#define _ENABLE_GL_TEXTURE_2_IMG 1
#define _ENABLE_QOS 1

#ifdef _ENABLE_GL_RENDER
#include "glRender.h"
#endif

class StreamingCtrl : public EventCallback
{
	ServerManager* m_pServerManager;
	ImageFile* m_pImageFile[4];
	int m_iFrameCount;
	int m_iFPS;
	HWND m_hWnd;

	VideoDecoder* m_pVideoDecoder;

#ifdef _ENABLE_GL_RENDER
	//unsigned char* m_pTmpBuf;
	glRender* m_pglRender;
	bool m_bFinished;
	DWORD m_dwLastTime;
#endif

	bool m_bIsAlive;
	HANDLE m_hImageThread;
	CRITICAL_SECTION m_CriticalSection;
protected:
public:
	StreamingCtrl();
	~StreamingCtrl();

	void Init(HWND hWnd);
	void Start();
	void Stop();
	void SetQuality(int iValue);
	void SetFPS(int iValue);
	void UpdateVideo();
	void UpdateVideo2();
	bool IsAlive();
	void CheckFPS();

	void DrawBMP(unsigned char* pBuffer, int x, int y, int w, int h, int iSrcW, int iSrcH);
	void DisplayVideoFrame(unsigned char* pBuf, int iLen);

	void* Event(int iMsg, void* pParameter1, void* pParameter2);

	void TimeEvent();
	bool IsFinished();

	void Test();
	
};

#endif