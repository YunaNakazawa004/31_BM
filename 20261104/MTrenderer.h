//========================================================================
// 
// マルチターゲットレンダリングヘッダー [ MTrenderer.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _MTRENDERER_H_		// このマクロ定義がされていなかったら
#define _MTRENDERER_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//************************************************************************
// マルチターゲットレンダラークラス
//************************************************************************
class CMTRenderer
{
public:
	CMTRenderer();
	~CMTRenderer();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void ChangeTarget(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 vecU);
	LPDIRECT3DTEXTURE9 GetTextureMT(void) { return m_apTextureMT[0]; }

	void SetMultiRendering(const bool bMultiRendering);
	void FirstMTRendering(void);
	void BackMTRendering(void);
	void ResetMTRendering(void);

private:
	LPDIRECT3DTEXTURE9 m_apTextureMT[2];		// レンダリングターゲット用テクスチャ
	LPDIRECT3DSURFACE9 m_apRenderMT[2];			// テクスチャレンダリング用インターフェース
	LPDIRECT3DSURFACE9 m_pZBuffMT;				// テクスチャレンダリング用Zバッファ
	D3DVIEWPORT9 m_viewportMT;					// テクスチャレンダリング用ビューポート
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffMT;		// フィードバック用ポリゴンの頂点バッファ
	bool m_bMultiRendering;						// マルチターゲットレンダリングをするかどうか
	float m_fOverSize;							// 画面をどれくらい大きくするか
	float m_fOverSizeDest;						// 画面をどれくらい大きくするかの目的の値

	LPDIRECT3DSURFACE9 m_pRenderDef;			// 保存用
	LPDIRECT3DSURFACE9 m_pZBuffDef;				// 保存用
	D3DVIEWPORT9 m_viewportDef;					// 保存用
	D3DXMATRIX m_mtxViewDef;					// 保存用
	D3DXMATRIX m_mtxProjectionDef;				// 保存用
};								  

#endif