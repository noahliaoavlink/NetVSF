#include "stdafx.h"
#include "VSFServerDLLTester.h"
#include "../../../Include/sstring.h"

VSFServerDLLTester::VSFServerDLLTester()
{
	m_pVSFServerDLL = new VSFServerDLL;

	m_iFrameCount = 0;
}

VSFServerDLLTester::~VSFServerDLLTester()
{
	if (m_pVSFServerDLL)
		delete m_pVSFServerDLL;

	for (int i = 0; i < 4; i++)
		delete m_pImageFile[i];
}

void VSFServerDLLTester::Init(HWND hWnd)
{
	char szAPPath[256];
	char szFileName[512];

	if (m_pVSFServerDLL)
	{
		m_pVSFServerDLL->LoadLib();
		m_pVSFServerDLL->Init(hWnd);
	}

	for (int i = 0; i < 4; i++)
		m_pImageFile[i] = new ImageFile;

	
	GetExecutionPath(NULL, szAPPath);

	sprintf(szFileName, "%s\\data\\2012-01-07_15-54-57_327.bmp", szAPPath);
	m_pImageFile[0]->Load(szFileName);
	m_pImageFile[0]->RGB2BGR(m_pImageFile[0]->GetBuffer(), m_pImageFile[0]->GetWidth(), m_pImageFile[0]->GetHeight());
	sprintf(szFileName, "%s\\data\\2012-01-07_15-55-13_140.bmp", szAPPath);
	m_pImageFile[1]->Load(szFileName);
	m_pImageFile[1]->RGB2BGR(m_pImageFile[1]->GetBuffer(), m_pImageFile[1]->GetWidth(), m_pImageFile[1]->GetHeight());
	sprintf(szFileName, "%s\\data\\2012-01-07_15-56-08_766.bmp", szAPPath);
	m_pImageFile[2]->Load(szFileName);
	m_pImageFile[2]->RGB2BGR(m_pImageFile[2]->GetBuffer(), m_pImageFile[2]->GetWidth(), m_pImageFile[2]->GetHeight());
	sprintf(szFileName, "%s\\data\\2012-01-07_15-56-41_596.bmp", szAPPath);
	m_pImageFile[3]->Load(szFileName);
	m_pImageFile[3]->RGB2BGR(m_pImageFile[3]->GetBuffer(), m_pImageFile[3]->GetWidth(), m_pImageFile[3]->GetHeight());
}

void VSFServerDLLTester::Start()
{
	int iPort = 2019;
	if (m_pVSFServerDLL)
		m_pVSFServerDLL->Start(iPort);
}

void VSFServerDLLTester::Stop()
{
	if (m_pVSFServerDLL)
		m_pVSFServerDLL->Stop();
}

void VSFServerDLLTester::SetQuality(int iValue)
{
	if (m_pVSFServerDLL)
		m_pVSFServerDLL->SetQuality(iValue);
}

void VSFServerDLLTester::SetFPS(int iValue)
{
	if (m_pVSFServerDLL)
		m_pVSFServerDLL->SetFPS(iValue);
}

void VSFServerDLLTester::TimeEvent()
{
	if (m_pVSFServerDLL)
	{
		//
		int iIndex = m_iFrameCount % 4;
		int iRet = m_pVSFServerDLL->InputVideo(m_pImageFile[iIndex]->GetBuffer(), m_pImageFile[iIndex]->GetWidth(), m_pImageFile[iIndex]->GetHeight());
		if (iRet == 1)
			m_iFrameCount++;

		m_pVSFServerDLL->TimeEvent();
	}
}
