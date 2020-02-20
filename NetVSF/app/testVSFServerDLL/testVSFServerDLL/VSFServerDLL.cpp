#include "stdafx.h"
#include "VSFServerDLL.h"

VSFServerDLL::VSFServerDLL()
{
	m_hInst = 0;
	m_pVSFSObj = 0;
}

VSFServerDLL::~VSFServerDLL()
{
	if (m_pVSFSObj)
		_Net_VSFS_Destroy(m_pVSFSObj);

	FreeLib();
}

bool VSFServerDLL::LoadLib()
{
	char szFileName[] = "VSFServer.dll";
	m_hInst = LoadLibraryA(szFileName);

	if (m_hInst)
	{
		_Net_VSFS_Create = (_Net_VSFS_Create_Proc)GetProcAddress(m_hInst, "_Net_VSFS_Create");
		_Net_VSFS_Destroy = (_Net_VSFS_Destroy_Proc)GetProcAddress(m_hInst, "_Net_VSFS_Destroy");

		_Net_VSFS_Init = (_Net_VSFS_Init_Proc)GetProcAddress(m_hInst, "_Net_VSFS_Init");
		_Net_VSFS_Start = (_Net_VSFS_Start_Proc)GetProcAddress(m_hInst, "_Net_VSFS_Start");
		_Net_VSFS_Stop = (_Net_VSFS_Stop_Proc)GetProcAddress(m_hInst, "_Net_VSFS_Stop");
		_Net_VSFS_SetQuality = (_Net_VSFS_SetQuality_Proc)GetProcAddress(m_hInst, "_Net_VSFS_SetQuality");
		_Net_VSFS_SetFPS = (_Net_VSFS_SetFPS_Proc)GetProcAddress(m_hInst, "_Net_VSFS_SetFPS");
		//_Net_VSFS_UpdateVideo = (_Net_VSFS_UpdateVideo_Proc)GetProcAddress(m_hInst, "_Net_VSFS_UpdateVideo");
		//_Net_VSFS_UpdateVideo2 = (_Net_VSFS_UpdateVideo2_Proc)GetProcAddress(m_hInst, "_Net_VSFS_UpdateVideo2");
		_Net_VSFS_TimeEvent = (_Net_VSFS_TimeEvent_Proc)GetProcAddress(m_hInst, "_Net_VSFS_TimeEvent");

		_Net_VSFS_InputVideo = (_Net_VSFS_InputVideo_Proc)GetProcAddress(m_hInst, "_Net_VSFS_InputVideo");

		m_pVSFSObj = _Net_VSFS_Create();
		return true;
	}
	return false;
}

void VSFServerDLL::FreeLib()
{
	if (m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = 0;
	}
}

void VSFServerDLL::Init(HWND hWnd)
{
	if (m_pVSFSObj)
		_Net_VSFS_Init(m_pVSFSObj, hWnd);
}

void VSFServerDLL::Start(int iPort)
{
	if (m_pVSFSObj)
		_Net_VSFS_Start(m_pVSFSObj, iPort);
}

void VSFServerDLL::Stop()
{
	if (m_pVSFSObj)
		_Net_VSFS_Stop(m_pVSFSObj);
}

void VSFServerDLL::SetQuality(int iValue)
{
	if (m_pVSFSObj)
		_Net_VSFS_SetQuality(m_pVSFSObj, iValue);
}

void VSFServerDLL::SetFPS(int iValue)
{
	if (m_pVSFSObj)
		_Net_VSFS_SetFPS(m_pVSFSObj, iValue);
}

/*
void VSFServerDLL::UpdateVideo()
{
	if (m_pVSFSObj)
		_Net_VSFS_UpdateVideo(m_pVSFSObj);
}

void VSFServerDLL::UpdateVideo2()
{
	if (m_pVSFSObj)
		_Net_VSFS_UpdateVideo2(m_pVSFSObj);
}
*/

void VSFServerDLL::TimeEvent()
{
	if (m_pVSFSObj)
		_Net_VSFS_TimeEvent(m_pVSFSObj);
}

int VSFServerDLL::InputVideo(unsigned char* pBuffer, int w, int h)
{
	if (m_pVSFSObj)
		return _Net_VSFS_InputVideo(m_pVSFSObj,pBuffer,w,h);
	return 0;
}