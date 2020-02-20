#ifndef _Server_H_
#define _Server_H_

#include <winsock2.h>

#include "..\..\..\Include\NetCommon.h"
#include "..\..\..\Include\QBufferManagerEx.h"
#include "PacketParser.h"
#include "PacketMaker.h"
#include "VideoEncoder.h"

#define _ENABLE_MSG_Q 1

#ifdef _ENABLE_MSG_Q

#include "..\..\..\Include\SQQueue.h"
//#include "..\..\..\Include\MathCommon.h"

#include "AverageIf.h"

typedef struct
{
	long lCmd;
	long lParm;
	long lParm2;
	long lParm3;
	long lParm4;
	unsigned char* pBuf;
}MsgItem;
#endif

class Server
{	
		SOCKET m_hSocket;
		char* m_pRcvBuffer;
		char* m_pSendBuffer;
		char* m_pBuffer;

		PacketParser* m_pPacketParser;

		bool m_bIsPassed;
		int m_iStatus;

		int m_iCodecType;
		int m_iWidth;
		int m_iHeight;

		DWORD m_dwLastHello;
		unsigned long m_ulLastTimestamp;
		bool m_bUpdateTimestamp;
		AverageIf m_FPSAvg;
		int m_iFrameCount;

#ifdef _ENABLE_MSG_Q
		SQQueue* m_pMsgQueue;
#endif

		QBufferManagerEx* m_pQBuffer;
		HANDLE m_hRecvThread;
		bool m_bIsAlive;
		CRITICAL_SECTION m_CriticalSection;

#ifdef _ENABLE_MSG_Q
		HANDLE m_hSendMsgThread;
		CRITICAL_SECTION m_MsgQCriticalSection;
#endif

   protected:
   public:
		Server();
        ~Server();	

		void Init(int iCodecType, int iWidth, int iHeight);
		bool IsAlive();

		void SetSocket(SOCKET hSocket);
		SOCKET GetSocket();
		int GetStatus();

		void Close();
		void Receive();

		void Hello();
		void Login(char* szID, char* szPW);
		void Setup();
		void Play();
		void Stop();

		int AddVideoPacket(unsigned char* pBuf, long lStart, long lEnd, unsigned short usSN, bool bMark);
		void UpdateTimestamp();

		void ProcessRecvData();
#ifdef _ENABLE_MSG_Q
		void SendPacket();
#endif

		void ResendHello();
		float GetAvgFPS();

		void Test();
};

#endif
