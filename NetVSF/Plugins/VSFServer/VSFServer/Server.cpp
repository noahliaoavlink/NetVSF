#include "stdafx.h"
#include "Server.h"
#include "Mmsystem.h"

#pragma comment(lib, "Winmm.lib")

DWORD WINAPI RecvThread(LPVOID lpParam);

#ifdef _ENABLE_MSG_Q
DWORD WINAPI SendMsgThread(LPVOID lpParam);
#endif

Server::Server()
{
	m_hSocket = 0;

	m_pRcvBuffer = new char[_MAX_RCV_BUFFER];
	//m_pSendBuffer = new char[_MAX_SEND_BUFFER];
	m_pBuffer = new char[_MAX_RCV_BUFFER];

	m_pQBuffer = new QBufferManagerEx;
	m_pQBuffer->SetMax(5);
	m_pQBuffer->SetBufferSize(_MAX_RCV_BUFFER);

	m_pPacketParser = new PacketParser;
	m_bIsPassed = false;

	m_iCodecType = CT_NONE;
	m_iWidth = 0;
	m_iHeight = 0;

	m_iStatus = VSFS_FAIL;
	m_ulLastTimestamp = 0;
	m_bUpdateTimestamp = true;
	m_iFrameCount = 0;

	m_FPSAvg.SetMax(12);
	m_FPSAvg.SetConditionType(ACT_LOW); //ACT_LOW

#ifdef _ENABLE_MSG_Q
	m_pMsgQueue = new SQQueue;
	m_pMsgQueue->Alloc(sizeof(MsgItem), 400);

	InitializeCriticalSection(&m_MsgQCriticalSection);
#endif

	m_hRecvThread = 0;
	InitializeCriticalSection(&m_CriticalSection);
}

Server::~Server()
{
	m_bIsAlive = false;
	::WaitForSingleObject(m_hRecvThread,10000);

#ifdef _ENABLE_MSG_Q
	::WaitForSingleObject(m_hSendMsgThread, 10000);
#endif

	delete m_pRcvBuffer;
	//delete m_pSendBuffer;
	delete m_pBuffer;
//	delete m_pCmdParser;
	delete m_pQBuffer;

	if (m_pPacketParser)
		delete m_pPacketParser;

#ifdef _ENABLE_MSG_Q
	if(m_pMsgQueue)
		delete m_pMsgQueue;
	DeleteCriticalSection(&m_MsgQCriticalSection);
#endif

	DeleteCriticalSection(&m_CriticalSection);
}

void Server::Init(int iCodecType, int iWidth, int iHeight)
{
	m_iCodecType = iCodecType;
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	m_bIsAlive = true;
	DWORD nThreadID;
	m_hRecvThread = CreateThread(0, 0, RecvThread, (void *)this, 0, &nThreadID);

#ifdef _ENABLE_MSG_Q
	m_hSendMsgThread = CreateThread(0, 0, SendMsgThread, (void *)this, 0, &nThreadID);
#endif
}

bool Server::IsAlive()
{
	return m_bIsAlive;
}

void Server::SetSocket(SOCKET hSocket)
{
	m_hSocket = hSocket;
}

SOCKET Server::GetSocket()
{
	return m_hSocket;
}

void Server::Close()
{
	char szMsg[1024];
	sprintf(szMsg,"Server::Close()\n");
	OutputDebugStringA(szMsg);

	m_bIsAlive = false;

	::WaitForSingleObject(m_hRecvThread, 10000);

#ifdef _ENABLE_MSG_Q
	::WaitForSingleObject(m_hSendMsgThread, 10000);
#endif

	if(m_hSocket)
		closesocket(m_hSocket);
	m_hSocket = 0;
}

void Server::Receive()
{
	ZeroMemory(m_pRcvBuffer,_MAX_RCV_BUFFER);
	int iLen = recv(m_hSocket,m_pRcvBuffer,_MAX_RCV_BUFFER,0);

	if(iLen > 0)
		bool bRet = m_pQBuffer->Add(m_pRcvBuffer,iLen);
}

void Server::ProcessRecvData()
{
	EnterCriticalSection(&m_CriticalSection);
	if(m_pQBuffer->GetTotal() > 0)
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
						case NID_LOGIN:
							Login((char*)pCurPacketItem->szString, (char*)pCurPacketItem->szString2);
							break;
						case NID_SETUP:
							Setup();
							break;
						case NID_PLAY:
							Play();
							break;
						case NID_STOP:
							Stop();
							break;
						case NID_UPDATE_TIMESTAMP:
							UpdateTimestamp();
							break;
						//case CID_LOGOUT:
							//break;
					}
				}
			}
		}
	}

	Sleep(1);
	LeaveCriticalSection(&m_CriticalSection);
}

