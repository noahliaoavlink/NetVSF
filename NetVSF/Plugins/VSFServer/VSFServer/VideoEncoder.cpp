#include "stdafx.h"
#include "VideoEncoder.h"

VideoEncoder::VideoEncoder()
{
	m_iCodecType = CT_NONE;
	m_iQuality = 90;

	m_plibJpeg = 0;// new libJpeg;
	m_pOutBuf = 0;
	m_bIsOpened = false;
	m_usSerialNumber = 0;
	m_usFrameCount = 0;
}

VideoEncoder::~VideoEncoder()
{
	if (m_pOutBuf)
		delete m_pOutBuf;

	if (m_plibJpeg)
		delete m_plibJpeg;
}

void VideoEncoder::Open(int iCodecType,int w,int h)
{
	m_iCodecType = iCodecType;
	m_iWidth = w;
	m_iHeight = h;

//	if (m_pOutBuf)
//		delete m_pOutBuf;

	//m_pOutBuf = new unsigned char[w * h * 2];

	if (m_iCodecType == CT_JPEG)
	{
		m_plibJpeg = new libJpeg;
	}

	m_bIsOpened = true;
}

void VideoEncoder::Close()
{
	if (m_iCodecType == CT_JPEG)
	{
		if (m_plibJpeg)
			delete m_plibJpeg;
		m_plibJpeg = 0;
	}

	m_bIsOpened = false;
}

void VideoEncoder::SetQuality(int iQuality)
{
	m_iQuality = iQuality;
}

int VideoEncoder::GetCodecType()
{
	return m_iCodecType;
}

bool VideoEncoder::IsOpened()
{
	return m_bIsOpened;
}

int VideoEncoder::Encode(unsigned char* pInBuf)
{
	int iRet = -1;
	if (m_iCodecType == CT_JPEG)
	{
		if (m_plibJpeg)
		{
			if (m_pOutBuf)
				delete m_pOutBuf;

			iRet = m_plibJpeg->Encode(pInBuf, &m_pOutBuf, m_iWidth, m_iHeight, m_iQuality);
		}
		m_usSerialNumber = m_usFrameCount;
		m_usFrameCount++;
	}
	return iRet;
}

unsigned char* VideoEncoder::GetOutBuffer()
{
	return m_pOutBuf;
}
/*
unsigned short VideoEncoder::GetSerialNumber()
{
	return m_usSerialNumber;
}
*/