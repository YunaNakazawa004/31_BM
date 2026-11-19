//========================================================================
// 
// 球ヘッダー [	ball.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _BALL_H_		// このマクロ定義がされていなかったら
#define _BALL_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"
#include "objectX.h"

//************************************************************************
// 前方宣言
//************************************************************************
class CMeshOrbit;
class CShadowSt;

//************************************************************************
// マクロ定義
//************************************************************************
#define MAX_MODEL_TEXTURE		(16)		// モデルの最大テクスチャ

//************************************************************************
// 球クラス
//************************************************************************
class CBall : public CObjectX
{
public:
	// 球の状態
	typedef enum
	{
		STATE_NONE = 0,		// 状態なし
		STATE_NORMAL,		// 通常状態
		STATE_MAX
	}STATE;

	CBall(const int nPriority = PRIORITY_3);
	~CBall();

	static HRESULT Load(const char* pFilename);
	static void Unload(void);
	static CBall* Create(const D3DXVECTOR3 pos, const float fRadius,
		const float fSpeed, const float fSpeedInertia);
	HRESULT Init(const D3DXVECTOR3 pos, const float fRadius, 
		const float fSpeed, const float fSpeedInertia);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXMATRIX MatrixCalc(void);

	float GetRadius(void) { return m_fRadius; }
	void SetMove(const D3DXVECTOR3 move) { m_move = move; }
	D3DXVECTOR3 GetVelocity(void) { return GetPosition() - m_posOld; }
	void SetVecAxis(const D3DXVECTOR3 vecAxis) { m_vecAxis = vecAxis; }
	D3DXVECTOR3 GetVecAxis(void) { return m_vecAxis; }
	void SetValueRot(const float fValueRot) { m_fValueRot = fValueRot; }
	void AddValueRot(const float fValueRot) { m_fValueRot += fValueRot; }
	float GetValueRot(void) { return m_fValueRot; }
	void SetRotDest(const D3DXVECTOR3 rotDest) { m_rotDest = rotDest; }
	D3DXVECTOR3 GetRotDest(void) { return m_rotDest; }

	bool Movement(void);
	void SubControll(void);

	static CBall* Collision(const D3DXVECTOR3 pos, const float fRadius, const CObject* pOwn);

private:
	static int m_nIdxModel;		// モデルのインデックス
	D3DXVECTOR3 m_posOld;		// 前回の位置
	D3DXVECTOR3 m_move;			// 移動量
	D3DXVECTOR3 m_rotDest;		// 目的の向き
	D3DXVECTOR3 m_rotOld;		// 前回の向き
	STATE m_state;				// 状態
	int m_nCounterState;		// 状態カウンター

	CShadowSt* m_pShadowSt;		// ステンシルシャドウ

	CMeshOrbit* m_pMeshOrbit;	// オービット
	D3DXMATRIX m_mtxRot;		// 回転マトリックス（保存用）
	D3DXQUATERNION m_quat;		// クォータニオン
	D3DXVECTOR3 m_vecAxis;		// 回転軸
	float m_fValueRot;			// 回転角（回転量）
	float m_fSpeedRot;			// 回転速度
	float m_fSpeedInertia;		// 回転速度
	float m_fRadius;			// 半径

	int m_nCounter;				// 汎用カウンター
};

#endif