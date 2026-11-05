//========================================================================
// 
// 風 [	wind.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "wind.h"

#include "renderer.h"
#include "MTrenderer.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "texture.h"
#include "XModel.h"
#include "debugproc.h"

#include "camera.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define DEFAULT_SIZE		(30.0f)			// 基本サイズ

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************
int CWind::m_nIdxModel = {};			// モデルのインデックス

//========================================================================
// モデルのロード処理
//========================================================================
HRESULT CWind::Load(const char* pFilename)
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
void CWind::Unload(void)
{
	// モデルのインデックスを削除
	memset(&m_nIdxModel, -1, sizeof m_nIdxModel);
}

//========================================================================
// 風クラスの生成処理
//========================================================================
CWind* CWind::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 vec, const float fStrength)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CWind* pWind = nullptr;

	if (pWind == nullptr)
	{// NULLチェック
		// 風の生成
		pWind = new CWind;
	}

	if (pWind != nullptr)
	{// NULLチェック
		// モデルファイル設定
		pWind->BindModel(m_nIdxModel);

		// 初期化処理
		if (FAILED(pWind->Init(pos, vec, fStrength)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! 風の初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// 種類を設定
		pWind->SetType(TYPE_WIND);

		return pWind;
	}

	OutputDebugStringA("! ! ! 風の生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// 風クラスのコンストラクタ
//========================================================================
CWind::CWind(const int nPriority) :CObjectX(nPriority)
{
	// 風クラスの値をクリア
	m_vec = DEFAULT_VECTOR3;
	m_fStrength = 0.0f;
	m_fAngle = 0.0f;
}

//========================================================================
// 風クラスのデストラクタ
//========================================================================
CWind::~CWind()
{
}

//========================================================================
// 風クラスの初期化処理
//========================================================================
HRESULT CWind::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 vec, const float fStrength)
{
	// 初期化処理
	CObjectX::Init(pos);

	// 風の初期化
	m_vec = vec;
	m_fStrength = fStrength;
	m_fAngle = 0.0f;

	// サイズ
	D3DXVECTOR3 scale = GetScale();
	scale.x = (fStrength * DEFAULT_SIZE) / DEFAULT_SIZE;
	scale.y = (fStrength * DEFAULT_SIZE) / DEFAULT_SIZE;
	scale.z = (fStrength * DEFAULT_SIZE) / DEFAULT_SIZE;
	SetScale(scale);

	return S_OK;
}

//========================================================================
// 風クラスの終了処理
//========================================================================
void CWind::Uninit(void)
{
	// 終了処理
	CObjectX::Uninit();
}

//========================================================================
// 風クラスの更新処理
//========================================================================
void CWind::Update(void)
{
	// ローカル変数
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();			// ジョイパッド入力の取得
	CDebugProc* pDebug = CManager::GetDebugProc();
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 rot = GetRotation();
	D3DXVECTOR3 CameraRot = CManager::GetCamera()->GetRotation();

	// 風の位置
	if (pInputKeyboard->GetPress(DIK_UP) == true)		// キーボード移動
	{// 奥に移動
		if (pInputKeyboard->GetPress(DIK_LEFT) == true)
		{// 左奥に移動
			pos.x += sinf(-D3DX_PI * 0.75f - CameraRot.y) * 5.0f;
			pos.z += cosf(-D3DX_PI * 0.25f + CameraRot.y) * 5.0f;
		}
		else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
		{// 右奥に移動
			pos.x += sinf(D3DX_PI * 0.75f - CameraRot.y) * 5.0f;
			pos.z += cosf(D3DX_PI * 0.25f + CameraRot.y) * 5.0f;
		}
		else if (pInputKeyboard->GetPress(DIK_UP) == true)
		{// 奥に移動
			pos.x += sinf(CameraRot.y) * 5.0f;
			pos.z += cosf(CameraRot.y) * 5.0f;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
	{// 手前に移動
		if (pInputKeyboard->GetPress(DIK_LEFT) == true)
		{// 左手前に移動
			pos.x += sinf(-D3DX_PI * 0.25f - CameraRot.y) * 5.0f;
			pos.z += cosf(-D3DX_PI * 0.75f + CameraRot.y) * 5.0f;
		}
		else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
		{// 右手前に移動
			pos.x += sinf(D3DX_PI * 0.25f - CameraRot.y) * 5.0f;
			pos.z += cosf(D3DX_PI * 0.75f + CameraRot.y) * 5.0f;
		}
		else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
		{// 手前に移動
			pos.x += sinf(D3DX_PI * 1.0f + CameraRot.y) * 5.0f;
			pos.z += cosf(D3DX_PI * 1.0f + CameraRot.y) * 5.0f;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_LEFT) == true)
	{// 左に移動
		pos.x += sinf(-D3DX_PI * 0.5f + CameraRot.y) * 5.0f;
		pos.z += cosf(-D3DX_PI * 0.5f + CameraRot.y) * 5.0f;
	}
	else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
	{// 右に移動
		pos.x += sinf(D3DX_PI * 0.5f + CameraRot.y) * 5.0f;
		pos.z += cosf(D3DX_PI * 0.5f + CameraRot.y) * 5.0f;
	}

	// 風の向き変更
	if (pInputKeyboard->GetPress(DIK_NUMPAD4) == true)
	{// 右に旋回
		rot.y += -0.035f;

		m_vec.x = sinf(D3DX_PI + rot.y) * sinf((D3DX_PI * 0.5f) - m_fAngle);
		m_vec.y = cosf((D3DX_PI * 0.5f) - m_fAngle);
		m_vec.z = cosf(D3DX_PI + rot.y) * sinf((D3DX_PI * 0.5f) - m_fAngle);
	}
	else if (pInputKeyboard->GetPress(DIK_NUMPAD6) == true)
	{// 左に旋回
		rot.y += 0.035f;

		m_vec.x = sinf(D3DX_PI + rot.y) * sinf((D3DX_PI * 0.5f) - m_fAngle);
		m_vec.y = cosf((D3DX_PI * 0.5f) - m_fAngle);
		m_vec.z = cosf(D3DX_PI + rot.y) * sinf((D3DX_PI * 0.5f) - m_fAngle);
	}

	if (pInputKeyboard->GetPress(DIK_NUMPAD5) == true)
	{// 上に移動
		m_fAngle += 0.035f;

		if (m_fAngle > D3DX_PI * 1.0f)
		{// 上の制限
			m_fAngle = D3DX_PI * 1.0f;
		}

		CorrectAngle(&m_fAngle, m_fAngle);

		m_vec.x = sinf(D3DX_PI + rot.y) * sinf((D3DX_PI * 0.5f) - m_fAngle);
		m_vec.y = cosf((D3DX_PI * 0.5f) - m_fAngle);
		m_vec.z = cosf(D3DX_PI + rot.y) * sinf((D3DX_PI * 0.5f) - m_fAngle);
	}
	else if (pInputKeyboard->GetPress(DIK_NUMPAD8) == true)
	{// 下に移動
		m_fAngle += -0.035f;

		if (m_fAngle < -D3DX_PI * 0.0f)
		{// 下の制限
			m_fAngle = -D3DX_PI * 0.0f;
		}

		CorrectAngle(&m_fAngle, m_fAngle);

		m_vec.x = sinf(D3DX_PI + rot.y) * sinf((D3DX_PI * 0.5f) - m_fAngle);
		m_vec.y = cosf((D3DX_PI * 0.5f) - m_fAngle);
		m_vec.z = cosf(D3DX_PI + rot.y) * sinf((D3DX_PI * 0.5f) - m_fAngle);
	}

	// 正規化
	D3DXVec3Normalize(&m_vec, &m_vec);

	// 位置を適用
	SetPosition(pos);

	// デバッグコメント
	pDebug->Print("\n< WIND >\n");

	pDebug->Print("位置 : ( %f %f %f )\n", pos.x, pos.y, pos.z);
	pDebug->Print("風の向き [ NUMPAD 8/4/5/6 ] : ( %f %f %f )\n", m_vec.x, m_vec.y, m_vec.z);
	pDebug->Print("風の強さ [ ↑ / ↓ ] : ( %f )\n", m_fStrength);
}

//========================================================================
// 風クラスの描画処理
//========================================================================
void CWind::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// 頂点法線正規化をオン
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// 描画処理
	CObjectX::Draw();

	// 頂点法線正規化をオフ
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}

//========================================================================
// 風による影響を計算
//========================================================================
void CWind::WindAffect(D3DXVECTOR3* pPos, D3DXVECTOR3* pVecAxis, D3DXVECTOR3* pRotDest, float* pValueRot)
{
	// ローカル変数
	D3DXVECTOR3 distVec = *pPos - GetPosition();
	float fWindAngle = atan2f(m_vec.x, -m_vec.z);
	float fDistAngle = atan2f(-distVec.x, -distVec.z);

	// 向きを設定
	pRotDest->y = (fWindAngle + fDistAngle) / 2.0f;

	// 回転量を設定
	*pValueRot += m_fStrength * 0.0001f;
}