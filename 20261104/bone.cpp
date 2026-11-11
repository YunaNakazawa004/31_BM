//========================================================================
// 
// ボーン [ bone.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "bone.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

#include "line.h"
#include "object.h"

//************************************************************************
// マクロ定義
//************************************************************************

//========================================================================
// ボーンクラスの生成処理
//========================================================================
CBone* CBone::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return NULL;
	}
#endif

	CBone* pBone = NULL;

	if (pBone == NULL)
	{// NULLチェック
		// ボーンの生成
		pBone = new CBone;
	}

	if (pBone != NULL)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pBone->Init(pos, rot)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! ボーンの初期化に失敗しました ! ! !\n");

			return NULL;
		}

		return pBone;
	}

	OutputDebugStringA("! ! ! ボーンの生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// ボーンクラスのコンストラクタ
//========================================================================
CBone::CBone()
{
	// ボーンクラスの値をクリア
	m_pParent = nullptr;
}

//========================================================================
// ボーンクラスのデストラクタ
//========================================================================
CBone::~CBone()
{
}

//========================================================================
// ボーンクラスの初期化処理
//========================================================================
HRESULT CBone::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	// 初期化処理
	CPart::Init(pos, rot);

	return S_OK;
}

//========================================================================
// ボーンクラスの終了処理
//========================================================================
void CBone::Uninit(void)
{
	// 終了処理
	CPart::Uninit();
}

//========================================================================
// ボーンクラスの更新処理
//========================================================================
void CBone::Update(void)
{
}

//========================================================================
// ボーンクラスの描画処理
//========================================================================
void CBone::Draw(void)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス
	D3DXMATRIX mtxParent, mtxWorld;				// 親のマトリックス

	// 描画処理
	CPart::Draw();

	// マトリックスを取得
	mtxWorld = GetMtxWorld();

	// パーツの「親のマトリックス」を設定
	if (m_pParent != NULL)
	{// 親ボーンがある場合
		// 親ボーンのマトリックスを取得する
		mtxParent = m_pParent->GetMtxWorld();
	}
	else
	{// 親ボーンがない場合
		// 現在（最新）のマトリックスを取得する[ = プレイヤーのマトリックス]
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
	}

	// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxParent);

	// マトリックスを設定
	SetMtxWorld(mtxWorld);
}