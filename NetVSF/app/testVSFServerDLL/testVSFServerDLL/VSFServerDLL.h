#pragma once

#ifndef _VSFServerDLL_H
#define _VSFServerDLL_H

typedef void* (*_Net_VSFS_Create_Proc)();
typedef void (*_Net_VSFS_Destroy_Proc)(void* pVSFSObj);

typedef void (*_Net_VSFS_Init_Proc)(void* pVSFSObj, HWND hWnd);
typedef void (*_Net_VSFS_Start_Proc)(void* pVSFSObj, int iPort);
typedef void (*_Net_VSFS_Stop_Proc)(void* pVSFSObj);
typedef void (*_Net_VSFS_SetQuality_Proc)(void* pVSFSObj, int iValue);
typedef void (*_Net_VSFS_SetFPS_Proc)(void* pVSFSObj, int iValue);
//typedef void (*_Net_VSFS_UpdateVideo_Proc)(void* pVSFSObj);
//typedef void (*_Net_VSFS_UpdateVideo2_Proc)(void* pVSFSObj);
typedef void (*_Net_VSFS_TimeEvent_Proc)(void* pVSFSObj);
typedef int (*_Net_VSFS_InputVideo_Proc)(void* pVSFSObj, unsigned char* pBuffer, int w, int h);

class VSFServerDLL
{
	HINSTANCE m_hInst;
	void* m_pVSFSObj;
public:
	VSFServerDLL();
	~VSFServerDLL();

	bool LoadLib();
	void FreeLib();

	void Init(HWND hWnd);
	void Start(int iPort);
	void Stop();
	void SetQuality(int iValue);
	void SetFPS(int iValue);
	//void UpdateVideo();
	//void UpdateVideo2();
	void TimeEvent();
	int InputVideo(unsigned char* pBuffer, int w, int h);

	_Net_VSFS_Create_Proc _Net_VSFS_Create;
	_Net_VSFS_Destroy_Proc _Net_VSFS_Destroy;
	_Net_VSFS_Init_Proc _Net_VSFS_Init;
	_Net_VSFS_Start_Proc _Net_VSFS_Start;
	_Net_VSFS_Stop_Proc _Net_VSFS_Stop;
	_Net_VSFS_SetQuality_Proc _Net_VSFS_SetQuality;
	_Net_VSFS_SetFPS_Proc _Net_VSFS_SetFPS;
	//_Net_VSFS_UpdateVideo_Proc _Net_VSFS_UpdateVideo;
	//_Net_VSFS_UpdateVideo2_Proc _Net_VSFS_UpdateVideo2;
	_Net_VSFS_TimeEvent_Proc _Net_VSFS_TimeEvent;
	_Net_VSFS_InputVideo_Proc _Net_VSFS_InputVideo;
};

#endif