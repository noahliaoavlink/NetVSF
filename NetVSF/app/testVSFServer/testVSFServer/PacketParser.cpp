#include "stdafx.h"
#include "PacketParser.h"

PacketParser::PacketParser()
{
	m_pTmpBuffer = new unsigned char[_MAX_BUFFER_LEN * 2];
	m_pTmpBuffer2 = new unsigned char[_MAX_BUFFER_LEN * 2];
	m_pDataBuffer = new unsigned char[_MAX_BUFFER_LEN];
	m_pDataBuffer2 = new unsigned char[_MAX_BUFFER_LEN];
	m_iTmpBufLen = 0;

	m_pPacketBuffer = new unsigned char[_MAX_BUFFER_LEN];
	m_iPacketBufLen = 0;

	m_PacketItem.pBuf = new unsigned char[_SPLIT_SIZE];
}

PacketParser::~PacketParser()
{
	if (m_pTmpBuffer)
		delete m_pTmpBuffer;

	if (m_pTmpBuffer2)
		delete m_pTmpBuffer2;

	if (m_pDataBuffer)
		delete m_pDataBuffer;

	if (m_pDataBuffer2)
		delete m_pDataBuffer2;

	if (m_pPacketBuffer)
		delete m_pPacketBuffer;

	delete m_PacketItem.pBuf;
}

void PacketParser::Input(unsigned char* pBuffer, int iLen)
{
	memcpy(m_pTmpBuffer + m_iTmpBufLen, pBuffer, iLen);
	m_iTmpBufLen += iLen;
}

int PacketParser::Parse()
{
	char szMsg[512];
	int iRet = -1;
	int iCount = 0;
	unsigned long ulPacketLen = 0;

	if (m_iTmpBufLen == 0)
		return -1;

#if 0
	bool bHasHeader = CheckPacketHeader(m_pTmpBuffer, m_iTmpBufLen);
	if (bHasHeader)
	{
		iRet = 0;

		iCount += 3;
		m_PacketItem.ucPacketType = m_pTmpBuffer[iCount];
		iCount += sizeof(unsigned char);
		memcpy(&m_PacketItem.lID, m_pTmpBuffer + iCount, sizeof(long));
		iCount += sizeof(long);

		memcpy(&ulPacketLen, m_pTmpBuffer + iCount, sizeof(unsigned long));
		iCount += sizeof(unsigned long);

		switch (m_PacketItem.ucPacketType)
		{
			case PT_CMD_ONLY:
				break;
			case PT_SIMPLE:
				memcpy(&m_PacketItem.lData, m_pTmpBuffer + iCount, sizeof(long));
				iCount += sizeof(long);
				break;
				/*
			case PT_DUAL_LONG:
				memcpy(&m_PacketItem.lData, m_pTmpBuffer + iCount, sizeof(long));
				iCount += sizeof(long);
				memcpy(&m_PacketItem.lData2, m_pTmpBuffer + iCount, sizeof(long));
				iCount += sizeof(long);
				break;
				*/
			case PT_STRING:
				memcpy(&m_PacketItem.iLen, m_pTmpBuffer + iCount, sizeof(long));
				iCount += sizeof(long);

				memcpy(m_pDataBuffer, m_pTmpBuffer + iCount, m_PacketItem.iLen);
				m_pDataBuffer[m_PacketItem.iLen] = '\0';
				iCount += m_PacketItem.iLen;

				//m_PacketItem.pData = m_pDataBuffer;
				memcpy(m_PacketItem.szString, m_pDataBuffer, m_PacketItem.iLen + 1);
				break;
				
			case PT_STRING2:
				memcpy(&m_PacketItem.iLen, m_pTmpBuffer + iCount, sizeof(long));
				iCount += sizeof(long);

				memcpy(m_pDataBuffer, m_pTmpBuffer + iCount, m_PacketItem.iLen);
				m_pDataBuffer[m_PacketItem.iLen] = '\0';
				iCount += m_PacketItem.iLen;

				//m_PacketItem.pData = m_pDataBuffer;
				memcpy(m_PacketItem.szString, m_pDataBuffer, m_PacketItem.iLen + 1);

				memcpy(&m_PacketItem.iLen2, m_pTmpBuffer + iCount, sizeof(long));
				iCount += sizeof(long);

				memcpy(m_pDataBuffer2, m_pTmpBuffer + iCount, m_PacketItem.iLen2);
				m_pDataBuffer2[m_PacketItem.iLen2] = '\0';
				iCount += m_PacketItem.iLen2;

				//m_PacketItem.pData2 = m_pDataBuffer2;
				memcpy(m_PacketItem.szString2, m_pDataBuffer2, m_PacketItem.iLen2 + 1);
				break;
			case PT_VSF_SETUP:
				memcpy(&m_PacketItem.lData[0], m_pTmpBuffer + iCount, sizeof(long)); //codec type
				iCount += sizeof(long);
				memcpy(&m_PacketItem.lData[1], m_pTmpBuffer + iCount, sizeof(long)); //w
				iCount += sizeof(long);
				memcpy(&m_PacketItem.lData[2], m_pTmpBuffer + iCount, sizeof(long)); //h
				iCount += sizeof(long);
				break;
			case PT_VSF_VIDEO_PACKET:
				{
					unsigned char ucVersion;
					unsigned char ucMark;
					unsigned short usSN;
					unsigned long ulLength;

					memcpy(&ucVersion, m_pTmpBuffer + iCount, sizeof(unsigned char)); //version
					iCount += sizeof(unsigned char);

					memcpy(&ucMark, m_pTmpBuffer + iCount, sizeof(unsigned char)); //mark
					iCount += sizeof(unsigned char);

					memcpy(&usSN, m_pTmpBuffer + iCount, sizeof(unsigned short)); //SN
					iCount += sizeof(unsigned short);

					memcpy(&ulLength, m_pTmpBuffer + iCount, sizeof(unsigned long)); //long
					iCount += sizeof(unsigned long);

					m_PacketItem.lData[0] = ucVersion;
					m_PacketItem.lData[1] = ucMark;
					m_PacketItem.lData[2] = usSN;
					m_PacketItem.lData[3] = ulLength;

					memcpy(m_PacketItem.pBuf, m_pTmpBuffer + iCount, ulLength); //long
					iCount += ulLength;
				}
				break;
		}

		bool bHasTailer = CheckPacketTailer(m_pTmpBuffer + iCount, m_iTmpBufLen - iCount);

		if (bHasTailer)
		{
			iCount += 3;

			int iNewLen = m_iTmpBufLen - iCount;

			memcpy(m_pTmpBuffer2, m_pTmpBuffer + iCount, iNewLen);
			memcpy(m_pTmpBuffer, m_pTmpBuffer2, iNewLen);
			m_iTmpBufLen = iNewLen;

			iRet = 1;
		}
		else
		{
			sprintf(szMsg, "TC PacketParser::Parse() - lose Tailer!!\n");
			OutputDebugStringA(szMsg);
		}
	}
	else
	{
		sprintf(szMsg, "TC PacketParser::Parse() - no Header!!\n");
		OutputDebugStringA(szMsg);
	}
#else
	m_iPacketBufLen = CheckPacketLen(m_pTmpBuffer, m_iTmpBufLen);
	if (m_iPacketBufLen > 0)
	{
		memcpy(m_pPacketBuffer, m_pTmpBuffer, m_iPacketBufLen);

		iRet = Parse2(m_pPacketBuffer, m_iPacketBufLen);
	}
	else
	{
		//sprintf(szMsg, "TC PacketParser::Parse() - CheckPacketLen Error!!\n");
		//OutputDebugStringA(szMsg);
	}
#endif

	return iRet;
}

