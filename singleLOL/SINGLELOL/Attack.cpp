#include "stdafx.h"

VOID Attack(int iCurrentRow, int iCurrentColumn)
{
	if (cEasyMonster.m_iCurrentX == iCurrentRow && cEasyMonster.m_iCurrentY == iCurrentColumn)
	{
		cEasyMonster.Attacked_by_Champion();
	}
	if (cNormalMonster.m_iCurrentX == iCurrentRow && cNormalMonster.m_iCurrentY == iCurrentColumn)
	{
		cNormalMonster.Attacked_by_Champion();
	}
	if (cHardMonster.m_iCurrentX == iCurrentRow && cHardMonster.m_iCurrentY == iCurrentColumn)
	{
		cHardMonster.Attacked_by_Champion();
	}

	if ((cMiddleTower.m_iCurrentX == iCurrentRow || cMiddleTower.m_iCurrentX - 1 == iCurrentRow) && cMiddleTower.m_iCurrentY == iCurrentColumn)
	{
		cMiddleTower.Attacked_by_Champion();
	}

	if ((cLastTower.m_iCurrentX == iCurrentRow || cLastTower.m_iCurrentX - 1 == iCurrentRow || cLastTower.m_iCurrentX - 2 == iCurrentRow) && cLastTower.m_iCurrentY == iCurrentColumn)
	{
		cLastTower.Attacked_by_Champion();
	}

	if (cEasyMonster2.m_iCurrentX == iCurrentRow && cEasyMonster2.m_iCurrentY == iCurrentColumn)
	{
		cEasyMonster2.Attacked_by_Champion();
	}
	if (cEasyMonster3.m_iCurrentX == iCurrentRow && cEasyMonster3.m_iCurrentY == iCurrentColumn)
	{
		cEasyMonster3.Attacked_by_Champion();
	}
	if (cEasyMonster4.m_iCurrentX == iCurrentRow && cEasyMonster4.m_iCurrentY == iCurrentColumn)
	{
		cEasyMonster4.Attacked_by_Champion();
	}
		
}