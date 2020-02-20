#pragma once

#ifndef _VideoDecoder_H_
#define _VideoDecoder_H_

#include "libJpeg.h"
#include "..\..\..\Include\NetCommon.h"

class VideoDecoder
{
	int m_iCodecType;
	int m_iWidth;
	int m_iHeight;
	int m_iQuality;
	unsigned short m_usSerialNumber;
	unsigned short m_usFrameCount;

	unsigned char* m_pOutBuf;
	int m_iBufLen;

	bool m_bIsOpened;
	libJpeg* m_plibJpeg;
protected:
public:
	VideoDecoder();
	~VideoDecoder();


	void Open(int iCodecType, int w, int h);
	void Close();
	int GetCodecType();
	bool IsOpened();
	unsigned char* GetOutBuffer();
	int Decode(unsigned char* pInBuf, int iLen);

	int GetBufLen();
	int GetWidth();
	int GetHeight();
};

#endif
