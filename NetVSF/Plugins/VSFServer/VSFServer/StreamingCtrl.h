#pragma once

#ifndef _StreamingCtrl_H_
#define _StreamingCtrl_H_

#include "ImageFile.h"
#include "ServerManager.h"

#include "..\..\..\Include\MediaFileCommon.h"
#include "..\..\..\Include\NetCommon.h"
#include "VideoDecoder.h"
#include "../../../Include/SQQueue.h"

#define _ENABLE_GL_RENDER 1
//#define _ENABLE_GL_TEXTURE_2_IMG 1
#define _ENABLE_QOS 1

#ifdef _ENABLE_GL_RENDER
#include "glRender.h"
#endif

#define _INPUT_VIDEO_MODE 1

class StreamingCtrl : public EventCallback
{
	ServerManager* m_pServerManager;
#ifndef _INPUT_VIDEO_MODE
	ImageFile* m_pImageFile[4];
#endif
	int m_iFrameCount;
	int m_iFPS;
	HWND m_hWnd;

	VideoDecoder* m_pVideoDecoder;

	SQQueue* m_pVideoQueue;
	int m_iWidth;
	int m_iHeight;

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
	void Start(int iPort = 2019);
	void Stop();
	void SetQuality(int iValue);
	void SetFPS(int iValue);
#ifndef _INPUT_VIDEO_MODE
	void UpdateVideo();
	void UpdateVideo2();
#endif
	void UpdateVideo3();
	bool IsAlive();
	void CheckFPS();

	void DrawBMP(unsigned char* pBuffer, int x, int y, int w, int h, int iSrcW, int iSrcH);
	void DisplayVideoFrame(unsigned char* pBuf, int iLen);

	void* Event(int iMsg, void* pParameter1, void* pParameter2);

	void TimeEvent();
	bool IsFinished();
	int InputVideo(unsigned char* pBuffer, int w, int h);

	void Test();
	
};

#endif