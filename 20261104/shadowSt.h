//========================================================================
// 
// ステンシルシャドウヘッダー [	shadowSt.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _SHADOWST_H_		// このマクロ定義がされていなかったら
#define _SHADOWST_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"
#include "objectX.h"

//************************************************************************
// 前方宣言
//************************************************************************

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// ステンシルシャドウクラス
//************************************************************************
class CShadowSt : public CObjectX
{
public:
	// 種類
	enum TYPE
	{
		TYPE_CYLINDERX = 0,		// 円柱Xファイル
		TYPE_MESH,				// メッシュ
		TYPE_MAX
	};

	CShadowSt(const int nPriority = PRIORITY_7);
	~CShadowSt();

	static HRESULT Load(const char* pFilename);
	static void Unload(void);
	static CShadowSt* Create(const D3DXVECTOR3 pos, const int type = TYPE_CYLINDERX);
	HRESULT Init(const D3DXVECTOR3 pos, const int type);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static void DrawShadow(void);
	D3DXMATRIX MatrixCalc(void);

	void CreateCylinder(const D3DXVECTOR3 pos, const D3DXVECTOR2 block);
	void SetVtxPos(const int nIdx, const D3DXVECTOR3 posVtx);
	void DrawMesh(D3DXMATRIX mtxWorld);
	int GetVtxNum(void) { return m_nNumVtx; }

private:
	static int m_nIdxModel;		// モデルのインデックス
	static LPDIRECT3DVERTEXBUFFER9 m_pShadowVtxBuff;	// 影の頂点バッファへのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;			// インデックスバッファへのポインタ
	D3DXVECTOR2 m_block;		// 分割数
	float m_fHeight;			// 高さ
	int m_nNumVtx;				// 頂点数
	int m_nNumIdx;				// インデックス数
	int m_type;					// 種類
	bool m_bShadow;				// 影モデルの表示切り替え
};

#endif