#ifdef _ENABLE_MSG_Q
void Server::SendPacket()
{
	char szMsg[512];
	EnterCriticalSection(&m_MsgQCriticalSection);
	if (m_pMsgQueue->GetTotal() > 0)
	{
		MsgItem* pCurPacketItem = (MsgItem*)m_pMsgQueue->GetNext();
		if (pCurPacketItem)
		{
			PacketMaker packet_maker;
			switch (pCurPacketItem->lCmd)
			{
				case NID_LOGIN_RET:
					{
						packet_maker.Make(pCurPacketItem->lCmd, pCurPacketItem->lParm);
						int iRet = send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
					}
					break;
				case NID_SETUP_RET:
					{
						packet_maker.MakeVSFSetup(pCurPacketItem->lCmd, pCurPacketItem->lParm, pCurPacketItem->lParm2, pCurPacketItem->lParm3);
						int iRet = send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
					}
					break;
				case NID_VIDEO_PACKET:
					{
						if (m_bUpdateTimestamp)
						{
							m_bUpdateTimestamp = false;
							m_ulLastTimestamp = timeGetTime();
						}

						try
						{
							VSFVideoPacket video_packet;
							video_packet.ucVersion = 1;
							video_packet.ucMarker = pCurPacketItem->lParm4;
							video_packet.usSequenceNumber = pCurPacketItem->lParm3;
							video_packet.ulLength = pCurPacketItem->lParm2 - pCurPacketItem->lParm;
							video_packet.pBuf = pCurPacketItem->pBuf + pCurPacketItem->lParm;
							packet_maker.MakeVSFVideoPacket(pCurPacketItem->lCmd, &video_packet);
							int iBufLen = packet_maker.GetBufLen();
							int iRet = send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);

							if (iRet > 0 && iRet < iBufLen)
							{
								sprintf(szMsg, "VSFError - send Error!! iRet < iBufLen [%d < %d] 00\n", iRet, iBufLen);
								OutputDebugStringA(szMsg);
							}

							if (iRet < 0)
							{
								bool bFinished = false;

								for (int i = 0; i < 200; i++)
								{
									iRet = send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);

									if (iRet > 0 && iRet < iBufLen)
									{
										sprintf(szMsg, "VSFError - send Error!! iRet < iBufLen [%d < %d] 01\n", iRet, iBufLen);
										OutputDebugStringA(szMsg);
									}

									if (iRet > 0)
									{
										bFinished = true;
										break;
									}
									Sleep(1);
								}

								if (!bFinished)
								{
									
									sprintf(szMsg, "VSFError - send Error!! [%d]\n", iRet);
									OutputDebugStringA(szMsg);
								}
							}
						}
						catch (...)
						{
							OutputDebugStringA("VSFError - Catching an exception in SendPacket - NID_VIDEO_PACKET !! \n");
							//				::MessageBoxA(NULL, "LoadObj() Failed!!", "", MB_OK);
						}
					}
					break;
				default:
					{
						packet_maker.Make(pCurPacketItem->lCmd);
						int iBufLen = packet_maker.GetBufLen();

						if (pCurPacketItem->lCmd == NID_HELLO)
						{
							sprintf(szMsg, "VSFError - Server::SendPacket()!! hello \n");
							OutputDebugStringA(szMsg);
						}

						int iRet = send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
						if (iRet < 0)
						{
							bool bFinished = false;

							for (int i = 0; i < 200; i++)
							{
								iRet = send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);

								if (iRet > 0 && iRet < iBufLen)
								{
									sprintf(szMsg, "VSFError - send Error!! iRet < iBufLen [%d < %d] 01\n", iRet, iBufLen);
									OutputDebugStringA(szMsg);
								}

								if (iRet > 0)
								{
									bFinished = true;
									break;
								}
								Sleep(1);
							}

							if (!bFinished)
							{

								sprintf(szMsg, "VSFError - send Error!! [%d]\n", iRet);
								OutputDebugStringA(szMsg);
							}
						}
					}
					break;
			}
		}
	}

	if(m_iFrameCount % 16 == 0)
		Sleep(1);

	m_iFrameCount++;
	LeaveCriticalSection(&m_MsgQCriticalSection);
}
#endif

