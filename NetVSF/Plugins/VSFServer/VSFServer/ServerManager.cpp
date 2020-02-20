#include "stdafx.h"
#include "ServerManager.h"

#include <Ws2tcpip.h>

#include "Mmsystem.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "wsock32.lib")

#define WM_SERVER_SOCKET_EVENT  WM_USER + 1
#define WM_CLIENT_SOCKET_EVENT  WM_USER + 2

DWORD WINAPI TCThread(LPVOID lpParam);

ServerManager::ServerManager()
{
	m_Socket = 0;
	m_pServerWin = 0;
	m_pDispatchWin = 0;

	m_bIsAlive = false;
	m_hTCThread = NULL;

	Startup();

	m_pVideoQueue = new SQQueue;
	m_pVideoQueue->Alloc(_VIDEO_BUFFER_SIZE,8);

	m_pVideoEncoder = new VideoEncoder;
	m_pSplitter = new Splitter;

	m_iCodecType = CT_NONE;
	m_iWidth = 0;
	m_iHeight = 0;

	m_pEventCallback = 0;

	InitializeCriticalSection(&m_CriticalSection);
}

ServerManager::~ServerManager()
{
	Cleanup();

	RemoveAll();

	if (m_pVideoQueue)
	{
		m_pVideoQueue->Reset();
		delete m_pVideoQueue;
	}

	if (m_pVideoEncoder)
		delete m_pVideoEncoder;

	if (m_pSplitter)
		delete m_pSplitter;

	//delete m_pPlayer;
	DeleteCriticalSection(&m_CriticalSection);
}

bool ServerManager::Startup()
{
	UUID uuid;
	UuidCreate(&uuid);
	char szServerWinName[256];
	char szDispatchWinName[256];

	sprintf(szServerWinName, "MSMServerWin_%d", uuid.Data1);
	sprintf(szDispatchWinName, "MSDispatchWin_%d", uuid.Data1);

	m_pServerWin = new CNWin(szServerWinName,0);
	m_pDispatchWin = new CNWin(szDispatchWinName,0);

	m_pServerWin->SetCallback(this);
	m_pDispatchWin->SetCallback(this);

	WORD wVersionRequested = MAKEWORD(1,1);
	WSADATA wsaData;
	if(WSAStartup(wVersionRequested,&wsaData) != 0)
	{
       m_iErrorCode = WSAGetLastError();
       return false;
	}

	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0)
	{
       m_iErrorCode = 20000;
       return false;
	}

	return true;
}

void ServerManager::Cleanup()
{
	if(m_pServerWin)
	{
		m_pServerWin->DestroyWindow();
		delete m_pServerWin;
	}

	if(m_pDispatchWin)
	{
		m_pDispatchWin->DestroyWindow();
		delete m_pDispatchWin;
	}

    WSACleanup();
}

bool ServerManager::Init(int iCodecType,int iWidth,int iHeight)
{
	m_iCodecType = iCodecType;
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	m_pSplitter->SetUnit(_SPLIT_SIZE);

	return true;
}

void ServerManager::SetFrameSize(int w, int h)
{
	m_iWidth = w;
	m_iHeight = h;
}


void ServerManager::SetEventCallback(EventCallback* pObj)
{
	m_pEventCallback = pObj;
}

int ServerManager::Open(int iPort)
{
	int rVal;
	hostent* localHost;
	char* szLocalIP;

	//create socket
	m_Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(m_Socket == INVALID_SOCKET)
	{
		m_iErrorCode = WSAGetLastError();
		WSACleanup();
        return false;
	}

	//fill in sockaddr_in struct 
	SOCKADDR_IN sin;
	sin.sin_family = PF_INET;
	sin.sin_port = htons(iPort);
	sin.sin_addr.s_addr = INADDR_ANY;

	//bind the socket
	rVal = bind(m_Socket, (LPSOCKADDR)&sin, sizeof(sin));
	if(rVal == SOCKET_ERROR)
	{
		m_iErrorCode = WSAGetLastError();
		WSACleanup();
		return false;
	}

	localHost = gethostbyname("");
	szLocalIP = inet_ntoa(*(struct in_addr *)*localHost->h_addr_list);

	//get socket to listen 
	rVal = listen(m_Socket, SOMAXCONN);
	if(rVal == SOCKET_ERROR)
	{
		m_iErrorCode = WSAGetLastError();
		WSACleanup();
		return false;
	}
	SetSocket((void*)m_Socket,iPort);

	m_bIsAlive = true;
	DWORD nThreadID;
	m_hTCThread = CreateThread(0, 0, TCThread, (void *)this, 0, &nThreadID);

	return 0;
}

