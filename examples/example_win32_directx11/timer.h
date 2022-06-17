// タイマー表示
#pragma once

#include "Main.h"

HRESULT InitTimer();			// 初期化
void UninitTimer();				// 終了処理
void DrawTimer();				// 描画処理
void UpdateTimer();				// 更新処理

int GetTimer();					// 取得
void resetTimer();				// リセット
