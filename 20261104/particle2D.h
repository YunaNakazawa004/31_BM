//========================================================================
// 
// パーティクル2Dヘッダー [ particle2D.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _PARTICLE2D_H_		// このマクロ定義がされていなかったら
#define _PARTICLE2D_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"
#include "effect2D.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define PARTICLE2D_EFFECT		(500)			// 一度に使えるエフェクトの数
#define PARTICLE2D_WIDTH		(24.0f)			// パーティクル2Dの幅
#define PARTICLE2D_HEIGHT		(48.0f)			// パーティクル2Dの高さ

//************************************************************************
// パーティクル2Dクラス
//************************************************************************
class CParticle2D : public CObject
{
public:
	// 種類
	typedef enum
	{
		TYPE_NORMAL = 0,		// 通常
		TYPE_HOMING,			// ホーミング
		TYPE_BURST,				// バースト
		TYPE_MAX
	}TYPE;

	CParticle2D(const int nPriority = PRIORITY_UI);
	~CParticle2D();

	static CParticle2D* Create(const D3DXVECTOR3 posP, const D3DXVECTOR2 nor, const D3DXVECTOR2 angle, const int nLifeP, const int nValue,
		const float fRadius, const float fAddRadius, CEffect2D::TYPE typeE, CParticle2D::TYPE typeP,
		const int nLifeE, const float fSpeedE, const D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), const float fDist = 0.0f,
		const bool bHoming = false, const D3DXVECTOR3 HomingPos = DEFAULT_VECTOR3, const float fSpeedHoming = 0.0f);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 posP, const D3DXVECTOR2 nor, const D3DXVECTOR2 angle, const int nLifeP, const int nValue,
		const float fRadius, const float fAddRadius, CEffect2D::TYPE typeE, CParticle2D::TYPE typeP,
		const int nLifeE, const float fSpeedE, const D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), const float fDist = 0.0f,
		const bool bHoming = false, const D3DXVECTOR3 HomingPos = DEFAULT_VECTOR3, const float fSpeedHoming = 0.0f);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	D3DXVECTOR3 GetPosition(void) { return m_posP; }
	D3DXVECTOR3 GetRotation(void) { return DEFAULT_VECTOR3; }

private:
	CEffect2D* m_apEffect[PARTICLE2D_EFFECT];		// エフェクトのインスタンス
	D3DXVECTOR3 m_posP;			// 基準の位置
	D3DXVECTOR2 m_nor;			// パーティクルのベクトル
	D3DXVECTOR2 m_angle;		// パーティクルの拡散角度
	int m_nLifeP;				// パーティクルの寿命
	int m_nValue;				// 出すエフェクトの量
	float m_fRadius;			// エフェクトの半径
	float m_fAddRadius;			// エフェクトの半径の変化量
	CEffect2D::TYPE m_typeE;	// エフェクトの種類
	CParticle2D::TYPE m_typeP;	// パーティクルの種類
	int m_nLifeE;				// エフェクトの寿命
	float m_fSpeedE;			// エフェクトの移動量
	D3DXCOLOR m_col;			// 色
	float m_fDist;				// 中心からの距離
	bool m_bHoming;				// ホーミングするかどうか
	D3DXVECTOR3 m_HomingPos;	// ホーミング対象の位置
	float m_fSpeedHoming;		// ホーミングの移動量
};

#endif