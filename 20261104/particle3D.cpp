//========================================================================
// 
// パーティクル3D [ particle3D.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "particle3D.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"

#include "ObjectBillboard.h"
#include "effect3D.h"

//************************************************************************
// マクロ定義
//************************************************************************

//========================================================================
// パーティクル3Dクラスの生成処理
//========================================================================
CParticle3D* CParticle3D::Create(const D3DXVECTOR3 posP, const int nLifeP, const int nValue, const D3DXVECTOR3 nor, const D3DXVECTOR3 angle,
	const float fRadius, const float fAddRadius, const float fMinusAlpha, CEffect3D::TYPE typeE, CParticle3D::TYPE typeP,
	const bool bGravity, const int nLifeE, const float fSpeedE, const bool bSpeedEInertia, const D3DXCOLOR col, const float fDist,
	const bool bShadow, const bool bHoming, CObject* HomingObj, D3DXVECTOR3 HomingPos, const float fSpeedHoming)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CParticle3D* pParticle3D = nullptr;

	if (pParticle3D == nullptr)
	{// NULLチェック
		// パーティクル3Dの生成
		pParticle3D = new CParticle3D;
	}

	if (pParticle3D != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pParticle3D->Init(posP, nLifeP, nValue, nor, angle, fRadius, fAddRadius, fMinusAlpha, typeE, typeP, 
			bGravity, nLifeE, fSpeedE, bSpeedEInertia, col, fDist, bShadow, bHoming, HomingObj, HomingPos, fSpeedHoming)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! パーティクル3Dの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// オブジェクトの種類を設定
		pParticle3D->SetType(TYPE_PARTICLE3D);

		return pParticle3D;
	}

	OutputDebugStringA("! ! ! パーティクル3Dの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// パーティクル3Dクラスのコンストラクタ
//========================================================================
CParticle3D::CParticle3D(const int nPriority) :CObject(nPriority)
{
	// パーティクル3Dクラスの値をクリア
	m_posP = DEFAULT_VECTOR3;
	m_nLifeP = 0;
	m_nValue = 0;
	m_nor = DEFAULT_VECTOR3;
	m_angle = DEFAULT_VECTOR3;
	m_fRadius = 0.0f;
	m_fAddRadius = 0.0f;
	m_typeE = CEffect3D::TYPE_NORMAL;
	m_typeP = CParticle3D::TYPE_NORMAL;
	m_bGravity = false;
	m_nLifeE = 0;
	m_fSpeedE = 0.0f;
	m_col = COLOR_WHITE;
	m_bShadow = false;
	m_fDist = 0.0f;
	m_bHoming = false;
	m_HomingObj = nullptr;
	m_HomingPos = DEFAULT_VECTOR3;
	m_fSpeedHoming = 0.0f;
}

//========================================================================
// パーティクル3Dクラスのデストラクタ
//========================================================================
CParticle3D::~CParticle3D()
{
}

//========================================================================
// パーティクル3Dクラスの初期化処理
//========================================================================
HRESULT CParticle3D::Init(const D3DXVECTOR3 posP, const int nLifeP, const int nValue, const D3DXVECTOR3 nor, const D3DXVECTOR3 angle,
	const float fRadius, const float fAddRadius, const float fMinusAlpha, CEffect3D::TYPE typeE, CParticle3D::TYPE typeP,
	const bool bGravity, const int nLifeE, const float fSpeedE, const bool bSpeedEInertia, const D3DXCOLOR col, const float fDist,
	const bool bShadow, const bool bHoming, CObject* HomingObj, D3DXVECTOR3 HomingPos, const float fSpeedHoming)
{
	// パーティクル3Dクラスの値を初期化
	m_posP = posP;
	m_nLifeP = nLifeP;
	m_nValue = nValue;
	m_nor = nor;
	m_angle = angle;
	m_fRadius = fRadius;
	m_fAddRadius = fAddRadius;
	m_fMinusAlpha = fMinusAlpha;
	m_typeE = typeE;
	m_typeP = typeP;
	m_bGravity = bGravity;
	m_nLifeE = nLifeE;
	m_fSpeedE = fSpeedE;
	m_bSpeedEInertia = bSpeedEInertia;
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
// パーティクル3Dクラスの終了処理
//========================================================================
void CParticle3D::Uninit(void)
{
	// 自分自身を破棄
	CObject::Release();
}

//========================================================================
// パーティクル3Dクラスの更新処理
//========================================================================
void CParticle3D::Update(void)
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
		}

		// 3Dエフェクトの生成
		CEffect3D::Create(pos, rot, m_bGravity, m_fSpeedE, m_bSpeedEInertia, m_nLifeE, m_fRadius,
			m_fAddRadius, m_fMinusAlpha, m_typeE, m_col, m_bShadow, m_bHoming, m_HomingObj, m_HomingPos, m_fSpeedHoming);
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
// パーティクル3Dクラスの描画処理
//========================================================================
void CParticle3D::Draw(void)
{
}