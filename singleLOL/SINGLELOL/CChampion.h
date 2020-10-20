#pragma once

class CChampion : public CCharacter
{
public:
	int		m_iEXP;
	int		m_iLev;
	int		m_iLevArr[5];
	int		m_iRecoveryMP;
	char	m_cShape;
public:
	CChampion();
	VOID SelectChampion();
	VOID ChampionStatus();
};