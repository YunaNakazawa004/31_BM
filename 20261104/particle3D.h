//========================================================================
// 
// パーティクル3Dヘッダー [ particle3D.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _PARTICLE3D_H_		// このマクロ定義がされていなかったら
#define _PARTICLE3D_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"
#include "effect3D.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define PARTICLE3D_WIDTH		(24.0f)			// パーティクル3Dの幅
#define PARTICLE3D_HEIGHT		(48.0f)			// パーティクル3Dの高さ

//************************************************************************
// パーティクル3Dクラス
//************************************************************************
class CParticle3D : public CObject
{
public:
	// 種類
	typedef enum
	{
		TYPE_NORMAL = 0,		// 通常
		TYPE_HOMING,			// ホーミング
		TYPE_FAN,				// 風
		TYPE_MAX
	}TYPE;

	CParticle3D(const int nPriority = PRIORITY_2);
	~CParticle3D();

	static CParticle3D* Create(const D3DXVECTOR3 posP, const int nLifeP, const int nValue, const D3DXVECTOR3 nor, const D3DXVECTOR3 angle,
		const float fRadius, const float fAddRadius, const float fMinusAlpha, CEffect3D::TYPE typeE, CParticle3D::TYPE typeP, const bool bGravity,
		const int nLifeE, const float fSpeedE, const bool bSpeedEInertia, const D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), const float fDist = 0.0f,
		const bool bShadow = false, const bool bHoming = false, CObject* HomingObj = nullptr, D3DXVECTOR3 HomingPos = DEFAULT_VECTOR3, const float fSpeedHoming = 0.0f);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 posP, const int nLifeP, const int nValue, const D3DXVECTOR3 nor, const D3DXVECTOR3 angle,
		const float fRadius, const float fAddRadius, const float fMinusAlpha, CEffect3D::TYPE typeE, CParticle3D::TYPE typeP, const bool bGravity,
		const int nLifeE, const float fSpeedE, const bool bSpeedEInertia, const D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), const float fDist = 0.0f,
		const bool bShadow = false, const bool bHoming = false, CObject* HomingObj = nullptr, D3DXVECTOR3 HomingPos = DEFAULT_VECTOR3, const float fSpeedHoming = 0.0f);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	D3DXVECTOR3 GetPosition(void) { return m_posP; }
	D3DXVECTOR3 GetRotation(void) { return DEFAULT_VECTOR3; }

private:
	D3DXVECTOR3 m_posP;			// 基準の位置
	int m_nLifeP;				// パーティクルの寿命
	int m_nValue;				// 出すエフェクトの量
	D3DXVECTOR3 m_nor;			// パーティクルのベクトル
	D3DXVECTOR3 m_angle;		// パーティクルの拡散角度
	float m_fRadius;			// エフェクトの半径
	float m_fAddRadius;			// エフェクトの半径の変化量
	float m_fMinusAlpha;		// エフェクトの透明度の変化量
	CEffect3D::TYPE m_typeE;	// エフェクトの種類
	CParticle3D::TYPE m_typeP;	// パーティクルの種類
	bool m_bGravity;			// エフェクトの重力の有無
	int m_nLifeE;				// エフェクトの寿命
	float m_fSpeedE;			// エフェクトの移動量
	bool m_bSpeedEInertia;		// エフェクトの移動量に慣性をつけるかどうか
	D3DXCOLOR m_col;			// 色
	bool m_bShadow;				// 影の有無
	float m_fDist;				// 中心からの距離
	bool m_bHoming;				// ホーミングするかどうか
	CObject* m_HomingObj;		// ホーミング対象
	D3DXVECTOR3 m_HomingPos;	// ホーミング位置
	float m_fSpeedHoming;		// ホーミングの移動量
};

#endif