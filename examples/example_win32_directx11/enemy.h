//=============================================================================
//
// 敵機処理 [enemy.h]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#pragma once

#include "Main.h"

#ifdef _DEBUG
#define MAX_ENEMY			(1)		// 敵最大(デバッグ用)
#else
#define MAX_ENEMY			(50)	// 敵最大(ほんちゃん)
#endif

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);
int CollisionEnemy(XMFLOAT3 pos, float radius, float damage);
int GetEnemyKillSum();
