#include "stdafx.h"




int _tmain()
{	
	int iCurrent_Row		= 0;
	int iCurrent_Column		= 0;
	int i					= 0;
	int j					= 0;
	int iTemp				= 0;
	int iEasyMonsterCount	= 0;
	int iNormalMonsterCount = 1;
	int iHardMonsterCount	= 1;
	int iMaxMP				= 0;
	char cTemp				= NULL;
	char cKey				= NULL;
	struct tm *date;
	

	for (i = 0; i<MAPSIZE_ROW; i++)
	{
		for(j = 0; j<MAPSIZE_COLUMN; j++)
		{
			cMap[i][j]		= ' ';
		}
	}

	//Champion Set
	cChampion.SelectChampion();
	iMaxMP		= cChampion.m_iMP;
	
	//Tower Set
	cMiddleTower.SetTower(MIDDLETOWER);
	cLastTower.SetTower(LASTTOWER);

	//Monster Set
	cEasyMonster.Monster_Select(EASY);
	cNormalMonster.Monster_Select(NORMAL);
	cHardMonster.Monster_Select(HARD);

	cEasyMonster2.Monster_Select(EASY);
	cEasyMonster3.Monster_Select(EASY);
	cEasyMonster4.Monster_Select(EASY);

	system("cls");	
	
	//Start location
	iCurrent_Row		= 3;
	iCurrent_Column		= 3;

	Map_Create(iCurrent_Row, iCurrent_Column);

	
	while (TRUE)
	{
		//Bad ending
		if (cChampion.m_iHP <= 0)
		{
			system("cls");
			printf("\n\tThe Player was dead...\n\n");
			return 0;
		}


		//Good ending
		if (cLastTower.m_iHP <= 0)
		{
			system("cls");
			printf("\n\tNice work!! Game clear!!\n\n");
			return 0;
		}

		const time_t t		= time(NULL);
		date	= localtime(&t);
		

		//Monster Setting
		if (iTemp != date->tm_sec)
		{
			if (cChampion.m_iMP < iMaxMP)
			{
				cChampion.m_iMP		+= cChampion.m_iRecoveryMP;
				gotoxy(0, 0);
				cChampion.ChampionStatus();
			}
			iTemp	= date->tm_sec;
			iEasyMonsterCount++;
			iNormalMonsterCount++;
			iHardMonsterCount++;


			if (iEasyMonsterCount == EASY_TIMEDELAY)
			{					
				//Insert the source about Monster Moving
				if (cEasyMonster.m_iHP > 0)
				{
					cEasyMonster.Attack_and_Moving();
				}
				if (cEasyMonster2.m_iHP > 0)
				{
					cEasyMonster2.Attack_and_Moving();
				}
			
				
				gotoxy(0, 0);
				Map_Load();
				iEasyMonsterCount	= 0;
			}




			if (iNormalMonsterCount == NORMAL_TIMEDELAY)
			{
				if (cEasyMonster4.m_iHP > 0)
				{
					cEasyMonster4.Attack_and_Moving();
				}
				//Insert the source about Monster Moving
				if (cMiddleTower.m_iHP < MIDDLETOWER_HP || cNormalMonster.m_iHP < NORMAL_HP)
				{
					cNormalMonster.Attack_and_Moving();
					gotoxy(0, 0);
					Map_Load();
				}			
				iNormalMonsterCount = 0;
			}




			if (iHardMonsterCount == HARD_TIMEDELAY)
			{
				if (cEasyMonster3.m_iHP > 0)
				{
					cEasyMonster3.Attack_and_Moving();
				}
				//Insert the source about Monster Moving
				if (cLastTower.m_iHP < LASTTOWER_HP || cHardMonster.m_iHP < HARD_HP)
				{
					cHardMonster.Attack_and_Moving();
					gotoxy(0, 0);
					Map_Load();
				}				
				iHardMonsterCount	= 0;
			}
		}
		
		if (_kbhit())			//If you did not have pressed the keyboard, not execute
		{
			cKey		= _getch();
			if (cKey == -32 || cKey == 0)
			{
				cKey	= _getch();

				switch (cKey)
				{
				case GO_NORTH:
					{
						
						if (cMap[iCurrent_Row - 1][iCurrent_Column] == ' ')
						{
							cMap[iCurrent_Row][iCurrent_Column]	= ' ';
							iCurrent_Row	+= -1;
							cMap[iCurrent_Row][iCurrent_Column]	= cChampion.m_cShape;
							gotoxy(0, 0);
							Map_Load();
						}
						break;
					}
				case GO_SOUTH:
					{
						if (cMap[iCurrent_Row + 1][iCurrent_Column] == ' ')
						{
							cMap[iCurrent_Row][iCurrent_Column]	= ' ';
							iCurrent_Row	+= 1;
							cMap[iCurrent_Row][iCurrent_Column]	= cChampion.m_cShape;
							gotoxy(0, 0);
							Map_Load();
						}
						break;
					}
				case GO_WEST:
					{
						if (cMap[iCurrent_Row][iCurrent_Column - 1] == ' ')
						{
							cMap[iCurrent_Row][iCurrent_Column]	= ' ';
							iCurrent_Column	+= -1;
							cMap[iCurrent_Row][iCurrent_Column]	= cChampion.m_cShape;
							gotoxy(0, 0);
							Map_Load();
						}
						break;
					}
				case GO_EAST:
					{
						if (cMap[iCurrent_Row][iCurrent_Column + 1] == ' ')
						{
							cMap[iCurrent_Row][iCurrent_Column]	= ' ';
							iCurrent_Column	+= 1;
							cMap[iCurrent_Row][iCurrent_Column]	= cChampion.m_cShape;
							gotoxy(0, 0);
							Map_Load();
						}
						break;
					}
					
				}//switch end
			}
			else if (cKey == ATTACK_NORTH)
			{
				if (iCurrent_Row - 1 > 0)
				{
					if (cChampion.m_iMP >= 10)
					{
						cChampion.m_iMP -= 10;
						cTemp		= cMap[iCurrent_Row - 1][iCurrent_Column];					
						cMap[iCurrent_Row - 1][iCurrent_Column] = 0x14;
						gotoxy(0, 0);
						Map_Load();
						Sleep(250);

						cMap[iCurrent_Row - 1][iCurrent_Column] = cTemp;
						if (cTemp != ' ')
						{
							Attack(iCurrent_Row - 1, iCurrent_Column);
						}
						gotoxy(0, 0);
						Map_Load();
					}
					else
					{
						
					}
				}
			}
			else if (cKey == ATTACK_SOUTH)
			{
				if (iCurrent_Row + 2 < MAPSIZE_ROW)
				{
					if (cChampion.m_iMP >= 10)
					{
						cChampion.m_iMP -= 10;
						cTemp		= cMap[iCurrent_Row + 1][iCurrent_Column];			
						cMap[iCurrent_Row + 1][iCurrent_Column] = 0x14;
						gotoxy(0, 0);
						Map_Load();
						Sleep(250);
						cMap[iCurrent_Row + 1][iCurrent_Column] = cTemp;
						if (cTemp != ' ')
						{
							Attack(iCurrent_Row + 1, iCurrent_Column);
						}
						gotoxy(0, 0);
						Map_Load();
					}
					else
					{
					}
				}
			}
			else if (cKey == ATTACK_WEST)
			{
				if (iCurrent_Column - 1 > 0)
				{
					if (cChampion.m_iMP >= 10)
					{
						cChampion.m_iMP -= 10;
						cTemp		= cMap[iCurrent_Row][iCurrent_Column - 1];					
						cMap[iCurrent_Row][iCurrent_Column - 1] = 0x14;
						gotoxy(0, 0);
						Map_Load();
						Sleep(250);
						cMap[iCurrent_Row][iCurrent_Column - 1] = cTemp;
						if (cTemp != ' ')
						{
							Attack(iCurrent_Row, iCurrent_Column - 1);
						}
						gotoxy(0, 0);
						Map_Load();
					}
					else
					{
					}
				}
			}
			else if (cKey == ATTACK_EAST)
			{
				if (iCurrent_Column + 2 <  MAPSIZE_COLUMN)
				{
					if (cChampion.m_iMP >= 10)
					{
						cChampion.m_iMP -= 10;
						cTemp		= cMap[iCurrent_Row][iCurrent_Column + 1];			
						cMap[iCurrent_Row][iCurrent_Column + 1] = 0x14;
						gotoxy(0, 0);
						Map_Load();
						Sleep(250);
						cMap[iCurrent_Row][iCurrent_Column + 1] = cTemp;
						if (cTemp != ' ')
						{
							Attack(iCurrent_Row, iCurrent_Column + 1);
						}
						gotoxy(0, 0);
						Map_Load();
					}
					else
					{
					}
				}
			}
			else
			{
				printf("There is no function. This key : %d\n", cKey);
			}
			
		}//if end
	}//while end
	

	

	
	return 0;
}


VOID gotoxy(int x, int y)
{
	COORD pos;
	pos.X	= x;
	pos.Y	= y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}