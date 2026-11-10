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
#include "input.h"

//************************************************************************
// 前方宣言
//************************************************************************
class CMeshOrbit;

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
	static CWind* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 vec, const float fStrength, const float fAffect);
	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 vec, const float fStrength, const float fAffect);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	D3DXMATRIX MatrixCalc(void);

	void WindAffect(D3DXVECTOR3* pPos, D3DXVECTOR3* pVecAxis, D3DXVECTOR3* pRotDest, float* pValueRot);
	void StickControll(CInputJoypad::JOYKEY joykey);
	void AutoMove(const D3DXVECTOR3 move);
	void RandomRot(const float fValue);

private:
	static int m_nIdxModel;		// モデルのインデックス
	D3DXVECTOR3 m_move;			// 移動量
	D3DXVECTOR3 m_rotDest;		// 目的の向き
	D3DXVECTOR3 m_vec;			// 風の吹く向き
	float m_fStrength;			// 風の強さ
	float m_fAngle;				// Y軸に対する傾き
	float m_fAffect;			// 影響力
	CMeshOrbit* m_pMeshOrbit;	// 軌跡
	float m_fFlow;				// 軌跡の波うち
	D3DXVECTOR3 m_randPos;		// 軌跡の場所
	int m_nCounter;				// 汎用カウンター
};

#endif