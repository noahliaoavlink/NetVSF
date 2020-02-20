#include "stdafx.h"
#include "../Include/SQQueue.h"

SQQueue::SQQueue()
{
	m_pQBuffer = new QBufferManagerEx;
	m_pQBuffer->SetMax(0);

#ifdef _DOUBLE_BUFFER
	m_pTempBuffer[0] = 0;
	m_pTempBuffer[1] = 0;
	m_iBufIndex = 0;
#else
	m_pTempBuffer = 0;
#endif

}

SQQueue::~SQQueue()
{
	if (m_pQBuffer)
		delete m_pQBuffer;

#ifdef _DOUBLE_BUFFER
	delete m_pTempBuffer[0];
	delete m_pTempBuffer[1];
#else
	delete m_pTempBuffer;
#endif
}

void SQQueue::Alloc(int iBufferSize, int iMax)
{
	int iNewBufferSize = iBufferSize + 64;

#ifdef _DOUBLE_BUFFER
	for (int i = 0; i < 2; i++)
	{
		if (m_pTempBuffer[i])
		{
			delete m_pTempBuffer[i];
			m_pTempBuffer[i] = 0;
		}

		m_pTempBuffer[i] = new unsigned char[iNewBufferSize];
	}
#else
	if (m_pTempBuffer)
	{
		delete m_pTempBuffer;
		m_pTempBuffer = 0;
	}
	
	m_pTempBuffer = new unsigned char[iNewBufferSize];
#endif

	m_pQBuffer->SetMax(iMax);
	m_pQBuffer->SetBufferSize(iNewBufferSize);
}

bool SQQueue::Add(unsigned char* pBuffer,int iLen)
{
	bool bRet = m_pQBuffer->Add(pBuffer,iLen);
	return bRet;
}

unsigned char* SQQueue::GetNext()
{
#ifdef _DOUBLE_BUFFER
	int iCurIndex = m_iBufIndex;
	m_iLen = m_pQBuffer->GetNext(m_pTempBuffer[iCurIndex]);
	int iNewIndex = m_iBufIndex + 1;
	m_iBufIndex = iNewIndex % 2;
	return m_pTempBuffer[iCurIndex];
#else
	m_iLen = m_pQBuffer->GetNext(m_pTempBuffer);
	return m_pTempBuffer;
#endif
}

int SQQueue::GetLength()
{
	return m_iLen;
}

void SQQueue::Reset()
{
	m_pQBuffer->Reset();
}

bool SQQueue::IsFulled()
{
	return m_pQBuffer->IsFull();
}

int SQQueue::GetTotal()
{
	return m_pQBuffer->GetTotal();
}

int SQQueue::GetMax()
{
	return m_pQBuffer->GetMax();
}
