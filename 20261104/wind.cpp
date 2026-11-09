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
#include "sound.h"
#include "texture.h"
#include "XModel.h"
#include "debugproc.h"

#include "camera.h"
#include "line.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define DEFAULT_SIZE		(30.0f)			// 基本サイズ
#define INERTIA				(0.1f)			// 慣性

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
CWind* CWind::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 vec, const float fStrength, const float fAffect)
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
		if (FAILED(pWind->Init(pos, vec, fStrength, fAffect)))
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
	m_move = DEFAULT_VECTOR3;
	m_vec = DEFAULT_VECTOR3;
	m_rotDest = DEFAULT_VECTOR3;
	m_fStrength = 0.0f;
	m_fAngle = 0.0f;
	m_fAffect = 0.0f;
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
HRESULT CWind::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 vec, const float fStrength, const float fAffect)
{
	// 初期化処理
	CObjectX::Init(pos);

	// 風の初期化
	m_move = DEFAULT_VECTOR3;
	m_vec = vec;
	m_rotDest = DEFAULT_VECTOR3;
	m_fStrength = fStrength;
	m_fAngle = 0.0f;
	m_fAffect = fAffect;

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

	// 移動
	pos += m_move;

	// 慣性
	m_move.x += (0.0f - m_move.x) * INERTIA;
	m_move.z += (0.0f - m_move.z) * INERTIA;

	// 風の強さ
	if (pInputKeyboard->GetRepeat(DIK_UP) == true)
	{// 左に移動
		m_fStrength += 0.1f;
	}
	else if (pInputKeyboard->GetRepeat(DIK_DOWN) == true)
	{// 右に移動
		m_fStrength -= 0.1f;
	}

	// 風の向き変更
	if (pInputKeyboard->GetPress(DIK_NUMPAD4) == true)
	{// 右に旋回
		m_rotDest.y += -0.035f;
	}
	else if (pInputKeyboard->GetPress(DIK_NUMPAD6) == true)
	{// 左に旋回
		m_rotDest.y += 0.035f;
	}

	m_vec.x = sinf(D3DX_PI + rot.y) * sinf((D3DX_PI * 0.5f) - m_fAngle);
	m_vec.y = cosf((D3DX_PI * 0.5f) - m_fAngle);
	m_vec.z = cosf(D3DX_PI + rot.y) * sinf((D3DX_PI * 0.5f) - m_fAngle);

	// 正規化
	D3DXVec3Normalize(&m_vec, &m_vec);

	// 目的の向きを修正
	float fmoveAngle = m_rotDest.y - rot.y;

	// 向きを調整
	CorrectAngle(&m_rotDest.y, fmoveAngle);

	// 角度を慣性ありで加算
	rot.y += (m_rotDest.y - rot.y) * 0.1f;

	// 位置/向きを適用
	SetPosition(pos);
	SetRotation(rot);

	// カメラ内にいるかどうか判定
	bool bInside = CManager::GetCamera()->IsInside(pos, 0.0f);
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
	CDebugProc* pDebug = CManager::GetDebugProc();
	D3DXVECTOR3 distVec = GetPosition() - *pPos;
	float fWindAngle = atan2f(m_vec.x, m_vec.z);
	float fDistAngle = atan2f(distVec.x, distVec.z);
	CorrectAngle(&fWindAngle, fWindAngle - fDistAngle);
	float fSumAngle = (fWindAngle + fDistAngle) / 2.0f;
	CorrectAngle(&fSumAngle, fSumAngle - pRotDest->y);

#ifdef _DEBUG
	D3DXVECTOR3 line = GetPosition();
	line.x += sinf(fSumAngle) * m_fStrength * 10.0f;
	line.z += cosf(fSumAngle) * m_fStrength * 10.0f;
	CLine::Create(GetPosition(), line, COLOR_MAGENTA, COLOR_WHITE);
#endif

	// 向きを設定
	pRotDest->y += (fSumAngle - pRotDest->y) * m_fAffect;

	// 回転量を設定
	*pValueRot += m_fStrength * 0.0001f;
}

//========================================================================
// 風のコントロール
//========================================================================
void CWind::StickControll(CInputJoypad::JOYKEY joykey)
{
	CCamera* pCamera = CManager::GetCamera();							// カメラの取得
	D3DXVECTOR3 CameraRot = pCamera->GetRotation();
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();			// ジョイパッド入力の取得
	int nValueH, nValueV;
	float fAngle = 0.0f;

	// パッド移動
	if (pInputJoypad->GetStick(0, joykey, &nValueH, &nValueV) == true)
	{// パッドの移動
		m_rotDest.y = CameraRot.y + atan2f(-(float)nValueH, -D3DX_PI - (float)nValueV) + D3DX_PI;

		// 移動方向
		fAngle = atan2f((float)(nValueH), (float)(nValueV));

		m_move.x += sinf(fAngle + CameraRot.y) * 0.3f;
		m_move.z += cosf(fAngle + CameraRot.y) * 0.3f;
	}

	// 正規化
	D3DXVec3Normalize(&m_vec, &m_vec);
}

//========================================================================
// 自動移動
//========================================================================
void CWind::AutoMove(const D3DXVECTOR3 move)
{
	D3DXVECTOR3 pos = GetPosition();

	pos += move;

	// 位置を適用
	SetPosition(pos);
}