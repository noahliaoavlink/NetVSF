#pragma once

#ifndef _Client_H_
#define _Client_H_

#include "..\..\..\Include\QBufferManagerEx.h"
#include "PacketParser.h"
#include "PacketMaker.h"
#include "..\..\..\Include\NetCommon.h"
#include "VideoDecoder.h"
#include "..\..\..\Include\MediaFileCommon.h"

class Client
{
	int m_iErrorCode;
	SOCKET m_hSocket;
	char m_szIP[64];
	int m_iPort;
	char m_szID[64];
	char m_szPW[64];

	bool m_bIsPassed;

	DWORD m_dwLoginStartTime;
	DWORD m_dwIoctlLastTime;
	int m_iSelectTimeOutCount;

	int m_iCodecType;
	int m_iWidth;
	int m_iHeight;

	bool m_bIsAlive;
	HANDLE m_hRcvThread;
	HANDLE m_hProcessThread;
	CRITICAL_SECTION m_CriticalSection;
	CRITICAL_SECTION m_RcvCriticalSection;

	char* m_pRcvBuffer;
	char* m_pBuffer;
	QBufferManagerEx* m_pQBuffer;

	PacketParser* m_pPacketParser;

	unsigned char* m_pVideoBuffer;
	unsigned char* m_pTmpVideoBuffer;
	unsigned long m_ulVideoBufLen;
	unsigned long m_ulTmpVideoBufLen;
	long m_lLastSN;

	unsigned long m_ulPacketCount;

	VideoDecoder* m_pVideoDecoder;

	EventCallback* m_pEventCallback;

protected:
public:
	Client();
	~Client();

	bool Startup();
	void Cleanup();

	void SetEventCallback(EventCallback* pObj);

	bool Connect(char* szIP, int iPort);
	void Close();
	void DoLogin();
	void Login(char* szID, char* szPW);
	void UpdateTimestamp();
	void Setup();
	void Play();
	void Stop();
	void SetupRet(PacketItem* pPacketItem);
	void RecvVideoPacket(PacketItem* pPacketItem);

	bool IsAlive();
	void ReceiveLoop();
	void Checkioctl();
	void Receive();
	void CheckReceiveStatus();
	void ProcessRecvData();
	void ResetRcvStatus();

	int GetWidth();
	int GetHeight();
};

#endif
