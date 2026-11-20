//========================================================================
// 
// メッシュフィールドヘッダー [ meshfield.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _MESHFIELD_H_		// このマクロ定義がされていなかったら
#define _MESHFIELD_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "mesh.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define ERROR_HEIGHT			(-99999.0f)					// 高さを算出できなかった場合の数値
#define FIELD_TEXTURE_NUM		(2)							// 地面のテクスチャの数

//************************************************************************
// メッシュフィールドクラス
//************************************************************************
class CMeshField : public CMesh
{
public:
	// 地形変化タイプ
	enum TERRAIN
	{
		TERRAIN_DOT = 0,		// 点
		TERRAIN_FLAT,			// 平面
		TERRAIN_HILL,			// 丘
		TERRAIN_MOUTAIN,		// 山
		TERRAIN_MAX
	};

	CMeshField(const int nPriority);
	virtual ~CMeshField();

	static CMeshField* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block,
		const D3DXVECTOR2 size, const CObject::TYPE type, const char* pFilename, const int nPriority = PRIORITY_3);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,
		const D3DXVECTOR2 block, const D3DXVECTOR2 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void BindTexture(const int nTexture) { m_nIdxTexture = nTexture; }

	void SetPosVtx(const int nCntVtx, const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosVtx(const int nCntVtx);
	void SetColor(const D3DXCOLOR col);
	void SetColor(const int nCntVtx, const D3DXCOLOR col);
	D3DXCOLOR GetColor(void) { return m_col; }
	void SetTexUV(const int nCntVtx, const float ftexU, const float ftexV);
	void SetTexScroll(const int nCntVtx, const float fScrollU, const float fScrollV);
	int GetNumVtx(void) { return m_nNumVtx; }
	int GetNumIdx(void) { return m_nNumIdx; }
	void SetNor(void);
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	void SwapDisp(void) { m_bDisp = m_bDisp ? false : true; }

	int GetNearestVtx(const D3DXVECTOR3 pos, const float fRadius);
	void MoveVtx(const int nVtx, const D3DXVECTOR3 move, const float fRadius, const int terrain);

	D3DXVECTOR2 GetPolygonIdx(const D3DXVECTOR3 pos);
	float GetHeight(const D3DXVECTOR3 pos, const D3DXVECTOR2 polygonIdx);
	float GetSlope(const D3DXVECTOR3 pos, const D3DXVECTOR2 polygonIdx);

	void ResetData(void);
	HRESULT WriteData(const char* pFilename);
	HRESULT ReadData(const char* pFilename);

private:
	int m_nIdxTexture;			// テクスチャのインデックス
	D3DXCOLOR m_col;			// 色
	int m_nNumVtx;				// 頂点の総数
	int m_nNumIdx;				// インデックスの総数
	bool m_bDisp;				// 表示するかどうか
};

#endif