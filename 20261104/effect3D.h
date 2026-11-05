//========================================================================
// 
// 3Dエフェクトヘッダー [ effect3D.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _EFFECT3D_H_		// このマクロ定義がされていなかったら
#define _EFFECT3D_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "ObjectBillboard.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define EFFECT3D_TEXFILE	"data\\TEXTURE\\effect000.jpg"		// 3Dエフェクトのテクスチャ
#define EFFECT3D_WIDTH		(10.0f)								// 3Dエフェクトの幅
#define EFFECT3D_HEIGHT		(10.0f)								// 3Dエフェクトの高さ

//************************************************************************
// 3Dエフェクトクラス
//************************************************************************
class CEffect3D : public CObjectBillboard
{
public:
	// 種類
	typedef enum
	{
		TYPE_NORMAL = 0,		// 通常合成
		TYPE_NORMAL_NULL,		// 通常合成テクスチャなし
		TYPE_NORMAL_DISTORT,	// 通常合成テクスチャなし(歪)
		TYPE_BLENDADD,			// 加算合成
		TYPE_BLENDADD_NULL,		// 加算合成テクスチャ無し
		TYPE_MAX
	}TYPE;

	CEffect3D(const int nPriority = PRIORITY_3);
	~CEffect3D();

	static HRESULT Load(void);
	static void Unload(void);
	static CEffect3D* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const bool bGravity, const float fSpeed, const bool bSpeedInertia,
		const int nLife, const float fRadius, const float fAddRadius, const float fMinusAlpha, CEffect3D::TYPE type,
		const D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), const bool bShadow = false, const bool bHoming = false,
		CObject* HomingObj = nullptr, D3DXVECTOR3 HomingPos = DEFAULT_VECTOR3, const float fSpeedHoming = 0.0f);

	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const bool bGravity, const float fSpeed, const bool bSpeedInertia,
		const int nLife, const float fRadius, const float fAddRadius, const float fMinusAlpha, CEffect3D::TYPE type,
		const D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), const bool bShadow = false, const bool bHoming = false,
		CObject* HomingObj = nullptr, D3DXVECTOR3 HomingPos = DEFAULT_VECTOR3, const float fSpeedHoming = 0.0f);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	static int m_nIdxTexture;	// テクスチャのインデックス
	D3DXVECTOR3 m_move;			// 各方向への移動量
	bool m_bGravity;			// 重力の有無
	float m_fSpeed;				// 移動量
	bool m_bSpeedInertia;		// 移動の慣性
	int m_nLife;				// 寿命
	float m_fRadius;			// 半径
	float m_fAddRadius;			// 半径の変更量
	float m_fMinusAlpha;		// 透明度の変更量
	CEffect3D::TYPE m_type;		// 種類
	bool m_bHoming;				// ホーミングするかどうか
	CObject* m_HomingObj;		// ホーミング対象
	D3DXVECTOR3 m_HomingPos;	// ホーミング位置
	float m_fSpeedHoming;		// ホーミングの移動量
	bool m_bShadow;				// 影の有無
};

#endif