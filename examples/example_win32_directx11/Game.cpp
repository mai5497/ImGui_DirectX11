#include "Game.h"
#include "player.h"
#include "shadow.h"
#include "bg.h"
#include "bullet.h"
#include "explosion.h"
#include "effect.h"
#include "smoke.h"
#include "meshwall.h"
#include "enemy.h"
#include "meshfield.h"
#include "timer.h"
#include "fade.h"
#include "Sound.h"
#include "DwarfEffect.h"
#include "Boss.h"


HRESULT InitGame() {
	HRESULT hr = S_OK;

	// 丸影初期化
	hr = InitShadow();
	if (FAILED(hr))
		return hr;

	// 自機初期化
	hr = InitPlayer();
	if (FAILED(hr))
		return hr;

	// 敵初期化
	hr = InitEnemy();
	if (FAILED(hr)) {
		return hr;
	}
	hr = InitBoss();
	if (FAILED(hr)) {
		return hr;
	}

	// フィールド初期化
	hr = InitMeshField(16, 16, 80.0f, 80.0f);
	if (FAILED(hr))
		return hr;

	// 背景初期化
	hr = InitBG();
	if (FAILED(hr))
		return hr;

	// ビルボード弾初期化
	hr = InitBullet();
	if (FAILED(hr))
		return hr;

	// 爆発初期化
	hr = InitExplosion();
	if (FAILED(hr))
		return hr;

	// エフェクト初期化
	hr = InitEffect();
	if (FAILED(hr))
		return hr;

	hr = InitDwarfEffect();
	if (FAILED(hr))
		return hr;


	// 煙初期化
	hr = InitSmoke();
	if (FAILED(hr))
		return hr;

	// 壁初期化
	hr = InitMeshWall();
	if (FAILED(hr))
		return hr;

	// タイマー初期化
	hr = InitTimer();
	if (FAILED(hr))
		return hr;

	SetMeshWall(XMFLOAT3(0.0f, 0.0f, 640.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, XMFLOAT2(80.0f, 80.0f));
	SetMeshWall(XMFLOAT3(-640.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -90.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, XMFLOAT2(80.0f, 80.0f));
	SetMeshWall(XMFLOAT3(640.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 90.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, XMFLOAT2(80.0f, 80.0f));
	SetMeshWall(XMFLOAT3(0.0f, 0.0f, -640.0f), XMFLOAT3(0.0f, 180.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, XMFLOAT2(80.0f, 80.0f));
	
	SetMeshWall(XMFLOAT3(0.0f, 0.0f, 640.0f), XMFLOAT3(0.0f, 180.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 0.2f), 16, 2, XMFLOAT2(80.0f, 80.0f));
	SetMeshWall(XMFLOAT3(-640.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 90.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 0.2f), 16, 2, XMFLOAT2(80.0f, 80.0f));
	SetMeshWall(XMFLOAT3(640.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -90.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 0.2f), 16, 2, XMFLOAT2(80.0f, 80.0f));
	SetMeshWall(XMFLOAT3(0.0f, 0.0f, -640.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 0.2f), 16, 2, XMFLOAT2(80.0f, 80.0f));

	CSound::Play(BGM_GAME);

	return hr;

}
void UninitGame() {
	// タイマー終了処理
	UninitTimer();

	// 壁終了処理
	UninitMeshWall();

	// 煙終了処理
	UninitSmoke();

	// エフェクト終了処理
	UninitEffect();
	UninitDwarfEffect();

	// 爆発終了処理
	UninitExplosion();

	// ビルボード弾終了処理
	UninitBullet();

	// 背景終了処理
	UninitBG();

	// フィールド終了処理
	UninitMeshField();

	// 自機終了処理
	UninitPlayer();

	// 敵機終了処理
	UninitBoss();
	UninitEnemy();

	// 丸影終了処理
	UninitShadow();

	CSound::Stop(BGM_GAME);

}

void UpdateGame() {
	// 自機更新
	UpdatePlayer();

	// 敵更新
	UpdateEnemy();
	if (GetEnemyKillSum() == MAX_ENEMY) {
		bool isBossAlive = UpdateBoss();
		if (!isBossAlive) {
			StartFadeOut(SCENE_RESULT);
		}
	}


	// 背景更新
	UpdateBG();

	// フィールド更新
	UpdateMeshField();

	// 丸影更新
	UpdateShadow();

	// ビルボード弾更新
	UpdateBullet();

	// 爆発更新
	UpdateExplosion();

	// エフェクト更新
	UpdateEffect();
	UpdateDwarfEffect();

	// 煙更新
	UpdateSmoke();

	// 壁更新
	UpdateMeshWall();

	// タイマー更新
	UpdateTimer();

}
void DrawGame() {
	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);

	// 背景描画
	DrawBG();

	// タイマー描画
	DrawTimer();

	// Zバッファ有効(Zチェック有&Z更新有)
	SetZBuffer(true);

	// フィールド描画
	DrawMeshField();

	// 壁描画 (不透明部分)
	DrawMeshWall(DRAWPART_OPAQUE);

	// 自機描画
	DrawPlayer();

	// 敵機描画
	DrawEnemy();
	if (GetEnemyKillSum() == MAX_ENEMY) {
		DrawBoss();
	}

	// 丸影描画
	DrawShadow();

	// ビルボード弾描画
	DrawBullet();

	// 煙描画
	DrawSmoke();

	// エフェクト描画
	DrawEffect();
	DrawDwarfEffect();

	// 爆発描画
	DrawExplosion();

	// 壁描画 (半透明部分)
	DrawMeshWall(DRAWPART_TRANSLUCENT);

}
