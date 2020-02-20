#include "stdafx.h"
#include "VideoDecoder.h"

VideoDecoder::VideoDecoder()
{
	m_iCodecType = CT_NONE;
	m_iQuality = 90;

	m_plibJpeg = 0;// new libJpeg;
	m_pOutBuf = 0;
	m_bIsOpened = false;
	m_usSerialNumber = 0;
	m_usFrameCount = 0;

	m_iWidth = 0;
	m_iHeight = 0;
	m_iBufLen = 0;
}

VideoDecoder::~VideoDecoder()
{
	Close();

	if (m_pOutBuf)
		delete m_pOutBuf;

	if (m_plibJpeg)
		delete m_plibJpeg;
}

void VideoDecoder::Open(int iCodecType, int w, int h)
{
	m_iCodecType = iCodecType;
	m_iWidth = w;
	m_iHeight = h;

	if (m_pOutBuf)
		delete m_pOutBuf;

	m_pOutBuf = new unsigned char[w * h * 3];

	if (m_iCodecType == CT_JPEG)
	{
		m_plibJpeg = new libJpeg;
	}

	m_bIsOpened = true;
}

void VideoDecoder::Close()
{
	if (m_iCodecType == CT_JPEG)
	{
		if (m_plibJpeg)
			delete m_plibJpeg;
		m_plibJpeg = 0;
	}

	m_bIsOpened = false;
}

int VideoDecoder::GetCodecType()
{
	return m_iCodecType;
}

bool VideoDecoder::IsOpened()
{
	return m_bIsOpened;
}

unsigned char* VideoDecoder::GetOutBuffer()
{
	return m_pOutBuf;
}

int VideoDecoder::GetBufLen()
{
	return m_iBufLen;
}

int VideoDecoder::GetWidth()
{
	return m_iWidth;
}

int VideoDecoder::GetHeight()
{
	return m_iHeight;
}

int VideoDecoder::Decode(unsigned char* pInBuf,int iLen)
{
	int iRet = -1;
	if (m_iCodecType == CT_JPEG)
	{
		iRet = m_plibJpeg->Decode(pInBuf, m_pOutBuf, iLen, &m_iWidth, &m_iHeight, &m_iBufLen);
	}
	return iRet;
}