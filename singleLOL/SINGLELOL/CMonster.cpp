#include "stdafx.h"


CMonster::CMonster()
{
	m_iHP		= 0;
	m_iMP		= 0;
	m_iATK		= 0;
	m_iDEF		= 0;
	m_iGiveEXP	= 0;
	m_iCurrentX	= 0;
	m_iCurrentY = 0;
}


VOID CMonster::Monster_Select(int iMonster)
{
	if (iMonster == 1)
	{
		m_iHP		= EASY_HP;
		m_iMP		= EASY_MP;
		m_iATK		= EASY_ATK;
		m_iDEF		= EASY_DEF;
		m_iGiveEXP	= 3;
	}
	else if (iMonster == 2)
	{
		m_iHP		= NORMAL_HP;
		m_iMP		= NORMAL_MP;
		m_iATK		= NORMAL_ATK;
		m_iDEF		= NORMAL_DEF;
		m_iGiveEXP	= 7;
	}
	else if (iMonster == 3)
	{
		m_iHP		= HARD_HP;
		m_iMP		= HARD_MP;
		m_iATK		= HARD_ATK;
		m_iDEF		= HARD_DEF;
		m_iGiveEXP	= 12;
	}
	else
	{
		printf("[ERROR] Invalid Difficulty Value\n");
	}
}