void ServerManager::Close()
{
	char szMsg[512];

	m_bIsAlive = false;
	::WaitForSingleObject(m_hTCThread, 10000);

	sprintf(szMsg, "ServerManager::Close() %d 0\n", m_Socket);
	OutputDebugStringA(szMsg);

	RemoveAll();

	if(m_Socket)
		closesocket(m_Socket);
	m_Socket = 0;

	sprintf(szMsg, "ServerManager::Close() %d 1\n", m_Socket);
	OutputDebugStringA(szMsg);
}

//callback functions
bool ServerManager::Accept()
{
	char szMsg[512];
	SOCKET hSocket = accept(m_Socket, NULL, NULL);

	sprintf(szMsg,"ServerManager::Accept() %d\n",hSocket);
	OutputDebugStringA(szMsg);

	if(hSocket == INVALID_SOCKET)
	{
		m_iErrorCode = WSAGetLastError();
		WSACleanup();
		return false;
	}
	else
	{
		int iSize = 65536;
		setsockopt(hSocket,SOL_SOCKET,SO_SNDBUF,(const char *)&iSize,sizeof(int));
		setsockopt(hSocket,SOL_SOCKET,SO_RCVBUF,(const char *)&iSize,sizeof(int));

		Server* pObj = new Server;
		m_SocketList.push_back(pObj);

		pObj->SetSocket(hSocket);
		pObj->Init(m_iCodecType, m_iWidth, m_iHeight);
		/*
		Server* pCurItem = m_SocketList.back();
		pCurItem->SetSocket(hSocket);
		pCurItem->Init(m_iCodecType, m_iWidth, m_iHeight);
		*/

		WSAAsyncSelect(hSocket,m_pDispatchWin->GetSafeHwnd(),hSocket+30000,FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE|FD_ACCEPT);

		//Sleep(1000);
		pObj->Hello();
	}
	return true;
}

void ServerManager::DeleteObj(unsigned long ulID)
{
	if(m_SocketList.size() > 0)
	{
		std::list<Server*>::iterator it;
		for( it = m_SocketList.begin() ; it != m_SocketList.end(); it++ )
		{
			Server* pCurItem = *it;
			if(pCurItem)
			{
				if(pCurItem->GetSocket() == (ulID - 30000))
				{
					delete pCurItem;
					it = m_SocketList.erase(it);
					//break;
				}
			}
		}
	}
}

void ServerManager::CloseClient(unsigned long ulID)
{
	if(m_SocketList.size() > 0)
	{
		std::list<Server*>::iterator it;
		for( it = m_SocketList.begin() ; it != m_SocketList.end(); it++ )
		{
			Server* pCurItem = *it;
			if(pCurItem)
			{
				if(pCurItem->GetSocket() == (ulID - 30000))
				{
					pCurItem->Close();
					m_SocketList.erase(it);
					break;
				}
			}
		}
	}
}

void ServerManager::ReceiveData(unsigned long ulID)
{
	if(m_SocketList.size() > 0)
	{
		std::list<Server*>::iterator it;
		for( it = m_SocketList.begin() ; it != m_SocketList.end(); it++ )
		{
			Server* pCurItem = *it;
			if(pCurItem)
			{
				if(pCurItem->GetSocket() == (ulID - 30000))
				{
					pCurItem->Receive();
					break;
				}
			}
		}
	}
}

void ServerManager::SetSocket(void* pSocket,int iPort)
{
	m_Socket = (SOCKET)pSocket;
	m_iPort = iPort;
	WSAAsyncSelect(m_Socket,m_pServerWin->GetSafeHwnd(),WM_SERVER_SOCKET_EVENT,FD_READ|FD_WRITE|FD_CONNECT|FD_CLOSE|FD_ACCEPT);
}

bool ServerManager::IsAlive()
{
	return m_bIsAlive;
}


bool ServerManager::IsWorking()
{
	if (m_Socket)
		return true;
	return false;
}

void ServerManager::RemoveAll()
{
	if (m_SocketList.size() > 0)
	{
		std::list<Server*>::iterator it;
		//for (it = m_SocketList.begin(); it != m_SocketList.end(); it++)
		for (it = m_SocketList.begin(); it != m_SocketList.end(); it)
		{
			Server* pCurItem = *it;
			if (pCurItem)
			{
				pCurItem->Close();
				delete pCurItem;
			}
			it = m_SocketList.erase(it);
		}
	}

	m_SocketList.clear();
}

int ServerManager::GetTotalOfClients()
{
	return m_SocketList.size();
}

