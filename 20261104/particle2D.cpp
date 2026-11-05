//========================================================================
// 
// パーティクル2D [ particle2D.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "particle2D.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"

#include "object2D.h"
#include "effect2D.h"

//************************************************************************
// マクロ定義
//************************************************************************

//========================================================================
// パーティクル2Dクラスの生成処理
//========================================================================
CParticle2D* CParticle2D::Create(const D3DXVECTOR3 posP, const D3DXVECTOR2 nor, const D3DXVECTOR2 angle,
	const int nLifeP, const int nValue, const float fRadius, const float fAddRadius, CEffect2D::TYPE typeE,
	CParticle2D::TYPE typeP, const int nLifeE, const float fSpeedE, const D3DXCOLOR col, const float fDist,
	const bool bHoming, const D3DXVECTOR3 HomingPos, const float fSpeedHoming)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CParticle2D* pParticle2D = nullptr;

	if (pParticle2D == nullptr)
	{// NULLチェック
		// パーティクル2Dの生成
		pParticle2D = new CParticle2D;
	}

	if (pParticle2D != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pParticle2D->Init(posP, nor, angle, nLifeP, nValue, fRadius, fAddRadius, typeE, typeP, nLifeE, fSpeedE,
			col, fDist, bHoming, HomingPos, fSpeedHoming)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! パーティクル2Dの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// オブジェクトの種類を設定
		pParticle2D->SetType(TYPE_PARTICLE2D);

		return pParticle2D;
	}

	OutputDebugStringA("! ! ! パーティクル2Dの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// パーティクル2Dクラスのコンストラクタ
//========================================================================
CParticle2D::CParticle2D(const int nPriority) :CObject(nPriority)
{
	// パーティクル2Dクラスの値をクリア
	memset(&m_apEffect[0], NULL, sizeof m_apEffect);
	m_posP = DEFAULT_VECTOR3;
	m_nor = DEFAULT_VECTOR2;
	m_angle = DEFAULT_VECTOR2;
	m_nLifeP = 0;
	m_nValue = 0;
	m_fRadius = 0.0f;
	m_fAddRadius = 0.0f;
	m_typeE = CEffect2D::TYPE_NORMAL;
	m_typeP = CParticle2D::TYPE_NORMAL;
	m_nLifeE = 0;
	m_fSpeedE = 0.0f;
	m_col = COLOR_WHITE;
	m_fDist = 0.0f;
	m_bHoming = false;
	m_HomingPos = DEFAULT_VECTOR3;
	m_fSpeedHoming = 0.0f;
}

//========================================================================
// パーティクル2Dクラスのデストラクタ
//========================================================================
CParticle2D::~CParticle2D()
{
}

//========================================================================
// パーティクル2Dクラスの初期化処理
//========================================================================
HRESULT CParticle2D::Init(const D3DXVECTOR3 posP, const D3DXVECTOR2 nor, const D3DXVECTOR2 angle,
	const int nLifeP, const int nValue, const float fRadius, const float fAddRadius, CEffect2D::TYPE typeE,
	CParticle2D::TYPE typeP, const int nLifeE, const float fSpeedE, const D3DXCOLOR col, const float fDist,
	const bool bHoming, const D3DXVECTOR3 HomingPos, const float fSpeedHoming)
{
	// パーティクル2Dクラスの値を初期化
	memset(&m_apEffect[0], NULL, sizeof m_apEffect);
	m_posP = posP;
	m_nor = nor;
	m_angle = angle;
	m_nLifeP = nLifeP;
	m_nValue = nValue;

	if (m_nValue > PARTICLE2D_EFFECT)
	{// エフェクトの最大数
		m_nValue = PARTICLE2D_EFFECT;
	}

	m_fRadius = fRadius;
	m_fAddRadius = fAddRadius;
	m_typeE = typeE;
	m_typeP = typeP;
	m_nLifeE = nLifeE;
	m_fSpeedE = fSpeedE;
	m_col = col;
	m_fDist = fDist;
	m_bHoming = bHoming;
	m_HomingPos = HomingPos;
	m_fSpeedHoming = fSpeedHoming;

	return S_OK;
}

//========================================================================
// パーティクル2Dクラスの終了処理
//========================================================================
void CParticle2D::Uninit(void)
{
	for (int nCntParticle2D = 0; nCntParticle2D < PARTICLE2D_EFFECT; nCntParticle2D++)
	{
		if (m_apEffect[nCntParticle2D] != nullptr)
		{// NULLチェック
			// 終了処理
			m_apEffect[nCntParticle2D] = nullptr;
		}
	}

	// 自分自身を破棄
	CObject::Release();
}

//========================================================================
// パーティクル2Dクラスの更新処理
//========================================================================
void CParticle2D::Update(void)
{
	// ローカル変数
	D3DXVECTOR3 rot = {};	// エフェクトの移動角度
	D3DXVECTOR3 pos = {};	// エフェクトの発生位置

	for (int nCntAppear = 0; nCntAppear < m_nValue; nCntAppear++)
	{// 生成するだけ繰り返す
		if (m_apEffect[nCntAppear] == nullptr)
		{// NULLチェック
			switch (m_typeP)
			{
			case TYPE_NORMAL:		// 通常

				rot.x = m_nor.x + RANDOM_ROT((int)(m_angle.x * 100.0f));		// 角度を設定
				rot.y = m_nor.y + RANDOM_ROT((int)(m_angle.y * 100.0f));		// 角度を設定
				rot.z = 0.0f;

				D3DXVec3Normalize(&rot, &rot);						// 正規化

				pos = m_posP;

				break;

			case TYPE_HOMING:		// ホーミング

				rot.x = m_nor.x + RANDOM_ROT((int)(m_angle.x * 100.0f));		// 角度を設定
				rot.y = m_nor.y + RANDOM_ROT((int)(m_angle.y * 100.0f));		// 角度を設定
				rot.z = 0.0f;

				D3DXVec3Normalize(&rot, &rot);						// 正規化

				pos = m_posP + rot * m_fDist;

				break;

			case TYPE_BURST:		// バースト

				rot.x = m_nor.x + RANDOM_ROT((int)(m_angle.x * 100.0f));		// 角度を設定
				rot.y = m_nor.y + RANDOM_ROT((int)(m_angle.y * 100.0f));		// 角度を設定
				rot.z = 0.0f;

				D3DXVec3Normalize(&rot, &rot);						// 正規化

				pos = m_posP + rot * m_fDist;

				break;
			}

			// 2Dエフェクトの生成
			m_apEffect[nCntAppear] = CEffect2D::Create(pos, rot, m_fSpeedE, m_nLifeE, m_fRadius, m_fAddRadius,
				m_typeE, m_col, m_bHoming, m_HomingPos, m_fSpeedHoming);
		}
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
// パーティクル2Dクラスの描画処理
//========================================================================
void CParticle2D::Draw(void)
{
}