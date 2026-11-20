//========================================================================
// 
// マップオブジェクトヘッダー [ map_object.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _MAP_OBJECT_H_		// このマクロ定義がされていなかったら
#define _MAP_OBJECT_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

#include <vector>

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// 前方宣言
//************************************************************************
class CObjectX;

//************************************************************************
// マップオブジェクトクラス
//************************************************************************
class CMapObject
{
public:
	// 生成可能な配置オブジェクト
	enum MAP_OBJ
	{
		MAP_OBJ_ROCK = 0,		// 岩
		MAP_OBJ_MAX
	};

	CMapObject();
	virtual ~CMapObject();

	HRESULT Load(const char* pFilename);
	void Unload(void);
	void Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const int mapObj, const int nIdxModel);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	bool Collision(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, D3DXVECTOR3* pRotDest,
		D3DXVECTOR3* pVecAxis, float* pValueRot, const float fRadius);

	void Reset(void);

private:
	int m_nNumObject;		// 配置したオブジェクトの総数
	int m_nNumObjectFile;	// 配置可能オブジェクトファイルの総数
	std::vector<CObjectX*> m_vpObject;		// 配置したオブジェクト
	std::vector<int> m_vType;				// 配置したオブジェクトの種類
	std::vector<int> m_vModelFileIdx;		// 配置可能オブジェクトのファイル
};

#endif