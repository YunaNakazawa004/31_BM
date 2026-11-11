//========================================================================
// 
// バッチ描画エフェクトヘッダー [ effect_batching.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _EFFECT_BATCHING_H_		// このマクロ定義がされていなかったら
#define _EFFECT_BATCHING_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"
#include <vector>

//************************************************************************
// マクロ定義
//************************************************************************
#define EFFECT_BATCHING_WIDTH		(10.0f)								// 3Dエフェクトの幅
#define EFFECT_BATCHING_HEIGHT		(10.0f)								// 3Dエフェクトの高さ

//************************************************************************
// バッチ描画エフェクトクラス
//************************************************************************
class CEffectBatching
{
public:
	// 種類
	enum TYPE
	{
		TYPE_ADD = 0,		// 加算合成
		TYPE_NORMAL,		// 通常合成
		TYPE_MAX
	};

	CEffectBatching();
	~CEffectBatching();

	static HRESULT Load(void);
	static void Unload(void);
	static CEffectBatching* Create(const int nMax);

	HRESULT Init(const int nMax);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawNormal(void);
	void DrawShadow(void);

	void ShadowCalc(D3DXMATRIX matrix, const float fRadius);

	void Set(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const float fSpeed, const float fSpeedInertia, const int type,
		const float fGravity, const float fBounce, const int nLife, const float fRadius, const float fAddRadius, const float fMinusAlpha,
		const D3DXCOLOR col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), const bool bShadow = false, const bool bHoming = false,
		CObject* HomingObj = nullptr, D3DXVECTOR3 HomingPos = DEFAULT_VECTOR3, const float fSpeedHoming = 0.0f);

	void Reset(void);
	void SwapAndPop(const int nIdx);

private:
	static int m_aIdxTexture[2];				// テクスチャのインデックス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;			// インデックスバッファへのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffNormal;	// 通常描画の頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffNormal;	// 通常描画のインデックスバッファへのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffShadow;	// 影の頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffShadow;	// 影のインデックスバッファへのポインタ
	int m_nMax;									// 最大数
	int m_nNumUse;								// 使用数
	int m_nNumUseShadow;						// 影使用数
	std::vector<D3DXMATRIX> m_vMtxWorld;	// ワールドマトリックス
	std::vector<D3DXVECTOR3> m_vPos;		// 位置
	std::vector<D3DXVECTOR3> m_vMove;		// 各方向への移動量
	std::vector<float> m_vfSpeed;			// 移動量
	std::vector<float> m_vfSpeedInertia;	// 移動の慣性
	std::vector<int> m_vType;				// 種類
	std::vector<float> m_vfGravity;			// 重力
	std::vector<float> m_vfBounce;			// 跳ね返りs
	std::vector<int> m_vnLife;				// 寿命
	std::vector<float> m_vfRadius;			// 半径
	std::vector<float> m_vfAddRadius;		// 半径の変更量
	std::vector<float> m_vfMinusAlpha;		// 透明度の変更量
	std::vector<D3DXCOLOR> m_vCol;			// 色
	std::vector<bool> m_vbHoming;			// ホーミングするかどうか
	std::vector<CObject*> m_vHomingObj;		// ホーミング対象
	std::vector<D3DXVECTOR3> m_vHomingPos;	// ホーミング位置
	std::vector<float> m_vfSpeedHoming;		// ホーミングの移動量
	std::vector<bool> m_vbShadow;			// 影の有無
};

#endif