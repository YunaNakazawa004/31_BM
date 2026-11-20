//========================================================================
// 
// 球 [	ball.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "ball.h"

#include "renderer.h"
#include "MTrenderer.h"
#include "manager.h"
#include "input.h"
#include "sound.h"
#include "texture.h"
#include "XModel.h"
#include "debugproc.h"

#include "camera.h"
#include "wind.h"
#include "line.h"
#include "wallMM.h"
#include "meshwallM.h"
#include "meshring.h"
#include "meshorbit.h"
#include "particle3D.h"
#include "particleX.h"
#include "sand.h"
#include "shadowSt.h"
#include "map_object.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define MOVEMENT				((m_fValueRot / (2.0f * D3DX_PI)) * (2.0f * D3DX_PI * m_fRadius))		
																		// 移動量
#define MAX_VALUEROT			(1.0f)									// 最大回転角
#define SPEED					(m_fSpeedRot)							// 回転量
#define MAX_SPEED				(0.1f)									// 最大回転量
#define MIN_SPEED				(0.001f)								// 最低回転量
#define SPEED_INERTIA			(m_fSpeedInertia)						// 回転量の慣性
#define MAX_SPEED_INERTIA		(0.2f)									// 最大回転の慣性
#define MIN_SPEED_INERTIA		(0.01f)									// 最低回転の慣性
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define GRAVITY					(-0.3f)									// 重力
#define BOUND					(-0.7f)									// 跳ね返り
#define ANTI_BOUND				(2.5f)									// 跳ね返りを消す値
#define DEFAULT_SIZE			(10.0f)									// 規定のモデルサイズ
#define MAX_SIZE				(50.0f)									// 最大サイズ
#define MIN_SIZE				(5.0f)									// 最小サイズ
#define REFLECT_INTERVAL		(60)									// 反射後の移動インターバル

#define INFORMATIONENABLE		// 球の情報をコメント化
#undef INFORMATIONENABLE

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************
int CBall::m_nIdxModel = {};			// モデルのインデックス

//========================================================================
// モデルのロード処理
//========================================================================
HRESULT CBall::Load(const char* pFilename)
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
void CBall::Unload(void)
{
	// モデルのインデックスを削除
	memset(&m_nIdxModel, -1, sizeof m_nIdxModel);
}