int ServerManager::UpdateVideo(unsigned char* pBuf,int w,int h)
{
	if (m_pVideoQueue->IsFulled())
		return -1;
	else
	{ 
		if (!m_pVideoEncoder->IsOpened())
			m_pVideoEncoder->Open(CT_JPEG, w, h);

		int iRet = m_pVideoEncoder->Encode(pBuf);

		if (iRet > 0)
		{
			m_pVideoQueue->Add(m_pVideoEncoder->GetOutBuffer(), iRet);
		}
		else
			return -2;
	}

	return 1;
}

void ServerManager::SendVideoLoop()
{
	EnterCriticalSection(&m_CriticalSection);

	if (m_pVideoQueue->GetTotal() > 0)
	{
		unsigned char* pBuf = m_pVideoQueue->GetNext();
		int iLen = m_pVideoQueue->GetLength();

		m_pSplitter->DoSplit(iLen);

		if (m_SocketList.size() > 0)
		{

			char szMsg[512];
			sprintf(szMsg,"Splitter - %d\n", m_pSplitter->GetTotal());
			OutputDebugStringA(szMsg);

			for (int i = 0; i < m_pSplitter->GetTotal(); i++)
			{
				UnitPositionInfo* pUnitPosInfo = m_pSplitter->GetItemInfo(i);
				std::list<Server*>::iterator it;
				for (it = m_SocketList.begin(); it != m_SocketList.end(); it++)
				{
					Server* pCurItem = *it;
					if (pCurItem)
					{
						if (pCurItem->GetStatus() == VSFS_PLAY)
						{
							int iRet = pCurItem->AddVideoPacket(pBuf, pUnitPosInfo->ulStart, pUnitPosInfo->ulEnd, pUnitPosInfo->usSN, pUnitPosInfo->bIsEnded);

							if (iRet < 0)
							{
								sprintf(szMsg, "VSFError - SendVideoLoop - AddVideoPacket [%d]\n", iRet);
								OutputDebugStringA(szMsg);
							}
						}
						else if (pCurItem->GetStatus() == VSFS_FAIL)
							pCurItem->ResendHello();
					}
				}
//				Sleep(1);
			}

//			if (m_pEventCallback)
//				m_pEventCallback->Event(CCE_DISPLAY_VIDEO_FRAME, pBuf, &iLen);
		}
	}

	Sleep(1);
	LeaveCriticalSection(&m_CriticalSection);
}

void ServerManager::SetQuality(int iQuality)
{
	m_pVideoEncoder->SetQuality(iQuality);
}

float ServerManager::GetAvgFPS(int iIndex)
{
	if (m_SocketList.size() > 0)
	{
		int iItemIndex = 0;
		std::list<Server*>::iterator it;
		for (it = m_SocketList.begin(); it != m_SocketList.end(); it++)
		{
			Server* pCurItem = *it;
			if (pCurItem)
			{
				if (iItemIndex == iIndex)
				{
					return pCurItem->GetAvgFPS();
				}
			}

			iItemIndex++;
		}
	}
	return 0.0;
}

void* ServerManager::WinMsg(HWND hWnd,int iMsg,void* pParameter1,void* pParameter2)
{
	if(hWnd == m_pServerWin->GetSafeHwnd())
	{
		switch(WSAGETSELECTEVENT(pParameter2))  //lParam
		{
		   case FD_CONNECT:
			   {
				   bool bConnected = true;
				   if(WSAGETSELECTERROR(pParameter2)) //lParam //cannot connect to..
					   bConnected = false;
			   }
			   break;
		   case FD_READ:
			  break;
		   case FD_WRITE:
			  break;
		   case FD_CLOSE:
			  break;
		   case FD_ACCEPT:
			   Accept();
			 break;
		   default:
			   ;
		}
	}
	else if (hWnd == m_pDispatchWin->GetSafeHwnd())
	{
		switch(WSAGETSELECTEVENT(pParameter2))
		{
		   case FD_CONNECT:
			   {
				   bool bConnected = true;
				   if(WSAGETSELECTERROR(pParameter2))//lParam //cannot connect to..
					   bConnected = false;
			   }
			   break;
		   case FD_READ:
			   {
				   //OutputDebugString("FD_READ..\n");
				   bool bBreak = false;
				   if(WSAGETSELECTERROR(pParameter2))
					   bBreak = true;
				   ReceiveData(iMsg);
			   }
			  break;
		   case FD_WRITE:
			  break;
		   case FD_CLOSE:
			   {
				   OutputDebugStringA("FD_CLOSE..\n");
				   CloseClient(iMsg);
				   //DeleteObj(iMsg);
			   }
			  break;
		   case FD_ACCEPT:
			 break;
		   default:
			   ;
		}
	}
	
	return 0;
}

DWORD WINAPI TCThread(LPVOID lpParam)
{
	ServerManager* pObj = (ServerManager*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->SendVideoLoop();
		Sleep(1);
	}
	return 0;
}


