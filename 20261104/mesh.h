//========================================================================
// 
// メッシュヘッダー [ mesh.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _MESH_H_		// このマクロ定義がされていなかったら
#define _MESH_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

#include <vector>

//************************************************************************
// 前方宣言
//************************************************************************
class CObjectX;

//************************************************************************
// メッシュクラス
//************************************************************************
class CMesh : public CObject
{
public:
	// 種類
	enum MESH
	{
		MESHWALL = 0,		// メッシュウォール
		MESHFIELD,			// メッシュフィールド
		MESHRING,			// メッシュリング
		MESHCYLINDER,		// メッシュシリンダー
		MESHDOME,			// メッシュドーム
		MESH_MAX
	};

	CMesh(const int nPriority = PRIORITY_3);
	virtual ~CMesh();

	static CMesh* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size, const int type, const bool bCalc = true);
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	void UninitAll(void);

	void SetPosition(const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	void SetRotation(const D3DXVECTOR3 rot);
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	void SetScale(const D3DXVECTOR3 scale);
	D3DXVECTOR3 GetScale(void) { return m_scale; }
	void SetSize(const D3DXVECTOR2 size) { m_size = size; }
	D3DXVECTOR2 GetSize(void) { return m_size; }
	void SetBlock(const D3DXVECTOR2 block) { m_block = block; }
	D3DXVECTOR2 GetBlock(void) { return m_block; }
	void SetMatrix(const D3DXMATRIX mtxWorld) { m_mtxWorld = mtxWorld; }
	void SetCalc(const bool bCalc) { m_bCalc = bCalc; }

	HRESULT CreateVtxBuff(const int nValue);
	HRESULT CreateVtxBuffMulti(const int nValue);
	HRESULT CreateIdxBuff(const int nValue);
	LPDIRECT3DVERTEXBUFFER9 GetVtxBuff(void) { return m_pVtxBuff; }
	LPDIRECT3DINDEXBUFFER9 GetIdxBuff(void) { return m_pIdxBuff; }

	void CreatePosOff(const int nNumBone);
	void CreateBoneInfo(const int nNumBone);
	void CreateShadowVtx(const int nValue);
	void SetPosOff(const int nIdxBone, const D3DXVECTOR3 posBone);
	int* GetBoneRef(void) { return m_pBoneRef; }
	float* GetWeight(void) { return m_pWeight; }

	int GetVtxNum(void) { return m_nNumVtxAll; }
	int GetIdxNum(void) { return m_nNumIdxAll; }
	int GetVtxIdx(void) { return m_nVtxIdx; }
	D3DXVECTOR3 GetVtxPos(void);

	void SetBoneDraw(const bool bBoneDraw) { m_bBoneDraw = bBoneDraw; }

	HRESULT WriteTxtData(const char* pFilename);
	HRESULT WriteBinData(const char* pFilename);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;				// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;				// インデックスバッファへのポインタ
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_rot;			// 向き
	D3DXVECTOR3 m_scale;		// スケール
	D3DXVECTOR2 m_block;		// 分割数
	D3DXVECTOR2 m_size;			// サイズ
	int m_nNumVtxAll;			// 頂点の総数
	int m_nNumIdxAll;			// インデックスの総数
	int m_nVtxIdx;				// 選択されている頂点
	std::vector<int> m_vnShadow;		// 影に使う頂点の番号
	int m_nNumShadowVtx;				// 影に使う頂点の数
	D3DXVECTOR3* m_pPosOff;		// 頂点位置のオフセット
	int* m_pBoneRef;			// 影響を受けるボーン
	float* m_pWeight;			// ウェイト
	int m_nNumBone;				// 設定するボーン数
	int m_type;					// 種類
	bool m_bBoneDraw;			// ボーンに紐づけて描画するかどうか
	bool m_bVtxDisp;			// 頂点モデルを表示するかどうか
	bool m_bCalc;				// 移動させたり計算したりするかどうか
};

#endif