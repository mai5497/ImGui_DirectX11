
//=============================================================================
//
// ボス処理 [Boss.cpp]
// Author : 伊地田真衣
//
//=============================================================================
#include "Boss.h"
#include "Main.h"
#include "AssimpModel.h"
#include "debugproc.h"
#include "shadow.h"
#include "collision.h"
#include "explosion.h"
#include "DwarfEffect.h"
#include "Sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MODEL_BOSS			"data/model/kobitored.fbx"

#define	VALUE_MOVE_BOSS	(0.40f)		// 移動速度
#define	RATE_MOVE_BOSS		(0.20f)		// 移動慣性係数
#define	VALUE_ROTATE_BOSS	(7.0f)		// 回転速度
#define	RATE_ROTATE_BOSS	(0.20f)		// 回転慣性係数
#define BOSS_RADIUS		(200.0f)		// 境界球半径


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct TBoss {
	XMFLOAT3	m_pos;		// 現在の位置
	XMFLOAT3	m_rot;		// 現在の向き
	XMFLOAT3	m_rotDest;	// 目的の向き
	XMFLOAT3	m_move;		// 移動量
	XMFLOAT3	m_size;		// 大きさ
	bool		m_use;		// 使用中かどうか
	int			m_HP;		// HP


	XMFLOAT4X4	m_mtxWorld;	// ワールドマトリックス

	int			m_nShadow;	// 丸影番号
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CAssimpModel	g_model;			// モデル
static TBoss		g_boss;	// 敵機情報

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBoss(void) {
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// モデルデータの読み込み
	if (!g_model.Load(pDevice, pDeviceContext, MODEL_BOSS)) {
		MessageBoxA(GetMainWnd(), "モデルデータ読み込みエラー", "InitBoss", MB_OK);
		return E_FAIL;
	}

	// 位置・回転・スケール・サイズの初期設定
	g_boss.m_pos = XMFLOAT3(rand() % 620 - 310.0f,
		0.0f,
		rand() % 620 - 310.0f);
	g_boss.m_rot = XMFLOAT3(0.0f, rand() % 360 - 180.0f, 0.0f);
	g_boss.m_rotDest = g_boss.m_rot;
	g_boss.m_move = XMFLOAT3(
		-SinDeg(g_boss.m_rot.y) * VALUE_MOVE_BOSS,
		0.0f,
		-CosDeg(g_boss.m_rot.y) * VALUE_MOVE_BOSS);
	g_boss.m_size = XMFLOAT3(200.0f, 200.0f, 200.0f);
	// 丸影の生成
	g_boss.m_nShadow = CreateShadow(g_boss.m_pos, 12.0f);
	g_boss.m_use = true;
	g_boss.m_HP = 10;

	return hr;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBoss(void) {
	// 丸影の解放
	ReleaseShadow(g_boss.m_nShadow);
	

	// モデルの解放
	g_model.Release();
}

//=============================================================================
// 更新処理
//=============================================================================
bool UpdateBoss(void) {
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScale;

	if (!g_boss.m_use) {
		return false;
	}
	// 移動
	g_boss.m_pos.x += g_boss.m_move.x;
	g_boss.m_pos.y += g_boss.m_move.y;
	g_boss.m_pos.z += g_boss.m_move.z;

	// 壁にぶつかった
	bool lr = false, fb = false;
	if (g_boss.m_pos.x < -310.0f) {
		g_boss.m_pos.x = -310.0f;
		lr = true;
	}
	if (g_boss.m_pos.x > 310.0f) {
		g_boss.m_pos.x = 310.0f;
		lr = true;
	}
	if (g_boss.m_pos.z < -310.0f) {
		g_boss.m_pos.z = -310.0f;
		fb = true;
	}
	if (g_boss.m_pos.z > 310.0f) {
		g_boss.m_pos.z = 310.0f;
		fb = true;
	}
	if (g_boss.m_pos.y < 0.0f) {
		g_boss.m_pos.y = 0.0f;
	}
	if (g_boss.m_pos.y > 80.0f) {
		g_boss.m_pos.y = 80.0f;
	}
	if (fabsf(g_boss.m_rot.y - g_boss.m_rotDest.y) < 0.0001f) {
		if (lr) {
			g_boss.m_move.x *= -1.0f;
		}
		if (fb) {
			g_boss.m_move.z *= -1.0f;
		}
		if (lr || fb) {
			g_boss.m_rotDest.y = XMConvertToDegrees(atan2f(-g_boss.m_move.x, -g_boss.m_move.z));
		}
	}

	// 目的の角度までの差分
	float fDiffRotY = g_boss.m_rotDest.y - g_boss.m_rot.y;
	if (fDiffRotY >= 180.0f) {
		fDiffRotY -= 360.0f;
	}
	if (fDiffRotY < -180.0f) {
		fDiffRotY += 360.0f;
	}

	// 目的の角度まで慣性をかける
	g_boss.m_rot.y += fDiffRotY * RATE_ROTATE_BOSS;
	if (g_boss.m_rot.y >= 180.0f) {
		g_boss.m_rot.y -= 360.0f;
	}
	if (g_boss.m_rot.y < -180.0f) {
		g_boss.m_rot.y += 360.0f;
	}
	g_boss.m_move = XMFLOAT3(
		-SinDeg(g_boss.m_rot.y) * VALUE_MOVE_BOSS,
		0.0f,
		-CosDeg(g_boss.m_rot.y) * VALUE_MOVE_BOSS);

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	/* ワールド座標の変換は必ず、拡大、回転、移動の順で行う */

	// スケールを反映
	mtxScale = XMMatrixScaling(g_boss.m_size.x, g_boss.m_size.y, g_boss.m_size.z);
	mtxWorld = XMMatrixMultiply(mtxScale, mtxWorld);


	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(g_boss.m_rot.x),
		XMConvertToRadians(g_boss.m_rot.y),
		XMConvertToRadians(g_boss.m_rot.z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(
		g_boss.m_pos.x,
		g_boss.m_pos.y,
		g_boss.m_pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックス設定
	XMStoreFloat4x4(&g_boss.m_mtxWorld, mtxWorld);

	// 丸影の移動
	MoveShadow(g_boss.m_nShadow, g_boss.m_pos);

	return true;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBoss(void) {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	// 不透明部分を描画
	g_model.Draw(pDC, g_boss.m_mtxWorld, eOpacityOnly);

	// 半透明部分を描画
	SetBlendState(BS_ALPHABLEND);	// アルファブレンド有効
	SetZWrite(false);				// Zバッファ更新しない
	g_model.Draw(pDC, g_boss.m_mtxWorld, eTransparentOnly);
	SetZWrite(true);				// Zバッファ更新する
	SetBlendState(BS_NONE);			// アルファブレンド無効
}

//=============================================================================
// 衝突判定
//=============================================================================
int CollisionBoss(XMFLOAT3 pos, float radius, float damage) {
	int hitNum = 0;
	bool hit = CollisionSphere(g_boss.m_pos, BOSS_RADIUS, pos, radius);
	if (hit) {
		hitNum++;
		// 爆発開始
		int nExp = -1;
		if (damage > 0.0f) {
			g_boss.m_HP--;
			StartDwarfEffect(g_boss.m_pos);
			//nExp = SetEffect(g_boss[i].m_pos, XMFLOAT4(0.85f, 0.05f, 0.25f, 0.80f), XMFLOAT2(8.0f, 8.0f), 50);

		} else {
			nExp = StartExplosion(g_boss.m_pos, XMFLOAT2(20.0f, 20.0f));
		}
		SetExplosionColor(nExp, XMFLOAT4(1.0f, 0.7f, 0.7f, 1.0f));

		if (g_boss.m_HP < 0) {	// ぼすしんだ
			g_boss.m_use = false;
			ReleaseShadow(g_boss.m_nShadow);
			CSound::Play(SE_KILL);
		}
	}
	return hitNum;
}

