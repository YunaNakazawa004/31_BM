//=============================================================================
// 
// カメラヘッダー [ camera.h ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#ifndef _CAMERA_H_		// このマクロ定義がされていなかったら
#define _CAMERA_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define VIEWFRUSTUM_NUM			(6)				// 視錐台の数

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class CObject;

//*****************************************************************************
// カメラクラス
//*****************************************************************************
class CCamera
{
public:
	// カメラの種類
	typedef enum
	{
		TYPE_PLAYER = 0,		// プレイヤー追従
		TYPE_FOLLOW,			// 追従
		TYPE_SOLO,				// 単体カメラ
		TYPE_ROTATION,			// 定点周回カメラ
		TYPE_STOP,				// 定点カメラ
		TYPE_MAX
	}TYPE;

	CCamera();
	~CCamera();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void SetCamera(void);
	void SetPosition(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 rot, TYPE type);
	void SetViewAngle(float fViewAngle);
	void SetShake(const float fValue);
	void SetType(const TYPE type) { m_type = type; }
	void SetFollow(CObject* pFollow);
	void CalcViewFrustum(void);
	bool IsInside(const D3DXVECTOR3 pos, const float fRadius);

	D3DXVECTOR3 GetPositionV(void) { return m_posV; }
	D3DXVECTOR3 GetPositionR(void) { return m_posR; }
	D3DXVECTOR3 GetRotation(void) { return m_rot; }

	void CorrectAngle(float* pAngle, float fAngle);

private:
	D3DXVECTOR3 m_posV;				// 視点
	D3DXVECTOR3 m_posR;				// 注視点
	D3DXVECTOR3 m_posVDest;			// 目的の視点
	D3DXVECTOR3 m_posRDest;			// 目的の注視点
	D3DXVECTOR3 m_posVOff;			// オフセットの視点
	D3DXVECTOR3 m_vecU;				// 上方向ベクトル
	D3DXVECTOR3 m_rot;				// 向き
	float m_fAngle;					// 上下移動用の角度
	float m_fDistance;				// 視点と注視点の距離
	float m_fRDistance;				// 注視点の差異
	float m_fViewAngle;				// 視野角
	float m_fMoveVA;				// 視野角の増加量
	D3DXMATRIX m_mtxProjection;		// プロジェクションマトリックス(モニターのサイズ)
	D3DXMATRIX m_mtxView;			// ビューマトリックス(カメラの画面のサイズ)
	D3DVIEWPORT9 m_viewport;		// ビューポート
	TYPE m_type;					// 種類
	CObject* m_pFollow;				// 追従対象
	D3DXPLANE m_aPlane[6];			// 視錐台
	bool m_bShake;					// カメラの振動
};

#endif