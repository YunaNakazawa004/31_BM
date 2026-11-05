//========================================================================
// 
// オブジェクト2Dヘッダー [ object2D.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _OBJECT2D_H_		// このマクロ定義がされていなかったら
#define _OBJECT2D_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

//************************************************************************
// オブジェクト2Dクラス
//************************************************************************
class CObject2D : public CObject
{
public:
	// 頂点の位置
	enum POS
	{
		POS_CENTER = 0,			// 真ん中
		POS_LEFT,				// 左
		POS_LEFT_TOP,			// 左上
		POS_MID_BOTTOM,			// 真ん中下
		POS_MAX
	};

	CObject2D(const int nPriority);
	virtual ~CObject2D();

	static CObject2D* Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const CObject::TYPE type, 
		const char* pFilename, const int nPriority = PRIORITY_1, const int posType = POS_CENTER);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const int posType = POS_CENTER);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void BindTexture(const int nIdxTexture) { m_nIdxTexture = nIdxTexture; }

	void SetPosition(const D3DXVECTOR3 pos);
	void SetPosition(const D3DXVECTOR3 pos, const float fWidth, const float fHeight);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	void SetSize(const float fLength);
	float GetSize(void) { return m_fLength; }
	void SetRotation(const D3DXVECTOR3 rot);
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	void SetColor(const D3DXCOLOR col);
	D3DXCOLOR GetColor(void) { return m_col; }
	void SetTexUV(const int nPatternAnim, const int nTexWidth, const int nTexHeight, const float fScrollX, const float fScrollY);
	void SetTexUV(const float fStartX, const float fEndX, const float fStartY, const float fEndY);
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	bool GetDisp(void) { return m_bDisp; }
	void SwapDisp(void) { m_bDisp = m_bDisp ? false : true; }

	bool MouseCollision(const POINT MousePos, const float fWidth, const float fHeight);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
	int m_nIdxTexture;			// テクスチャのインデックス
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_rot;			// 向き
	D3DXCOLOR m_col;			// 色
	float m_fAngle;				// 角度
	float m_fLength;			// 対角線の長さ
	int m_posType;				// ポリゴンにおける頂点の位置
	bool m_bDisp;				// 表示するかどうか
};

#endif