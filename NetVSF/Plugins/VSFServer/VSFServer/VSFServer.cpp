// VSFServer.cpp: 定義 DLL 的初始化常式。
//

#include "stdafx.h"
#include "VSFServer.h"
#include "StreamingCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  如果這個 DLL 是動態地對 MFC DLL 連結，
//		那麼從這個 DLL 匯出的任何會呼叫
//		MFC 內部的函式，都必須在函式一開頭加上 AFX_MANAGE_STATE
//		巨集。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此處為正常函式主體
//		}
//
//		這個巨集一定要出現在每一個
//		函式中，才能夠呼叫 MFC 的內部。這意味著
//		其必須是函式內的第一個陳述式
//		，甚至必須在任何物件變數宣告前面
//		，因為它們的建構函式可能會產生對 MFC
//		DLL 內部的呼叫。
//
//		請參閱 MFC 技術提示 33 和 58 中的
//		詳細資料。
//

// CVSFServerApp

BEGIN_MESSAGE_MAP(CVSFServerApp, CWinApp)
END_MESSAGE_MAP()


// CVSFServerApp 建構

CVSFServerApp::CVSFServerApp()
{
	// TODO:  在此加入建構程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
}


// 唯一一個 CVSFServerApp 物件

CVSFServerApp theApp;


// CVSFServerApp 初始化

BOOL CVSFServerApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec(dllexport)void* _Net_VSFS_Create()
{
	return new StreamingCtrl;
}

extern "C" __declspec(dllexport)void _Net_VSFS_Destroy(void* pVSFSObj)
{
	delete (StreamingCtrl*)pVSFSObj;
}

extern "C" __declspec(dllexport)void _Net_VSFS_Init(void* pVSFSObj,HWND hWnd)
{
	((StreamingCtrl*)pVSFSObj)->Init(hWnd);
}

extern "C" __declspec(dllexport)void _Net_VSFS_Start(void* pVSFSObj,int iPort)
{
	((StreamingCtrl*)pVSFSObj)->Start(iPort);
}

extern "C" __declspec(dllexport)void _Net_VSFS_Stop(void* pVSFSObj)
{
	((StreamingCtrl*)pVSFSObj)->Stop();
}

extern "C" __declspec(dllexport)void _Net_VSFS_SetQuality(void* pVSFSObj, int iValue)
{
	((StreamingCtrl*)pVSFSObj)->SetQuality(iValue);
}

extern "C" __declspec(dllexport)void _Net_VSFS_SetFPS(void* pVSFSObj, int iValue)
{
	((StreamingCtrl*)pVSFSObj)->SetFPS(iValue);
}

extern "C" __declspec(dllexport)void _Net_VSFS_UpdateVideo(void* pVSFSObj)
{
#ifndef _INPUT_VIDEO_MODE
	((StreamingCtrl*)pVSFSObj)->UpdateVideo();
#endif
}

extern "C" __declspec(dllexport)void _Net_VSFS_UpdateVideo2(void* pVSFSObj)
{
#ifndef _INPUT_VIDEO_MODE
	((StreamingCtrl*)pVSFSObj)->UpdateVideo2();
#endif
}

extern "C" __declspec(dllexport)void _Net_VSFS_TimeEvent(void* pVSFSObj)
{
	((StreamingCtrl*)pVSFSObj)->TimeEvent();
}

extern "C" __declspec(dllexport)int _Net_VSFS_InputVideo(void* pVSFSObj,unsigned char* pBuffer, int w, int h)
{
	return ((StreamingCtrl*)pVSFSObj)->InputVideo(pBuffer,w,h);
}