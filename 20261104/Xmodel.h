//=============================================================================
// 
// モデル管理ヘッダー [ Xmodel.h ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#ifndef _XMODEL_H_		// このマクロ定義がされていなかったら
#define _XMODEL_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MODEL			(128)			// モデル管理の最大数
#define MAX_STRING			(256)			// ファイル名の文字数

//*****************************************************************************
// モデル管理クラス
//*****************************************************************************
class CXModel
{
public:
	CXModel();
	~CXModel();

	HRESULT Load(void);
	void Unload(void);

	int Register(const char* pFilename);
	LPD3DXMESH GetMesh(const int nIdx);
	LPD3DXBUFFER GetBuffMat(const int nIdx);
	DWORD GetNumMat(const int nIdx);

private:
	LPD3DXMESH m_apMesh[MAX_MODEL];						// メッシュ(頂点情報)へのポインタ
	LPD3DXBUFFER m_apBuffMat[MAX_MODEL];				// マテリアルへのポインタ
	DWORD m_adwNumMat[MAX_MODEL];						// マテリアルの数
	const char* m_pFilename[MAX_MODEL];					// モデル管理のファイル名収納
	char m_aCopyFilename[MAX_MODEL][MAX_STRING];		// モデル管理のファイル名コピー用
	static int m_nNumAll;			// モデル管理の使用数
};

#endif