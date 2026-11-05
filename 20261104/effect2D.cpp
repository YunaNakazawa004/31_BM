//========================================================================
// 
// 2Dエフェクト [ effect2D.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "effect2D.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define MOVE				(5.0f)						// 移動量
#define ALPHA_MINUS			(0.001f)					// アルファ値の減衰
#define SIZE_MINUS			(0.1f)						// 半径の変更量
#define EFFECT_INERTIA		(0.017f)					// 慣性
#define GRAVITY				(0.02f)						// 重力

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************
int CEffect2D::m_nIdxTexture = -1;				// テクスチャのインデックス

//========================================================================
// テクスチャの生成
//========================================================================
HRESULT CEffect2D::Load(void)
{
	// ローカル変数宣言
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ

	// テクスチャの設定
	m_nIdxTexture = pTexture->Register("data\\TEXTURE\\effect000.jpg");

	if (m_nIdxTexture == -1)
	{// テクスチャが設定できていない
		OutputDebugStringA("! ! ! テクスチャの設定に失敗しました ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}

//========================================================================
// テクスチャの破棄
//========================================================================
void CEffect2D::Unload(void)
{
	// テクスチャのインデックスを削除
	m_nIdxTexture = -1;
}

//========================================================================
// 2Dエフェクトクラスの生成処理
//========================================================================
CEffect2D* CEffect2D::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const float fSpeed,
	const int nLife, const float fRadius, const float fAddRadius, CEffect2D::TYPE type,
	const D3DXCOLOR col, const bool bHoming, const D3DXVECTOR3 HomingPos, const float fSpeedHoming)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CEffect2D* pEffect2D = nullptr;

	if (pEffect2D == nullptr)
	{// NULLチェック
		// 2Dエフェクトの生成
		pEffect2D = new CEffect2D;
	}

	if (pEffect2D != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pEffect2D->Init(pos, move, fSpeed, nLife, fRadius, fAddRadius, type, 
			col, bHoming, HomingPos, fSpeedHoming)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! 2Dエフェクトの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// オブジェクトの種類を設定
		pEffect2D->SetType(TYPE_EFFECT2D);

		if (type == CEffect2D::TYPE_NORMAL || type == CEffect2D::TYPE_BLENDADD)
		{// テクスチャありの場合
			// テクスチャの割り当て
			pEffect2D->BindTexture(m_nIdxTexture);
		}

		return pEffect2D;
	}

	OutputDebugStringA("! ! ! 2Dエフェクトの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// 2Dエフェクトクラスのコンストラクタ
//========================================================================
CEffect2D::CEffect2D(const int nPriority) :CObject2D(nPriority)
{
	// 2Dエフェクトクラスの値をクリア
	m_move = DEFAULT_VECTOR3;
	m_fSpeed = 0.0f;
	m_nLife = 0;
	m_fRadius = 0.0f;
	m_fAddRadius = 0.0f;
	m_type = TYPE_NORMAL;
	m_bHoming = false;
	m_HomingPos = DEFAULT_VECTOR3;
	m_fSpeedHoming = 0.0f;
}

//========================================================================
// 2Dエフェクトクラスのデストラクタ
//========================================================================
CEffect2D::~CEffect2D()
{
}

//========================================================================
// 2Dエフェクトクラスの初期化処理
//========================================================================
HRESULT CEffect2D::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const float fSpeed,
	const int nLife, const float fRadius, const float fAddRadius, CEffect2D::TYPE type,
	const D3DXCOLOR col, const bool bHoming, const D3DXVECTOR3 HomingPos, const float fSpeedHoming)
{
	if (FAILED(CObject2D::Init(pos, fRadius, fRadius)))
	{// 2Dエフェクトの初期化に失敗した場合
		OutputDebugStringA("! ! ! 2Dエフェクトの初期化に失敗しました ! ! !\n");

		return E_FAIL;
	}

	// 色を設定
	SetColor(col);

	// 2Dエフェクトクラスの値を初期化
	m_move = move;
	m_fSpeed = fSpeed;
	m_nLife = nLife;
	m_fRadius = fRadius;
	m_fAddRadius = fAddRadius;
	m_type = type;
	m_bHoming = bHoming;
	m_HomingPos = HomingPos;
	m_fSpeedHoming = fSpeedHoming;

	return S_OK;
}

//========================================================================
// 2Dエフェクトクラスの終了処理
//========================================================================
void CEffect2D::Uninit(void)
{
	CObject2D::Uninit();
}

//========================================================================
// 2Dエフェクトクラスの更新処理
//========================================================================
void CEffect2D::Update(void)
{
	// ローカル変数宣言
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	D3DXVECTOR3 pos = CObject2D::GetPosition();
	D3DXCOLOR col = CObject2D::GetColor();
	m_fRadius = CObject2D::GetSize();

	// 位置を更新
	pos.x += m_move.x * m_fSpeed;
	pos.y += m_move.y * m_fSpeed;

	if (m_bHoming == true)
	{// ホーミングする場合
		pos += (m_HomingPos - pos) * m_fSpeedHoming;

		if (pos == m_HomingPos)
		{// 着いたら消す
			// 終了処理
			Uninit();

			return;
		}
	}

	// サイズを変更する
	m_fRadius += m_fAddRadius;

	// 不透明度を減らす
	col.a -= ALPHA_MINUS;

	// 位置/サイズ/色を適用
	SetPosition(pos);
	SetSize(m_fRadius);
	SetColor(col);

	// 寿命を減らす
	m_nLife--;

	if (pos.x < 0.0f || pos.x > SCREEN_WIDTH || pos.y < 0.0f || pos.y > SCREEN_HEIGHT ||
		m_fRadius < 0.0f || col.a < 0.0f || m_nLife < 0)
	{// 2Dエフェクトが消える場合
		// 終了処理
		Uninit();

		return;
	}
}

//========================================================================
// 2Dエフェクトクラスの描画処理
//========================================================================
void CEffect2D::Draw(void)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ

	if (m_type == CEffect2D::TYPE_BLENDADD || m_type == CEffect2D::TYPE_BLENDADD_NULL)
	{// 加算合成
		// レンダーステートを加算合成にする
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	CObject2D::Draw();

	// レンダーステートを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}