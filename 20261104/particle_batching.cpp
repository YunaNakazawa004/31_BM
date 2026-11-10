//========================================================================
// 
// バッチ描画パーティクル [ particle_batching.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "particle_batching.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"

#include "effect_batching.h"

//************************************************************************
// マクロ定義
//************************************************************************

//========================================================================
// バッチ描画パーティクルクラスの生成処理
//========================================================================
CParticleBatching* CParticleBatching::Create(const D3DXVECTOR3 posP, const int nLifeP, const int nValue, const D3DXVECTOR3 nor, const D3DXVECTOR3 angle,
	const float fRadius, const float fAddRadius, const float fMinusAlpha, CParticleBatching::TYPE typeP, CEffectBatching::TYPE type,
	const float fGravity, const float fBounce, const int nLifeE, const float fSpeedE, const float fSpeedEInertia, const D3DXCOLOR col, const float fDist,
	const bool bShadow, const bool bHoming, CObject* HomingObj, D3DXVECTOR3 HomingPos, const float fSpeedHoming)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CParticleBatching* pParticleBatching = nullptr;

	if (pParticleBatching == nullptr)
	{// NULLチェック
		// バッチ描画パーティクルの生成
		pParticleBatching = new CParticleBatching;
	}

	if (pParticleBatching != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pParticleBatching->Init(posP, nLifeP, nValue, nor, angle, fRadius, fAddRadius, fMinusAlpha, typeP,type,
			fGravity, fBounce, nLifeE, fSpeedE, fSpeedEInertia, col, fDist, bShadow, bHoming, HomingObj, HomingPos, fSpeedHoming)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! バッチ描画パーティクルの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// オブジェクトの種類を設定
		pParticleBatching->SetType(TYPE_PARTICLE3D);

		return pParticleBatching;
	}

	OutputDebugStringA("! ! ! バッチ描画パーティクルの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// バッチ描画パーティクルクラスのコンストラクタ
//========================================================================
CParticleBatching::CParticleBatching(const int nPriority) :CObject(nPriority)
{
	// バッチ描画パーティクルクラスの値をクリア
	m_posP = DEFAULT_VECTOR3;
	m_nLifeP = 0;
	m_nValue = 0;
	m_nor = DEFAULT_VECTOR3;
	m_angle = DEFAULT_VECTOR3;
	m_fRadius = 0.0f;
	m_fAddRadius = 0.0f;
	m_typeP = CParticleBatching::TYPE_NORMAL;
	m_fGravity = 0.0f;
	m_fBounce = 0.0f;
	m_nLifeE = 0;
	m_fSpeedE = 0.0f;
	m_fSpeedEInertia = 0.0f;
	m_col = COLOR_WHITE;
	m_bShadow = false;
	m_fDist = 0.0f;
	m_bHoming = false;
	m_HomingObj = nullptr;
	m_HomingPos = DEFAULT_VECTOR3;
	m_fSpeedHoming = 0.0f;
}

//========================================================================
// バッチ描画パーティクルクラスのデストラクタ
//========================================================================
CParticleBatching::~CParticleBatching()
{
}

//========================================================================
// バッチ描画パーティクルクラスの初期化処理
//========================================================================
HRESULT CParticleBatching::Init(const D3DXVECTOR3 posP, const int nLifeP, const int nValue, const D3DXVECTOR3 nor, const D3DXVECTOR3 angle,
	const float fRadius, const float fAddRadius, const float fMinusAlpha, CParticleBatching::TYPE typeP, CEffectBatching::TYPE type,
	const float fGravity, const float fBounce, const int nLifeE, const float fSpeedE, const float fSpeedEInertia, const D3DXCOLOR col, const float fDist,
	const bool bShadow, const bool bHoming, CObject* HomingObj, D3DXVECTOR3 HomingPos, const float fSpeedHoming)
{
	// バッチ描画パーティクルクラスの値を初期化
	m_posP = posP;
	m_nLifeP = nLifeP;
	m_nValue = nValue;
	m_nor = nor;
	m_angle = angle;
	m_fRadius = fRadius;
	m_fAddRadius = fAddRadius;
	m_fMinusAlpha = fMinusAlpha;
	m_typeP = typeP;
	m_type = type;
	m_fGravity = fGravity;
	m_fBounce = fBounce;
	m_nLifeE = nLifeE;
	m_fSpeedE = fSpeedE;
	m_fSpeedEInertia = fSpeedEInertia;
	m_col = col;
	m_fDist = fDist;
	m_bShadow = bShadow;
	m_bHoming = bHoming;
	m_HomingObj = HomingObj;
	m_HomingPos = HomingPos;
	m_fSpeedHoming = fSpeedHoming;

	return S_OK;
}

//========================================================================
// バッチ描画パーティクルクラスの終了処理
//========================================================================
void CParticleBatching::Uninit(void)
{
	// 自分自身を破棄
	CObject::Release();
}

//========================================================================
// バッチ描画パーティクルクラスの更新処理
//========================================================================
void CParticleBatching::Update(void)
{
	// ローカル変数
	D3DXVECTOR3 rot = {};	// エフェクトの移動角度
	D3DXVECTOR3 pos = {};	// エフェクトの発生位置

	for (int nCntAppear = 0; nCntAppear < m_nValue; nCntAppear++)
	{// 生成するだけ繰り返す
		switch (m_typeP)
		{
		case TYPE_NORMAL:		// 通常

			rot.x = m_nor.x + RANDOM_ROT((int)(m_angle.x * 100.0f));		// 角度を設定
			rot.y = m_nor.y + RANDOM_ROT((int)(m_angle.y * 100.0f));		// 角度を設定
			rot.z = m_nor.z + RANDOM_ROT((int)(m_angle.z * 100.0f));		// 角度を設定

			D3DXVec3Normalize(&rot, &rot);						// 正規化

			pos = m_posP + rot * m_fDist;

			break;

		case TYPE_HOMING:		// ホーミング

			rot.x = m_nor.x + RANDOM_ROT((int)(m_angle.x * 100.0f));		// 角度を設定
			rot.y = m_nor.y + RANDOM_ROT((int)(m_angle.y * 100.0f));		// 角度を設定
			rot.z = m_nor.z + RANDOM_ROT((int)(m_angle.z * 100.0f));		// 角度を設定

			D3DXVec3Normalize(&rot, &rot);						// 正規化

			pos = m_posP + rot * m_fDist;

			break;

		case TYPE_FAN:			// 風

			rot.x = (float)(rand() % 300 - 150) / 1000.0f;		// 角度を設定
			rot.y = 1.0f;		// 角度を設定
			rot.z = (float)(rand() % 300 - 150) / 1000.0f;		// 角度を設定

			D3DXVec3Normalize(&rot, &rot);						// 正規化

			pos.x = m_posP.x + rot.x * m_fDist;
			pos.y = m_posP.y;
			pos.z = m_posP.z + rot.z * m_fDist;

			break;

		case TYPE_SAND:			// 砂

			rot.x = m_nor.x + RANDOM_ROT((int)(m_angle.x * 100.0f));		// 角度を設定
			rot.y = m_nor.y + RANDOM_ROT((int)(m_angle.y * 100.0f));		// 角度を設定
			rot.z = m_nor.z + RANDOM_ROT((int)(m_angle.z * 100.0f));		// 角度を設定

			D3DXVec3Normalize(&rot, &rot);						// 正規化

			pos.x = m_posP.x + rot.x * m_fDist/*(m_fDist * ((float)(rand() % 5 + 7) * 0.1f))*/;
			pos.y = m_posP.y + rot.y;
			pos.z = m_posP.z + rot.z * m_fDist/*(m_fDist * ((float)(rand() % 5 + 7) * 0.1f))*/;

			break;
		}

		// バッチ描画エフェクトの生成
		CManager::GetEffectBatching()->Set(pos, rot, m_fSpeedE, m_fSpeedEInertia, m_type, m_fGravity, m_fBounce, m_nLifeE, m_fRadius,
			m_fAddRadius, m_fMinusAlpha, m_col, m_bShadow, m_bHoming, m_HomingObj, m_HomingPos, m_fSpeedHoming);
	}

	// 寿命を減らす
	m_nLifeP--;

	if (m_nLifeP < 0)
	{// 寿命が尽きた
		// 終了処理
		Uninit();

		return;
	}
}

//========================================================================
// バッチ描画パーティクルクラスの描画処理
//========================================================================
void CParticleBatching::Draw(void)
{
}