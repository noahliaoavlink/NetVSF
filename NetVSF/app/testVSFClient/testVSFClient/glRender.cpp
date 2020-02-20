#include "stdafx.h"
#include "glRender.h"
#include "Mmsystem.h"

glRender::glRender()
{
	m_hDC = 0;
	m_hRC = 0;

	m_pglTexture = 0;

	m_pBuf = 0;
	m_bUpdate = false;
}

glRender::~glRender()
{
	if (m_pglTexture)
		delete m_pglTexture;

	if (m_pBuf)
		delete m_pBuf;
}

void glRender::Init(HWND hWnd)
{
	RECT rect;
	m_hWnd = hWnd;

//	glewInit();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	GetClientRect(hWnd, &rect);

	Create(hWnd);

	SetupProjection(rect.right, rect.bottom);

	glClearColor(0.0, 0.0, 0.0, 0.0);

	m_pglTexture = new glTexture;
}

void glRender::Create(HWND hWnd)
{
	m_hDC = GetDC(hWnd);
	SetupPixelFormat(m_hDC);
	m_hRC = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, m_hRC);
}

void glRender::Destroy()
{
	wglMakeCurrent(m_hDC, NULL);
	wglDeleteContext(m_hRC);
}

void glRender::SetupPixelFormat(HDC hDC)
{
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0,0,0,0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0,
	};

	pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);
}

void glRender::SetupProjection(int width, int height)
{
	if (height == 0) height = 1;

#if 1
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(52.0, (GLfloat)width / (GLfloat)height, 1.0f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

//	m_windowWidth = width;
//	m_windowHeight = height;
#else
	int x = 0;
	int y = 0;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glHint(GL_CLIP_VOLUME_CLIPPING_HINT_EXT, GL_FASTEST);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glOrtho(x, x + width, y, y + height, -1.0, 1.0);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
#endif
}

void glRender::UpdateTexture(unsigned char* pBuf,int w,int h)
{
//	HGLRC hRC = wglGetCurrentContext();
	//DWORD dwStartTime = timeGetTime();
	
	if (!m_pBuf)
		m_pBuf = new unsigned char[w * h * 3];

	memcpy(m_pBuf, pBuf, w * h * 3);
	m_iW = w;
	m_iH = h;

	m_bUpdate = true;
	/*
	DWORD dwEndTime = timeGetTime();

	char szMsg[512];
	sprintf(szMsg, "VSFClient glRender::UpdateTexture [%d]\n", dwEndTime - dwStartTime);
	OutputDebugStringA(szMsg);
	*/
//	m_pglTexture->Make(pBuf, w,h);
}

void glRender::Draw()
{
	m_pglTexture->Draw();
	SwapBuffers(m_hDC);
}

void glRender::Update()
{
	if (m_bUpdate)
	{
		m_bUpdate = false;

//		DWORD dwTime1 = timeGetTime();

//		HGLRC hRC = wglGetCurrentContext();
		m_pglTexture->Make(m_pBuf, m_iW, m_iH);

//		DWORD dwTime2 = timeGetTime();

		Draw();
		/*
		DWORD dwTime3 = timeGetTime();

		char szMsg[512];
		sprintf(szMsg, "VSFClient glRender::Update [%d , %d]\n", dwTime2 - dwTime1, dwTime3 - dwTime2);
		OutputDebugStringA(szMsg);
		*/
	}
}

void glRender::Test()
{
#if 0
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
	glLoadIdentity(); // Reset The Current Modelview Matrix
//	glTranslatef(-0.0f, 0.0f, -2.5f); // Move Left 1.5 Units And Into The Screen 6.0

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
/*
	glBegin(GL_TRIANGLES); // Drawing Using Triangles
	glColor3f(1.0f, 0.0f, 0.0f); // Set The Color To Red
	glVertex3f(0.0f, 1.0f, 0.0f); // Top
	glColor3f(0.0f, 1.0f, 0.0f); // Set The Color To Green
	glVertex3f(-1.0f, -1.0f, 0.0f); // Bottom Left
	glColor3f(0.0f, 0.0f, 1.0f); // Set The Color To Blue
	glVertex3f(1.0f, -1.0f, 0.0f); // Bottom Right
	glEnd(); // Finished Drawing The Triangle
	*/
//	glTranslatef(0.0f, 0.0f, 0.0f); // Move Right 3 Units
	glColor3f(0.5f, 0.5f, 1.0f); // Set The Color To Blue One Time Only
	glBegin(GL_QUADS); // Draw A Quad
	glVertex3f(-1.0f, 1.0f, 0.0f); // Top Left 
	glVertex3f(1.0f, 1.0f, 0.0f); // Top Right
	glVertex3f(1.0f, -1.0f, 0.0f); // Bottom Right
	glVertex3f(-1.0f, -1.0f, 0.0f); // Bottom Left
	glEnd(); /// Done Drawing The Quad
#else
	HDC hdc = wglGetCurrentDC();
	m_pglTexture->Make("D:\\noah_src\\android_mapping\\NetVSF\\app\\testVSFServer\\Debug\\data\\2012-01-07_15-54-57_327.bmp");
	m_pglTexture->Draw();
#endif

	SwapBuffers(m_hDC);
}

//參考   https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/