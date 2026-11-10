//========================================================================
// 
// 砂パーティクル [ sand.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "sand.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"

//************************************************************************
// マクロ定義
//************************************************************************

//========================================================================
// 砂パーティクルクラスの生成処理
//========================================================================
CSand* CSand::Create(const D3DXVECTOR3 posP, const D3DXVECTOR3 nor, const float fStrength)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CSand* pSand = nullptr;

	if (pSand == nullptr)
	{// NULLチェック
		// 砂パーティクルの生成
		pSand = new CSand;
	}

	if (pSand != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pSand->Init(posP, nor, fStrength)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! 砂パーティクルの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// オブジェクトの種類を設定
		pSand->SetType(TYPE_PARTICLE3D);

		return pSand;
	}

	OutputDebugStringA("! ! ! 砂パーティクルの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// 砂パーティクルクラスのコンストラクタ
//========================================================================
CSand::CSand(const int nPriority) :CParticleBatching(nPriority)
{
	// 砂パーティクルクラスの値をクリア
}

//========================================================================
// 砂パーティクルクラスのデストラクタ
//========================================================================
CSand::~CSand()
{
}

//========================================================================
// 砂パーティクルクラスの初期化処理
//========================================================================
HRESULT CSand::Init(const D3DXVECTOR3 posP, const D3DXVECTOR3 nor, const float fStrength)
{
	// 初期化処理
	CParticleBatching::Init(posP, 1, 20 * fStrength, nor, D3DXVECTOR3(D3DX_PI * 0.8f, D3DX_PI * 0.5f, D3DX_PI * 0.8f), 2.0f, -0.001f, 0.001f,
		CParticleBatching::TYPE_SAND, CEffectBatching::TYPE_NORMAL, -0.001f, -0.1f, 100 * fStrength, 6.0f, 0.003f, COLOR_BEIGE, 50.0f);

	return S_OK;
}

//========================================================================
// 砂パーティクルクラスの終了処理
//========================================================================
void CSand::Uninit(void)
{
	// 終了処理
	CParticleBatching::Uninit();
}

//========================================================================
// 砂パーティクルクラスの更新処理
//========================================================================
void CSand::Update(void)
{
	// 更新処理
	CParticleBatching::Update();
}

//========================================================================
// 砂パーティクルクラスの描画処理
//========================================================================
void CSand::Draw(void)
{
	// 描画処理
	CParticleBatching::Draw();
}