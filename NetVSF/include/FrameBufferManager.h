#ifndef _FrameBufferManager_H_
#define _FrameBufferManager_H_

#include "QBufferManagerEx.h"
#include "../../../Include/IDStreamCommon.h"

#define _ENABLE_DUAL_BUFFER 1

class FrameBufferManager
{
#if (_ENABLE_DUAL_BUFFER == 1)
	QBufferManagerEx* m_pFrameQBuffer[2];
	int m_iCurQBIndex;
#else
		QBufferManagerEx* m_pFrameQBuffer;
#endif
		unsigned char* m_pTempFrameBuffer1;
		unsigned char* m_pTempFrameBuffer2;

		StreamFrame* m_pStreamFrame;
   protected:
   public:
	   FrameBufferManager();
		//FrameBufferManager(int iBufferSize = _VIDEO_BUFFER_SIZE,int iMax = 50);
        ~FrameBufferManager();

		void Alloc(int iBufferSize,int iMax);
		void AddToQueueBuffer(StreamFrame* pInfo);
		StreamFrame* GetNext();
		void Reset();
		bool IsFulled();
		bool IsReady();
		int GetTotal();
		int GetMax();
		void ReAlloc(int iBufferLen);
		unsigned long CalBuffLen(int iLen);
};

#endif