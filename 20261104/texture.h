//=============================================================================
// 
// テクスチャヘッダー [ texture.h ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#ifndef _TEXTURE_H_		// このマクロ定義がされていなかったら
#define _TEXTURE_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_TEXTURE			(128)			// テクスチャの最大数
#define MAX_STRING			(256)			// ファイル名の文字数

//*****************************************************************************
// テクスチャクラス
//*****************************************************************************
class CTexture
{
public:
	// 固定テクスチャの種類
	typedef enum
	{
		TYPE_FIELD = 0,			// 地面
		TYPE_MAX
	}TYPE;

	CTexture();
	~CTexture();

	HRESULT Load(void);
	void Unload(void);

	int Register(const char* pFilename);
	LPDIRECT3DTEXTURE9 GetAddress(const int nIdx);

private:
	LPDIRECT3DTEXTURE9 m_apTexture[MAX_TEXTURE];		// テクスチャへのポインタ
	const char* m_pFilename[MAX_TEXTURE];				// テクスチャのファイル名収納
	char m_aCopyFilename[MAX_TEXTURE][MAX_STRING];		// テクスチャのファイル名コピー用
	static const char* c_apFilername[TYPE_MAX];			// 固定テクスチャファイル名
	static int m_nNumAll;			// テクスチャの使用数
};

#endif