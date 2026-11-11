//========================================================================
// 
// ボーンヘッダー [ bone.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _BONE_H_		// このマクロ定義がされていなかったら
#define _BONE_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "part.h"

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// ボーンクラス
//************************************************************************
class CBone : public CPart
{
public:
	CBone();
	~CBone();

	static CBone* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);
	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetParent(CBone* pBone) { m_pParent = pBone; }

private:
	CBone* m_pParent;			// 親ボーンへのポインタ
};

#endif