#pragma once

#ifndef _glTexture_H_
#define _glTexture_H_

#include "..\..\..\lib\gl\glutdlls37beta\include\gl\glew.h"
#include "..\..\..\lib\gl\glutdlls37beta\include\gl\glut.h"

class glTexture
{
	unsigned int m_iTextureID;

	int m_iWidth;
	int m_iHeight;
protected:
public:
	glTexture();
	~glTexture();

	void Make(unsigned char* pBuf, int w, int h);
	void Make(char* szFileName);

	int GetTextureID();
	void Draw();
};


#endif