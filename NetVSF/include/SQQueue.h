#pragma once

#ifndef _SQQueue_H_
#define _SQQueue_H_

#include "QBufferManagerEx.h"

#define _DOUBLE_BUFFER 1

class SQQueue
{
	QBufferManagerEx* m_pQBuffer;

	//unsigned char* m_pTempBuffer1;
	//unsigned char* m_pTempBuffer2;
	
#ifdef _DOUBLE_BUFFER
	unsigned char* m_pTempBuffer[2];
	int m_iBufIndex;
#else	
	unsigned char* m_pTempBuffer;
#endif

	int m_iLen;
public:
	SQQueue();
	~SQQueue();

	void Alloc(int iBufferSize, int iMax);
	bool Add(unsigned char* pBuffer, int iLen);
	unsigned char* GetNext();
	int GetLength();
	void Reset();
	bool IsFulled();
	int GetTotal();
	int GetMax();
};

#endif
