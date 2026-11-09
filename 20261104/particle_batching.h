//========================================================================
// 
// バッチ描画パーティクルヘッダー [ particle_batching.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _PARTICLE_BATCHING_H_		// このマクロ定義がされていなかったら
#define _PARTICLE_BATCHING_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"
#include "effect_batching.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define PARTICLE_BATCHING_WIDTH			(24.0f)			// バッチ描画パーティクルの幅
#define PARTICLE_BATCHING_HEIGHT		(48.0f)			// バッチ描画パーティクルの高さ

//************************************************************************
// バッチ描画パーティクルクラス
//************************************************************************
class CParticleBatching : public CObject
{
public:
	// 種類
	typedef enum
	{
		TYPE_NORMAL = 0,		// 通常
		TYPE_HOMING,			// ホーミング
		TYPE_FAN,				// 風
		TYPE_SAND,				// 砂
		TYPE_MAX
	}TYPE;

	CParticleBatching(const int nPriority = PRIORITY_2);
	~CParticleBatching();

	static CParticleBatching* Create(const D3DXVECTOR3 posP, const int nLifeP, const int nValue, const D3DXVECTOR3 nor, const D3DXVECTOR3 angle,
		const float fRadius, const float fAddRadius, const float fMinusAlpha, CParticleBatching::TYPE typeP, const float fGravity, const float fBounce,
		const int nLifeE, const float fSpeedE, const float fSpeedEInertia, const D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), const float fDist = 0.0f,
		const bool bShadow = false, const bool bHoming = false, CObject* HomingObj = nullptr, D3DXVECTOR3 HomingPos = DEFAULT_VECTOR3, const float fSpeedHoming = 0.0f);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 posP, const int nLifeP, const int nValue, const D3DXVECTOR3 nor, const D3DXVECTOR3 angle,
		const float fRadius, const float fAddRadius, const float fMinusAlpha, CParticleBatching::TYPE typeP, const float fGravity, const float fBounce,
		const int nLifeE, const float fSpeedE, const float fSpeedEInertia, const D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), const float fDist = 0.0f,
		const bool bShadow = false, const bool bHoming = false, CObject* HomingObj = nullptr, D3DXVECTOR3 HomingPos = DEFAULT_VECTOR3, const float fSpeedHoming = 0.0f);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	D3DXVECTOR3 GetPosition(void) { return m_posP; }
	D3DXVECTOR3 GetRotation(void) { return DEFAULT_VECTOR3; }

private:
	D3DXVECTOR3 m_posP;					// 基準の位置
	int m_nLifeP;						// パーティクルの寿命
	int m_nValue;						// 出すエフェクトの量
	D3DXVECTOR3 m_nor;					// パーティクルのベクトル
	D3DXVECTOR3 m_angle;				// パーティクルの拡散角度
	float m_fRadius;					// エフェクトの半径
	float m_fAddRadius;					// エフェクトの半径の変化量
	float m_fMinusAlpha;				// エフェクトの透明度の変化量
	CParticleBatching::TYPE m_typeP;	// パーティクルの種類
	float m_fGravity;					// エフェクトの重力
	float m_fBounce;					// エフェクトの跳ね返り
	int m_nLifeE;						// エフェクトの寿命
	float m_fSpeedE;					// エフェクトの移動量
	float m_fSpeedEInertia;				// エフェクトの移動量の慣性
	D3DXCOLOR m_col;					// 色
	bool m_bShadow;						// 影の有無
	float m_fDist;						// 中心からの距離
	bool m_bHoming;						// ホーミングするかどうか
	CObject* m_HomingObj;				// ホーミング対象
	D3DXVECTOR3 m_HomingPos;			// ホーミング位置
	float m_fSpeedHoming;				// ホーミングの移動量
};

#endif