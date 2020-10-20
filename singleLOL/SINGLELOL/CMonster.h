#pragma once

class CMonster : public CCharacter
{
public:
	int m_iGiveEXP;
	int m_iCurrentX;
	int m_iCurrentY;
public:
	CMonster();
	VOID Monster_Select(int iMonster);
	VOID Attack_and_Moving();
	VOID Renew_Location(char cShape);
	VOID Attacked_by_Champion();
};