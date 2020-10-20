#include "stdafx.h"


VOID Map_Load()
{
	int i	= 0;
	int j	= 0;

	
	cChampion.ChampionStatus();

	//Print Tower Status
	printf("\n\tMiddleTower HP \t: %4d\n", cMiddleTower.m_iHP);
	printf("\n\tLastTower HP \t: %4d\n", cLastTower.m_iHP);
	
	//Renew location of Monster
	cEasyMonster.Renew_Location(0x3A);
	cNormalMonster.Renew_Location(0x25);
	cHardMonster.Renew_Location(0x0F);
	cEasyMonster2.Renew_Location(0x3A);
	cEasyMonster3.Renew_Location(0x3A);
	cEasyMonster4.Renew_Location(0x3A);

	//ALL SIZE MAP LOAD
	for (i = 0; i<MAPSIZE_ROW; i++)
	{
		for (j = 0; j<MAPSIZE_COLUMN; j++)
		{
			printf("%c", cMap[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

VOID Map_Create(int iCurrent_Row, int iCurrent_Column)
{
	int i	= 0;
	
	//Champion Start location
	cMap[iCurrent_Row][iCurrent_Column]		= cChampion.m_cShape;
	
	//Create Map LINE
	for (i = 1; i < MAPSIZE_COLUMN - 1; i++)
	{
		cMap[0][i]		= 0x06;
	}

	for (i = 1; i < MAPSIZE_COLUMN - 1; i++)
	{
		cMap[MAPSIZE_ROW - 1][i]		= 0x06;
	}

	for (i = 1; i < MAPSIZE_ROW - 1; i++)
	{
		cMap[i][0]		= 0x05;
	}

	for (i = 1; i < MAPSIZE_ROW - 1; i++)
	{
		cMap[i][MAPSIZE_COLUMN - 1]		= 0x05;
	}
	cMap[0][0]	= 0x01;
	cMap[MAPSIZE_ROW - 1][0]	= 0x03;
	cMap[0][MAPSIZE_COLUMN - 1]	= 0x02;
	cMap[MAPSIZE_ROW - 1][MAPSIZE_COLUMN - 1]	= 0x04;


	//Create Tower
	cMap[MAPSIZE_ROW/2][MAPSIZE_COLUMN/2]			= 0x18;
	cMap[(MAPSIZE_ROW/2) - 1][MAPSIZE_COLUMN/2]		= 0x18;

	cMap[MAPSIZE_ROW - 2][MAPSIZE_COLUMN - 2]		= 0x15;
	cMap[MAPSIZE_ROW - 3][MAPSIZE_COLUMN - 2]		= 0x15;
	cMap[MAPSIZE_ROW - 4][MAPSIZE_COLUMN - 2]		= 0x15;
	

	//Create Monster
	cEasyMonster.m_iCurrentX		= MAPSIZE_ROW/2 - 3;
	cEasyMonster.m_iCurrentY		= MAPSIZE_COLUMN/2 + 5;

	cNormalMonster.m_iCurrentX		= MAPSIZE_ROW/2;
	cNormalMonster.m_iCurrentY		= MAPSIZE_COLUMN/2 - 2;

	cHardMonster.m_iCurrentX		= MAPSIZE_ROW - 2;
	cHardMonster.m_iCurrentY		= MAPSIZE_COLUMN - 6;

	cEasyMonster2.m_iCurrentX		= MAPSIZE_ROW/2 - 5;
	cEasyMonster2.m_iCurrentY		= MAPSIZE_COLUMN/2 - 6;

	cEasyMonster3.m_iCurrentX		= MAPSIZE_ROW/2 + 2;
	cEasyMonster3.m_iCurrentY		= MAPSIZE_COLUMN/2 + 10;

	cEasyMonster4.m_iCurrentX		= MAPSIZE_ROW/2 + 7;
	cEasyMonster4.m_iCurrentY		= MAPSIZE_COLUMN/2 - 7;
	Map_Load();
}