//========================================================================
// 球クラスの生成処理
//========================================================================
CBall* CBall::Create(const D3DXVECTOR3 pos, const float fRadius,
	const float fSpeed, const float fSpeedInertia)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CBall* pBall = nullptr;

	if (pBall == nullptr)
	{// NULLチェック
		// 球の生成
		pBall = new CBall;
	}

	if (pBall != nullptr)
	{// NULLチェック
		// モデルファイル設定
		pBall->BindModel(m_nIdxModel);

		// 初期化処理
		if (FAILED(pBall->Init(pos, fRadius, fSpeed, fSpeedInertia)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! 球の初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// 種類を設定
		pBall->SetType(TYPE_BALL);

		return pBall;
	}

	OutputDebugStringA("! ! ! 球の生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// 球クラスのコンストラクタ
//========================================================================
CBall::CBall(const int nPriority) :CObjectX(nPriority)
{
	// 球クラスの値をクリア
	m_posOld = DEFAULT_VECTOR3;
	m_move = DEFAULT_VECTOR3;
	m_rotDest = DEFAULT_VECTOR3;
	m_rotOld = DEFAULT_VECTOR3;
	m_state = STATE_NORMAL;
	m_nCounterState = 0;
	m_pShadowSt = nullptr;
	m_pMeshOrbit = nullptr;
	D3DXMatrixIdentity(&m_mtxRot);
	D3DXQuaternionIdentity(&m_quat);
	m_vecAxis = DEFAULT_VECTOR3;
	m_fValueRot = 0.0f;
	m_fSpeedRot = 0.0f;
	m_fSpeedInertia = 0.0f;
	m_fRadius = 0.0f;
	m_nCounter = 0;
}

//========================================================================
// 球クラスのデストラクタ
//========================================================================
CBall::~CBall()
{
}

//========================================================================
// 球クラスの初期化処理
//========================================================================
HRESULT CBall::Init(const D3DXVECTOR3 pos, const float fRadius,
	const float fSpeed, const float fSpeedInertia)
{
	// 初期化処理
	CObjectX::Init(pos, DEFAULT_VECTOR3);

	// 球の初期化
	m_posOld = DEFAULT_VECTOR3;
	m_move = DEFAULT_VECTOR3;
	m_rotDest = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);
	m_rotOld = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_state = STATE_NORMAL;
	m_nCounterState = 0;
	m_vecAxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_fValueRot = 0.0f;
	m_fSpeedRot = fSpeed;
	m_fSpeedInertia = fSpeedInertia;
	m_fRadius = fRadius;
	m_nCounter = 0;

	D3DXVECTOR3 scale = CBall::GetScale();
	scale.x = m_fRadius / DEFAULT_SIZE;
	scale.y = m_fRadius / DEFAULT_SIZE;
	scale.z = m_fRadius / DEFAULT_SIZE;
	SetScale(scale);
	SetRotation(D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));

	// ステンシルシャドウを生成
	if (m_pShadowSt == nullptr)
	{// NULLチェック
		m_pShadowSt = CShadowSt::Create(pos);

		if (m_pShadowSt != nullptr)
		{// NULLチェック
			m_pShadowSt->SetScale(scale);
		}
	}

	return S_OK;
}

//========================================================================
// 球クラスの終了処理
//========================================================================
void CBall::Uninit(void)
{
	// 終了処理
	CObjectX::Uninit();
}

//========================================================================
// 球クラスの更新処理
//========================================================================
void CBall::Update(void)
{
	// ローカル変数
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();			// ジョイパッド入力の取得
	CDebugProc* pDebug = CManager::GetDebugProc();
	D3DXVECTOR3 pos = CBall::GetPosition();
	D3DXVECTOR3 rot = CBall::GetRotation();
	float frotDiff = 0.0f;

	m_posOld = pos;
	m_rotOld = m_rotDest;

	// 状態による管理
	switch (m_state)
	{
	case STATE_NONE:		// 状態無し
		break;

	case STATE_REFLECT:		// 反射状態
		m_nCounterState--;

		if (m_nCounterState < 0)
		{// 一定時間経過
			m_nCounterState = 0;
			m_state = STATE_NORMAL;
		}

		break;
	}

#ifdef _DEBUG
	// 値の微調整
	SubControll();
#endif

	// 回転軸を設定
	m_vecAxis.x = sinf(rot.y - D3DX_PI * 0.5f);
	m_vecAxis.z = cosf(rot.y - D3DX_PI * 0.5f);
	D3DXVec3Normalize(&m_vecAxis, &m_vecAxis);

	// 移動量を計算
	m_move.x = -sinf(rot.y) * MOVEMENT;
	m_move.z = -cosf(rot.y) * MOVEMENT;

	// 位置に移動量を加算
	pos += m_move;

	// 移動量に慣性
	m_move.y += GRAVITY;
	m_fValueRot += (0.0f - m_fValueRot) * SPEED_INERTIA;

	if (pos.y - m_fRadius < 0.0f)
	{// 地面との当たり判定
		pos.y = m_fRadius;

		if (m_nCounter % 10 == 0 && MOVEMENT > 0.0f)
		{// 一定間隔で移動時に
			// 振動
			pInputJoypad->SetVibration(0, 250, 300, 1);
		}
	}

	D3DXVECTOR2 move = D3DXVECTOR2(m_move.x, m_move.z);

	// 移動速度に応じてフィードバックエフェクト
	if (D3DXVec2Length(&move) >= 5.0f)
	{// 速い
		// マルチターゲットレンダリングON
		CManager::GetMTRenderer()->SetMultiRendering(true);
	}
	else
	{// 普通
		// マルチターゲットレンダリングOFF
		CManager::GetMTRenderer()->SetMultiRendering(false);
	}

	// 移動制限
	if (pos.x + m_fRadius > CManager::GetLimit().x)
	{// X上限
		pos.x = CManager::GetLimit().x - m_fRadius;
	}
	else if (pos.x - m_fRadius < -CManager::GetLimit().x)
	{// X下限
		pos.x = -CManager::GetLimit().x + m_fRadius;
	}
	if (pos.z + m_fRadius > CManager::GetLimit().y)
	{// Z上限
		pos.z = CManager::GetLimit().y - m_fRadius;
	}
	else if (pos.z - m_fRadius < -CManager::GetLimit().y)
	{// Z下限
		pos.z = -CManager::GetLimit().y + m_fRadius;
	}

	// 目的の向きを修正
	float fmoveAngle = m_rotDest.y - rot.y;

	// 向きを調整
	CorrectAngle(&m_rotDest.y, fmoveAngle);

	// 角度を慣性ありで加算
	rot.y += (m_rotDest.y - rot.y) * 0.1f;

	// 位置/向きを適用
	SetPosition(pos);
	SetRotation(rot);

	// 影の位置を設定
	if (m_pShadowSt != nullptr)
	{// NULLチェック
		D3DXVECTOR3 scale = m_pShadowSt->GetScale();
		scale.x = m_fRadius / DEFAULT_SIZE;
		scale.y = (m_fRadius / DEFAULT_SIZE) + (float)((int)pos.y % 10);
		scale.z = m_fRadius / DEFAULT_SIZE;

		m_pShadowSt->SetPosition(pos);
		m_pShadowSt->SetScale(scale);
		m_pShadowSt->SetColor(COLOR_WHITE_ALPHA);
	}

	// 軌跡を計算
	D3DXMATRIX mtxWorld = MatrixCalc();

	if (m_pMeshOrbit == nullptr)
	{// 最初は生成
		m_pMeshOrbit = CMeshOrbit::Create(D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f),
			COLOR_WHITE, COLOR_WHITE, &mtxWorld, "data\\TEXTURE\\sea.jpg", CObject::TYPE_MESHORBIT, 5);
	}
	else
	{// 存在するならそれを使う
		m_pMeshOrbit->SetPosition(D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f),
			COLOR_WHITE, COLOR_WHITE, &mtxWorld);
	}

	m_nCounter++;

