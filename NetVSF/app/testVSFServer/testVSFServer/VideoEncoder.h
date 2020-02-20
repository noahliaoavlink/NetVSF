#pragma once

#ifndef _VideoEncoder_H_
#define _VideoEncoder_H_

#include "libJpeg.h"
#include "..\..\..\Include\NetCommon.h"
/*
enum CodecType
{
	CT_NONE = 0,
	CT_JPEG,
	//CT_H264,
};
*/
class VideoEncoder
{
	int m_iCodecType;
	int m_iWidth;
	int m_iHeight;
	int m_iQuality;
	unsigned short m_usSerialNumber;
	unsigned short m_usFrameCount;

	unsigned char* m_pOutBuf;

	bool m_bIsOpened;
	libJpeg* m_plibJpeg;
protected:
public:
	VideoEncoder();
	~VideoEncoder();

	void Open(int iCodecType, int w, int h);
	void Close();
	int Encode(unsigned char* pInBuf);
	unsigned char* GetOutBuffer();
	//unsigned short GetSerialNumber();
	
	void SetQuality(int iQuality);
	int GetCodecType();
	bool IsOpened();
};

#endif
