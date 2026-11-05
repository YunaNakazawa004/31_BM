//========================================================================
// 
// パーティクルX [ particleX.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "particleX.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"

#include "ObjectX.h"
#include "effectX.h"

//************************************************************************
// マクロ定義
//************************************************************************

//========================================================================
// パーティクルXクラスの生成処理
//========================================================================
CParticleX* CParticleX::Create(const char* pFilename, const D3DXVECTOR3 posP, const int nLifeP, const int nValue,
	const D3DXVECTOR3 nor, const D3DXVECTOR3 angle, const D3DXVECTOR3 scale, const float fAddRadius, CParticleX::TYPE typeP,
	const bool bGravity, const int nLifeE, const float fSpeedE, const bool bSpeedEInertia, const float fDist)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CParticleX* pParticleX = nullptr;

	if (pParticleX == nullptr)
	{// NULLチェック
		// パーティクルXの生成
		pParticleX = new CParticleX;
	}

	if (pParticleX != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pParticleX->Init(pFilename, posP, nLifeP, nValue, nor, angle, scale, fAddRadius, typeP, bGravity, nLifeE,
			fSpeedE, bSpeedEInertia, fDist)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! パーティクルXの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// オブジェクトの種類を設定
		pParticleX->SetType(TYPE_PARTICLE3D);

		return pParticleX;
	}

	OutputDebugStringA("! ! ! パーティクルXの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// パーティクルXクラスのコンストラクタ
//========================================================================
CParticleX::CParticleX(const int nPriority) :CObject(nPriority)
{
	// パーティクルXクラスの値をクリア
	m_pFilename = nullptr;
	m_posP = DEFAULT_VECTOR3;
	m_nLifeP = 0;
	m_nValue = 0;
	m_nor = DEFAULT_VECTOR3;
	m_angle = DEFAULT_VECTOR3;
	m_scale = DEFAULT_VECTOR3;
	m_fAddRadius = 0.0f;
	m_typeP = CParticleX::TYPE_NORMAL;
	m_bGravity = false;
	m_nLifeE = 0;
	m_fSpeedE = 0.0f;
	m_fDist = 0.0f;
}

//========================================================================
// パーティクルXクラスのデストラクタ
//========================================================================
CParticleX::~CParticleX()
{
}

//========================================================================
// パーティクルXクラスの初期化処理
//========================================================================
HRESULT CParticleX::Init(const char* pFilename, const D3DXVECTOR3 posP, const int nLifeP, const int nValue,
	const D3DXVECTOR3 nor, const D3DXVECTOR3 angle, const D3DXVECTOR3 scale, const float fAddRadius, CParticleX::TYPE typeP,
	const bool bGravity, const int nLifeE, const float fSpeedE, const bool bSpeedEInertia, const float fDist)
{
	// パーティクルXクラスの値を初期化
	m_pFilename = pFilename;
	m_posP = posP;
	m_nLifeP = nLifeP;
	m_nValue = nValue;
	m_nor = nor;
	m_angle = angle;
	m_scale = scale;
	m_fAddRadius = fAddRadius;
	m_typeP = typeP;
	m_bGravity = bGravity;
	m_nLifeE = nLifeE;
	m_fSpeedE = fSpeedE;
	m_bSpeedEInertia = bSpeedEInertia;
	m_fDist = fDist;

	return S_OK;
}

//========================================================================
// パーティクルXクラスの終了処理
//========================================================================
void CParticleX::Uninit(void)
{
	// 自分自身を破棄
	CObject::Release();
}

//========================================================================
// パーティクルXクラスの更新処理
//========================================================================
void CParticleX::Update(void)
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

		case TYPE_BOUND:			// 風

			rot.x = m_nor.x + RANDOM_ROT((int)(m_angle.x * 100.0f));		// 角度を設定
			rot.y = m_nor.y + RANDOM_ROT((int)(m_angle.y * 100.0f));		// 角度を設定
			rot.z = m_nor.z + RANDOM_ROT((int)(m_angle.z * 100.0f));		// 角度を設定

			D3DXVec3Normalize(&rot, &rot);						// 正規化

			pos = m_posP + rot * m_fDist;

			break;
		}

		// Xエフェクトの生成
		CEffectX::Create(m_pFilename, pos, rot, m_fSpeedE, m_bSpeedEInertia, m_nLifeE, m_scale,
			m_fAddRadius, m_bGravity);
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
// パーティクルXクラスの描画処理
//========================================================================
void CParticleX::Draw(void)
{
}