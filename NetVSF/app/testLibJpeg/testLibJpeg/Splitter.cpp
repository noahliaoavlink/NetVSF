#include "stdafx.h"
#include "Splitter.h"

Splitter::Splitter()
{
	m_iUnit = 40960;

	m_pDataList = new SQList;
	m_pDataList->EnableRemoveData(false);
}

Splitter::~Splitter()
{
	RemoveAll();

	delete m_pDataList;
}

void Splitter::SetUnit(int iUnit)
{
	m_iUnit = iUnit;
}

void Splitter::DoSplit(unsigned long ulDataLen)
{
	bool bDo = true;
	unsigned long ulCurPos = 0;
	unsigned long ulEndPos = 0;

	RemoveAll();

	while (bDo)
	{
		if (ulCurPos + m_iUnit < ulDataLen)
		{
			UnitPositionInfo* pNewItem = new UnitPositionInfo;

			ulEndPos = ulCurPos + m_iUnit;

			pNewItem->ulStart = ulCurPos;
			pNewItem->ulEnd = ulEndPos;
			m_pDataList->Add(pNewItem);

			ulCurPos += m_iUnit;
		}
		else
		{
			UnitPositionInfo* pNewItem = new UnitPositionInfo;
			
			ulEndPos = ulDataLen;

			pNewItem->ulStart = ulCurPos;
			pNewItem->ulEnd = ulEndPos;
			m_pDataList->Add(pNewItem);

			bDo = false;
		}
	}
}

int Splitter::GetTotal()
{
	return m_pDataList->GetTotal();
}

UnitPositionInfo* Splitter::GetItemInfo(int iIndex)
{
	if (iIndex < m_pDataList->GetTotal())
		return (UnitPositionInfo*)m_pDataList->Get(iIndex);

	return 0;
}

void Splitter::RemoveAll()
{
	int iTotal = m_pDataList->GetTotal();
	for (int i = 0; i < iTotal; i++)
	{
		UnitPositionInfo* pCurItem = (UnitPositionInfo*)m_pDataList->Get(i);
		if (pCurItem)
			delete pCurItem;
	}

	m_pDataList->Reset();
}