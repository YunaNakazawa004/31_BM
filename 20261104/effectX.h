//========================================================================
// 
// Xエフェクトヘッダー [ effectX.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _EFFECTX_H_		// このマクロ定義がされていなかったら
#define _EFFECTX_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "objectX.h"

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// Xエフェクトクラス
//************************************************************************
class CEffectX : public CObjectX
{
public:
	CEffectX(const int nPriority = PRIORITY_3);
	~CEffectX();

	static HRESULT Load(const char* pFilename);
	static void Unload(void);
	static CEffectX* Create(const char* pFilename, const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const float fSpeed, const bool bSpeedInertia,
		const int nLife, const D3DXVECTOR3 scale, const float fAddRadius, const bool bGravity);

	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const float fSpeed, const bool bSpeedInertia,
		const int nLife, const D3DXVECTOR3 scale, const float fAddRadius, const bool bGravity);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	static int m_nIdxModel;		// モデルのインデックス
	D3DXVECTOR3 m_move;			// 各方向への移動量
	float m_fSpeed;				// 移動量
	bool m_bSpeedInertia;		// 移動の慣性
	int m_nLife;				// 寿命
	float m_fAddRadius;			// 半径の変更量
	float m_fMinusAlpha;		// 透明度の変更量
	bool m_bGravity;			// 重力の有無
	float m_fRadiusDef;			// 規定の半径
	float m_fRadius;			// 半径
};

#endif