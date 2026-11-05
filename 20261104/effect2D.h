//========================================================================
// 
// 2Dエフェクトヘッダー [ effect2D.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _EFFECT2D_H_		// このマクロ定義がされていなかったら
#define _EFFECT2D_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object2D.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define EFFECT2D_TEXFILE	"data\\TEXTURE\\effect000.jpg"		// 2Dエフェクトのテクスチャ
#define EFFECT2D_WIDTH		(10.0f)								// 2Dエフェクトの幅
#define EFFECT2D_HEIGHT		(10.0f)								// 2Dエフェクトの高さ

//************************************************************************
// 2Dエフェクトクラス
//************************************************************************
class CEffect2D : public CObject2D
{
public:
	// 種類
	typedef enum
	{
		TYPE_NORMAL = 0,		// 通常合成
		TYPE_NORMAL_NULL,		// 通常合成テクスチャなし
		TYPE_BLENDADD,			// 加算合成
		TYPE_BLENDADD_NULL,		// 加算合成テクスチャ無し
		TYPE_MAX
	}TYPE;

	CEffect2D(const int nPriority = PRIORITY_UI);
	~CEffect2D();

	static HRESULT Load(void);
	static void Unload(void);
	static CEffect2D* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const float fSpeed,
		const int nLife, const float fRadius, const float fAddRadius, CEffect2D::TYPE type,
		const D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), const bool bHoming = false,
		const D3DXVECTOR3 HomingPos = DEFAULT_VECTOR3, const float fSpeedHoming = 0.0f);

	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const float fSpeed,
		const int nLife, const float fRadius, const float fAddRadius, CEffect2D::TYPE type,
		const D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), const bool bHoming = false,
		const D3DXVECTOR3 HomingPos = DEFAULT_VECTOR3, const float fSpeedHoming = 0.0f);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	static int m_nIdxTexture;	// テクスチャのインデックス
	D3DXVECTOR3 m_move;			// 各方向への移動量
	float m_fSpeed;				// 移動量
	int m_nLife;				// 寿命
	float m_fRadius;			// 半径
	float m_fAddRadius;			// 半径の変更量
	CEffect2D::TYPE m_type;		// 種類
	bool m_bHoming;				// ホーミングするかどうか
	D3DXVECTOR3 m_HomingPos;	// ホーミング対象の位置
	float m_fSpeedHoming;		// ホーミングの移動量
};

#endif