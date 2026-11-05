//=============================================================================
// 
// メッシュオービットヘッダー [ meshorbit.h ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#ifndef _MESHORBIT_H_		// このマクロ定義がされていなかったら
#define _MESHORBIT_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ORBIT_VTX		(50)			// オービットの頂点数

//************************************************************************
// メッシュオービットクラス
//************************************************************************
class CMeshOrbit : public CObject
{
public:
	CMeshOrbit(const int nPriority = PRIORITY_2);
	virtual ~CMeshOrbit();

	static CMeshOrbit* Create(const D3DXVECTOR3 Offset0, const D3DXVECTOR3 Offset1, const D3DXCOLOR col0, 
		const D3DXCOLOR col1, const D3DMATRIX* pMtxParent,
		const char* pFilename, const CObject::TYPE type, const int nPriority = PRIORITY_2);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 Offset0, const D3DXVECTOR3 Offset1, const D3DXCOLOR col0, const D3DXCOLOR col1, 
		const D3DMATRIX* pMtxParent);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void BindTexture(const int nTexture);

	void SetPosition(const D3DXVECTOR3 Offset0, const D3DXVECTOR3 Offset1, const D3DXCOLOR col0, 
		const D3DXCOLOR col1, const D3DMATRIX* pMtxParent);

	D3DXVECTOR3 GetPosition(void) { return DEFAULT_VECTOR3; }
	D3DXVECTOR3 GetRotation(void) { return DEFAULT_VECTOR3; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	void SwapDisp(void) { m_bDisp = m_bDisp ? false : true; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;				// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;				// インデックスバッファへのポインタ
	int m_nIdxTexture;								// テクスチャのインデックス
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	D3DMATRIX m_pMtxParent;					// 親のマトリックス
	D3DXVECTOR3 m_aOffset[2];				// 両端のオフセット
	D3DXCOLOR m_aCol[2];					// 両端の基準の色
	D3DXVECTOR3 m_aPosPoint[MAX_ORBIT_VTX];	// 計算後の各頂点座標
	D3DXCOLOR m_aColPoint[MAX_ORBIT_VTX];	// 各頂点カラー
	bool m_bDisp;							// 表示するかどうか
};
#endif