#include "stdafx.h"
#include "Client.h"
#include "Mmsystem.h"

#pragma comment(lib, "Winmm.lib")

#define TIMEOUT 3000

unsigned int __stdcall RcvThreadProc(void* lpvThreadParm);
unsigned int __stdcall ProcessThreadProc(void* lpvThreadParm);

unsigned long getAddrFromIP(char *ip)
{
	if (ip)
	{
		if (isalpha(*ip))
		{
			HOSTENT *host;
			host = gethostbyname(ip);
			if (host)
				return ((LPIN_ADDR)host->h_addr)->s_addr;
		}
		return inet_addr(ip);
	}
	return htonl(INADDR_ANY);
}

Client::Client()
{
	m_dwIoctlLastTime = 0;
	m_iSelectTimeOutCount = 0;

	m_bIsPassed = false;

	m_pRcvBuffer = new char[_MAX_RCV_BUFFER];
	m_pBuffer = new char[_MAX_RCV_BUFFER];

	m_pQBuffer = new QBufferManagerEx;
	m_pQBuffer->SetMax(400);
	m_pQBuffer->SetBufferSize(_MAX_RCV_BUFFER);

	m_pPacketParser = new PacketParser;

	m_iCodecType = 0;
	m_iWidth = 0;
	m_iHeight = 0;
	m_ulVideoBufLen = 0;
	m_ulTmpVideoBufLen = 0;
	m_lLastSN = -1;
	m_ulPacketCount = 0;

	m_pVideoBuffer = new unsigned char[_VIDEO_BUFFER_SIZE];
	m_pTmpVideoBuffer = new unsigned char[_VIDEO_BUFFER_SIZE];

	m_pVideoDecoder = new VideoDecoder;

	m_pEventCallback = 0;

	InitializeCriticalSection(&m_RcvCriticalSection);
	InitializeCriticalSection(&m_CriticalSection);
}

Client::~Client()
{
	Cleanup();

	delete m_pQBuffer;

	if (m_pRcvBuffer)
		delete m_pRcvBuffer;

	if (m_pBuffer)
		delete m_pBuffer;

	if (m_pVideoBuffer)
		delete m_pVideoBuffer;

	if (m_pTmpVideoBuffer)
		delete m_pTmpVideoBuffer;

	if (m_pPacketParser)
		delete m_pPacketParser;

	if (m_pVideoDecoder)
		delete m_pVideoDecoder;

	DeleteCriticalSection(&m_RcvCriticalSection);
	DeleteCriticalSection(&m_CriticalSection);
}

bool Client::Startup()
{
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		m_iErrorCode = WSAGetLastError();
		return false;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0)
	{
		m_iErrorCode = 20000;
		return false;
	}
	return true;
}

void Client::Cleanup()
{
	if (m_hSocket)
		Close();

	WSACleanup();
}

void Client::SetEventCallback(EventCallback* pObj)
{
	m_pEventCallback = pObj;
}

