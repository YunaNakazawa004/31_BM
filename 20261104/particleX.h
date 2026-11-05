//========================================================================
// 
// パーティクルXヘッダー [ particleX.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _PARTICLEX_H_		// このマクロ定義がされていなかったら
#define _PARTICLEX_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// パーティクルXクラス
//************************************************************************
class CParticleX : public CObject
{
public:
	// 種類
	typedef enum
	{
		TYPE_NORMAL = 0,		// 通常
		TYPE_BOUND,				// バウンド
		TYPE_MAX
	}TYPE;

	CParticleX(const int nPriority = PRIORITY_2);
	~CParticleX();

	static CParticleX* Create(const char* pFilename, const D3DXVECTOR3 posP, const int nLifeP, const int nValue,
		const D3DXVECTOR3 nor, const D3DXVECTOR3 angle, const D3DXVECTOR3 scale, const float fAddRadius, CParticleX::TYPE typeP,
		const bool bGravity, const int nLifeE, const float fSpeedE, const bool bSpeedEInertia,  const float fDist = 0.0f);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const char* pFilename, const D3DXVECTOR3 posP, const int nLifeP, const int nValue,
		const D3DXVECTOR3 nor, const D3DXVECTOR3 angle, const D3DXVECTOR3 scale, const float fAddRadius, CParticleX::TYPE typeP,
		const bool bGravity, const int nLifeE, const float fSpeedE, const bool bSpeedEInertia, const float fDist = 0.0f);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	D3DXVECTOR3 GetPosition(void) { return m_posP; }
	D3DXVECTOR3 GetRotation(void) { return DEFAULT_VECTOR3; }

private:
	const char* m_pFilename;	// モデルファイルの名前
	D3DXVECTOR3 m_posP;			// 基準の位置
	int m_nLifeP;				// パーティクルの寿命
	int m_nValue;				// 出すエフェクトの量
	D3DXVECTOR3 m_nor;			// パーティクルのベクトル
	D3DXVECTOR3 m_angle;		// パーティクルの拡散角度
	D3DXVECTOR3 m_scale;		// エフェクトのスケール
	float m_fAddRadius;			// エフェクトの半径の変化量
	CParticleX::TYPE m_typeP;	// パーティクルの種類
	bool m_bGravity;			// エフェクトの重力の有無
	int m_nLifeE;				// エフェクトの寿命
	float m_fSpeedE;			// エフェクトの移動量
	bool m_bSpeedEInertia;		// エフェクトの移動量に慣性をつけるかどうか
	float m_fDist;				// 中心からの距離
};

#endif