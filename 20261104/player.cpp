//========================================================================
// 
// プレイヤー [	player.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "player.h"

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
#define JUMP_HEIGHT				(m_fJumpHeight)							// ジャンプの高さ
#define BOUND					(-0.7f)									// 跳ね返り
#define ANTI_BOUND				(2.5f)									// 跳ね返りを消す値
#define DEFAULT_SIZE			(10.0f)									// 規定のモデルサイズ
#define MAX_SIZE				(50.0f)									// 最大サイズ
#define MIN_SIZE				(5.0f)									// 最小サイズ
#define REFLECT_INTERVAL		(30)									// 反射後の移動インターバル

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************
int CPlayer::m_nIdxModel = {};			// モデルのインデックス

//========================================================================
// モデルのロード処理
//========================================================================
HRESULT CPlayer::Load(const char* pFilename)
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
void CPlayer::Unload(void)
{
	// モデルのインデックスを削除
	memset(&m_nIdxModel, -1, sizeof m_nIdxModel);
}

//========================================================================
// プレイヤークラスの生成処理
//========================================================================
CPlayer* CPlayer::Create(const D3DXVECTOR3 pos, const float fRadius, const float fJump,
	const float fSpeed, const float fSpeedInertia)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CPlayer* pPlayer = nullptr;

	if (pPlayer == nullptr)
	{// NULLチェック
		// プレイヤーの生成
		pPlayer = new CPlayer;
	}

	if (pPlayer != nullptr)
	{// NULLチェック
		// モデルファイル設定
		pPlayer->BindModel(m_nIdxModel);

		// 初期化処理
		if (FAILED(pPlayer->Init(pos, fRadius, fJump, fSpeed, fSpeedInertia)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! プレイヤーの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// 種類を設定
		pPlayer->SetType(TYPE_PLAYER);

		return pPlayer;
	}

	OutputDebugStringA("! ! ! プレイヤーの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// プレイヤークラスのコンストラクタ
//========================================================================
CPlayer::CPlayer(const int nPriority) :CObjectX(nPriority)
{
	// プレイヤークラスの値をクリア
	m_posOld = DEFAULT_VECTOR3;
	m_move = DEFAULT_VECTOR3;
	m_rotDest = DEFAULT_VECTOR3;
	m_rotOld = DEFAULT_VECTOR3;
	m_bJump = false;
	m_fJumpHeight = 0.0f;
	m_bCharge = false;
	m_fSpinAngle = 0.0f;
	m_state = STATE_NORMAL;
	m_nCounterState = 0;
	m_pMeshOrbit = nullptr;
	D3DXMatrixIdentity(&m_mtxRot);
	D3DXQuaternionIdentity(&m_quat);
	m_vecAxis = DEFAULT_VECTOR3;
	m_fValueRot = 0.0f;
	m_fSpeedRot = 0.0f;
	m_fSpeedInertia = 0.0f;
	m_fRadius = 0.0f;
	m_pWindLeft = nullptr;
	m_pWindRight = nullptr;
	m_nCounter = 0;
}

//========================================================================
// プレイヤークラスのデストラクタ
//========================================================================
CPlayer::~CPlayer()
{
}

//========================================================================
// プレイヤークラスの初期化処理
//========================================================================
HRESULT CPlayer::Init(const D3DXVECTOR3 pos, const float fRadius, const float fJump,
	const float fSpeed, const float fSpeedInertia)
{
	// 初期化処理
	CObjectX::Init(pos);

	// プレイヤーの初期化
	m_posOld = DEFAULT_VECTOR3;
	m_move = DEFAULT_VECTOR3;
	m_rotDest = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);
	m_rotOld = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_bJump = false;
	m_fJumpHeight = fJump;
	m_bCharge = false;
	m_fSpinAngle = 0.0f;
	m_state = STATE_NORMAL;
	m_nCounterState = 0;
	m_vecAxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_fValueRot = 0.0f;
	m_fSpeedRot = fSpeed;
	m_fSpeedInertia = fSpeedInertia;
	m_fRadius = fRadius;
	m_nCounter = 0;

	// 風の生成
	if (m_pWindLeft == nullptr)
	{// NULLチェック
		m_pWindLeft = CWind::Create(D3DXVECTOR3(pos.x - 200.0f, pos.y, pos.z), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 3.0f, 0.4f);
	}

	if (m_pWindRight == nullptr)
	{// NULLチェック
		m_pWindRight = CWind::Create(D3DXVECTOR3(pos.x + 200.0f, pos.y, pos.z), D3DXVECTOR3(0.0f, 0.0f, -1.0f), 3.0f, 0.4f);
	}

	D3DXVECTOR3 scale = CPlayer::GetScale();
	scale.x = m_fRadius / DEFAULT_SIZE;
	scale.y = m_fRadius / DEFAULT_SIZE;
	scale.z = m_fRadius / DEFAULT_SIZE;
	SetScale(scale);
	SetRotation(D3DXVECTOR3(0.0f, D3DX_PI, 0.0f));

	// カメラの追従対象を設定
	CManager::GetCamera()->SetFollow(this);

	return S_OK;
}

//========================================================================
// プレイヤークラスの終了処理
//========================================================================
void CPlayer::Uninit(void)
{
	// 終了処理
	CObjectX::Uninit();
}

//========================================================================
// プレイヤークラスの更新処理
//========================================================================
void CPlayer::Update(void)
{
	// ローカル変数
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();			// ジョイパッド入力の取得
	CDebugProc* pDebug = CManager::GetDebugProc();
	D3DXVECTOR3 pos = CPlayer::GetPosition();
	D3DXVECTOR3 rot = CPlayer::GetRotation();
	float frotDiff = 0.0f;

	m_posOld = pos;
	m_rotOld = m_rotDest;

	// 状態による管理
	switch (m_state)
	{
	case STATE_NONE:		// 状態無し
		break;
	}

#ifdef _DEBUG
	// 値の微調整
	SubControll();
#endif

	// 風の移動
	m_pWindLeft->StickControll(CInputJoypad::JOYKEY_LEFTSTICK);
	m_pWindRight->StickControll(CInputJoypad::JOYKEY_RIGHTSTICK);

	if (m_bJump == false && m_bCharge == false)
	{// ジャンプしていないとき
		if (pInputKeyboard->GetTrigger(DIK_SPACE) == true || pInputJoypad->GetTrigger(0, CInputJoypad::JOYKEY_A) == true)
		{// ジャンプ
			m_bJump = true;
			m_move.y = m_fJumpHeight;

			// パーティクルの法線
			D3DXVECTOR3 nor;
			nor.x = sinf(m_rotDest.y);
			nor.y = 1.0f;
			nor.z = cosf(m_rotDest.y);
			D3DXVec3Normalize(&nor, &nor);

			// 煙パーティクル
			CParticleX::Create("data\\MODEL\\smoke.x", D3DXVECTOR3(pos.x, 0.0f, pos.z), 1, 20,
				nor, D3DXVECTOR3(D3DX_PI * 0.4f, D3DX_PI * 0.5f, D3DX_PI * 0.4f),
				D3DXVECTOR3(3.0f, 3.0f, 3.0f), 0.97f, CParticleX::TYPE_BOUND, true, 200, 2.0f, true, m_fRadius);
		}
	}

	// 回転軸を設定
	m_vecAxis.x = sinf(rot.y - D3DX_PI * 0.5f);
	m_vecAxis.z = cosf(rot.y - D3DX_PI * 0.5f);
	D3DXVec3Normalize(&m_vecAxis, &m_vecAxis);

	// 移動量を計算
	m_move.x = -sinf(rot.y) * MOVEMENT;
	m_move.z = -cosf(rot.y) * MOVEMENT;

	if (m_bCharge != true)
	{// チャージしていないとき
		// 位置に移動量を加算
		pos += m_move;

		// 移動量に慣性
		m_move.y += GRAVITY;
		m_fValueRot += (0.0f - m_fValueRot) * SPEED_INERTIA;
	}

	if (pos.y - m_fRadius < 0.0f)
	{// 地面との当たり判定
		if (m_bJump == true)
		{// 着地
			m_move.y *= BOUND;

			// パーティクルの法線
			D3DXVECTOR3 nor;
			nor.x = sinf(m_rotDest.y);
			nor.y = 1.0f;
			nor.z = cosf(m_rotDest.y);
			D3DXVec3Normalize(&nor, &nor);

			// 煙パーティクル
			CParticleX::Create("data\\MODEL\\smoke.x", D3DXVECTOR3(pos.x, 0.0f, pos.z), 1, 20,
				nor, D3DXVECTOR3(D3DX_PI * 0.4f, D3DX_PI * 0.5f, D3DX_PI * 0.4f),
				D3DXVECTOR3(3.0f, 3.0f, 3.0f), 0.97f, CParticleX::TYPE_BOUND, true, 200, 2.0f, true, m_fRadius);

			// 振動
			pInputJoypad->SetVibration(0, 6000, 10000, 3);

			// 音
			CManager::GetSound()->PlaySound(CSound::SE_LAND);
		}

		pos.y = m_fRadius;

		if (m_nCounter % 10 == 0 && MOVEMENT > 0.0f)
		{// 一定間隔で移動時に
			// 振動
			pInputJoypad->SetVibration(0, 250, 300, 1);
		}

		if (m_move.y > -ANTI_BOUND && m_move.y < ANTI_BOUND && m_bJump == true)
		{// バウンドしなくなった
			m_bJump = false;
			m_move.y = 0.0f;

			// パーティクルの法線
			D3DXVECTOR3 nor;
			nor.x = sinf(m_rotDest.y);
			nor.y = 1.0f;
			nor.z = cosf(m_rotDest.y);
			D3DXVec3Normalize(&nor, &nor);

			// 煙パーティクル
			CParticleX::Create("data\\MODEL\\smoke.x", D3DXVECTOR3(pos.x, 0.0f, pos.z), 1, 20,
				nor, D3DXVECTOR3(D3DX_PI * 0.4f, D3DX_PI * 0.5f, D3DX_PI * 0.4f),
				D3DXVECTOR3(3.0f, 3.0f, 3.0f), 0.97f, CParticleX::TYPE_BOUND, true, 200, 2.0f, true, m_fRadius);

			// 振動
			pInputJoypad->SetVibration(0, 6000, 10000, 3);

			// 音
			CManager::GetSound()->PlaySound(CSound::SE_LAND);
		}
	}
	else if (pos.y - m_fRadius > 0.0f)
	{// 空中にいる
		m_bJump = true;
	}

	// 移動速度に応じてフィードバックエフェクト
	if (D3DXVec3Length(&m_move) >= 5.0f && m_bJump == true)
	{// 速い
		// マルチターゲットレンダリングON
		CManager::GetMTRenderer()->SetMultiRendering(true);
	}
	else
	{// 普通
		// マルチターゲットレンダリングOFF
		CManager::GetMTRenderer()->SetMultiRendering(false);
	}

	// メッシュウォールとの当たり判定
	CWallMM::CollisionAll(&pos, &m_posOld, m_fRadius);

	// 風の影響
	CManager::GetWind()->WindAffect(&pos, &m_vecAxis, &m_rotDest, &m_fValueRot);

	// 左右の風の影響
	m_pWindLeft->WindAffect(&pos, &m_vecAxis, &m_rotDest, &m_fValueRot);
	m_pWindRight->WindAffect(&pos, &m_vecAxis, &m_rotDest, &m_fValueRot);

	// 風の自動移動
	CManager::GetWind()->AutoMove(pos - m_posOld);
	m_pWindLeft->AutoMove(pos - m_posOld);
	m_pWindRight->AutoMove(pos - m_posOld);

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

	// デバッグコメント
	pDebug->Print("\n< PLAYER >\n");
	pDebug->Print("タブ切り替え [ 0 ]\n");

	pDebug->Print("状態 : ( %s )\n", (m_state == STATE_NONE) ? "状態無し" : "通常状態");
	pDebug->Print("位置 : ( %f %f %f )\n", pos.x, pos.y, pos.z);
	pDebug->Print("移動ベクトル : ( %f %f %f )\n", m_move.x, m_move.y, m_move.z);
	pDebug->Print("現在の向き : ( %f )\n", m_rotDest.y);
	pDebug->Print("前回の向き : ( %f )\n", m_rotOld.y);
	pDebug->Print("回転軸 : ( %f %f %f )\n", m_vecAxis.x, m_vecAxis.y, m_vecAxis.z);
	pDebug->Print("回転角 : ( %f )\n", m_fValueRot);
	pDebug->Print("移動量 : ( %f )\n", MOVEMENT);
	pDebug->Print("半径 : ( %f )\n", m_fRadius);
	pDebug->Print("回転速度 : ( %f )\n", SPEED);
	pDebug->Print("回転減衰 : ( %f )\n", SPEED_INERTIA);

#ifdef _DEBUG
	// デバッグライン
	D3DXVECTOR3 front = DEFAULT_VECTOR3;
	front.x = sinf(m_rotDest.y) * -15.0f;
	front.y = 0.0f;
	front.z = cosf(m_rotDest.y) * -15.0f;
	CLine::Create(pos + (m_vecAxis * 20.0f), pos + (-m_vecAxis * 20.0f), COLOR_RED);
	CLine::Create(pos, pos + front, COLOR_LIMEGREEN);
#endif
}

//========================================================================
// プレイヤークラスの描画処理
//========================================================================
void CPlayer::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxWorld;		// 計算用マトリックス

	// 頂点法線正規化をオン
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// マトリックスを計算
	mtxWorld = MatrixCalc();

	// 描画処理
	CObjectX::Draw(mtxWorld);

	// 描画処理
	CObjectX::DrawShadow(mtxWorld, DEFAULT_VECTOR3, D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	// 頂点法線正規化をオフ
	pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
}

//========================================================================
// マトリックスを計算
//========================================================================
D3DXMATRIX CPlayer::MatrixCalc(void)
{
	D3DXMATRIX mtxWorld, mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス
	D3DXVECTOR3 pos, scale;			// プレイヤーの情報
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
// 移動制御
//========================================================================
bool CPlayer::Movement(void)
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
void CPlayer::SubControll(void)
{
	// ローカル変数
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();			// ジョイパッド入力の取得
	D3DXVECTOR3 scale = CPlayer::GetScale();

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
		CSand::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.1f, 0.0f));
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