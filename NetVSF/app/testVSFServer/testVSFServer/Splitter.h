#pragma once

#ifndef _Splitter_H_
#define _Splitter_H_

#include "..\..\..\Include\SQList.h"

typedef struct
{
	unsigned long ulStart;
	unsigned long ulEnd;
	unsigned short usSN;
	bool bIsEnded;
}UnitPositionInfo;

class Splitter
{
	int m_iUnit;

	unsigned short m_usPacketCount;

	SQList* m_pDataList;
protected:
public:
	Splitter();
	~Splitter();

	void SetUnit(int iUnit);
	int GetUnit();
	void DoSplit(unsigned long ulDataLen);
	int GetTotal();
	UnitPositionInfo* GetItemInfo(int iIndex);
	void RemoveAll();

	//void Test();
};

#endif