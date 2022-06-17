//----- インクルード部 -----
#include "title.h"
#include "polygon.h"
#include "Texture.h"
#include "input.h"
#include "Scene.h"
#include "fade.h"
#include "Sound.h"

//----- 定数定義 -----
#define TITLE_POS_X			0.0f
#define TITLE_POS_Y			0.0f
#define TITLE_WIDTH			SCREEN_WIDTH
#define TITLE_HEIGHT		SCREEN_HEIGHT

//----- グローバル変数宣言 -----
static LPCWSTR g_pszTexFName = {
	L"data/texture/title.png"
};

static ID3D11ShaderResourceView *g_pTexture;

//------------------------------------------------------------------------------------
//
//				初期化
//
//------------------------------------------------------------------------------------
HRESULT	InitTitle() {
	HRESULT hr = S_OK;
	ID3D11Device *pDevice = GetDevice();
	hr = CreateTextureFromFile(pDevice, g_pszTexFName, &g_pTexture);
	if (FAILED(hr)) {
		return hr;
	}


	// BGM再生
	CSound::Play(BGM_TITLE);

	return hr;
}

//------------------------------------------------------------------------------------
//
//				終了
//
//------------------------------------------------------------------------------------
void UninitTitle() {

	// BGM再生停止
	CSound::Stop(BGM_TITLE);
	// 背景テクスチャ開放
	SAFE_RELEASE(g_pTexture);
}

//------------------------------------------------------------------------------------
//
//				更新
//
//------------------------------------------------------------------------------------
void UpdateTitle() {
	//クリックまたは[Enter]押下
	if (GetMouseRelease(MOUSEBUTTON_L) || GetKeyRelease(VK_RETURN)) {
		CSound::Play(SE_DECIDE);
		//モード選択画面へ
		//SetScene(SCENE_GAME);
		StartFadeOut(SCENE_GAME);
		return;
	}
}

//------------------------------------------------------------------------------------
//
//				描画
//
//------------------------------------------------------------------------------------
void DrawTitle() {
	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);

	ID3D11DeviceContext *pDC = GetDeviceContext();
	SetPolygonSize(TITLE_WIDTH, TITLE_HEIGHT);
	SetPolygonPos(TITLE_POS_X, TITLE_POS_Y);
	SetPolygonTexture(g_pTexture);
	SetPolygonUV(0.0f, 0.0f);

	DrawPolygon(pDC);
}