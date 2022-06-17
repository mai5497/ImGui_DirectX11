//フェードイン・フェードアウト
#pragma once

#include "Scene.h"

enum EFade{
	FADE_NONE = 0,		// 何もしていない
	FADE_IN,			// フェードイン処理中
	FADE_OUT,			// フェードアウト処理中

	MAX_FADE			// フェード最大値
};


// プロトタイプ宣言
HRESULT InitFade();									// 初期化
void UninitFade();									// 終了
void UpdateFade();									// 更新
void DrawFade();									// 描画
//enum EScene;
void StartFadeOut(EScene eNext);						// フェードアウト開始

EFade GetFade();									// 状態取得
void SetFadeColor(float fR, float fG, float fB);	// フェードアウト色設定