#ifdef INFORMATIONENABLE
	// デバッグコメント
	pDebug->Print("\n< PLAYER >\n");
	pDebug->Print("状態 : ( %s )\n", (m_state == STATE_NONE) ? "状態無し" : "通常状態");
	pDebug->Print("位置 : ( %f %f %f )\n", pos.x, pos.y, pos.z);
	pDebug->Print("移動ベクトル : ( %f %f %f )\n", m_move.x, m_move.y, m_move.z);
	pDebug->Print("現在の向き : ( %f )\n", m_rotDest.y);
	pDebug->Print("回転軸 : ( %f %f %f )\n", m_vecAxis.x, m_vecAxis.y, m_vecAxis.z);
	pDebug->Print("回転角 : ( %f )\n", m_fValueRot);
	pDebug->Print("移動量 : ( %f )\n", MOVEMENT);
	pDebug->Print("半径 : ( %f )\n", m_fRadius);
	pDebug->Print("回転速度 : ( %f )\n", SPEED);
	pDebug->Print("回転減衰 : ( %f )\n", SPEED_INERTIA);
#endif

#ifdef _DEBUG
	// デバッグライン
	D3DXVECTOR3 front = DEFAULT_VECTOR3;
	front.x = sinf(m_rotDest.y) * -15.0f;
	front.y = 0.0f;
	front.z = cosf(m_rotDest.y) * -15.0f;
	CLine::Create(pos + (m_vecAxis * 20.0f), pos + (-m_vecAxis * 20.0f), COLOR_RED);		// 回転軸
	CLine::Create(pos, pos + front, COLOR_LIMEGREEN);		// 向き

	D3DXVECTOR3 nor = DEFAULT_VECTOR3;
	nor.x = sinf(m_rotDest.y + D3DX_PI * 1.0f);
	nor.z = cosf(m_rotDest.y + D3DX_PI * 1.0f);
	D3DXVec3Normalize(&nor, &nor);
	CLine::Create(pos, pos + (nor * 30.0f), COLOR_BLUE);
#endif
}

//========================================================================
// 球クラスの描画処理
//========================================================================
void CBall::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxWorld;		// 計算用マトリックス
	static bool bShadow = false;

	// 頂点法線正規化をオン
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// マトリックスを計算
	mtxWorld = MatrixCalc();

	// 描画処理
	CObjectX::Draw(mtxWorld);

	// 頂点法線正規化をオフ
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}

