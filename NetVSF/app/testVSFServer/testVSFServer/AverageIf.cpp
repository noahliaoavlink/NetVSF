#include "stdafx.h"
#include "AverageIf.h"

AverageIf::AverageIf()
{
	m_iMax = _MAX_ITEMS;
	m_iConditionType = ACT_NONE;
	Reset();
}

AverageIf::~AverageIf()
{
}

void AverageIf::Reset()
{
	m_iTotalOfItems = 0;
	for (int i = 0; i < _MAX_ITEMS; i++)
		m_fValue[i] = 0;
	m_fAvgValue = 0.0;
	m_ulCount = 0;
}

void AverageIf::SetMax(int iValue)
{
	m_iMax = iValue;
}

void AverageIf::SetConditionType(int iType)
{
	m_iConditionType = iType;
}

void AverageIf::Calculate(float fValue)
{
	m_fValue[m_ulCount % m_iMax] = fValue;

	if (m_iTotalOfItems < m_iMax)
		m_iTotalOfItems++;

	switch (m_iConditionType)
	{
		case ACT_NONE:
			{
				float fTotalOfValues = 0.0;
				for (int i = 0; i < m_iTotalOfItems; i++)
					fTotalOfValues += m_fValue[i];

				m_fAvgValue = (float)fTotalOfValues / (float)m_iTotalOfItems;
			}
			break;
		case ACT_LOW:
			{
				Sort();

				float fTotalOfValues = 0.0;
				for (int i = 0; i < m_iTotalOfItems / 2; i++)
					fTotalOfValues += m_fValue2[i];

				m_fAvgValue = (float)fTotalOfValues / (float)(m_iTotalOfItems / 2);
			}
			break;
		case ACT_LOW2:
			{
				Sort();

				int iStart = 0;
				int iEnd = iStart + m_iTotalOfItems * 6 / 10;

				float fTotalOfValues = 0.0;
				for (int i = iStart; i < iEnd; i++)
					fTotalOfValues += m_fValue2[i];

				m_fAvgValue = (float)fTotalOfValues / (float)(m_iTotalOfItems * 6 / 10);
			}
			break;
		case ACT_HIGH:
			{
				Sort();

				float fTotalOfValues = 0.0;
				for (int i = m_iTotalOfItems / 2; i < m_iTotalOfItems; i++)
					fTotalOfValues += m_fValue2[i];

				m_fAvgValue = (float)fTotalOfValues / (float)(m_iTotalOfItems / 2);
			}
			break;
		case ACT_CENTER:
			{
				Sort();

				int iStart = m_iTotalOfItems / 4;
				int iEnd = iStart + m_iTotalOfItems / 2;

				float fTotalOfValues = 0.0;
				for (int i = iStart; i < iEnd; i++)
					fTotalOfValues += m_fValue2[i];

				m_fAvgValue = (float)fTotalOfValues / (float)(m_iTotalOfItems / 2);
			}
			break;
	}

	m_ulCount++;
}

void AverageIf::Sort()
{
	int i;
	int iIndex = 0;
	float fCurItem = 0.0;
	float fTmpItem = 0.0;

	for (i = 0; i < m_iTotalOfItems; i++)
		m_fValue2[i] = m_fValue[i];

	for (i = 0; i < m_iTotalOfItems - 1; i++)
	{
		iIndex = i;
		fCurItem = m_fValue2[i];
		for (int j = i + 1; j < m_iTotalOfItems; j++)
		{
			if (m_fValue2[j] < fCurItem)
			{
				iIndex = j;
				fCurItem = m_fValue2[j];
			}
		}
		fTmpItem = m_fValue2[i];
		m_fValue2[i] = m_fValue2[iIndex];
		m_fValue2[iIndex] = fTmpItem;
	}
}

float AverageIf::GetAverage()
{
	return m_fAvgValue;
}

int AverageIf::GetCount()
{
	return m_ulCount;
}