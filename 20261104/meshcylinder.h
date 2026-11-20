//=============================================================================
// 
// メッシュシリンダーヘッダー [ meshcylinder.h ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#ifndef _MESHCYLINDER_H_		// このマクロ定義がされていなかったら
#define _MESHCYLINDER_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "mesh.h"

//*****************************************************************************
// メッシュシリンダークラス
//*****************************************************************************
class CMeshCylinder : public CMesh
{
public:
	CMeshCylinder(const int nPriority = PRIORITY_2);
	virtual ~CMeshCylinder();

	static CMeshCylinder* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block,
		const D3DXVECTOR2 size, const D3DXCOLOR col, const bool bInside, const char* pFilename, const CObject::TYPE type, const int nPriority = PRIORITY_2);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,
		const D3DXVECTOR2 block, const D3DXVECTOR2 size, const D3DXCOLOR col, const bool bInside);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void BindTexture(const int nTexture) { m_nIdxTexture = nTexture; }

	void SetPosVtx(const int nCntVtx, const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosVtx(const int nCntVtx);
	void SetSize(const D3DXVECTOR2 size);
	void SetColor(const D3DXCOLOR col);
	void SetColor(const int nCntVtx, const D3DXCOLOR col);
	D3DXCOLOR GetColor(void) { return m_col; }
	void SetTexUV(const int nCntVtx, const float ftexU, const float ftexV);
	void SetTexScroll(const int nCntVtx, const float fScrollU, const float fScrollV);
	int GetNumVtx(void) { return m_nNumVtx; }
	int GetNumIdx(void) { return m_nNumIdx; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	void SwapDisp(void) { m_bDisp = m_bDisp ? false : true; }

	bool Collision(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, const  float fRadius, const float fHeight);

private:
	int m_nIdxTexture;								// テクスチャのインデックス
	D3DXCOLOR m_col;			// 色
	int m_nNumVtx;				// 頂点の総数
	int m_nNumIdx;				// インデックスの総数
	bool m_bInside;				// 内向きか外向きか
	bool m_bDisp;				// 表示するかどうか
};
#endif