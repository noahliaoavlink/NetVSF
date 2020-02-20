#include "stdafx.h"
#include "PacketMaker.h"

PacketMaker::PacketMaker()
{
	m_pBuffer = new unsigned char[_MAX_BUFFER_LEN];
	m_iLen = 0;
}

PacketMaker::~PacketMaker()
{
	if (m_pBuffer)
		delete m_pBuffer;
}

void PacketMaker::Make(long lCmdID)
{
	int iCount = 0;
	long lPacketLenPos = 0;
	unsigned long ulPacketLen = 0;
	m_ucPacketType = PT_CMD_ONLY;

	memcpy(m_pBuffer,"<NP",strlen("<NP"));
	iCount += strlen("<NP");
	memcpy(m_pBuffer + iCount, &m_ucPacketType, sizeof(unsigned char));
	iCount += sizeof(unsigned char);
	memcpy(m_pBuffer + iCount, &lCmdID, sizeof(long));
	iCount += sizeof(long);

	lPacketLenPos = iCount;
	memcpy(m_pBuffer + iCount, &ulPacketLen, sizeof(unsigned long));
	iCount += sizeof(unsigned long);

	memcpy(m_pBuffer + iCount, "NP>", strlen("NP>"));
	iCount += strlen("NP>");

	m_iLen = iCount;

	ulPacketLen = m_iLen;
	memcpy(m_pBuffer + lPacketLenPos, &ulPacketLen, sizeof(unsigned long));
}

void PacketMaker::Make(long lCmdID,long lData)
{
	int iCount = 0;
	long lPacketLenPos = 0;
	unsigned long ulPacketLen = 0;
	m_ucPacketType = PT_SIMPLE;

	memcpy(m_pBuffer, "<NP", strlen("<NP"));
	iCount += strlen("<NP");
	memcpy(m_pBuffer + iCount, &m_ucPacketType, sizeof(unsigned char));
	iCount += sizeof(unsigned char);
	memcpy(m_pBuffer + iCount, &lCmdID, sizeof(long));
	iCount += sizeof(long);

	lPacketLenPos = iCount;
	memcpy(m_pBuffer + iCount, &ulPacketLen, sizeof(unsigned long));
	iCount += sizeof(unsigned long);

	memcpy(m_pBuffer + iCount, &lData, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, "NP>", strlen("NP>"));
	iCount += strlen("NP>");

	m_iLen = iCount;

	ulPacketLen = m_iLen;
	memcpy(m_pBuffer + lPacketLenPos, &ulPacketLen, sizeof(unsigned long));
}
/*
void PacketMaker::Make(long lCmdID, long lData1, long lData2)
{
	int iCount = 0;
	m_ucPacketType = PT_DUAL_LONG;

	memcpy(m_pBuffer, "<NP", strlen("<NP"));
	iCount += strlen("<NP");
	memcpy(m_pBuffer + iCount, &m_ucPacketType, sizeof(unsigned char));
	iCount += sizeof(unsigned char);
	memcpy(m_pBuffer + iCount, &lCmdID, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, &lData1, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, &lData2, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, "NP>", strlen("NP>"));
	iCount += strlen("NP>");

	m_iLen = iCount;
}
*/

void PacketMaker::Make(long lCmdID, unsigned char* pBuffer,int iLen)
{
	int iCount = 0;
	long lPacketLenPos = 0;
	unsigned long ulPacketLen = 0;
	m_ucPacketType = PT_STRING;

	memcpy(m_pBuffer, "<NP", strlen("<NP"));
	iCount += strlen("<NP");
	memcpy(m_pBuffer + iCount, &m_ucPacketType, sizeof(unsigned char));
	iCount += sizeof(unsigned char);
	memcpy(m_pBuffer + iCount, &lCmdID, sizeof(long));
	iCount += sizeof(long);

	lPacketLenPos = iCount;
	memcpy(m_pBuffer + iCount, &ulPacketLen, sizeof(unsigned long));
	iCount += sizeof(unsigned long);

	memcpy(m_pBuffer + iCount, &iLen, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, pBuffer, iLen);
	iCount += iLen;

	memcpy(m_pBuffer + iCount, "NP>", strlen("NP>"));
	iCount += strlen("NP>");

	m_iLen = iCount;

	ulPacketLen = m_iLen;
	memcpy(m_pBuffer + lPacketLenPos, &ulPacketLen, sizeof(unsigned long));
}

