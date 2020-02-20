#pragma once

#ifndef _ClientCtrl_H_
#define _ClientCtrl_H_

#define _ENABLE_GL_RENDER 1

#include "..\..\..\Include\MediaFileCommon.h"
#include "..\..\..\Include\NetCommon.h"

#include "Client.h"

#ifdef _ENABLE_GL_RENDER
#include "glRender.h"
#endif

class ClientCtrl : public EventCallback
{
	HWND m_hWnd;

	Client* m_pClient;

#ifdef _ENABLE_GL_RENDER
	unsigned char* m_pTmpBuf;
	glRender* m_pglRender;
#endif

protected:
public:
	ClientCtrl();
	~ClientCtrl();

	void Init(HWND hWnd);
	void Connect(char* szIP, int iPort);
	void Close();

	void DisplayVideoFrame(unsigned char* pBuf, int iLen);

	void DrawBMP(unsigned char* pBuffer, int x, int y, int w, int h, int iSrcW, int iSrcH);

	void TimeEvent();

	virtual void* Event(int iMsg, void* pParameter1, void* pParameter2);

	void Test();
};

#endif