int PacketParser::Parse2(unsigned char* pBuffer, int iLen)
{
	char szMsg[512];
	int iRet = -1;
	int iCount = 0;
	unsigned long ulPacketLen = 0;
	unsigned long ulLength;

	if (m_iTmpBufLen == 0)
		return -1;

	bool bHasHeader = CheckPacketHeader(pBuffer, iLen);
	if (bHasHeader)
	{
		iRet = 0;

		iCount += 3;
		m_PacketItem.ucPacketType = pBuffer[iCount];
		iCount += sizeof(unsigned char);
		memcpy(&m_PacketItem.lID, pBuffer + iCount, sizeof(long));
		iCount += sizeof(long);

		memcpy(&ulPacketLen, pBuffer + iCount, sizeof(unsigned long));
		iCount += sizeof(unsigned long);

		switch (m_PacketItem.ucPacketType)
		{
			case PT_CMD_ONLY:
				break;
			case PT_SIMPLE:
				memcpy(&m_PacketItem.lData, pBuffer + iCount, sizeof(long));
				iCount += sizeof(long);
				break;
			case PT_STRING:
				memcpy(&m_PacketItem.iLen, pBuffer + iCount, sizeof(long));
				iCount += sizeof(long);

				memcpy(m_pDataBuffer, pBuffer + iCount, m_PacketItem.iLen);
				m_pDataBuffer[m_PacketItem.iLen] = '\0';
				iCount += m_PacketItem.iLen;

				//m_PacketItem.pData = m_pDataBuffer;
				memcpy(m_PacketItem.szString, m_pDataBuffer, m_PacketItem.iLen + 1);
				break;
			case PT_STRING2:
				memcpy(&m_PacketItem.iLen, pBuffer + iCount, sizeof(long));
				iCount += sizeof(long);

				memcpy(m_pDataBuffer, pBuffer + iCount, m_PacketItem.iLen);
				m_pDataBuffer[m_PacketItem.iLen] = '\0';
				iCount += m_PacketItem.iLen;

				//m_PacketItem.pData = m_pDataBuffer;
				memcpy(m_PacketItem.szString, m_pDataBuffer, m_PacketItem.iLen + 1);

				memcpy(&m_PacketItem.iLen2, pBuffer + iCount, sizeof(long));
				iCount += sizeof(long);

				memcpy(m_pDataBuffer2, pBuffer + iCount, m_PacketItem.iLen2);
				m_pDataBuffer2[m_PacketItem.iLen2] = '\0';
				iCount += m_PacketItem.iLen2;

				//m_PacketItem.pData2 = m_pDataBuffer2;
				memcpy(m_PacketItem.szString2, m_pDataBuffer2, m_PacketItem.iLen2 + 1);
				break;
			case PT_VSF_SETUP:
				memcpy(&m_PacketItem.lData[0], pBuffer + iCount, sizeof(long)); //codec type
				iCount += sizeof(long);
				memcpy(&m_PacketItem.lData[1], pBuffer + iCount, sizeof(long)); //w
				iCount += sizeof(long);
				memcpy(&m_PacketItem.lData[2], pBuffer + iCount, sizeof(long)); //h
				iCount += sizeof(long);
				break;
			case PT_VSF_VIDEO_PACKET:
			{
				unsigned char ucVersion;
				unsigned char ucMark;
				unsigned short usSN;

				memcpy(&ucVersion, pBuffer + iCount, sizeof(unsigned char)); //version
				iCount += sizeof(unsigned char);

				memcpy(&ucMark, pBuffer + iCount, sizeof(unsigned char)); //mark
				iCount += sizeof(unsigned char);

				memcpy(&usSN, pBuffer + iCount, sizeof(unsigned short)); //SN
				iCount += sizeof(unsigned short);

				memcpy(&ulLength, pBuffer + iCount, sizeof(unsigned long)); //long
				iCount += sizeof(unsigned long);

				m_PacketItem.lData[0] = ucVersion;
				m_PacketItem.lData[1] = ucMark;
				m_PacketItem.lData[2] = usSN;
				m_PacketItem.lData[3] = ulLength;

				memcpy(m_PacketItem.pBuf, pBuffer + iCount, ulLength); //long
				iCount += ulLength;
			}
			break;
		}

		bool bHasTailer = CheckPacketTailer(pBuffer + iCount, iLen - iCount);

		if (bHasTailer)
		{
			iCount += 3;

			int iNewLen = m_iTmpBufLen - iCount;

			memcpy(m_pTmpBuffer2, m_pTmpBuffer + iCount, iNewLen);
			memcpy(m_pTmpBuffer, m_pTmpBuffer2, iNewLen);
			m_iTmpBufLen = iNewLen;

			iRet = 1;
		}
		else
		{
			m_iTmpBufLen = 0;
			sprintf(szMsg, "TC PacketParser::Parse() - lose Tailer!![ulLength = %d , ulPacketLen = %d , m_iTmpBufLen = %d]\n", ulLength, ulPacketLen, m_iTmpBufLen);
			OutputDebugStringA(szMsg);
		}
	}

	return iRet;
}

