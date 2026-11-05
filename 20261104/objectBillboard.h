//========================================================================
// 
// オブジェクトビルボードヘッダー [ objectBillboard.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _OBJECTBILLBOARD_H_		// このマクロ定義がされていなかったら
#define _OBJECTBILLBOARD_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

//************************************************************************
// オブジェクトビルボードクラス
//************************************************************************
class CObjectBillboard : public CObject
{
public:
	CObjectBillboard(const int nPriority);
	virtual ~CObjectBillboard();

	static CObjectBillboard* Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight,
		const CObject::TYPE type, const char* pFilename, const int nPriority = PRIORITY_2);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 pos, const float fWidth, const float fHeight);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawShadow(const D3DXMATRIX mtxWorld, const D3DXVECTOR3 point, const D3DXVECTOR3 normal);

	void BindTexture(const int nTextureIdx) { m_nTextureIdx = nTextureIdx; }

	void SetPosition(const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return DEFAULT_VECTOR3; }
	void SetScale(const D3DXVECTOR3 scale);
	D3DXVECTOR3 GetScale(void) { return m_scale; }
	void SetSize(const float fRadius);
	float GetSize(void) { return m_fRadius; }
	void SetColor(const D3DXCOLOR col);
	D3DXCOLOR GetColor(void) { return m_col; }
	D3DXMATRIX GetMatrix(void) { return m_mtxWorld; }
	void SetTexUV(const int nPatternAnim, const int nTexWidth, const int nTexHeight, const float fScrollX, const float fScrollY);
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	void SwapDisp(void) { m_bDisp = m_bDisp ? false : true; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
	int m_nTextureIdx;			// テクスチャのインデックス
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_scale;		// スケール
	float m_fRadius;			// 半径
	D3DXCOLOR m_col;			// 色
	bool m_bDisp;				// 表示するかどうか
};

#endif