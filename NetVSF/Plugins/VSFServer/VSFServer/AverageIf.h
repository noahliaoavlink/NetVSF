#pragma once

#ifndef _AverageIf_H_
#define _AverageIf_H_

#define _MAX_ITEMS 1000//30

//condition
enum AverageConditionType
{
	ACT_NONE = 0,
	ACT_LOW,
	ACT_LOW2,
	ACT_HIGH,
	ACT_CENTER,
};

class AverageIf
{
	int m_iTotalOfItems;
	float m_fValue[_MAX_ITEMS];
	float m_fValue2[_MAX_ITEMS];
	float m_fAvgValue;
	unsigned long m_ulCount;

	int m_iMax;
	int m_iConditionType;
protected:
public:
	AverageIf();
	~AverageIf();

	void Reset();
	void SetMax(int iValue);
	void SetConditionType(int iType);
	void Calculate(float fValue);
	void Sort();
	float GetAverage();
	int GetCount();
};


#endif
