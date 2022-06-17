#ifndef __SCENE_H__
#define __SCENE_H__

#include "Main.h"


enum EScene {
	SCENE_NONE = 0,	// 未設定
	SCENE_TITLE,	// タイトル画面
	//SCENE_MODE,		// モード選択画面
	//SCENE_HOWTOPLAY,// 遊び方 
	SCENE_GAME,		// ゲーム画面
	SCENE_RESULT,	// 結果発表
	//SCENE_RANKING,	// ランキング	

	MAX_SCENE		// シーン総数
};

HRESULT InitScene();			// 初期化
void UninitScene();				// 終了処理
void UpdateScene();				// 更新
void DrawScene();				// 描画

void SetScene(EScene Escene);	// 切り替え
int GetScene();					// シーン番号返す

#endif
