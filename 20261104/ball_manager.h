//========================================================================
// 
// 球マネージャーヘッダー [ ball_manager.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _BALL_MANAGER_H_		// このマクロ定義がされていなかったら
#define _BALL_MANAGER_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

#include <vector>

//************************************************************************
// 前方宣言
//************************************************************************
class CBall;
class CWind;

//************************************************************************
// 球マネージャークラス
//************************************************************************
class CBallManager : public CObject
{
public:
	CBallManager(const int nPriority = PRIORITY_4);
	~CBallManager();

	static CBallManager* Create(const float fRadius, const float fSpeed, const float fSpeedInertia);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const float fRadius, const float fSpeed, const float fSpeedInertia);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Random(const int nValue, const D3DXVECTOR3 center, const float fRadius);

	CWind* GetWindLeft(void) { return m_pWindLeft; }
	CWind* GetWindRight(void) { return m_pWindRight; }
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	D3DXVECTOR3 GetRotation(void) { return DEFAULT_VECTOR3; }

private:
	std::vector<CBall*> m_vpBall;		// 球のインスタンス
	int m_nNumBall;			// 球の数
	float m_fRadius;		// 規定の半径
	float m_fSpeed;			// 規定の回転速度
	float m_fSpeedInertia;	// 規定の回転の慣性

	D3DXVECTOR3 m_pos;			// マネージャーの位置（風の間）
	CWind* m_pWindLeft;			// 左側の風
	CWind* m_pWindRight;		// 右側の風
};

#endif