bool Client::Connect(char* szIP, int iPort)
{
	char szMsg[512];
	sprintf(szMsg, "VSFClient::Connect() 0\n");
	OutputDebugStringA(szMsg);

	m_bIsAlive = true;

	strcpy(m_szIP, szIP);
	m_iPort = iPort;

	if ((m_hSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		m_iErrorCode = WSAGetLastError();
		WSACleanup();

		sprintf(szMsg, "VSFClient::Connect() -1\n");
		OutputDebugStringA(szMsg);

		return false;
	}

	//int size = 65535;
	//setsockopt(m_Socket,SOL_SOCKET,SO_RCVBUF,(const char *)&size,sizeof(int));
	int iSize = 65536;
	setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (const char *)&iSize, sizeof(int));
	setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (const char *)&iSize, sizeof(int));

	struct sockaddr_in dst_addr;
	dst_addr.sin_family = PF_INET;
	dst_addr.sin_addr.s_addr = getAddrFromIP((char*)szIP);//inet_addr(szIPAddr);
	dst_addr.sin_port = htons(iPort);

	unsigned long ulArgp = 1;  //Non-blocking
	int iRet = ioctlsocket(m_hSocket, FIONBIO, &ulArgp);
	if (!iRet)
	{
		if (connect(m_hSocket, (struct sockaddr FAR *)&dst_addr, sizeof(dst_addr)) != 0)
		{
			m_iErrorCode = WSAGetLastError();
			if (m_iErrorCode != WSAEWOULDBLOCK)
			{
				Close();

				sprintf(szMsg, "VSFClient::Connect() -2\n");
				OutputDebugStringA(szMsg);
				return false;
			}
			else
			{
				char szMsg[512];
				// nonblocking connect
				struct timeval oTV;
				oTV.tv_sec = TIMEOUT / 1000;
				oTV.tv_usec = TIMEOUT;
				fd_set oRead, oWrite;
				FD_ZERO(&oRead);
				FD_ZERO(&oWrite);
				int nResult, nError;

				FD_SET(m_hSocket, &oRead);
				oWrite = oRead;
				nResult = select(m_hSocket + 1, &oRead, &oWrite, 0, &oTV);
				if (nResult == 0)
				{
					//printf("Connection timeout\n");
					sprintf(szMsg, "VSFClient - Connection timeout\n");
					OutputDebugStringA(szMsg);
					closesocket(m_hSocket);
					return false;
				}
				if (FD_ISSET(m_hSocket, &oRead) || FD_ISSET(m_hSocket, &oWrite))
				{
					int nLen = sizeof(nError);
					if (getsockopt(m_hSocket, SOL_SOCKET, SO_ERROR, (char*)&nError, &nLen) < 0)
					{
						//printf("Connect error %d\n", nError);
						sprintf(szMsg, "VSFClient - Connect error %d\n", nError);
						OutputDebugStringA(szMsg);
						closesocket(m_hSocket);
						return false;
					}
				}
				else
				{
					//printf("Unknown err in connect\n");
					sprintf(szMsg, "VSFClient - Unknown err in connect\n");
					OutputDebugStringA(szMsg);
					closesocket(m_hSocket);
					return false;
				}
			}
		}
	}
	
	unsigned threadID;

	m_hRcvThread = (HANDLE)_beginthreadex(NULL, 0, RcvThreadProc, this, 0, &threadID);
	m_hProcessThread = (HANDLE)_beginthreadex(NULL, 0, ProcessThreadProc, this, 0, &threadID);

	sprintf(szMsg, "VSFClient::Connect() 1\n");
	OutputDebugStringA(szMsg);
	return true;
}

void Client::Close()
{
	char szMsg[512];
	sprintf(szMsg, "VSFClient::Close() 0\n");
	OutputDebugStringA(szMsg);

	m_bIsAlive = false;

	::WaitForSingleObject(m_hRcvThread, 10000);
	::WaitForSingleObject(m_hProcessThread, 10000);

	Stop();

	if (m_hSocket)
	{
		closesocket(m_hSocket);
//		shutdown(m_hSocket, 2);
	}

	m_hSocket = 0;
//	m_bConnected = false;
	m_bIsPassed = false;

	m_pQBuffer->Reset();
	ResetRcvStatus();

	sprintf(szMsg, "VSFClient::Close() 1\n");
	OutputDebugStringA(szMsg);
}

void Client::DoLogin()
{
	char szMsg[512];
	sprintf(szMsg, "VSFClient::DoLogin() - hello!!\n");
	OutputDebugStringA(szMsg);

	Login("", "");
}

void Client::Login(char* szID, char* szPW)
{
	char szMsg[512];
	char szID2[64]; //= "bruvis";
	char szPW2[64];// = "avlink";

	if (strcmp(szID, "") == 0 && strcmp(szPW, "") == 0)
	{
		strcpy(szID2, "bruvis");
		strcpy(szPW2, "avlink");
	}
	else
	{
		strcpy(szID2, szID);
		strcpy(szPW2, szPW);
	}

	strcpy(m_szID, szID2);
	strcpy(m_szPW, szPW2);

	PacketMaker packet_maker;
	packet_maker.Make(NID_LOGIN, (unsigned char*)szID2, strlen(szID2), (unsigned char*)szPW2, strlen(szPW2));

	m_dwLoginStartTime = timeGetTime();

	sprintf(szMsg, "TC Client::Login() - [%s , %s]\n", szID2, szPW2);
	OutputDebugStringA(szMsg);

	send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
}

void Client::UpdateTimestamp()
{
	PacketMaker packet_maker;
	packet_maker.Make(NID_UPDATE_TIMESTAMP);
	send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
}

