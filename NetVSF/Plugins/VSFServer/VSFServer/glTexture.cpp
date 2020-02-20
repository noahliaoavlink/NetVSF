#include "stdafx.h"
#include "glTexture.h"
#include "ImageFile.h"

//glew32.lib
//#pragma comment(lib, "glew32.lib")

glTexture::glTexture()
{
	m_iTextureID = 0;
	m_CopyTexture = 0;
	glGenTextures(1, &m_iTextureID);

	m_pOutputImage = 0;
}

glTexture::~glTexture()
{
	if (m_iTextureID)
	{
		glDeleteTextures(1, &m_iTextureID);
		m_iTextureID = 0;
	}

	if (m_CopyTexture)
	{
		glDeleteTextures(1, &m_CopyTexture);
		m_CopyTexture = 0;
	}

	if (m_pOutputImage)
		delete m_pOutputImage;
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
/*
void glTexture::AllocateTexture(int w,int h)
{
	GLenum target = GL_TEXTURE_RECTANGLE_EXT;
	GLuint tex;

	if (m_CopyTexture)
	{
		glDeleteTextures(1, &m_CopyTexture);
		m_CopyTexture = 0;
	}

	glGenTextures(1, &tex);
	glBindTexture(target, tex);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//supports GL_RGBA and GL_UNSIGNED_BYTE only.
	glTexImage2D(target, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	m_CopyTexture = tex;
}
*/
void glTexture::AllocateOutputImage(int w,int h)
{
	if (!m_pOutputImage)
	{
		m_pOutputImage = new unsigned char[w * h * 3];
	}
	else
	{
		if (w != m_iWidth || h != m_iHeight)
		{
			delete m_pOutputImage;
			m_pOutputImage = new unsigned char[w * h * 3];
		}
	}
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

void glTexture::CopyTexture()
{
	//memset(m_pOutputImage,0, m_iWidth * m_iHeight * 3);
#if 1
	//speed slow
	glBindTexture(GL_TEXTURE_2D, m_iTextureID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pOutputImage);
	glBindTexture(GL_TEXTURE_2D, 0);
#else
	//OpenGL ES unsupported
	glBindTexture(GL_TEXTURE_2D, m_iTextureID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pOutputImage);
	glBindTexture(GL_TEXTURE_2D, m_CopyTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iWidth, m_iHeight, GL_RGB, GL_UNSIGNED_BYTE, m_pOutputImage);
	glBindTexture(GL_TEXTURE_2D, 0);
#endif
}

unsigned char* glTexture::GetOutputImage()
{
	return m_pOutputImage;
}