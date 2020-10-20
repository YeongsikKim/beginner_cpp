#include "stdafx.h"

CChampion::CChampion()
{
	m_iHP			= 0;
	m_iMP			= 0;
	m_iATK			= 0;
	m_iDEF			= 0;
	m_iEXP			= 0;
	m_iLev			= 0;
	m_iRecoveryMP	= 0;
	m_cShape		= 0;

	m_iLevArr[0] = 10;
	m_iLevArr[1] = 30;
	m_iLevArr[2] = 60;
	m_iLevArr[3] = 100;
	m_iLevArr[4] = 150;
}

VOID CChampion::SelectChampion()
{
	int iWho;
	while (TRUE)
	{
		system("cls");
		printf("Select the Champion\n\n");
		printf("1.Darius \t\t 2. Izreal \t\t 3. Syndra\n\n");
		printf("Select : ");

		scanf_s("%d", &iWho, sizeof(int));
		switch (iWho)
		{
		case DARIUS:
			{
				m_iHP			= DARIUS_HP;
				m_iMP			= DARIUS_MP;
				m_iATK			= DARIUS_ATK;
				m_iDEF			= DARIUS_DEF;				
				m_iRecoveryMP	= DARIUS_RECOVMP;
				m_cShape		= 0x0B;
				break;
			}
		case IZREAL:
			{

				m_iHP			= IZREAL_HP;
				m_iMP			= IZREAL_MP;
				m_iATK			= IZREAL_ATK;
				m_iDEF			= IZREAL_DEF;
				m_iRecoveryMP	= IZREAL_RECOVMP;
				m_cShape		= 0x0E;
				break;
			}
		case SYNDRA:
			{

				m_iHP			= SYNDRA_HP;
				m_iMP			= SYNDRA_MP;
				m_iATK			= SYNDRA_ATK;
				m_iDEF			= SYNDRA_DEF;
				m_iRecoveryMP	= SYNDRA_RECOVMP;
				m_cShape		= 0x0C;
				break;
			}
		default:
			{
				system("cls");
				printf("[ERROR] You Enter the wrong value!\n");
				Sleep(1000);
				continue;
			}
		}
		break;
	}
}

VOID CChampion::ChampionStatus()
{
	int i					= 0;
	static int iChampMaxHP	= m_iHP;
	char EXP[EXP_BAR]	= {0,};


	if (m_iLev < 4)
	{
		while (m_iEXP >= m_iLevArr[m_iLev])
		{
			m_iEXP		-= m_iLevArr[m_iLev];			
			if (m_iLev == 4)
			{
				break;
			}
			else
			{
				m_iLev++;
				if (m_iHP + 50 <= iChampMaxHP)
				{
					m_iHP		+= 50;
				}
				else if (m_iHP + 50 > iChampMaxHP)
				{
					m_iHP		= iChampMaxHP;
				}
				
				m_iATK		+= 20;
				m_iDEF		+= 5;
			}
		}		

		for (i = 0; i < EXP_BAR; i++)
		{
			EXP[i]		= '-';
		}
		for (i = 0; i < ((m_iEXP * EXP_BAR)/m_iLevArr[m_iLev]); i++)
		{
			EXP[i]		= '=';
		}
	}
	else		//If you reach FULL Level
	{
		for (i = 0; i < EXP_BAR; i++)
		{
			EXP[i]		= '=';
		}
	}

	printf("\tLv : %3d\n", m_iLev + 1);
	printf("\tHP : %3d\t\tATK : %3d\n", m_iHP, m_iATK);
	printf("\tMP : %3d\t\tDEF : %3d\n", m_iMP, m_iDEF);
	printf("\tEXP : ");
	for (i = 0; i < EXP_BAR; i++)
	{
		printf("%c", EXP[i]);
	}
	printf("\n");
}
