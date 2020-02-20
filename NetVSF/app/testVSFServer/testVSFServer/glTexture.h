#pragma once

#ifndef _glTexture_H_
#define _glTexture_H_

#include "..\..\..\lib\gl\glutdlls37beta\include\gl\glew.h"
#include "..\..\..\lib\gl\glutdlls37beta\include\gl\glut.h"

class glTexture
{
	GLuint m_iTextureID;
	GLuint m_CopyTexture;

	int m_iWidth;
	int m_iHeight;

	unsigned char* m_pOutputImage;
protected:
public:
	glTexture();
	~glTexture();

	void Make(unsigned char* pBuf, int w, int h);
	void Make(char* szFileName);

	//void AllocateTexture(int w, int h);
	void AllocateOutputImage(int w, int h);
	void CopyTexture();
	unsigned char* GetOutputImage();

	int GetTextureID();
	void Draw();
};


#endif