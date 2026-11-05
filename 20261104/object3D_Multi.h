//========================================================================
// 
// オブジェクトマルチ3Dヘッダー [ object3D_Multi.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _OBJECT3D_MULTI_H_		// このマクロ定義がされていなかったら
#define _OBJECT3D_MULTI_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

//************************************************************************
// オブジェクトマルチ3Dクラス
//************************************************************************
class CObject3DM : public CObject
{
public:
	// posの位置
	typedef enum
	{
		MAINPOS_CENTER = 0,		// 真ん中
		MAINPOS_TOPLEFT,		// 左上
		MAINPOS_BOTTOMMID,		// 真ん中下
		MAINPOS_MAX
	}MAINPOS;

	CObject3DM(const int nPriority);
	virtual ~CObject3DM();

	static CObject3DM* Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const float fDepth,
		const CObject::TYPE type, const char* pFilename1, const char* pFilename2, const int nPriority = PRIORITY_1, const MAINPOS mainpos = MAINPOS_CENTER);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const float fDepth,
		const MAINPOS mainpos = MAINPOS_CENTER);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void BindTexture(const int* pTexture);

	void SetPosition(const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	void SetRotation(const D3DXVECTOR3 rot);
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	void SetScale(const D3DXVECTOR3 scale);
	D3DXVECTOR3 GetScale(void) { return m_scale; }
	void SetColor(const D3DXCOLOR col);
	D3DXCOLOR GetColor(void) { return m_col; }
	void SetTexUV(const int nPatternAnim, const int nTexWidth, const int nTexHeight);
	void SetTexMUV(const int nPatternAnim, const int nTexWidth, const int nTexHeight);
	void SetTexScroll(const float fScrollX, const float fScrollY);
	void SetTexMScroll(const float fScrollX, const float fScrollY);
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	void SwapDisp(void) { m_bDisp = m_bDisp ? false : true; }

	float GetHeight(const D3DXVECTOR3 pos);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
	int m_aIdxTexture[2];		// テクスチャのインデックス
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_rot;			// 向き
	D3DXVECTOR3 m_scale;		// スケール
	D3DXCOLOR m_col;			// 色
	bool m_bDisp;				// 表示するかどうか
};

#endif