void PacketMaker::Make(long lCmdID, unsigned char* pBuffer1, int iLen1, unsigned char* pBuffer2, int iLen2)
{
	int iCount = 0;
	long lPacketLenPos = 0;
	unsigned long ulPacketLen = 0;
	m_ucPacketType = PT_STRING2;

	memcpy(m_pBuffer, "<NP", strlen("<NP"));
	iCount += strlen("<NP");
	memcpy(m_pBuffer + iCount, &m_ucPacketType, sizeof(unsigned char));
	iCount += sizeof(unsigned char);
	memcpy(m_pBuffer + iCount, &lCmdID, sizeof(long));
	iCount += sizeof(long);

	lPacketLenPos = iCount;
	memcpy(m_pBuffer + iCount, &ulPacketLen, sizeof(unsigned long));
	iCount += sizeof(unsigned long);

	memcpy(m_pBuffer + iCount, &iLen1, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, pBuffer1, iLen1);
	iCount += iLen1;

	memcpy(m_pBuffer + iCount, &iLen2, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, pBuffer2, iLen2);
	iCount += iLen2;

	memcpy(m_pBuffer + iCount, "NP>", strlen("NP>"));
	iCount += strlen("NP>");

	m_iLen = iCount;

	ulPacketLen = m_iLen;
	memcpy(m_pBuffer + lPacketLenPos, &ulPacketLen, sizeof(unsigned long));
}

void PacketMaker::MakeVSFSetup(long lCmdID, long lData1, long lData2, long lData3)
{
	int iCount = 0;
	long lPacketLenPos = 0;
	unsigned long ulPacketLen = 0;
	m_ucPacketType = PT_VSF_SETUP;

	memcpy(m_pBuffer, "<NP", strlen("<NP"));
	iCount += strlen("<NP");
	memcpy(m_pBuffer + iCount, &m_ucPacketType, sizeof(unsigned char));
	iCount += sizeof(unsigned char);
	memcpy(m_pBuffer + iCount, &lCmdID, sizeof(long));
	iCount += sizeof(long);

	lPacketLenPos = iCount;
	memcpy(m_pBuffer + iCount, &ulPacketLen, sizeof(unsigned long));
	iCount += sizeof(unsigned long);

	memcpy(m_pBuffer + iCount, &lData1, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, &lData2, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, &lData3, sizeof(long));
	iCount += sizeof(long);

	memcpy(m_pBuffer + iCount, "NP>", strlen("NP>"));
	iCount += strlen("NP>");

	m_iLen = iCount;

	ulPacketLen = m_iLen;
	memcpy(m_pBuffer + lPacketLenPos, &ulPacketLen, sizeof(unsigned long));
}

void PacketMaker::MakeVSFVideoPacket(long lCmdID, VSFVideoPacket* pPacket)
{
	int iCount = 0;
	long lPacketLenPos = 0;
	unsigned long ulPacketLen = 0;
	m_ucPacketType = PT_VSF_VIDEO_PACKET;

	memcpy(m_pBuffer, "<NP", strlen("<NP"));
	iCount += strlen("<NP");
	memcpy(m_pBuffer + iCount, &m_ucPacketType, sizeof(unsigned char));
	iCount += sizeof(unsigned char);
	memcpy(m_pBuffer + iCount, &lCmdID, sizeof(long));
	iCount += sizeof(long);

	lPacketLenPos = iCount;
	memcpy(m_pBuffer + iCount, &ulPacketLen, sizeof(unsigned long));
	iCount += sizeof(unsigned long);

	//
	/*
	pPacket->ucVersion;
	pPacket->ucMarker;
	pPacket->usSequenceNumber;
	pPacket->ulLength;
	pPacket->pBuf;
	*/

	memcpy(m_pBuffer + iCount, &pPacket->ucVersion, sizeof(unsigned char));
	iCount += sizeof(unsigned char);

	memcpy(m_pBuffer + iCount, &pPacket->ucMarker, sizeof(unsigned char));
	iCount += sizeof(unsigned char);

	memcpy(m_pBuffer + iCount, &pPacket->usSequenceNumber, sizeof(unsigned short));
	iCount += sizeof(unsigned short);

	memcpy(m_pBuffer + iCount, &pPacket->ulLength, sizeof(unsigned long));
	iCount += sizeof(unsigned long);

	memcpy(m_pBuffer + iCount, pPacket->pBuf, pPacket->ulLength);
	iCount += pPacket->ulLength;
	//

	memcpy(m_pBuffer + iCount, "NP>", strlen("NP>"));
	iCount += strlen("NP>");

	m_iLen = iCount;

	ulPacketLen = m_iLen;
	memcpy(m_pBuffer + lPacketLenPos, &ulPacketLen, sizeof(unsigned long));
}

unsigned char* PacketMaker::GetBuffer()
{
	return m_pBuffer;
}

int PacketMaker::GetBufLen()
{
	return m_iLen;
}