void Client::SetupRet(PacketItem* pPacketItem)
{
	m_iCodecType = pPacketItem->lData[0];
	m_iWidth = pPacketItem->lData[1];
	m_iHeight = pPacketItem->lData[2];

	m_pVideoDecoder->Close();
	m_pVideoDecoder->Open(m_iCodecType, m_iWidth, m_iHeight);
}

void Client::Setup()
{
	PacketMaker packet_maker;
	packet_maker.Make(NID_SETUP);

	send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);	
}

void Client::Play()
{
	PacketMaker packet_maker;
	packet_maker.Make(NID_PLAY);

	send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
}

void Client::Stop()
{
	PacketMaker packet_maker;
	packet_maker.Make(NID_STOP);

	send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
}

void Client::RecvVideoPacket(PacketItem* pPacketItem)
{
	char szMsg[512];

	memcpy(m_pTmpVideoBuffer + m_ulTmpVideoBufLen, pPacketItem->pBuf, pPacketItem->lData[3]);

	m_ulTmpVideoBufLen += pPacketItem->lData[3];

	long lCurSN = pPacketItem->lData[2];

	if (abs(lCurSN - m_lLastSN) > 1)
	{
		sprintf(szMsg, "VSFClient::RecvVideoPacket() - loss packet : [m_lLastSN:%d , lCurSN:%d]\n", m_lLastSN, lCurSN);
		OutputDebugStringA(szMsg);
	}

	if (m_lLastSN != pPacketItem->lData[2])
	{
		m_lLastSN = pPacketItem->lData[2];
	}
	else
		;//  //error

	if (pPacketItem->lData[1] == 1) //mark
	{
		int iRet = m_pVideoDecoder->Decode(m_pTmpVideoBuffer, m_ulTmpVideoBufLen);

		if (iRet >= 1)
		{
			int iLen = m_pVideoDecoder->GetBufLen();
			if (m_pEventCallback)
				m_pEventCallback->Event(CCE_DISPLAY_VIDEO_FRAME,m_pVideoDecoder->GetOutBuffer(),&iLen);
		}

		m_ulVideoBufLen = 0;
		m_ulTmpVideoBufLen = 0;

		UpdateTimestamp();
	}
}

int Client::GetWidth()
{
	return m_iWidth;
}

int Client::GetHeight()
{
	return m_iHeight;
}

bool Client::IsAlive()
{
	return m_bIsAlive;
}

void Client::Checkioctl()
{
	char szMsg[512];
	int nResult, nError;
	struct timeval oTV;

	if (TIMEOUT >= 1000)
	{
		oTV.tv_sec = TIMEOUT / 1000;
		oTV.tv_usec = TIMEOUT - (oTV.tv_sec * 1000);
	}
	else
	{
		oTV.tv_sec = 0;
		oTV.tv_usec = TIMEOUT;
	}

	fd_set oRead;
	FD_ZERO(&oRead);
	FD_SET(m_hSocket, &oRead);

	m_dwIoctlLastTime = timeGetTime();

	try
	{
		nResult = select(m_hSocket + 1, &oRead, 0, 0, &oTV);
	}
	catch (...)
	{
		OutputDebugStringA("VSFError - Catching an exception in Checkioctl - select !! \n");
		//				::MessageBoxA(NULL, "LoadObj() Failed!!", "", MB_OK);
	}

	if (nResult == SOCKET_ERROR)
	{
		int iError = WSAGetLastError();
		sprintf(szMsg, "TC Client::Checkioctl() - select Error : %d\n", iError);
		OutputDebugStringA(szMsg);
	}

	if (nResult == 0)
	{
		m_iSelectTimeOutCount++;
		sprintf(szMsg, "TC Client::Checkioctl() - select timeout!!(%d)\n", m_iSelectTimeOutCount);
		OutputDebugStringA(szMsg);

		if (m_iSelectTimeOutCount > 60)
		{
			Close();
/*
			if (m_bEnableReconnect)
			{
				m_bDoReconnect = true;
				m_iReconnectStatus = RS_PING;// RS_RECONNECT;

				unsigned threadID;
				HANDLE hReconnectThread = (HANDLE)_beginthreadex(NULL, 0, ReconnectThreadProc, this, 0, &threadID);
			}
			*/
		}
	}

	if (FD_ISSET(m_hSocket, &oRead))
	{
		Receive();	
	}
}

