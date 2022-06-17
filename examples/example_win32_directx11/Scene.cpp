//----- インクルード部 -----
#include "Scene.h"
#include "Title.h"
#include "Game.h"
#include "Result.h"
//#include "HowToPlay.h"
#include "fade.h"


//----- グローバル変数 -----
static EScene g_eScene = SCENE_NONE;

//------------------------------------------------------------------------------------
//
//				初期化
//
//------------------------------------------------------------------------------------
HRESULT InitScene() {
	HRESULT hr = S_OK;

	SetScene(SCENE_TITLE);	// 最初はタイトル画面

	InitFade();

	return hr;
}


//------------------------------------------------------------------------------------
//
//				終了
//
//------------------------------------------------------------------------------------
void UninitScene() {
	SetScene(SCENE_NONE);	// 現在の画面を終了
	UninitFade();
}

//------------------------------------------------------------------------------------
//
//				更新
//
//------------------------------------------------------------------------------------
void UpdateScene() {
	switch (g_eScene) {
	case SCENE_TITLE:	// タイトル画面
		UpdateTitle();
		break;
	//case SCENE_MODE:	// モード選択
	//	UpdateMode();
	//	break;
	//case SCENE_HOWTOPLAY:	// 遊び方
	//	UpdateHowToPlay();
	//	break;
	case SCENE_GAME:	// ゲーム画面
		UpdateGame();
		break;
	case SCENE_RESULT:	// リザルト
		UpdateResult();
		break;
	//case SCENE_RANKING:	// ランキング
	//	UpdateRanking();
	//	break;
	default:
		break;
	}
	UpdateFade();
}

//------------------------------------------------------------------------------------
//
//				描画
//
//------------------------------------------------------------------------------------
void DrawScene() {
	switch (g_eScene) {
	case SCENE_TITLE:	// タイトル画面
		DrawTitle();
		break;
	//case SCENE_MODE:	// モード選択
	//	DrawMode();
	//	break;
	//case SCENE_HOWTOPLAY:	// 遊び方
	//	DrawHowToPlay();
	//	break;
	case SCENE_GAME:	// ゲーム画面
		DrawGame();
		break;
	case SCENE_RESULT:
		DrawResult();
		break;
	//case SCENE_RANKING:
	//	DrawRanking();
	//	break;
	default:
		break;
	}
	DrawFade();
}


//------------------------------------------------------------------------------------
//
//				切り替え
//
//------------------------------------------------------------------------------------
void SetScene(EScene eScene) {
	// 現在の画面を終了
	switch (g_eScene) {
	case SCENE_TITLE:		// タイトル画面
		UninitTitle();
		break;
	//case SCENE_MODE:	// モード選択
	//	UninitMode();
	//	break;
	//case SCENE_HOWTOPLAY:	// 遊び方
	//	UninitHowToPlay();
	//	break;
	case SCENE_GAME:		// ゲーム画面
		UninitGame();
		break;
	case SCENE_RESULT:		// リザルト
		UninitResult();
		break;
	//case SCENE_RANKING:		// ランキング
	//	UninitRanking();
	//	break;
	default:
		break;

	}
	// 画面を入れ替え
	g_eScene = eScene;
	// 次の画面を初期化
	switch (g_eScene) {
	case SCENE_TITLE:		// タイトル画面
		InitTitle();
		break;
	//case SCENE_MODE:	// モード選択
	//	InitMode();
	//	break;
	//case SCENE_HOWTOPLAY:	// 遊び方
	//	InitHowToPlay();
	//	break;
	case SCENE_GAME:		// ゲーム画面
		InitGame();
		break;
	case SCENE_RESULT:
		InitResult();
		break;
	//case SCENE_RANKING:
	//	InitRanking();
	//	break;
	default:
		break;
	}
}

//------------------------------------------------------------------------------------
//
//				シーン番号取得
//
//------------------------------------------------------------------------------------
int GetScene() {
	return g_eScene;
}
