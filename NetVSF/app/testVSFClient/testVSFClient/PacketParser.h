#pragma once

#ifndef _PacketParser_H_
#define _PacketParser_H_

#include "PacketMaker.h"

typedef struct
{
	unsigned char ucPacketType;
	long lID;
	long lData[4];
	//long lData2;
	//unsigned char* pData;
	unsigned char szString[128];
	int iLen;
	//unsigned char* pData2;
	unsigned char szString2[128];
	int iLen2;
	unsigned char* pBuf;
}PacketItem;

class PacketParser
{
	unsigned char* m_pTmpBuffer;
	unsigned char* m_pTmpBuffer2;
	int m_iTmpBufLen;

	unsigned char* m_pPacketBuffer;
	int m_iPacketBufLen;

	unsigned char* m_pDataBuffer;
	unsigned char* m_pDataBuffer2;
	PacketItem m_PacketItem;
protected:
public:
	PacketParser();
	~PacketParser();

	void Input(unsigned char* pBuffer, int iLen);
	int Parse();
	int Parse2(unsigned char* pBuffer, int iLen);
	bool CheckPacketHeader(unsigned char* pBuffer, int iLen);
	bool CheckPacketTailer(unsigned char* pBuffer, int iLen);
	int CheckPacketLen(unsigned char* pBuffer, int iLen);
	int FindHeader(unsigned char* pBuffer, int iLen);

	PacketItem* GetPacketItem();
};

#endif