void Client::Receive()
{
	int iLen = -1;
	try
	{
		iLen = recv(m_hSocket, m_pRcvBuffer, _MAX_RCV_BUFFER, 0);
	}
	catch (...)
	{
		OutputDebugStringA("VSFError - Catching an exception in Receive - recv !! \n");
		//				::MessageBoxA(NULL, "LoadObj() Failed!!", "", MB_OK);
	}

	if (iLen <= 0)
	{
		Close();
		return;
	}

	if (iLen > 0)
	{
		bool bRet = m_pQBuffer->Add(m_pRcvBuffer, iLen);

		if (!bRet)
		{
			char szMsg[512];
			sprintf(szMsg, "TCClient::Receive() - m_pQBuffer->Add failed : bRet = %d\n", bRet);
			OutputDebugStringA(szMsg);
		}
	}

	if (m_ulPacketCount % 16 == 0)
		Sleep(1);

	m_ulPacketCount++;
}

void Client::ReceiveLoop()
{
	EnterCriticalSection(&m_RcvCriticalSection);
	Checkioctl();
	LeaveCriticalSection(&m_RcvCriticalSection);
}

void Client::CheckReceiveStatus()
{
	char szMsg[512];
	DWORD dwCurTime = timeGetTime();
}

void Client::ResetRcvStatus()
{
	m_dwIoctlLastTime = 0;
	m_iSelectTimeOutCount = 0;
}

void Client::ProcessRecvData()
{
	char szMsg[1024];
	EnterCriticalSection(&m_CriticalSection);

	if (m_pQBuffer->GetTotal() > 0)
	{
		int iRet;
		ZeroMemory(m_pBuffer, _MAX_RCV_BUFFER);
		int iLen = m_pQBuffer->GetNext((unsigned char *)m_pBuffer);

		m_pPacketParser->Input((unsigned char *)m_pBuffer, iLen);

		bool bDo = true;
		while (bDo)
		{
			iRet = m_pPacketParser->Parse();
			if (iRet == -1 || iRet == 0)
				bDo = false;
			else
			{
				PacketItem* pCurPacketItem = m_pPacketParser->GetPacketItem();
				if (pCurPacketItem)
				{
					switch (pCurPacketItem->lID)
					{
					case NID_HELLO:
						DoLogin();
						break;
					case NID_LOGIN_RET:
						{
							ResetRcvStatus();
							DWORD dwLoginEndTime = timeGetTime();

							sprintf(szMsg, "VSFClient::ProcessRecvData() - CID_LOGIN_RET spend_time:%d\n", dwLoginEndTime - m_dwLoginStartTime);
							OutputDebugStringA(szMsg);

							if (pCurPacketItem->lData[0] == LR_SUCESSED)
								m_bIsPassed = true;
							else if (pCurPacketItem->lData[0] == LR_FAILED)
								m_bIsPassed = false;

							if(m_bIsPassed)
								Setup();
						}
					break;
					case NID_SETUP_RET:
						{
							SetupRet(pCurPacketItem);
							Play();
						}
						break;
					case NID_VIDEO_PACKET:
						{
							RecvVideoPacket(pCurPacketItem);
						}
						break;
					default:
					{
						sprintf(szMsg, "VSFClient::ProcessRecvData() - unknow packet!! [%d]\n", pCurPacketItem->lID);
						OutputDebugStringA(szMsg);
					}
					break;
					}
				}
			}
//			Sleep(1);
		}
	}

	//	Sleep(1);
	LeaveCriticalSection(&m_CriticalSection);
}

unsigned int __stdcall RcvThreadProc(void* lpvThreadParm)
{

	Client* pNetObj = (Client*)lpvThreadParm;
	while (pNetObj->IsAlive())
	{
		pNetObj->ReceiveLoop();
		Sleep(1);
	}

	return 0;
}

unsigned int __stdcall ProcessThreadProc(void* lpvThreadParm)
{

	Client* pNetObj = (Client*)lpvThreadParm;
	while (pNetObj->IsAlive())
	{

		pNetObj->CheckReceiveStatus();
		pNetObj->ProcessRecvData();
		pNetObj->ProcessRecvData();

		Sleep(1);
	}


	return 0;
}