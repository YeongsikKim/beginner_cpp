#pragma once

class CTower : public CCharacter
{
public:
	int m_iGiveEXP;
	int m_iCurrentX;
	int m_iCurrentY;
public:
	CTower();
	VOID SetTower(int iTower);
	VOID Attacked_by_Champion();
};