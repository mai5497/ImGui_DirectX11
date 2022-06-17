//=============================================================================
//
// エフェクト処理 [effect.h]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#pragma once

#include "Main.h"



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);

int SetEffect(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, XMFLOAT2 size, int timer);
int SetEffect(XMFLOAT3 pos, XMFLOAT4 col, XMFLOAT2 size, int timer);

void SetBurstFlg(bool flg);
void StartBurst();