//========================================================================
// マトリックスを計算
//========================================================================
D3DXMATRIX CBall::MatrixCalc(void)
{
	D3DXMATRIX mtxWorld, mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス
	D3DXVECTOR3 pos, scale;			// 球の情報
	scale = GetScale();				// スケールを取得
	pos = GetPosition();			// 位置を取得

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, scale.x, scale.y, scale.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScale);

	// 回転軸における指定の回転角からクォータニオンを作成
	D3DXQuaternionRotationAxis(&m_quat, &m_vecAxis, m_fValueRot);

	// クォータニオンから回転マトリックスを作成
	D3DXMatrixRotationQuaternion(&mtxRot, &m_quat);
	D3DXMatrixMultiply(&m_mtxRot, &m_mtxRot, &mtxRot);

	// 向きを反映
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &m_mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	return mtxWorld;
}

//========================================================================
// 状態を設定
//========================================================================
void CBall::SetState(const STATE state)
{
	if (m_state == state)
	{// 同じものは設定しない
		return;
	}

	m_state = state;

	if (m_state == STATE_REFLECT)
	{// 反射状態
		m_nCounterState = REFLECT_INTERVAL;
		SetRotation(m_rotDest);
	}
}

//========================================================================
// 移動制御
//========================================================================
bool CBall::Movement(void)
{
	// ローカル変数
	CCamera* pCamera = CManager::GetCamera();							// カメラの取得
	D3DXVECTOR3 CameraRot = pCamera->GetRotation();
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();			// ジョイパッド入力の取得
	int nValueH, nValueV;
	float fAngle;
	bool bMove = false;

	// パッド移動
	if (pInputJoypad->GetStick(0, CInputJoypad::JOYKEY_LEFTSTICK, &nValueH, &nValueV) == true)
	{// パッドの移動優先
		m_rotDest.y = CameraRot.y + atan2f(-(float)nValueH, -D3DX_PI - (float)nValueV);
		fAngle = atan2f((float)(nValueH), (float)(nValueV));

		m_vecAxis.x = sinf((fAngle + CameraRot.y) + (D3DX_PI * 0.5f));
		m_vecAxis.z = cosf((fAngle + CameraRot.y) + (D3DX_PI * 0.5f));
		D3DXVec3Normalize(&m_vecAxis, &m_vecAxis);

		bMove = true;
	}
	else if (pInputKeyboard->GetPress(DIK_W) == true)		// キーボード移動
	{// 奥に移動
		if (pInputKeyboard->GetPress(DIK_A) == true)
		{// 左奥に移動
			m_rotDest.y = CameraRot.y - (-D3DX_PI * 0.75f);
			m_vecAxis.x = sinf((-D3DX_PI * 0.75f - CameraRot.y) + (-D3DX_PI * 0.5f));
			m_vecAxis.z = cosf((-D3DX_PI * 0.25f + CameraRot.y) + (D3DX_PI * 0.5f));
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true)
		{// 右奥に移動
			m_rotDest.y = CameraRot.y - (D3DX_PI * 0.75f);
			m_vecAxis.x = sinf((D3DX_PI * 0.75f - CameraRot.y) + (-D3DX_PI * 0.5f));
			m_vecAxis.z = cosf((D3DX_PI * 0.25f + CameraRot.y) + (D3DX_PI * 0.5f));
		}
		else if (pInputKeyboard->GetPress(DIK_W) == true)
		{// 奥に移動
			m_rotDest.y = CameraRot.y - D3DX_PI;
			m_vecAxis.x = sinf(CameraRot.y + (D3DX_PI * 0.5f));
			m_vecAxis.z = cosf(CameraRot.y + (D3DX_PI * 0.5f));
		}

		D3DXVec3Normalize(&m_vecAxis, &m_vecAxis);

		bMove = true;
	}
	else if (pInputKeyboard->GetPress(DIK_S) == true)
	{// 手前に移動
		if (pInputKeyboard->GetPress(DIK_A) == true)
		{// 左手前に移動
			m_rotDest.y = CameraRot.y + (D3DX_PI * 0.25f);
			m_vecAxis.x = sinf((-D3DX_PI * 0.25f - CameraRot.y) + (-D3DX_PI * 0.5f));
			m_vecAxis.z = cosf((-D3DX_PI * 0.75f + CameraRot.y) + (D3DX_PI * 0.5f));
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true)
		{// 右手前に移動
			m_rotDest.y = CameraRot.y + (-D3DX_PI * 0.25f);
			m_vecAxis.x = sinf((D3DX_PI * 0.25f - CameraRot.y) + (-D3DX_PI * 0.5f));
			m_vecAxis.z = cosf((D3DX_PI * 0.75f + CameraRot.y) + (D3DX_PI * 0.5f));
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{// 手前に移動
			m_rotDest.y = CameraRot.y;
			m_vecAxis.x = sinf((D3DX_PI * 1.0f + CameraRot.y) + (D3DX_PI * 0.5f));
			m_vecAxis.z = cosf((D3DX_PI * 1.0f + CameraRot.y) + (D3DX_PI * 0.5f));
		}

		D3DXVec3Normalize(&m_vecAxis, &m_vecAxis);

		bMove = true;
	}
	else if (pInputKeyboard->GetPress(DIK_A) == true)
	{// 左に移動
		m_rotDest.y = CameraRot.y + (D3DX_PI * 0.5f);
		m_vecAxis.x = sinf((-D3DX_PI * 0.5f + CameraRot.y) + (D3DX_PI * 0.5f));
		m_vecAxis.z = cosf((-D3DX_PI * 0.5f + CameraRot.y) + (D3DX_PI * 0.5f));

		D3DXVec3Normalize(&m_vecAxis, &m_vecAxis);

		bMove = true;
	}
	else if (pInputKeyboard->GetPress(DIK_D) == true)
	{// 右に移動
		m_rotDest.y = CameraRot.y - (D3DX_PI * 0.5f);
		m_vecAxis.x = sinf((D3DX_PI * 0.5f + CameraRot.y) + (D3DX_PI * 0.5f));
		m_vecAxis.z = cosf((D3DX_PI * 0.5f + CameraRot.y) + (D3DX_PI * 0.5f));

		D3DXVec3Normalize(&m_vecAxis, &m_vecAxis);

		bMove = true;
	}

	// 角度を修正
	CorrectAngle(&m_rotDest.y, m_rotDest.y);

	return bMove;
}

//========================================================================
// 値を調節する処理
//========================================================================
void CBall::SubControll(void)
{
	// ローカル変数
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();			// ジョイパッド入力の取得
	D3DXVECTOR3 scale = CBall::GetScale();

	if (pInputKeyboard->GetTrigger(DIK_SPACE) == true)
	{// ジャンプ
		m_move.y = 10.0f;
	}

	// パーティクルX
	if (pInputKeyboard->GetTrigger(DIK_NUMPAD0) == true)
	{
		CParticleX::Create("data\\MODEL\\earth000.x", D3DXVECTOR3(0.0f, 20.0f, 0.0f), 1, 20,
			D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 2.0f, D3DX_PI * 0.5f, D3DX_PI * 2.0f),
			D3DXVECTOR3(3.0f, 3.0f, 3.0f), 0.97f, CParticleX::TYPE_BOUND, true, 200, 2.0f, true, m_fRadius);
	}
	else if (pInputKeyboard->GetTrigger(DIK_NUMPAD1) == true)
	{
		CParticleX::Create("data\\MODEL\\car000.x", D3DXVECTOR3(0.0f, 20.0f, 0.0f), 1, 20,
			D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 2.0f, D3DX_PI * 0.5f, D3DX_PI * 2.0f),
			D3DXVECTOR3(3.0f, 3.0f, 3.0f), 0.97f, CParticleX::TYPE_BOUND, true, 200, 2.0f, true, m_fRadius);
	}
	else if (pInputKeyboard->GetTrigger(DIK_NUMPAD2) == true)
	{
		CParticleX::Create("data\\MODEL\\balloon_Mickey.x", D3DXVECTOR3(0.0f, 20.0f, 0.0f), 1, 20,
			D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 2.0f, D3DX_PI * 0.5f, D3DX_PI * 2.0f),
			D3DXVECTOR3(3.0f, 3.0f, 3.0f), 0.97f, CParticleX::TYPE_BOUND, true, 200, 2.0f, true, m_fRadius);
	}

	if (pInputKeyboard->GetTrigger(DIK_NUMPAD3) == true)
	{
		CSand::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.1f, 0.0f), 3.0f);
	}

	if (pInputKeyboard->GetTrigger(DIK_NUMPAD9) == true)
	{
		CManager::GetWind()->RandomRot(D3DX_PI * 0.8f);
	}

	if (pInputKeyboard->GetRepeat(DIK_RIGHT) == true)
	{// 拡大
		m_fRadius += 1.0f;

		if (m_fRadius > MAX_SIZE)
		{// 上限
			m_fRadius = MAX_SIZE;
		}

		scale.x = m_fRadius / DEFAULT_SIZE;
		scale.y = m_fRadius / DEFAULT_SIZE;
		scale.z = m_fRadius / DEFAULT_SIZE;

		// スケールを適用
		SetScale(scale);
	}
	else if (pInputKeyboard->GetRepeat(DIK_LEFT) == true)
	{// 縮小
		m_fRadius -= 1.0f;

		if (m_fRadius < MIN_SIZE)
		{// 下限
			m_fRadius = MIN_SIZE;
		}

		scale.x = m_fRadius / DEFAULT_SIZE;
		scale.y = m_fRadius / DEFAULT_SIZE;
		scale.z = m_fRadius / DEFAULT_SIZE;

		// スケールを適用
		SetScale(scale);
	}

	if (pInputKeyboard->GetRepeat(DIK_UP) == true)
	{// 回転速度上げる
		m_fSpeedRot += 0.001f;

		if (m_fSpeedRot > MAX_SPEED)
		{// 上限
			m_fSpeedRot = MAX_SPEED;
		}
	}
	else if (pInputKeyboard->GetRepeat(DIK_DOWN) == true)
	{// 回転速度下げる
		m_fSpeedRot -= 0.001f;

		if (m_fSpeedRot < MIN_SPEED)
		{// 下限
			m_fSpeedRot = MIN_SPEED;
		}
	}

	if (pInputKeyboard->GetRepeat(DIK_Y) == true)
	{// 回転の慣性上げる
		m_fSpeedInertia += 0.01f;

		if (m_fSpeedInertia > MAX_SPEED_INERTIA)
		{// 上限
			m_fSpeedInertia = MAX_SPEED_INERTIA;
		}
	}
	else if (pInputKeyboard->GetRepeat(DIK_H) == true)
	{// 回転の慣性下げる
		m_fSpeedInertia -= 0.01f;

		if (m_fSpeedInertia < MIN_SPEED_INERTIA)
		{// 下限
			m_fSpeedInertia = MIN_SPEED_INERTIA;
		}
	}
}

