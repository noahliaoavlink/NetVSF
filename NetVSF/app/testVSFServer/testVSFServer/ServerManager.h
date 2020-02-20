#ifndef _ServerManager_H_
#define _ServerManager_H_

#include "Server.h"
#include "NWin.h"
#include <list>
//#include "UDPServer.h"
#include "../../../Include/SQQueue.h"
#include "VideoEncoder.h"
#include "Splitter.h"
#include "../../../Include/MediaFileCommon.h"

/*
enum TCStatus
{
	TCS_STOP = 0,
	TCS_RUNNING,
	TCS_PAUSE,
};
*/

class ServerManager: public WinMsgCallback2
{	
		std::list<Server*> m_SocketList;

		int m_iErrorCode;
		
		CNWin* m_pServerWin;
		CNWin* m_pDispatchWin;

		SOCKET m_Socket;
		int m_iPort;

		//int m_iWaitCount;

		SQQueue* m_pVideoQueue;
//		SQQueue* m_pVideoQueueSN;
		VideoEncoder* m_pVideoEncoder;
		Splitter* m_pSplitter;

		int m_iCodecType;
		int m_iWidth;
		int m_iHeight;

//		UDPServer* m_pUDPServer;

		EventCallback* m_pEventCallback;

		HANDLE m_hTCThread;
		bool m_bIsAlive;
		CRITICAL_SECTION m_CriticalSection;
   protected:
   public:
		ServerManager();
        ~ServerManager();

		bool Startup();
		void Cleanup();

		bool Accept();

		bool Init(int iCodecType, int iWidth, int iHeight);
		int Open(int iPort);
		void Close();

		void DeleteObj(unsigned long ulID);
		void CloseClient(unsigned long ulID);
		void ReceiveData(unsigned long ulID);

		void SetSocket(void* pSocket,int iPort);

		bool IsAlive();
		bool IsWorking();
		void RemoveAll();
		int GetTotalOfClients();

		int UpdateVideo(unsigned char* pBuf, int w, int h);
		void SendVideoLoop();
		void SetQuality(int iQuality);
		float GetAvgFPS(int iIndex);

		void SetEventCallback(EventCallback* pObj);

		void* WinMsg(HWND hWnd,int iMsg,void* pParameter1,void* pParameter2);
};

#endif
