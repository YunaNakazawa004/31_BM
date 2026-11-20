//=============================================================================
// 
// メッシュウォールヘッダー [ meshwallM.h ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#ifndef _MESHWALLM_H_		// このマクロ定義がされていなかったら
#define _MESHWALLM_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "mesh.h"

//************************************************************************
// メッシュウォールクラス
//************************************************************************
class CMeshWallM : public CMesh
{
public:
	CMeshWallM(const int nPriority = PRIORITY_2);
	virtual ~CMeshWallM();

	static CMeshWallM* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size,
		const char* pFilename1, const char* pFilename2, const CObject::TYPE type, const int nPriority = PRIORITY_2);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,
		const D3DXVECTOR2 block, const D3DXVECTOR2 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void BindTexture(const int* pTexture);
	void BindTexture(const int nIdx, const int nTexture);

	void SetPosVtx(const int nCntVtx, const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosVtx(const int nCntVtx);
	void SetColor(const D3DXCOLOR col);
	void SetColor(const int nCntVtx, const D3DXCOLOR col);
	D3DXCOLOR GetColor(void) { return m_col; }
	D3DXCOLOR GetColor(const int nCntVtx);
	void SetTexUV(const int nCntVtx, const float ftexU, const float ftexV);
	void SetTexMUV(const int nCntVtx, const float ftexU, const float ftexV);
	void SetTexScroll(const int nCntVtx, const float fScrollU, const float fScrollV);
	void SetTexMScroll(const int nCntVtx, const float fScrollU, const float fScrollV);
	void SetTexMScrollAll(const float fScrollU, const float fScrollV);
	void SetTexMBig(void);
	void SetTexMSmall(void);
	int GetNumVtx(void) { return m_nNumVtx; }
	int GetNumIdx(void) { return m_nNumIdx; }
	void SetNor(void);
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	void SwapDisp(void) { m_bDisp = m_bDisp ? false : true; }

	D3DXVECTOR2 GetPolygonIdx(const D3DXVECTOR3 pos);

private:
	int m_aIdxTexture[2];							// テクスチャのインデックス
	D3DXCOLOR m_col;			// 色
	int m_nNumVtx;				// 頂点の総数
	int m_nNumIdx;				// インデックスの総数
	bool m_bDisp;				// 表示するかどうか
};
#endif