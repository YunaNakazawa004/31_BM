//========================================================================
// 
// 風ヘッダー [	wind.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _WIND_H_		// このマクロ定義がされていなかったら
#define _WIND_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "objectX.h"

//************************************************************************
// 風クラス
//************************************************************************
class CWind : public CObjectX
{
public:
	CWind(const int nPriority = PRIORITY_4);
	~CWind();

	static HRESULT Load(const char* pFilename);
	static void Unload(void);
	static CWind* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 vec, const float fStrength);
	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 vec, const float fStrength);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void WindAffect(D3DXVECTOR3* pPos, D3DXVECTOR3* pVecAxis, D3DXVECTOR3* pRotDest, float* pValueRot);

private:
	static int m_nIdxModel;		// モデルのインデックス
	D3DXVECTOR3 m_vec;			// 風の吹く向き
	float m_fStrength;			// 風の強さ
	float m_fAngle;				// Y軸に対する傾き
};

#endif