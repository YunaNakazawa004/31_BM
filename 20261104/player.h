//========================================================================
// 
// プレイヤーヘッダー [	player.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _PLAYER_H_		// このマクロ定義がされていなかったら
#define _PLAYER_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "objectX.h"

//************************************************************************
// 前方宣言
//************************************************************************
class CMeshOrbit;

//************************************************************************
// マクロ定義
//************************************************************************
#define MAX_MODEL_TEXTURE		(16)		// モデルの最大テクスチャ

//************************************************************************
// プレイヤークラス
//************************************************************************
class CPlayer : public CObjectX
{
public:
	// プレイヤーの状態
	typedef enum
	{
		STATE_NONE = 0,		// 状態なし
		STATE_NORMAL,		// 通常状態
		STATE_MAX
	}STATE;

	CPlayer(const int nPriority = PRIORITY_4);
	~CPlayer();

	static HRESULT Load(const char* pFilename);
	static void Unload(void);
	static CPlayer* Create(const D3DXVECTOR3 pos, const float fRadius, const float fJump,
		const float fSpeed, const float fSpeedInertia);
	HRESULT Init(const D3DXVECTOR3 pos, const float fRadius, const float fJump, 
		const float fSpeed, const float fSpeedInertia);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXMATRIX MatrixCalc(void);

	float GetRadius(void) { return m_fRadius; }
	void SetMove(const D3DXVECTOR3 move) { m_move = move; }

	bool Movement(void);
	void SubControll(void);

private:
	static int m_nIdxModel;		// モデルのインデックス
	D3DXVECTOR3 m_posOld;		// 前回の位置
	D3DXVECTOR3 m_move;			// 移動量
	D3DXVECTOR3 m_rotDest;		// 目的の向き
	D3DXVECTOR3 m_rotOld;		// 前回の向き
	bool m_bJump;				// ジャンプ
	float m_fJumpHeight;		// ジャンプの高さ
	bool m_bCharge;				// チャージ
	float m_fSpinAngle;			// スピンの向き
	STATE m_state;				// 状態
	int m_nCounterState;		// 状態カウンター

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