//========================================================================
// 
// 壁ヘッダー [ wallMM.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _WALLMM_H_		// このマクロ定義がされていなかったら
#define _WALLMM_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "meshwallM.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define MAX_BOUND		(3)			// 跳ね返ったときの反応の最大数

//************************************************************************
// 壁クラス
//************************************************************************
class CWallMM : public CMeshWallM
{
public:
	// テクスチャの種類
	enum TYPE
	{
		TYPE_NORMAL = 0,		// 通常テクスチャ
		TYPE_SHOT,				// 発射テクスチャ
		TYPE_MOVE,				// 移動テクスチャ
		TYPE_MAX
	};

	CWallMM(const int nPriority = PRIORITY_3);
	virtual ~CWallMM();

	static HRESULT Load(const char* pFilename1, const char* pFilename2, const char* pFilename3);
	static void Unload(void);
	static CWallMM* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size);
	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetStartFlow(const D3DXVECTOR2 start);
	void Flow(void);
	void Reset(void);
	static bool CollisionAll(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, const float fRadius);
	static bool SetTypeAll(const int nType);
	bool Collision(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, const float fRadius);

	void SetTexType(const int type);

private:
	static int m_aIdxTexture[TYPE_MAX];	// テクスチャのインデックス
	D3DXVECTOR2 m_start[MAX_BOUND];		// 波紋の原点
	float m_fFlowSpeed;					// 波紋の速さ
	float m_fFlowDepth;					// 波紋の幅
	float m_fFlowRadius;				// 波紋の半径
	float m_fFlow[MAX_BOUND];			// 波紋の値
	int m_type;							// 種類
};

#endif