void Server::Login(char* szID, char* szPW)
{
#ifdef _ENABLE_MSG_Q
	MsgItem msg_item;

	if (strcmp(szID, "bruvis") == 0 && strcmp(szPW, "avlink") == 0)
	{
		msg_item.lCmd = NID_LOGIN_RET;
		msg_item.lParm = LR_SUCESSED;

		m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));

		m_iStatus = VSFS_STOP;

		m_bIsPassed = true;
	}
	else
	{
		msg_item.lCmd = NID_LOGIN_RET;
		msg_item.lParm = LR_FAILED;

		m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));

		m_bIsPassed = false;
		m_iStatus = VSFS_FAIL;
	}
#else
	PacketMaker packet_maker;

	if (strcmp(szID, "bruvis") == 0 && strcmp(szPW, "avlink") == 0)
	{
		packet_maker.Make(NID_LOGIN_RET, LR_SUCESSED);
		m_bIsPassed = true;
	}
	else
	{
		packet_maker.Make(NID_LOGIN_RET, LR_FAILED);
		m_bIsPassed = false;
	}

	send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);
#endif
}

void Server::Setup()
{
	if (!m_bIsPassed)
		return;

#ifdef _ENABLE_MSG_Q
	MsgItem msg_item;

	msg_item.lCmd = NID_SETUP_RET;
	msg_item.lParm = m_iCodecType; //codec id
	msg_item.lParm2 = m_iWidth; //w
	msg_item.lParm3 = m_iHeight; //h

	m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));
#else
#endif

	m_iStatus = VSFS_SETUP;
}

void Server::Play()
{
	if (!m_bIsPassed)
		return;

	m_iStatus = VSFS_PLAY;
}

void Server::Stop()
{
	m_iStatus = VSFS_STOP;
}

int Server::AddVideoPacket(unsigned char* pBuf,long lStart,long lEnd, unsigned short usSN,bool bMark)
{
#ifdef _ENABLE_MSG_Q
	if (m_pMsgQueue->IsFulled())
		return -1;
	else
	{
		MsgItem msg_item;

		msg_item.lCmd = NID_VIDEO_PACKET;
		msg_item.lParm = lStart;
		msg_item.lParm2 = lEnd; 
		msg_item.lParm3 = usSN; 
		msg_item.lParm4 = bMark;
		msg_item.pBuf = pBuf;

		m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));

		return 1;
	}
#endif
}

void Server::UpdateTimestamp()
{
	char szMsg[512];
	DWORD dwCurTime = timeGetTime();

	long lSpendTime = dwCurTime - m_ulLastTimestamp;

	if (lSpendTime > 0)
	{
		float fFPS = 1000.0 / (float)lSpendTime;

		m_FPSAvg.Calculate(fFPS);

		sprintf(szMsg, "TCS Server::UpdateTimestamp:  FPS = %3.2f , avg = %3.2f , (%d)\n", fFPS, m_FPSAvg.GetAverage(), lSpendTime);
		OutputDebugStringA(szMsg);
	}

	m_bUpdateTimestamp = true;
}

float Server::GetAvgFPS()
{
	return m_FPSAvg.GetAverage();
}

int Server::GetStatus()
{
	return m_iStatus;
}

void Server::Hello()
{
	m_dwLastHello = timeGetTime();
#ifdef _ENABLE_MSG_Q
	MsgItem msg_item;

	msg_item.lCmd = NID_HELLO;
	msg_item.lParm = -1;

	m_pMsgQueue->Add((unsigned char*)&msg_item, sizeof(MsgItem));
#else
	char szMsg[512];
	PacketMaker packet_maker;
	packet_maker.Make(CID_HELLO);
	int iRet = send(m_hSocket, (char*)packet_maker.GetBuffer(), packet_maker.GetBufLen(), 0);

	sprintf(szMsg, "TCS Server::Hello:  %d\n", iRet);
	OutputDebugStringA(szMsg);
#endif
}

void Server::ResendHello()
{
	DWORD dwCurTime = timeGetTime();

	if (dwCurTime - m_dwLastHello >= 3000)
	{
		Hello();
	}
}

void Server::Test()
{
}

DWORD WINAPI RecvThread(LPVOID lpParam)
{    
	Server* pObj = (Server*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->ProcessRecvData();
	}
	return 0;
}

#ifdef _ENABLE_MSG_Q
DWORD WINAPI SendMsgThread(LPVOID lpParam)
{
	Server* pObj = (Server*)lpParam;

	while (pObj->IsAlive())
	{
		pObj->SendPacket();
	}
	return 0;
}
#endif
