#include "stdafx.h"

CTower::CTower()
{
	m_iHP			= 0;
	m_iMP			= 0;
	m_iATK			= 0;
	m_iDEF			= 0;
	m_iGiveEXP		= 0;
	m_iCurrentX		= 0;
	m_iCurrentY		= 0;
}

VOID CTower::SetTower(int iTower)
{
	switch (iTower)
	{
	case MIDDLETOWER:
		{
			m_iHP		= MIDDLETOWER_HP;
			m_iMP		= MIDDLETOWER_MP;
			m_iATK		= MIDDLETOWER_ATK;
			m_iDEF		= MIDDLETOWER_DEF;
			m_iGiveEXP	= 50;
			m_iCurrentX		= MAPSIZE_ROW/2;
			m_iCurrentY		= MAPSIZE_COLUMN/2;
			break;
		}
	case LASTTOWER:
		{
			m_iHP		= LASTTOWER_HP;
			m_iMP		= LASTTOWER_MP;
			m_iATK		= LASTTOWER_ATK;
			m_iDEF		= LASTTOWER_DEF;
			m_iGiveEXP	= 100;
			m_iCurrentX		= MAPSIZE_ROW - 2;
			m_iCurrentY		= MAPSIZE_COLUMN - 2;
			break;
		}
	}
}


VOID CTower::Attacked_by_Champion()
{
	static int iFuncCount	= 0;
	m_iHP		-= cChampion.m_iATK - m_iDEF;
	
	if (m_iHP <= 0 && iFuncCount == 0)
	{
		m_iHP				= 0;
		iFuncCount++;
		cChampion.m_iEXP	+= m_iGiveEXP;
		cMap[m_iCurrentX][m_iCurrentY]		= ' ';
		cMap[m_iCurrentX - 1][m_iCurrentY]	= ' ';
		cMap[m_iCurrentX - 2][m_iCurrentY]	= ' ';
		m_iCurrentX		= 0;
		m_iCurrentY		= 0;
		gotoxy(0, 0);
		Map_Load();
	}
}