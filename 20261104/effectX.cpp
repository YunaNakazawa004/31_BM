//========================================================================
// 
// Xエフェクト [ effectX.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "effectX.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"
#include "Xmodel.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define MOVE				(5.0f)						// 移動量
#define ALPHA_MINUS			(0.1f)						// アルファ値の減衰
#define SIZE_MINUS			(0.1f)						// 半径の変更量
#define EFFECT_INERTIA		(0.017f)					// 慣性
#define GRAVITY				(-0.15f)					// 重力


//************************************************************************
// 静的メンバ変数宣言
//************************************************************************
int CEffectX::m_nIdxModel = -1;			// モデルのインデックス

//========================================================================
// モデルのロード処理
//========================================================================
HRESULT CEffectX::Load(const char* pFilename)
{
	// ローカル変数宣言
	CXModel* pXModel = CManager::GetXModel();			// モデル管理へのポインタ

	// テクスチャの設定
	m_nIdxModel = pXModel->Register(pFilename);

	if (m_nIdxModel == -1)
	{// モデルが設定できていない
		OutputDebugStringA("! ! ! モデルの設定に失敗しました ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}

//========================================================================
// モデルのアンロード処理
//========================================================================
void CEffectX::Unload(void)
{
	// モデルのインデックスを削除
	m_nIdxModel = -1;
}

//========================================================================
// Xエフェクトクラスの生成処理
//========================================================================
CEffectX* CEffectX::Create(const char* pFilename, const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const float fSpeed, const bool bSpeedInertia,
	const int nLife, const D3DXVECTOR3 scale, const float fAddRadius, const bool bGravity)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CEffectX* pEffectX = nullptr;

	if (pEffectX == nullptr)
	{// NULLチェック
		// Xエフェクトの生成
		pEffectX = new CEffectX;
	}

	if (pEffectX != nullptr)
	{// NULLチェック
		// モデルファイル設定
		Load(pFilename);
		pEffectX->BindModel(m_nIdxModel);

		// 初期化処理
		if (FAILED(pEffectX->Init(pos, move, fSpeed, bSpeedInertia, nLife, scale, fAddRadius, bGravity)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! Xエフェクトの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// オブジェクトの種類を設定
		pEffectX->SetType(TYPE_EFFECT3D);

		return pEffectX;
	}

	OutputDebugStringA("! ! ! Xエフェクトの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// Xエフェクトクラスのコンストラクタ
//========================================================================
CEffectX::CEffectX(const int nPriority) :CObjectX(nPriority)
{
	// Xエフェクトクラスの値をクリア
	m_move = DEFAULT_VECTOR3;
	m_fSpeed = 0.0f;
	m_bSpeedInertia = false;
	m_nLife = 0;
	m_fAddRadius = 0.0f;
	m_fMinusAlpha = 0.0f;
	m_bGravity = false;
	m_fRadius = 0.0f;
}

//========================================================================
// Xエフェクトクラスのデストラクタ
//========================================================================
CEffectX::~CEffectX()
{
}

//========================================================================
// Xエフェクトクラスの初期化処理
//========================================================================
HRESULT CEffectX::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const float fSpeed, const bool bSpeedInertia,
	const int nLife, const D3DXVECTOR3 scale, const float fAddRadius, const bool bGravity)
{
	if (FAILED(CObjectX::Init(pos)))
	{// Xエフェクトの初期化に失敗した場合
		OutputDebugStringA("! ! ! Xエフェクトの初期化に失敗しました ! ! !\n");

		return E_FAIL;
	}

	// Xエフェクトクラスの値を初期化
	m_move = move;
	m_fSpeed = fSpeed;
	m_bSpeedInertia = bSpeedInertia;
	m_nLife = nLife;
	m_fAddRadius = fAddRadius;
	m_bGravity = bGravity;

	// 向き/スケールを設定
	SetRotation(D3DXVECTOR3(0.0f, (float)(rand() % 629 - 314) / 100.0f, 0.0f));
	SetScale(scale * ((float)(rand() % 200 - 100) / 100.0f));

	// 半径を計算
	m_fRadiusDef = (GetVtxMax().y - GetVtxMin().y) / 2.0f;
	m_fRadius = m_fRadiusDef * scale.y;

	return S_OK;
}

//========================================================================
// Xエフェクトクラスの終了処理
//========================================================================
void CEffectX::Uninit(void)
{
	CObjectX::Uninit();
}

//========================================================================
// Xエフェクトクラスの更新処理
//========================================================================
void CEffectX::Update(void)
{
	// ローカル変数宣言
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	D3DXVECTOR3 pos = CObjectX::GetPosition();
	D3DXVECTOR3 scale = CObjectX::GetScale();

	// 位置を更新
	pos.x += m_move.x * m_fSpeed;
	pos.z += m_move.z * m_fSpeed;

	// サイズを変更する
	scale *= m_fAddRadius;

	// サイズに合わせて半径を計算
	m_fRadius = scale.x * m_fRadiusDef;
	m_fRadius = scale.y * m_fRadiusDef;
	m_fRadius = scale.z * m_fRadiusDef;

	if (m_bGravity == true)
	{// 重力あり
		pos.y += m_move.y;
		m_move.y += GRAVITY;

		if (pos.y - m_fRadius < 0.0f)
		{// 地面との当たり判定
			m_move.y *= -0.9f;
		}
	}
	else
	{// 重力無し
		pos.y += m_move.y * m_fSpeed;
	}

	if (m_bSpeedInertia == true)
	{// 慣性あり
		// 慣性
		m_fSpeed += (0.0f - m_fSpeed) * EFFECT_INERTIA;
	}

	// 位置/サイズ/色を適用
	SetPosition(pos);
	SetScale(scale);

	// 寿命を減らす
	m_nLife--;

	if (scale.x < 0.3f || m_nLife < 0)
	{// Xエフェクトが消える場合
		// 終了処理
		Uninit();

		return;
	}
}

//========================================================================
// Xエフェクトクラスの描画処理
//========================================================================
void CEffectX::Draw(void)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ

	// 頂点法線正規化をオン
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// 描画処理
	CObjectX::Draw();

	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}