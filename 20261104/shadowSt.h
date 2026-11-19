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
	CShadowSt(const int nPriority = PRIORITY_7);
	~CShadowSt();

	static HRESULT Load(const char* pFilename);
	static void Unload(void);
	static CShadowSt* Create(const D3DXVECTOR3 pos);
	HRESULT Init(const D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXMATRIX MatrixCalc(void);

private:
	static int m_nIdxModel;		// モデルのインデックス
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
};

#endif