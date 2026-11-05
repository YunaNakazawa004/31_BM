//========================================================================
// 
// 地面ヘッダー [ field.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _FIELD_H_		// このマクロ定義がされていなかったら
#define _FIELD_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object3D.h"

//************************************************************************
// 地面クラス
//************************************************************************
class CField : public CObject3D
{
public:
	CField(const int nPriority = PRIORITY_3);
	virtual ~CField();

	static HRESULT Load(const char* pFilename);
	static void Unload(void);
	static CField* Create(const D3DXVECTOR3 pos, const float fWidth, const float fDepth);
	HRESULT Init(const D3DXVECTOR3 pos, const float fWidth, const float fDepth);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	static int m_nIdxTexture;		// テクスチャのインデックス
};

#endif