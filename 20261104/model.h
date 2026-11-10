//========================================================================
// 
// モデルヘッダー [ model.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _MODEL_H_		// このマクロ定義がされていなかったら
#define _MODEL_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define MAX_MODEL_TEXTURE		(16)		// モデルの最大テクスチャ

//************************************************************************
// モデルクラス
//************************************************************************
class CModel
{
public:
	CModel();
	virtual ~CModel();

	static CModel* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const char* pFilename);
	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetParent(CModel* pModel) { m_pParent = pModel; }
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }

	void BindModel(const char* pStr) { m_pFileName = pStr; }

	void SetPosition(const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	void SetPosOff(const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosOff(void) { return m_posOff; }
	D3DXVECTOR3 GetPosOffC(void) { return m_posOffC; }
	void SetRotation(const D3DXVECTOR3 rot);
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	void SetRotOff(const D3DXVECTOR3 rot);
	D3DXVECTOR3 GetRotOff(void) { return m_rotOff; }
	D3DXVECTOR3 GetRotOffC(void) { return m_rotOffC; }
	void SetScale(const D3DXVECTOR3 scale);
	D3DXVECTOR3 GetScale(void) { return m_scale; }
	D3DXVECTOR3 GetVtxMax(void) { return m_VtxMax; }
	D3DXVECTOR3 GetVtxMin(void) { return m_VtxMin; }
	void SetColor(const D3DXCOLOR col);
	D3DXCOLOR GetColor(void) { return m_col; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	void SwapDisp(void) { m_bDisp = m_bDisp ? false : true; }

	bool Collision(const D3DXVECTOR3 posMe, const D3DXVECTOR3 rotMe, const D3DXVECTOR3 scaleMe, D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove,
		const float fRadius, const float fHeight);

private:
	LPD3DXMESH m_pMesh;						// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_pBuffMat;				// マテリアルへのポインタ
	int m_apTexture[MAX_MODEL_TEXTURE];		// テクスチャへのポインタ
	DWORD m_dwNumMat;						// マテリアルの数
	const char* m_pFileName;	// モデルファイルの名前
	D3DXVECTOR3 m_VtxMax;		// モデルの最大値
	D3DXVECTOR3 m_VtxMin;		// モデルの最小値
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_rot;			// 向き
	D3DXVECTOR3 m_posOff;		// 位置（オフセット（モーション以外でいじる用））
	D3DXVECTOR3 m_rotOff;		// 向き（オフセット（モーション以外でいじる用））
	D3DXVECTOR3 m_posOffC;		// 位置（オフセット（絶対いじらない保存用））
	D3DXVECTOR3 m_rotOffC;		// 向き（オフセット（絶対いじらない保存用））
	D3DXVECTOR3 m_scale;		// スケール
	D3DXCOLOR m_col;			// 色
	bool m_bDisp;				// 表示するかどうか
	CModel* m_pParent;			// 親モデルへのポインタ
};

#endif