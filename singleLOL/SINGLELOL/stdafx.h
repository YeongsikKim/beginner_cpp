#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <wchar.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>



#include "pre_define.h"
#include "CCharacter.h"
#include "CChampion.h"
#include "CMonster.h"
#include "CTower.h"
#include "ImageBuffer.h"
#include "Attack.h"

extern char			cMap[MAPSIZE_ROW][MAPSIZE_COLUMN];
extern CChampion	cChampion;
extern CTower		cMiddleTower;
extern CTower		cLastTower;
extern CMonster		cEasyMonster;
extern CMonster		cNormalMonster;
extern CMonster		cHardMonster;

extern CMonster		cEasyMonster2;
extern CMonster		cEasyMonster3;
extern CMonster		cEasyMonster4;


VOID gotoxy(int x, int y);