VOID CMonster::Attack_and_Moving()
{
	int i			= 0;
	int j			= 0;
	int iChampX		= 0;
	int iChampY		= 0;

	//If there are Champion around Monster, STOP and ATTACK
	if ( cMap[m_iCurrentX - 1][m_iCurrentY] == cChampion.m_cShape ||
		cMap[m_iCurrentX + 1][m_iCurrentY] == cChampion.m_cShape ||
		cMap[m_iCurrentX][m_iCurrentY + 1] == cChampion.m_cShape ||
		cMap[m_iCurrentX][m_iCurrentY - 1] == cChampion.m_cShape)
	{
		cChampion.m_iHP		-= m_iATK - cChampion.m_iDEF;
		return;
	}

	for (i = 0; i < MAPSIZE_ROW; i++)
	{
		for (j = 0; j < MAPSIZE_COLUMN; j++)
		{
			if (cMap[i][j] == cChampion.m_cShape)
			{
				iChampX		= i;
				iChampY		= j;
			}
		}
	}
//////////////////////////////////////////////////////////////////If meet Tower, go around//////////////////////////////////////////////////////////////////////////////////
	if (abs(m_iCurrentX - iChampX) < abs(m_iCurrentY - iChampY))
	{
		if (m_iCurrentY < iChampY)
		{
			if (cMap[m_iCurrentX][m_iCurrentY + 1] != ' ')
			{
				if (m_iCurrentX < iChampX)
				{
					m_iCurrentX += 1;
					cMap[m_iCurrentX - 1][m_iCurrentY] = ' ';
				}
				else if (m_iCurrentX > iChampX)
				{
					m_iCurrentX += -1;
					cMap[m_iCurrentX + 1][m_iCurrentY] = ' ';
				}	
			}
			else
			{
				m_iCurrentY += 1; 
				cMap[m_iCurrentX][m_iCurrentY - 1] = ' ';
			}
		}
		else if (m_iCurrentY > iChampY)
		{
			if (cMap[m_iCurrentX][m_iCurrentY - 1] != ' ')
			{
				if (m_iCurrentX < iChampX)
				{
					m_iCurrentX += 1;
					cMap[m_iCurrentX - 1][m_iCurrentY] = ' ';
				}
				else if (m_iCurrentX > iChampX)
				{
					m_iCurrentX += -1;
					cMap[m_iCurrentX + 1][m_iCurrentY] = ' ';
				}
			}
			else
			{
				m_iCurrentY += -1;
				cMap[m_iCurrentX][m_iCurrentY + 1] = ' ';
			}
		}		
	}
	else if (abs(m_iCurrentX - iChampX) > abs(m_iCurrentY - iChampY))
	{
		if (m_iCurrentX < iChampX)
		{
			if (cMap[m_iCurrentX + 1][m_iCurrentY] != ' ')
			{
				if (m_iCurrentY < iChampY)
				{
					m_iCurrentY += 1;
					cMap[m_iCurrentX][m_iCurrentY - 1] = ' ';
				}
				else if (m_iCurrentY > iChampY)
				{
					m_iCurrentY += -1;
					cMap[m_iCurrentX][m_iCurrentY + 1] = ' ';
				}
			}
			else
			{
			m_iCurrentX += 1;
			cMap[m_iCurrentX - 1][m_iCurrentY] = ' ';
			}
		}
		else if (m_iCurrentX > iChampX)
		{
			if (cMap[m_iCurrentX - 1][m_iCurrentY] != ' ')
			{
				if (m_iCurrentY < iChampY)
				{
					m_iCurrentY += 1;
					cMap[m_iCurrentX][m_iCurrentY - 1] = ' ';
				}
				else if (m_iCurrentY > iChampY)
				{
					m_iCurrentY += -1;
					cMap[m_iCurrentX][m_iCurrentY + 1] = ' ';
				}
			}
			else
			{
				m_iCurrentX += -1;
				cMap[m_iCurrentX + 1][m_iCurrentY] = ' ';
			}
		}
	}
	else if (abs(m_iCurrentX - iChampX) == abs(m_iCurrentY - iChampY))
	{
		if ((m_iCurrentX < iChampX) && (m_iCurrentY > iChampY))
		{
			if (cMap[m_iCurrentX - 1][m_iCurrentY - 1] == ' ')
			{
				m_iCurrentY += -1;
				cMap[m_iCurrentX][m_iCurrentY + 1] = ' ';
			}
			else if (cMap[m_iCurrentX + 1][m_iCurrentY + 1] == ' ')
			{
				m_iCurrentX += 1;
				cMap[m_iCurrentX - 1][m_iCurrentY] = ' ';
			}
			else if ((cMap[m_iCurrentX - 1][m_iCurrentY - 1] != ' ') && (cMap[m_iCurrentX + 1][m_iCurrentY + 1] != ' '))
			{
				//NOTHING
			}
		}
		else if ((m_iCurrentX > iChampX) && (m_iCurrentY < iChampY))
		{
			if (cMap[m_iCurrentX - 1][m_iCurrentY - 1] == ' ')
			{
				m_iCurrentX += -1;
				cMap[m_iCurrentX + 1][m_iCurrentY] = ' ';
			}
			else if (cMap[m_iCurrentX + 1][m_iCurrentY + 1] == ' ')
			{
				m_iCurrentY += 1;
				cMap[m_iCurrentX][m_iCurrentY - 1] = ' ';
			}
			else if ((cMap[m_iCurrentX - 1][m_iCurrentY - 1] != ' ') && (cMap[m_iCurrentX + 1][m_iCurrentY + 1] != ' '))
			{
				//NOTHING
			}
		}
		else if ((m_iCurrentX < iChampX) && (m_iCurrentY < iChampY))
		{
			if (cMap[m_iCurrentX - 1][m_iCurrentY + 1] == ' ')
			{
				m_iCurrentY += 1;
				cMap[m_iCurrentX][m_iCurrentY - 1] = ' ';
			}
			else if (cMap[m_iCurrentX + 1][m_iCurrentY - 1] == ' ')
			{
				m_iCurrentX += 1;
				cMap[m_iCurrentX - 1][m_iCurrentY] = ' ';
			}
			else if ((cMap[m_iCurrentX - 1][m_iCurrentY + 1] != ' ') && (cMap[m_iCurrentX + 1][m_iCurrentY - 1] != ' '))
			{
				//NOTHING
			}
		}
		else if ((m_iCurrentX > iChampX) && (m_iCurrentY > iChampY))
		{
			if (cMap[m_iCurrentX - 1][m_iCurrentY + 1] == ' ')
			{
				m_iCurrentX += -1;
				cMap[m_iCurrentX + 1][m_iCurrentY] = ' ';
			}
			else if (cMap[m_iCurrentX + 1][m_iCurrentY - 1] == ' ')
			{
				m_iCurrentY += -1;
				cMap[m_iCurrentX][m_iCurrentY + 1] = ' ';
			}
			else if ((cMap[m_iCurrentX - 1][m_iCurrentY + 1] != ' ') && (cMap[m_iCurrentX + 1][m_iCurrentY - 1] != ' '))
			{
				//NOTHING
			}
		}
	}
}


VOID CMonster::Renew_Location(char cShape)
{
	if (m_iHP > 0 && cMap[m_iCurrentX][m_iCurrentY] != 0x14)			//HP > 0, NOT ATTACK
	{
		cMap[m_iCurrentX][m_iCurrentY] = cShape;
	}
	else if (m_iHP <= 0 && cMap[m_iCurrentX][m_iCurrentY] == cShape)	//HP <= 0 , THE BODY remained
	{
		cMap[m_iCurrentX][m_iCurrentY] = ' ';
		m_iCurrentX		= 0;
		m_iCurrentY		= 0;
	}
}

VOID CMonster::Attacked_by_Champion()
{
	m_iHP		-= cChampion.m_iATK - m_iDEF;
	if (m_iHP <= 0)
	{
		cChampion.m_iEXP	+= m_iGiveEXP;
		gotoxy(0, 0);
		Map_Load();
	}
}