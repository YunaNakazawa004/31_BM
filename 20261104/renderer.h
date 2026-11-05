//========================================================================
// 
// レンダリングヘッダー [ renderer.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _RENDERER_H_		// このマクロ定義がされていなかったら
#define _RENDERER_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//************************************************************************
// レンダラークラス
//************************************************************************
class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	LPDIRECT3DDEVICE9 GetDevice(void) { return m_pD3DDevice; }

	void SwapWireFrame(void);

private:
	LPDIRECT3D9 m_pD3D;							// Direct3Dオブジェクトへのポインタ
	LPDIRECT3DDEVICE9 m_pD3DDevice;				// Direct3Dデバイスへのポインタ
};

#endif