PacketItem* PacketParser::GetPacketItem()
{
	return &m_PacketItem;
}

bool PacketParser::CheckPacketHeader(unsigned char* pBuffer, int iLen)
{
	if (iLen >= 3)
	{
		if (pBuffer[0] == '<' && pBuffer[1] == 'N' && pBuffer[2] == 'P')
			return true;
	}
	return false;
}

bool PacketParser::CheckPacketTailer(unsigned char* pBuffer, int iLen)
{
	if (iLen >= 3)
	{
		if (pBuffer[0] == 'N' && pBuffer[1] == 'P' && pBuffer[2] == '>')
			return true;
	}
	return false;
}

int PacketParser::CheckPacketLen(unsigned char* pBuffer, int iLen)
{
	char szMsg[512];
	int iCount = 0;
	unsigned long ulPacketLen = 0;
	bool bHasHeader = CheckPacketHeader(pBuffer, iLen);
	if (bHasHeader)
	{
		iCount += 3;
		m_PacketItem.ucPacketType = m_pTmpBuffer[iCount];
		iCount += sizeof(unsigned char);
		memcpy(&m_PacketItem.lID, m_pTmpBuffer + iCount, sizeof(long));
		iCount += sizeof(long);

		memcpy(&ulPacketLen, m_pTmpBuffer + iCount, sizeof(unsigned long));
		iCount += sizeof(unsigned long);

		if (iLen >= ulPacketLen)
			return ulPacketLen;
	}
	else
	{
		int iRet = FindHeader(pBuffer, iLen);

		sprintf(szMsg, "TC PacketParser::Parse() - no Header!! [iRet = %d]\n", iRet);
		OutputDebugStringA(szMsg);

		if (iRet == -1)
		{
			m_iTmpBufLen = 0; //skip all data.
		}
		else
		{
			int iNewLen = m_iTmpBufLen - iRet;

			memcpy(m_pTmpBuffer2, m_pTmpBuffer + iRet, iNewLen);
			memcpy(m_pTmpBuffer, m_pTmpBuffer2, iNewLen);
			m_iTmpBufLen = iNewLen;
		}
	}
	return 0;
}

int PacketParser::FindHeader(unsigned char* pBuffer, int iLen)
{
	//CheckPacketHeader(unsigned char* pBuffer, int iLen);
	bool bRet = false;
	for (int i = 0; i < iLen; i++)
	{
		bRet = CheckPacketHeader(pBuffer + i, iLen - i);
		if (bRet)
			return i;
	}
	return -1;
}
