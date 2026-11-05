//========================================================================
// 
// 数字ヘッダー [ number.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _NUMBER_H_		// このマクロ定義がされていなかったら
#define _NUMBER_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//************************************************************************
// 数字クラス
//************************************************************************
class CNumber
{
public:
	CNumber();
	~CNumber();

	static HRESULT Load(void);
	static void Unload(void);
	static CNumber* Create(const D3DXVECTOR3 pos,
		const float fWidth, const float fHeight);
	HRESULT Init(const D3DXVECTOR3 pos, const float fWidth, const float fHeight);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPosition(const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	float GetWidth(void) { return m_fWidth; }
	void SetColor(const D3DXCOLOR col);
	void SetTexUV(const int nNumber, const int nTexWidth, const int nTexHeight);
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
	static int m_nIdxTexture;	// テクスチャのインデックス
	D3DXVECTOR3 m_pos;			// 位置
	float m_fWidth;				// 幅
	float m_fHeight;			// 高さ
	bool m_bDisp;				// 表示するかどうか
};

#endif