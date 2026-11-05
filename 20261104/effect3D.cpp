//========================================================================
// 
// 3Dエフェクト [ effect3D.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "effect3D.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"

#include "effect_batching.h"

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
int CEffect3D::m_nIdxTexture = -1;				// テクスチャのインデックス

//========================================================================
// テクスチャの生成
//========================================================================
HRESULT CEffect3D::Load(void)
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
void CEffect3D::Unload(void)
{
	// テクスチャのインデックスを削除
	m_nIdxTexture = -1;
}

//========================================================================
// 3Dエフェクトクラスの生成処理
//========================================================================
CEffect3D* CEffect3D::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const bool bGravity, const float fSpeed, const bool bSpeedInertia,
	const int nLife, const float fRadius, const float fAddRadius, const float fMinusAlpha, CEffect3D::TYPE type,
	const D3DXCOLOR col, const bool bShadow, const bool bHoming, CObject* HomingObj, D3DXVECTOR3 HomingPos, const float fSpeedHoming)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CEffect3D* pEffect3D = nullptr;

	if (pEffect3D == nullptr)
	{// NULLチェック
		// 3Dエフェクトの生成
		pEffect3D = new CEffect3D;
	}

	if (pEffect3D != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pEffect3D->Init(pos, move, bGravity, fSpeed, bSpeedInertia, nLife, fRadius, fAddRadius, fMinusAlpha, type,
			col, bShadow, bHoming, HomingObj, HomingPos, fSpeedHoming)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! 3Dエフェクトの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// オブジェクトの種類を設定
		pEffect3D->SetType(TYPE_EFFECT3D);

		if (type == CEffect3D::TYPE_NORMAL || type == CEffect3D::TYPE_BLENDADD)
		{// テクスチャありの場合
			// テクスチャの割り当て
			pEffect3D->BindTexture(m_nIdxTexture);
		}

		return pEffect3D;
	}

	OutputDebugStringA("! ! ! 3Dエフェクトの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// 3Dエフェクトクラスのコンストラクタ
//========================================================================
CEffect3D::CEffect3D(const int nPriority) :CObjectBillboard(nPriority)
{
	// 3Dエフェクトクラスの値をクリア
	m_move = DEFAULT_VECTOR3;
	m_bGravity = false;
	m_fSpeed = 0.0f;
	m_bSpeedInertia = false;
	m_nLife = 0;
	m_fRadius = 0.0f;
	m_fAddRadius = 0.0f;
	m_fMinusAlpha = 0.0f;
	m_type = TYPE_NORMAL;
	m_bHoming = false;
	m_HomingObj = nullptr;
	m_HomingPos = DEFAULT_VECTOR3;;
	m_fSpeedHoming = 0.0f;
	m_bShadow = false;
}

//========================================================================
// 3Dエフェクトクラスのデストラクタ
//========================================================================
CEffect3D::~CEffect3D()
{
}

//========================================================================
// 3Dエフェクトクラスの初期化処理
//========================================================================
HRESULT CEffect3D::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const bool bGravity, const float fSpeed, const bool bSpeedInertia,
	const int nLife, const float fRadius, const float fAddRadius, const float fMinusAlpha, CEffect3D::TYPE type,
	const D3DXCOLOR col, const bool bShadow, const bool bHoming, CObject* HomingObj, D3DXVECTOR3 HomingPos, const float fSpeedHoming)
{
	if (type == TYPE_NORMAL_DISTORT)
	{// 歪
		if (FAILED(CObjectBillboard::Init(pos, fRadius + ((float)(rand() % (int)fRadius) - (fRadius * 0.5f)), fRadius)))
		{// 3Dエフェクトの初期化に失敗した場合
			OutputDebugStringA("! ! ! 3Dエフェクトの初期化に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}
	else
	{// 普通のかたち
		if (FAILED(CObjectBillboard::Init(pos, fRadius, fRadius)))
		{// 3Dエフェクトの初期化に失敗した場合
			OutputDebugStringA("! ! ! 3Dエフェクトの初期化に失敗しました ! ! !\n");

			return E_FAIL;
		}
	}

	// 色を設定
	SetColor(col);

	// 3Dエフェクトクラスの値を初期化
	m_move = move;
	m_bGravity = bGravity;
	m_fSpeed = fSpeed;
	m_bSpeedInertia = bSpeedInertia;
	m_nLife = nLife;
	m_fRadius = fRadius;
	m_fAddRadius = fAddRadius;
	m_fMinusAlpha = fMinusAlpha;
	m_type = type;
	m_bHoming = bHoming;
	m_HomingObj = HomingObj;
	m_HomingPos = HomingPos;
	m_fSpeedHoming = fSpeedHoming;
	m_bShadow = bShadow;

	if (m_bShadow == true)
	{// 影をバッチ描画エフェクトに設定する
		CManager::GetEffectBatching()->Set(pos, move, fSpeed, bSpeedInertia, bGravity, nLife, fRadius, fAddRadius, fMinusAlpha,
			col, bShadow, bHoming, HomingObj, HomingPos, fSpeedHoming);
	}

	return S_OK;
}

//========================================================================
// 3Dエフェクトクラスの終了処理
//========================================================================
void CEffect3D::Uninit(void)
{
	CObjectBillboard::Uninit();
}

//========================================================================
// 3Dエフェクトクラスの更新処理
//========================================================================
void CEffect3D::Update(void)
{
	// ローカル変数宣言
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	D3DXVECTOR3 pos = CObjectBillboard::GetPosition();
	D3DXCOLOR col = CObjectBillboard::GetColor();
	m_fRadius = CObjectBillboard::GetSize();

	// 位置を更新
	pos.x += m_move.x * m_fSpeed;
	pos.z += m_move.z * m_fSpeed;

	if (m_bHoming == true)
	{// ホーミングする場合
		D3DXVECTOR3 HomingPos;
		D3DXVECTOR3 dist;

		if (m_HomingObj != nullptr)
		{// NULLチェック
			HomingPos = m_HomingObj->GetPosition();
		}
		else
		{// ホーミング対象がいなかったら
			HomingPos = m_HomingPos;
		}

		pos += (HomingPos - pos) * m_fSpeedHoming;

		dist = pos - HomingPos;

		if (D3DXVec3Length(&dist) < 1.0f)
		{// 着いたら消す
			// 終了処理
			Uninit();

			return;
		}
	}

	if (m_bGravity == true)
	{// 重力あり
		pos.y += m_move.y;
		m_move.y += GRAVITY;

		if (pos.y < 0.0f)
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

	// サイズを変更する
	m_fRadius += m_fAddRadius;

	// 不透明度を減らす
	col.a -= m_fMinusAlpha;

	// 位置/サイズ/色を適用
	SetPosition(pos);
	SetSize(m_fRadius);
	SetColor(col);

	// 寿命を減らす
	m_nLife--;

	if (m_fRadius < 0.0f || col.a < 0.0f || m_nLife < 0)
	{// 3Dエフェクトが消える場合
		// 終了処理
		Uninit();

		return;
	}
}

//========================================================================
// 3Dエフェクトクラスの描画処理
//========================================================================
void CEffect3D::Draw(void)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxWorld;

	// ライティングオフ
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	if (m_type == CEffect3D::TYPE_BLENDADD || m_type == CEffect3D::TYPE_BLENDADD_NULL)
	{// 加算合成
		// レンダーステートを加算合成にする
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// 描画処理
	CObjectBillboard::Draw();

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// レンダーステートを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// ライティングオン
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}