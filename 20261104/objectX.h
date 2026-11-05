//========================================================================
// 
// オブジェクトXヘッダー [ objectX.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _OBJECTX_H_		// このマクロ定義がされていなかったら
#define _OBJECTX_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define MAX_MODEL_TEXTURE		(16)		// モデルの最大テクスチャ
#define MAX_MATERIAL_COLOR		(16)		// マテリアルの色

//************************************************************************
// オブジェクトXクラス
//************************************************************************
class CObjectX : public CObject
{
public:
	CObjectX(const int nPriority);
	virtual ~CObjectX();

	static CObjectX* Create(const D3DXVECTOR3 pos, const CObject::TYPE type, const char* pFilename,
		const int nPriority = PRIORITY_2);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Draw(const D3DXMATRIX mtxWorld);
	void DrawShadow(const D3DXMATRIX mtxWorld, const D3DXVECTOR3 point, const D3DXVECTOR3 normal);

	void BindModel(const int nIdxModel);

	void SetPosition(const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	void SetRotation(const D3DXVECTOR3 rot);
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	void SetScale(const D3DXVECTOR3 scale);
	D3DXVECTOR3 GetScale(void) { return m_scale; }
	void SetColor(const D3DXCOLOR col);
	D3DXCOLOR GetColor(void) { return *m_col; }
	D3DXVECTOR3 GetVtxMax(void) { return m_VtxMax; }
	D3DXVECTOR3 GetVtxMin(void) { return m_VtxMin; }
	D3DXMATRIX GetMatrix(void) { return m_mtxWorld; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	void SwapDisp(void) { m_bDisp = m_bDisp ? false : true; }

	bool Collision(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove,
		const float fRadius, const float fHeight, bool* pHead);
	bool Reflect(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, D3DXVECTOR3* pRotDest,
		D3DXVECTOR3* pVecAxis, float* pValueRot, const float fRadius);

private:
	int m_nIdxModel;						// モデルのインデックス
	int m_aTexture[MAX_MODEL_TEXTURE];		// テクスチャのインデックス
	const char* m_pStrFileName;				// モデルファイルの名前
	D3DXVECTOR3 m_VtxMax;					// モデルの最大値
	D3DXVECTOR3 m_VtxMin;					// モデルの最小値
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXVECTOR3 m_scale;					// スケール
	D3DXCOLOR m_col[MAX_MATERIAL_COLOR];	// 色
	bool m_bDisp;							// 表示するかどうか
};

#endif