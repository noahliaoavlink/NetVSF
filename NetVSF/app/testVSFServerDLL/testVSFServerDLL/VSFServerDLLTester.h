#pragma once

#ifndef _VSFServerDLLTester_H_
#define _VSFServerDLLTester_H_

#include "VSFServerDLL.h"
#include "ImageFile.h"

class VSFServerDLLTester
{
	VSFServerDLL* m_pVSFServerDLL;

	ImageFile* m_pImageFile[4];
	int m_iFrameCount;
protected:
public:
	VSFServerDLLTester();
	~VSFServerDLLTester();

	void Init(HWND hWnd);
	void Start();
	void Stop();
	void SetQuality(int iValue);
	void SetFPS(int iValue);
	void TimeEvent();
};

#endif
