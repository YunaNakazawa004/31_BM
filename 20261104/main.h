//========================================================================
// 
// メインヘッダー [ main.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _MAIN_H_		// このマクロ定義がされていなかったら
#define _MAIN_H_		// 2重インクルード防止のマクロを定義する

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#include <crtdbg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "d3dx9.h"		// 描画処理に必要
#define DIRECTINPUT_VERSION	(0x0800)		// ビルド時の警告対処用マクロ
#include "dinput.h"		// 入力処理に必要
#include "xaudio2.h"	// サウンド処理に必要
#include "xinput.h"		// ジョイパッド処理に必要

//************************************************************************
// ライブラリのリンク
//************************************************************************
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"d3d9.lib")		// 描画処理に必	要
#pragma comment(lib,"d3dx9.lib")	// [d3d9.lib]の拡張ライブラリ
#pragma comment(lib,"dxguid.lib")	// DirectXコンポーネント(部品)使用に必要
#pragma comment(lib,"winmm.lib")	// システム時刻取得に必要
#pragma comment(lib,"dinput8.lib")	// 入力処理に必要
#pragma comment(lib,"xinput.lib")	// ジョイパッド処理に必要

//************************************************************************
// マクロ定義
//************************************************************************
#define SCREEN_WIDTH		(1280)															// ウィンドウの幅
#define SCREEN_HEIGHT		(720)															// ウィンドウの高さ
#define FVF_VERTEX_2D		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)					// 頂点フォーマット[2D]
#define FVF_VERTEX_2D_MULTI	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2)					// 頂点フォーマット[2D_MULTI]
#define FVF_VERTEX_3D		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)		// 頂点フォーマット[3D]
#define FVF_VERTEX_3D_MULTI	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2)		// 頂点フォーマット[3D_MULTI]
#define MAX_PLAYER			(1)																// 最大プレイヤー数

//************************************************************************
// 頂点情報[2D]の構造体を定義
//************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 頂点座標(x.y.z)
	float rhw;					// 座標変換用係数(1.0fで固定)
	D3DCOLOR col;				// 頂点カラー(r,g,b,a)
	D3DXVECTOR2 tex;			// テクスチャ座標(x,y)
}VERTEX_2D;

//************************************************************************
// 頂点情報[2D_MULTI]の構造体を定義
//************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 頂点座標(x.y.z)
	float rhw;					// 座標変換用係数(1.0fで固定)
	D3DCOLOR col;				// 頂点カラー(r,g,b,a)
	D3DXVECTOR2 tex;			// テクスチャ座標0(x,y)
	D3DXVECTOR2 texM;			// テクスチャ座標1(x,y)
}VERTEX_2D_MULTI;

//*****************************************************************************
// 頂点情報[3D]の構造体を定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 頂点座標(x.y.z)
	D3DXVECTOR3 nor;			// 法線ベクトル
	D3DCOLOR col;				// 頂点カラー(r,g,b,a)
	D3DXVECTOR2 tex;			// テクスチャ座標(x,y)
}VERTEX_3D;

//*****************************************************************************
// 頂点情報[3D_MULTI]の構造体を定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点情報(X,Y,Z)
	D3DXVECTOR3 nor;	// 法線ベクトル
	D3DCOLOR col;		// 頂点カラー（r,g,b,a）

	D3DXVECTOR2 tex;	// テクスチャの座標0(X,Y)
	D3DXVECTOR2 texM;	// テクスチャの座標1(X,Y)

}VERTEX_3D_MULTI;

#endif