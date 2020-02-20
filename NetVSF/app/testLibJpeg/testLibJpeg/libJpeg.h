#pragma once

#ifndef _libJpeg_H_
#define _libJpeg_H_

#include "..\\..\\..\\lib\\jpegsr9c\jpeg-9c\\jpeglib.h"

class libJpeg
{

protected:
public:
	libJpeg();
	~libJpeg();

	int Encode(unsigned char* pInBuf, unsigned char** pOutBuf, int iWidth, int iHeight, int iQuality);
	int Decode(unsigned char* pInBuf, unsigned char* pOutBuf, int iInLen,int* w, int* h, int* iOutLen);

	int WriteJPEGFile(char* szOutFileName, int iQuality, unsigned char* pInputBuf, int iWidth, int iHeight);
	int ReadJPEGFile(char* szInFileName, unsigned char* pOutBuf, int* w, int* h, int* iBytes);

	void Test();
};

#endif