//========================================================================
// 当たり判定
//========================================================================
CBall* CBall::Collision(const D3DXVECTOR3 pos, const float fRadius, const CObject* pOwn)
{
	for (int nCntPri = 0; nCntPri < PRIORITY_MAX; nCntPri++)
	{
#ifdef LIST
		CObject* pObj = CObject::GetTop(nCntPri);

		while (pObj != nullptr)
		{
			CObject* pObjNext = pObj->GetNext();			// 次のオブジェクトを保存
			CObject::TYPE type;

			// オブジェクトの種類を取得
			type = pObj->GetType();

			if (type == CObject::TYPE_BALL)
			{// 球オブジェクトなら当たり判定する
				if (pOwn == pObj)
				{// 自分自身は判定しない
					pObj = pObjNext;			// 次のオブジェクトを代入

					continue;
				}

				D3DXVECTOR3 posBall, dist;

				// 球の位置を取得
				posBall = dynamic_cast<CBall*>(pObj)->GetPosition();

				// 距離を計算
				dist = pos - posBall;

				if ((D3DXVec3Length(&dist) < dynamic_cast<CBall*>(pObj)->GetRadius() + fRadius))
				{// 球と重なった
					return dynamic_cast<CBall*>(pObj);
				}
			}

			pObj = pObjNext;			// 次のオブジェクトを代入
		}

#else
		for (int nCntObj = 0; nCntObj < MAX_OBJECT; nCntObj++)
		{
			CObject* pObj;

			// オブジェクトを取得
			pObj = GetObject(nCntPri, nCntObj);

			if (pObj != nullptr)
			{// NULLチェック
				CObject::TYPE type;

				// オブジェクトの種類を取得
				type = pObj->GetType();

				if (type == CObject::TYPE_BALL)
				{// 球オブジェクトなら当たり判定する
					D3DXVECTOR3 posBalloon, dist;

					// 球の位置を取得
					posBalloon = dynamic_cast<CBall*>(pObj)->GetPosition();

					// 距離を計算
					dist = pos - posBalloon;

					if ((D3DXVec3Length(&dist) < dynamic_cast<CBall*>(pObj)->GetRadius() + fRadius))
					{// 球と重なった
						if (bOwn == true)
						{// 球どうしで当たり判定しているとき
							return dynamic_cast<CBall*>(pObj);
						}

						pColl = dynamic_cast<CBall*>(pObj);
				}
			}
		}
	}

#endif
}

	return nullptr;
}