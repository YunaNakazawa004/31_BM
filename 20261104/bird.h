//========================================================================
// 
// 鳥ヘッダー [ bird.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _BIRD_H_		// このマクロ定義がされていなかったら
#define _BIRD_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"
#include <vector>

//************************************************************************
// マクロ定義
//************************************************************************
#define MAX_MODEL_PLAYER		(16)		// 鳥の最大

//************************************************************************
// 前方宣言
//************************************************************************
class CModel;
class CBone;
class CMotion;

//************************************************************************
// 鳥クラス
//************************************************************************
class CBird : public CObject
{
public:
	// モーションの種類
	enum MOTION_TYPE
	{
		MOTIONTYPE_NEUTRAL = 0,		// 待機
		MOTIONTYPE_MOVE,			// 移動
		MOTIONTYPE_ACTION,			// アクション
		MOTIONTYPE_JUMP,			// ジャンプ
		MOTIONTYPE_LANDING,			// 着地
		MOTIONTYPE_DEATH,			// 死亡
		MOTIONTYPE_FALL,			// 転ぶ
		MOTIONTYPE_MAX
	};

	// 状態
	typedef enum
	{
		STATE_NONE = 0,		// 状態無し
		STATE_TUTORIAL,		// チュートリアル状態
		STATE_WAIT,			// 待機状態
		STATE_APPEAR,		// 出現状態
		STATE_NORMAL,		// 通常状態
		STATE_FALL,			// 転び状態
		STATE_DEATH,		// 死亡状態
		STATE_MAX
	}STATE;

	CBird(const int nPriority = PRIORITY_3);
	virtual ~CBird();

	static CBird* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPosition(const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	void SetRotation(const D3DXVECTOR3 rot);
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	void SetScale(const D3DXVECTOR3 scale);
	D3DXVECTOR3 GetScale(void) { return m_scale; }
	void SetColor(const D3DXCOLOR col);
	D3DXCOLOR GetColor(void) { return m_col; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	void SwapDisp(void) { m_bDisp = m_bDisp ? false : true; }
	void SetState(const STATE state) { m_state = state; }
	STATE GetState(void) { return m_state; }

	float GetHeight(void) { return m_fHeight; }

private:
	HRESULT SetBone(const char* pFilename);

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
	std::vector<CBone*> m_vpBone;				// ボーンのインスタンス
	int m_nNumBone;								// ボーンの総数
	D3DXVECTOR3* m_pPosOff;						// 頂点オフセットのデータ
	int* m_pIdxRef;								// 影響を受けるボーン番号
	float* m_pWeightValue;						// 各ボーンから受けるウェイト
	int m_nNumVtx;								// 頂点の総数
	int m_nNumPoly;								// プリミティブ数
	CMotion* m_pMotion;							// モーションへのポインタ

	D3DXMATRIX m_mtxWorld;			// ワールドマトリックス
	D3DXVECTOR3 m_pos;				// 現在の位置
	D3DXVECTOR3 m_posOld;			// 前回の位置
	D3DXVECTOR3 m_move;				// 移動量
	D3DXVECTOR3 m_rot;				// 向き
	D3DXVECTOR3 m_rotDest;			// 目的の向き
	D3DXVECTOR3 m_scale;			// スケール
	D3DXCOLOR m_col;				// 色
	float m_fRadius;				// 半径
	float m_fHeight;				// 高さ

	STATE m_state;					// 状態
	int m_nCounterState;			// 状態カウンター

	bool m_bDisp;					// 表示するかどうか
};

#endif