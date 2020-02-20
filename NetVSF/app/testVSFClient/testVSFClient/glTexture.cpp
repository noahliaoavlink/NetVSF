#include "stdafx.h"
#include "glTexture.h"
#include "ImageFile.h"

//glew32.lib
//#pragma comment(lib, "glew32.lib")

glTexture::glTexture()
{
	m_iTextureID = 0;
	glGenTextures(1, &m_iTextureID);
}

glTexture::~glTexture()
{
}

void glTexture::Make(unsigned char* pBuf,int w,int h)
{
//	if(m_iTextureID != 0)
	//	glDeleteTextures(1, &m_iTextureID);

	m_iWidth = w;
	m_iHeight = h;

	//generate an opengl texture
	glBindTexture(GL_TEXTURE_2D, m_iTextureID);
	//operation on current texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pBuf);
	//GLint iRet = gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, pBuf);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void glTexture::Make(char* szFileName)
{
	ImageFile img_file;
	img_file.Load(szFileName);

//	img_file.RGB2BGR(img_file.GetBuffer(), img_file.GetWidth(), img_file.GetHeight());

	Make(img_file.GetBuffer(), img_file.GetWidth(), img_file.GetHeight());
}

int glTexture::GetTextureID()
{
	return m_iTextureID;
}

void glTexture::Draw()
{
#if 1
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_iTextureID);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glEnd();
	glFlush();
#else
	int iBoundsW = 800;
	int iBoundsH = 600;

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor4f(1, 1, 1, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

//	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_iTextureID);

	//Always fit
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	/*
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);

	glTexCoord2f(m_iWidth, 0);
	glVertex2f(iBoundsW, 0);

	glTexCoord2f(m_iWidth, m_iHeight);
	glVertex2f(iBoundsW, m_iHeight);

	glTexCoord2f(0, m_iHeight);
	glVertex2f(0, iBoundsH);
	*/



	glEnd();

	glDisable(GL_TEXTURE_RECTANGLE_EXT);
	glDisable(GL_BLEND);

	glFlush();
#endif
}