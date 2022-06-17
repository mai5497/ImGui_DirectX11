//----- インクルード部 -----
#include "Result.h"
#include "polygon.h"
#include "Texture.h"
#include "input.h"
#include "Scene.h"
#include "Sound.h"
#include "number.h"
#include "timer.h"
#include "fade.h"


//----- 定数定義 -----
#define RESULT_POS_X		(0.0f)
#define RESULT_POS_Y		(0.0f)
#define RESULT_WIDTH		(SCREEN_WIDTH)
#define RESULT_HEIGHT		(SCREEN_HEIGHT)

#define TIMER_POS_Y			(0.0f + (NUMBER_SIZE_Y*2.5) * 0.5)				// 表示位置
#define MIN_CHRCNT			(2)												// 分文字数
#define SEC_CHRCNT			(2)												// 秒文字数
#define TIMERROGO_POS_Y		(TIMER_POS_Y - (NUMBER_SIZE_Y*2.5) * 0.5)

#define MINCHR_POS_X		(0.0f - (NUMBER_SIZE_X*2.5) * 0.5)															// 分文字位置
#define MINNUM_POS_X		(MINCHR_POS_X - MIN_CHRCNT * (NUMBER_SIZE_X*2.5) - (NUMBER_SIZE_X*2.5) / 2)	// 分数字位置
#define SECNUM_POS_X		(MINCHR_POS_X + (NUMBER_SIZE_X*2.5) - (NUMBER_SIZE_X*2.5) / 2)
#define SECCHR_POS_X		(SECNUM_POS_X + SEC_CHRCNT * (NUMBER_SIZE_X*2.5) + (NUMBER_SIZE_X*2.5) / 2)



enum TEXNUM {
	TEX_BG = 0,
	TEX_TIMER,

	MAX_TEXTURE,
};

enum TIMERROGONUM {
	MIN = 10,
	SEC,
};


//----- グローバル変数宣言 -----
static LPCWSTR g_pszTexFName[MAX_TEXTURE] = {
	L"data/texture/result.png",
	L"data/texture/number001.png",
};

static ID3D11ShaderResourceView *g_pTexture[MAX_TEXTURE];

//------------------------------------------------------------------------------------
//
//				初期化
//
//------------------------------------------------------------------------------------
HRESULT	InitResult() {
	HRESULT hr = S_OK;
	ID3D11Device *pDevice = GetDevice();
	for (int i = 0; i < MAX_TEXTURE; i++) {
		hr = CreateTextureFromFile(pDevice, g_pszTexFName[i], &g_pTexture[i]);
		if (FAILED(hr)) {
			return hr;
		}
	}

	// BGM再生
	CSound::Play(BGM_RESULT);

	return hr;
}

//------------------------------------------------------------------------------------
//
//				終了
//
//------------------------------------------------------------------------------------
void UninitResult() {

	// BGM再生停止
	CSound::Stop(BGM_RESULT);

	// テクスチャ開放
	for (int i = 0; i < MAX_TEXTURE; i++) {
		SAFE_RELEASE(g_pTexture[i]);
	}
}

//------------------------------------------------------------------------------------
//
//				更新
//
//------------------------------------------------------------------------------------
void UpdateResult() {
	//クリックまたは[Enter]押下
	if (GetMouseRelease(MOUSEBUTTON_L) || GetKeyRelease(VK_RETURN)) {
		CSound::Play(SE_DECIDE);
		//ランキング画面へ
		//SetScene(SCENE_TITLE);
		StartFadeOut(SCENE_TITLE);
		return;
	}
}

//------------------------------------------------------------------------------------
//
//				　描画
//
//------------------------------------------------------------------------------------
void DrawResult() {
	int min;
	int sec;

	ID3D11DeviceContext *pDC = GetDeviceContext();
	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);



	SetPolygonSize(RESULT_WIDTH, RESULT_HEIGHT);
	SetPolygonPos(RESULT_POS_X, RESULT_POS_Y);
	SetPolygonTexture(g_pTexture[TEX_BG]);
	DrawPolygon(pDC);

	SetBlendState(BS_ADDITIVE);		// 加算合成
	SetPolygonFrameSize(1.0f / NUMBER_COUNT_X, 1.0f / NUMBER_COUNT_Y);
	SetPolygonSize(NUMBER_SIZE_X * 2.5, NUMBER_SIZE_Y * 2.5);
	SetPolygonPos(MINCHR_POS_X, TIMERROGO_POS_Y);
	SetPolygonUV((MIN % NUMBER_COUNT_X) / (float)NUMBER_COUNT_X,
		(MIN / NUMBER_COUNT_X) / (float)NUMBER_COUNT_Y);
	SetPolygonTexture(g_pTexture[TEX_TIMER]);
	DrawPolygon(pDC);

	SetPolygonFrameSize(1.0f / NUMBER_COUNT_X, 1.0f / NUMBER_COUNT_Y);
	SetPolygonSize(NUMBER_SIZE_X * 2.5, NUMBER_SIZE_Y * 2.5);
	SetPolygonPos(SECCHR_POS_X, TIMERROGO_POS_Y);
	SetPolygonUV((SEC % NUMBER_COUNT_X) / (float)NUMBER_COUNT_X,
		(SEC / NUMBER_COUNT_X) / (float)NUMBER_COUNT_Y);
	SetPolygonTexture(g_pTexture[TEX_TIMER]);
	DrawPolygon(pDC);


	sec = GetTimer();
	min = sec / 60;
	if (sec > 59) {
		sec -= 60 * min;
	}

	DrawNumber(XMFLOAT2(MINNUM_POS_X, TIMER_POS_Y), (unsigned)min, MIN_CHRCNT,NUMBER_SIZE_X * 2.5, NUMBER_SIZE_Y * 2.5);
	DrawNumber(XMFLOAT2(SECNUM_POS_X, TIMER_POS_Y), (unsigned)sec, SEC_CHRCNT,NUMBER_SIZE_X * 2.5, NUMBER_SIZE_Y * 2.5);

	SetZBuffer(true);
	SetBlendState(BS_NONE);


	// 元に戻す
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonFrameSize(1.0f, 1.0f);

}