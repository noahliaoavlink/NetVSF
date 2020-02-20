#pragma once

#ifndef _glRender_H_
#define _glRender_H_

#include "..\..\..\lib\gl\glutdlls37beta\include\gl\glew.h"
#include "..\..\..\lib\gl\glutdlls37beta\include\gl\glut.h"

//#include "glShader.h"
#include "glTexture.h"

class glRender
{
	HWND m_hWnd;
	HDC m_hDC;
	HGLRC m_hRC;

	unsigned int m_iTextureID;

	glTexture* m_pglTexture;

	unsigned char* m_pBuf;
	int m_iW;
	int m_iH;
	bool m_bUpdate;
protected:
public:
	glRender();
	~glRender();

	void Init(HWND hWnd);
	void Create(HWND hWnd);
	void Destroy();

	void SetupPixelFormat(HDC hDC);
	void SetupProjection(int width, int height);

	void UpdateTexture(unsigned char* pBuf, int w, int h);
	void Draw();
	void Update();
	void CopyTexture2Image();
	unsigned char* GetOutputImage();
	int GetWidth();
	int GetHeight();

	void Test();
};

#endif