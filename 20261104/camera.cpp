//=============================================================================
// 
// カメラ [ camera.cpp ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#include "camera.h"

#include "renderer.h"
#include "manager.h"
#include "debugproc.h"
#include "input.h"

#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENABLE_PERSPECTIVFOV				// 透視投影で描画
//#undef ENABLE_PERSPECTIVFOV

#define ENABLE_AUTOROTATION					// 回り込み
#undef ENABLE_AUTOROTATION

#define ENABLE_ORTHOMOVE					// カメラの平行移動
#undef ENABLE_ORTHOMOVE

#define CAMERA_POS				(D3DXVECTOR3(0.0f, 300.0f, -500.0f))	// 初期座標
#define FIRST_ROT_X				(200.0f)								// 視点と注視点間の距離
#define HEIGHT					(10.0f)									// 視点の高さ
#define MOVEMENT				(D3DXVECTOR3(5.0f, 5.0f, 5.0f))			// 移動量
#define MAUSE_MOVEV				(0.005f)								// マウス使用時の視点移動量
#define MAUSE_MOVER				(0.003f)								// マウス使用時の注視点移動量
#define STICK_MOVE				(0.000002f)								// スティック使用時の移動量
#define ROT						(D3DXVECTOR3(0.035f, 0.035f, 0.035f))	// 向き移動量
#define AUTO_ROT				(D3DXVECTOR3(0.005f, 0.005f, 0.005f))	// 自動回転移動量
#define AUTOROTATE_COUNT		(180)									// 回り込みまでのカウント
#define DEFAULT_VIEW_ANGLE		(45.0f)									// 規定の視野角
#define INERTIA_VIEW_ANGLE		(0.05f)									// 視野角の慣性
#define INERTIA_POSR			(0.3f)									// 注視点の慣性
#define INERTIA_POSV			(0.3f)									// 視点の慣性
#define INERTIA_ROT				(0.02f)									// 回り込みの慣性
#define MAX_Y					(300.0f)								// 上の制限
#define MIN_Y					(-300.0f)								// 下の制限
#define MIN_DISTANCE			(20.0f)									// カメラの距離の最低値
#define MOVE_DIST				(10.0f)									// 距離の移動量
#define MIN_VIEW_DIST			(10.0f)									// ビューボリュームの近平面までの距離
#define MAX_VIEW_DIST			(80000.0f)								// ビューボリュームの遠平面までの距離

//=============================================================================
// カメラクラスのコンストラクタ
//=============================================================================
CCamera::CCamera()
{
	m_posV = DEFAULT_VECTOR3;
	m_posR = DEFAULT_VECTOR3;
	m_posVDest = DEFAULT_VECTOR3;
	m_posRDest = DEFAULT_VECTOR3;
	m_posVOff = DEFAULT_VECTOR3;
	m_vecU = DEFAULT_VECTOR3;
	m_rot = DEFAULT_VECTOR3;
	m_fAngle = 0.0f;
	m_fDistance = 0.0f;
	m_fRDistance = 0.0f;
	m_fViewAngle = 0.0f;
	m_fMoveVA = 0.0f;
	m_type = TYPE_STOP;
	m_pFollow = nullptr;
	memset(&m_aPlane[0], 0, sizeof m_aPlane);
	m_bShake = false;
}

//=============================================================================
// カメラクラスのデストラクタ
//=============================================================================
CCamera::~CCamera()
{
}

//=============================================================================
// カメラの初期化処理
//=============================================================================
HRESULT CCamera::Init(void)
{
	// カメラ情報の初期化
	m_posV = CAMERA_POS;
	m_posR = DEFAULT_VECTOR3;
	m_posVDest = CAMERA_POS;
	m_posRDest = DEFAULT_VECTOR3;
	m_posVOff = DEFAULT_VECTOR3;
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_rot = DEFAULT_VECTOR3;

	D3DXVECTOR3 dist = m_posV - m_posR;
	D3DXVECTOR2 distXZ = D3DXVECTOR2(dist.x, dist.z);
	float fDistXZ = D3DXVec2Length(&distXZ);

	m_fAngle = atan2f(dist.y, fDistXZ);
	m_fDistance = D3DXVec3Length(&dist);
	m_fRDistance = 0.0f;
	m_fViewAngle = DEFAULT_VIEW_ANGLE;
	m_fMoveVA = 0.0f;
	m_type = TYPE_FOLLOW;
	m_pFollow = nullptr;
	memset(&m_aPlane[0], 0, sizeof m_aPlane);
	m_bShake = false;

	m_viewport.X = 0;
	m_viewport.Y = 0;
	m_viewport.Width = SCREEN_WIDTH;
	m_viewport.Height = SCREEN_HEIGHT;
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 1.0f;

	return S_OK;
}

//=============================================================================
// カメラの終了処理
//=============================================================================
void CCamera::Uninit(void)
{
}

//=============================================================================
// カメラの更新処理
//=============================================================================
void CCamera::Update(void)
{
	// ローカル変数
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CInputMouse* pInputMouse = CManager::GetInputMouse();				// マウス入力の取得
	CInputJoypad* pInputJoypad = CManager::GetInputJoypad();			// ジョイパッド入力の取得
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	CPlayer* pPlayer;							// プレイヤー取得
	D3DXVECTOR3 MousePos;
	static D3DXVECTOR3 ClickPos, CameraRot;		// クリックされた位置、クリックされた瞬間のカメラの向き
	static float fCameraAngle;					// クリックされた瞬間のカメラの上下角度
	static int nAutoCameraCounter = 0;			// カメラの回り込みまでのカウンター
	D3DXVECTOR3 pos;
	D3DXVECTOR3 rot;

	// マウス移動用
	MousePos.x = (float)pInputMouse->GetPosition().x;
	MousePos.y = (float)pInputMouse->GetPosition().y;
	MousePos.z = 0.0f;

	if (pInputMouse->GetTrigger(CInputMouse::MOUSEBUTTON_LEFT) == true ||
		pInputMouse->GetTrigger(CInputMouse::MOUSEBUTTON_RIGHT) == true)
	{// クリックされた瞬間
		// クリックされた瞬間の情報を保存
		ClickPos = MousePos;
		CameraRot = m_rot;
		fCameraAngle = m_fAngle;
	}

	if (pInputKeyboard->GetTrigger(DIK_F5) == true || pInputJoypad->GetTrigger(0, CInputJoypad::JOYKEY_LEFT_THUMB) == true)
	{// 視点切り替え
		m_type = (m_type == TYPE_FOLLOW) ? TYPE_SOLO : TYPE_FOLLOW;
	}

	// それぞれのカメラの処理
	switch (m_type)
	{
	case TYPE_PLAYER:
		pPlayer = CManager::GetPlayer();			// プレイヤー取得
		pos = pPlayer->GetPosition();
		rot = pPlayer->GetRotation();

		int nValueH, nValueV;
		nAutoCameraCounter++;

		// プレイヤー向きを調整
		CorrectAngle(&rot.y, ((rot.y - D3DX_PI) - m_rot.y));

		// 目的の注視点を設定
		m_posRDest.x = pos.x + sinf(D3DX_PI + rot.y) * m_fRDistance;
		m_posRDest.y = pos.y + HEIGHT;
		m_posRDest.z = pos.z + cosf(D3DX_PI + rot.y) * m_fRDistance;

		// 目的の視点を設定
		m_posVDest.x = m_posRDest.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		m_posVDest.y = m_posRDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * m_fDistance;
		m_posVDest.z = m_posRDest.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);

		// 視点移動(キーボード)
		if (pInputKeyboard->GetPress(DIK_J) == true)
		{// 右に旋回
			m_rot.y += -ROT.y;

			m_posVDest.x = m_posRDest.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.y = m_posRDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * m_fDistance;
			m_posVDest.z = m_posRDest.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);

			// 操作されたらカウンターをリセット
			nAutoCameraCounter = 0;
		}
		else if (pInputKeyboard->GetPress(DIK_L) == true)
		{// 左に旋回
			m_rot.y += ROT.y;

			m_posVDest.x = m_posRDest.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.y = m_posRDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * m_fDistance;
			m_posVDest.z = m_posRDest.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);

			// 操作されたらカウンターをリセット
			nAutoCameraCounter = 0;
		}

		if (pInputKeyboard->GetPress(DIK_K) == true)
		{// 上に移動
			m_fAngle += ROT.y;

			if (m_fAngle > D3DX_PI * 0.4f)
			{// 上の制限
				m_fAngle = D3DX_PI * 0.4f;
			}

			CorrectAngle(&m_fAngle, m_fAngle);

			m_posVDest.x = m_posRDest.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.y = m_posRDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * m_fDistance;
			m_posVDest.z = m_posRDest.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);

			// 操作されたらカウンターをリセット
			nAutoCameraCounter = 0;
		}
		else if (pInputKeyboard->GetPress(DIK_I) == true)
		{// 下に移動
			m_fAngle += -ROT.y;

			if (m_fAngle < -D3DX_PI * 0.0f)
			{// 下の制限
				m_fAngle = -D3DX_PI * 0.0f;
			}

			CorrectAngle(&m_fAngle, m_fAngle);

			m_posVDest.x = m_posRDest.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.y = m_posRDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * m_fDistance;
			m_posVDest.z = m_posRDest.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);

			// 操作されたらカウンターをリセット
			nAutoCameraCounter = 0;
		}

		// 視点移動(マウス)
		if (pInputMouse->GetPress(CInputMouse::MOUSEBUTTON_LEFT) == true &&
			pInputMouse->GetPress(CInputMouse::MOUSEBUTTON_RIGHT) == false)
		{// クリックされている間
			// 最初にクリックされた位置から今の位置までの距離を求める
			D3DXVECTOR3 dist = MousePos - ClickPos;

			// 視点移動
			m_rot.y = CameraRot.y + dist.x * MAUSE_MOVEV;
			m_fAngle = fCameraAngle + dist.y * MAUSE_MOVEV;

			if (m_fAngle > D3DX_PI * 0.4f)
			{// 上の制限
				m_fAngle = D3DX_PI * 0.4f;
			}

			if (m_fAngle < -D3DX_PI * 0.0f)
			{// 下の制限
				m_fAngle = -D3DX_PI * 0.0f;
			}

			m_posVDest.x = m_posRDest.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.y = m_posRDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * m_fDistance;
			m_posVDest.z = m_posRDest.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);

			// 操作されたらカウンターをリセット
			nAutoCameraCounter = 0;
		}

		// 視点移動(ジョイパッド)
		if (pInputJoypad->GetStick(0, CInputJoypad::JOYKEY_RIGHTSTICK, &nValueH, &nValueV) == true)
		{// クリックされている間
			// 最初にクリックされた位置から今の位置までの距離を求める
			D3DXVECTOR3 dist = MousePos - ClickPos;

			// 視点移動
			m_rot.y += (float)(nValueH)*STICK_MOVE;
			m_fAngle -= (float)(nValueV)*STICK_MOVE;

			if (m_fAngle > D3DX_PI * 0.4f)
			{// 上の制限
				m_fAngle = D3DX_PI * 0.4f;
			}

			if (m_fAngle < -D3DX_PI * 0.0f)
			{// 下の制限
				m_fAngle = -D3DX_PI * 0.0f;
			}

			m_posVDest.x = m_posRDest.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.y = m_posRDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * m_fDistance;
			m_posVDest.z = m_posRDest.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);

			// 操作されたらカウンターをリセット
			nAutoCameraCounter = 0;
		}

		// カメラのズーム
		if (pInputMouse->GetWheel() != 0)
		{// ホイールが回転したとき
			m_fDistance += (float)(pInputMouse->GetWheel() / ONE_TICK) * MOVE_DIST;

			if (m_fDistance <= MIN_DISTANCE)
			{// 最低値
				m_fDistance = MIN_DISTANCE;
			}

			// ホイールの値をリセット
			pInputMouse->ResetWheel();
		}

#ifdef ENABLE_AUTOROTATION
		if (nAutoCameraCounter > AUTOROTATE_COUNT)
		{// プレイヤーの入力がない場合回り込む
			m_rot.y += ((rot.y - D3DX_PI) - m_rot.y) * INERTIA_ROT;
		}
#endif

		// カメラ向きを調整
		CorrectAngle(&m_rot.y, m_rot.y);

		// 注視点の補正
		m_posR += (m_posRDest - m_posR) * INERTIA_POSR;

		// 視点の補正
		m_posV += m_posVOff + (m_posVDest - m_posV) * INERTIA_POSV;

		break;

	case TYPE_FOLLOW:
		if (m_pFollow == nullptr)
		{// NULLチェック
			m_type = TYPE_SOLO;

			break;
		}

		pos = m_pFollow->GetPosition();
		rot = m_pFollow->GetRotation();

		nAutoCameraCounter++;

		// プレイヤー向きを調整
		CorrectAngle(&rot.y, ((rot.y - D3DX_PI) - m_rot.y));

		// 目的の注視点を設定
		m_posRDest.x = pos.x + sinf(D3DX_PI + rot.y) * m_fRDistance;
		m_posRDest.y = pos.y + HEIGHT;
		m_posRDest.z = pos.z + cosf(D3DX_PI + rot.y) * m_fRDistance;

		// 目的の視点を設定
		m_posVDest.x = m_posRDest.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		m_posVDest.y = m_posRDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * m_fDistance;
		m_posVDest.z = m_posRDest.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);

		// カメラのズーム
		if (pInputMouse->GetWheel() != 0)
		{// ホイールが回転したとき
			m_fDistance += (float)(pInputMouse->GetWheel() / ONE_TICK) * MOVE_DIST;

			if (m_fDistance <= MIN_DISTANCE)
			{// 最低値
				m_fDistance = MIN_DISTANCE;
			}

			// ホイールの値をリセット
			pInputMouse->ResetWheel();
		}

		// カメラ向きを調整
		CorrectAngle(&m_rot.y, m_rot.y);

		// 注視点の補正
		m_posR += (m_posRDest - m_posR) * INERTIA_POSR;

		// 視点の補正
		m_posV += m_posVOff + (m_posVDest - m_posV) * INERTIA_POSV;

		break;

	case TYPE_SOLO:			// 単体カメラ
		// 視点移動
		if (pInputMouse->GetPress(CInputMouse::MOUSEBUTTON_LEFT) == true &&
			pInputMouse->GetPress(CInputMouse::MOUSEBUTTON_RIGHT) == false)
		{// クリックされている間
			// 最初にクリックされた位置から今の位置までの距離を求める
			D3DXVECTOR3 dist = MousePos - ClickPos;

			// 視点移動
			m_rot.y = CameraRot.y + dist.x * MAUSE_MOVEV;
			m_fAngle = fCameraAngle + dist.y * MAUSE_MOVEV;

			if (m_fAngle > D3DX_PI * 0.4f)
			{// 上の制限
				m_fAngle = D3DX_PI * 0.4f;
			}

			if (m_fAngle < -D3DX_PI * 0.4f)
			{// 下の制限
				m_fAngle = -D3DX_PI * 0.4f;
			}

			m_posVDest.x = m_posRDest.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.y = m_posRDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * m_fDistance;
			m_posVDest.z = m_posRDest.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}

		// 注視点移動
		if (pInputMouse->GetPress(CInputMouse::MOUSEBUTTON_RIGHT) == true &&
			pInputMouse->GetPress(CInputMouse::MOUSEBUTTON_LEFT) == false)
		{// クリックされている間
			// 最初にクリックされた位置から今の位置までの距離を求める
			D3DXVECTOR3 dist = MousePos - ClickPos;

			// 注視点移動
			m_rot.y = CameraRot.y + dist.x * MAUSE_MOVER;
			m_fAngle = fCameraAngle + dist.y * MAUSE_MOVER;

			if (m_fAngle > D3DX_PI * 0.4f)
			{// 上の制限
				m_fAngle = D3DX_PI * 0.4f;
			}

			if (m_fAngle < -D3DX_PI * 0.4f)
			{// 下の制限
				m_fAngle = -D3DX_PI * 0.4f;
			}

			m_posRDest.x = m_posVDest.x + sinf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posRDest.y = m_posVDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * -m_fDistance;
			m_posRDest.z = m_posVDest.z + cosf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}

#ifdef ENABLE_ORTHOMOVE
		// 並行移動
		if (pInputKeyboard->GetPress(DIK_UP) == true)		// キーボード移動
		{// 奥に移動
			if (pInputKeyboard->GetPress(DIK_LEFT) == true)
			{// 左奥に移動
				m_posVDest.x += sinf(-D3DX_PI * 0.75f - m_rot.y) * MOVEMENT.x * sinf((D3DX_PI * 0.5f) - m_fAngle);
				m_posVDest.z += cosf(-D3DX_PI * 0.25f + m_rot.y) * MOVEMENT.z * sinf((D3DX_PI * 0.5f) - m_fAngle);
			}
			else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
			{// 右奥に移動
				m_posVDest.x += sinf(D3DX_PI * 0.75f - m_rot.y) * MOVEMENT.x * sinf((D3DX_PI * 0.5f) - m_fAngle);
				m_posVDest.z += cosf(D3DX_PI * 0.25f + m_rot.y) * MOVEMENT.z * sinf((D3DX_PI * 0.5f) - m_fAngle);
			}
			else if (pInputKeyboard->GetPress(DIK_UP) == true)
			{// 奥に移動
				m_posVDest.x += sinf(D3DX_PI * 0.0f + m_rot.y) * MOVEMENT.x * sinf((D3DX_PI * 0.5f) - m_fAngle);
				m_posVDest.z += cosf(D3DX_PI * 0.0f + m_rot.y) * MOVEMENT.z * sinf((D3DX_PI * 0.5f) - m_fAngle);
			}

			m_posRDest.x = m_posVDest.x + sinf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posRDest.z = m_posVDest.z + cosf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}
		else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
		{// 手前に移動
			if (pInputKeyboard->GetPress(DIK_LEFT) == true)
			{// 左手前に移動
				m_posVDest.x += sinf(-D3DX_PI * 0.25f - m_rot.y) * MOVEMENT.x * sinf((D3DX_PI * 0.5f) - m_fAngle);
				m_posVDest.z += cosf(-D3DX_PI * 0.75f + m_rot.y) * MOVEMENT.z * sinf((D3DX_PI * 0.5f) - m_fAngle);
			}
			else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
			{// 右手前に移動
				m_posVDest.x += sinf(D3DX_PI * 0.25f - m_rot.y) * MOVEMENT.x * sinf((D3DX_PI * 0.5f) - m_fAngle);
				m_posVDest.z += cosf(D3DX_PI * 0.75f + m_rot.y) * MOVEMENT.z * sinf((D3DX_PI * 0.5f) - m_fAngle);
			}
			else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
			{// 手前に移動
				m_posVDest.x += sinf(D3DX_PI * 1.0f + m_rot.y) * MOVEMENT.x * sinf((D3DX_PI * 0.5f) - m_fAngle);
				m_posVDest.z += cosf(D3DX_PI * 1.0f + m_rot.y) * MOVEMENT.z * sinf((D3DX_PI * 0.5f) - m_fAngle);
			}

			m_posRDest.x = m_posVDest.x + sinf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posRDest.z = m_posVDest.z + cosf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}
		else if (pInputKeyboard->GetPress(DIK_LEFT) == true)
		{// 左に移動
			m_posVDest.x += sinf(-D3DX_PI * 0.5f + m_rot.y) * MOVEMENT.x * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.z += cosf(-D3DX_PI * 0.5f + m_rot.y) * MOVEMENT.z * sinf((D3DX_PI * 0.5f) - m_fAngle);

			m_posRDest.x = m_posVDest.x + sinf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posRDest.z = m_posVDest.z + cosf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}
		else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
		{// 右に移動
			m_posVDest.x += sinf(D3DX_PI * 0.5f + m_rot.y) * MOVEMENT.x * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.z += cosf(D3DX_PI * 0.5f + m_rot.y) * MOVEMENT.z * sinf((D3DX_PI * 0.5f) - m_fAngle);

			m_posRDest.x = m_posVDest.x + sinf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posRDest.z = m_posVDest.z + cosf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}
#endif

		// 注視点移動
		if (pInputKeyboard->GetPress(DIK_T) == true)
		{// 上に移動
			m_fAngle += ROT.y;

			if (m_fAngle > D3DX_PI * 0.4f)
			{// 上の制限
				m_fAngle = D3DX_PI * 0.4f;
			}

			CorrectAngle(&m_fAngle, m_fAngle);

			m_posRDest.x = m_posVDest.x + sinf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posRDest.y = m_posVDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * -m_fDistance;
			m_posRDest.z = m_posVDest.z + cosf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}
		else if (pInputKeyboard->GetPress(DIK_B) == true)
		{// 下に移動
			m_fAngle += -ROT.y;

			if (m_fAngle < -D3DX_PI * 0.4f)
			{// 下の制限
				m_fAngle = -D3DX_PI * 0.4f;
			}

			CorrectAngle(&m_fAngle, m_fAngle);

			m_posRDest.x = m_posVDest.x + sinf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posRDest.y = m_posVDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * -m_fDistance;
			m_posRDest.z = m_posVDest.z + cosf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}

		if (pInputKeyboard->GetPress(DIK_Q) == true)
		{// 左に旋回
			m_rot.y += -ROT.y;

			m_posRDest.x = m_posV.x + sinf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posRDest.z = m_posV.z + cosf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}
		else if (pInputKeyboard->GetPress(DIK_E) == true)
		{// 右に旋回
			m_rot.y += ROT.y;

			m_posRDest.x = m_posV.x + sinf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posRDest.z = m_posV.z + cosf(m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}

		// 視点移動
		if (pInputKeyboard->GetPress(DIK_Y) == true)
		{// 上に移動
			m_fAngle += ROT.y;

			if (m_fAngle > D3DX_PI * 0.4f)
			{// 上の制限
				m_fAngle = D3DX_PI * 0.4f;
			}

			CorrectAngle(&m_fAngle, m_fAngle);

			m_posVDest.x = m_posRDest.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.y = m_posRDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * m_fDistance;
			m_posVDest.z = m_posRDest.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}
		else if (pInputKeyboard->GetPress(DIK_N) == true)
		{// 下に移動
			m_fAngle += -ROT.y;

			if (m_fAngle < -D3DX_PI * 0.4f)
			{// 下の制限
				m_fAngle = -D3DX_PI * 0.4f;
			}

			CorrectAngle(&m_fAngle, m_fAngle);

			m_posVDest.x = m_posRDest.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.y = m_posRDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * m_fDistance;
			m_posVDest.z = m_posRDest.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}

		if (pInputKeyboard->GetPress(DIK_Z) == true)
		{// 左に旋回
			m_rot.y += ROT.y;

			m_posVDest.x = m_posR.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.z = m_posR.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}
		else if (pInputKeyboard->GetPress(DIK_C) == true)
		{// 右に旋回
			m_rot.y += -ROT.y;

			m_posVDest.x = m_posR.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.z = m_posR.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}

		// カメラのズーム
		if (pInputMouse->GetWheel() != 0)
		{// ホイールが回転したとき
			m_fDistance += (float)(pInputMouse->GetWheel() / ONE_TICK) * MOVE_DIST;

			if (m_fDistance <= MIN_DISTANCE)
			{// 最低値
				m_fDistance = MIN_DISTANCE;
			}

			// ホイールの値をリセット
			pInputMouse->ResetWheel();

			// 目的の視点を設定
			m_posVDest.x = m_posRDest.x + sinf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
			m_posVDest.y = m_posRDest.y + cosf((D3DX_PI * 0.5f) - m_fAngle) * m_fDistance;
			m_posVDest.z = m_posRDest.z + cosf(D3DX_PI + m_rot.y) * m_fDistance * sinf((D3DX_PI * 0.5f) - m_fAngle);
		}

		// リセット
		if (pInputKeyboard->GetPress(DIK_R) == true)
		{
			m_posV = DEFAULT_VECTOR3;
			m_posR = DEFAULT_VECTOR3;
			m_posVDest = DEFAULT_VECTOR3;
			m_posRDest = DEFAULT_VECTOR3;
			m_rot = DEFAULT_VECTOR3;
		}

		// カメラ向きを調整
		CorrectAngle(&m_rot.y, m_rot.y);

		// 注視点の補正
		m_posR += (m_posRDest - m_posR) * INERTIA_POSR;

		// 視点の補正
		m_posV += m_posVOff + (m_posVDest - m_posV) * INERTIA_POSV;

		break;

	case TYPE_ROTATION:		// 自動回転カメラ
		m_rot.y += -AUTO_ROT.y;

		// カメラ向きを調整
		CorrectAngle(&m_rot.y, m_rot.y);

		m_posV.x = m_posR.x + sinf(D3DX_PI + m_rot.y) * m_fDistance;
		m_posV.z = m_posR.z + cosf(D3DX_PI + m_rot.y) * m_fDistance;

		m_fViewAngle = DEFAULT_VIEW_ANGLE;
		m_fMoveVA = DEFAULT_VIEW_ANGLE;

		break;

	case TYPE_STOP:			// 定点カメラ
		// カメラ向きを調整
		CorrectAngle(&m_rot.y, m_rot.y);

		m_posV.x = m_posR.x + sinf(D3DX_PI + m_rot.y) * m_fDistance;
		m_posV.z = m_posR.z + cosf(D3DX_PI + m_rot.y) * m_fDistance;

		m_fViewAngle = DEFAULT_VIEW_ANGLE;
		m_fMoveVA = DEFAULT_VIEW_ANGLE;

		break;
	}

	if (m_fMoveVA != 0.0f)
	{// 視野角
		m_fViewAngle += (m_fMoveVA - m_fViewAngle) * INERTIA_VIEW_ANGLE;
	}

	// オフセットを震わせる
	m_posVOff.x *= -1.0f;
	m_posVOff.z *= -1.0f;

	pDebugProc->Print("\n< CAMERA >\n");
	pDebugProc->Print("プレイヤー追従 [ F5 / 左スティック押し込み ] : ( %s )\n", (m_type == TYPE_PLAYER) ? "ON" : "OFF");
}

//=============================================================================
// カメラの設定処理
//=============================================================================
void CCamera::SetCamera(void)
{
	// デバイスの取得
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// ビューポートの設定
	pDevice->SetViewport(&m_viewport);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

#ifdef ENABLE_PERSPECTIVFOV		// 透視投影
	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
		D3DXToRadian(m_fViewAngle),								// 視野角
		(float)m_viewport.Width / (float)m_viewport.Height,		// 画面のアスペクト比
		MIN_VIEW_DIST,											// カメラから一番近い描画距離
		MAX_VIEW_DIST);											// 最大描画距離

#else		// 平行投影
	// プロジェクションマトリックスを作成
	D3DXMatrixOrthoLH(&m_mtxProjection,
		(float)m_viewport.Width,		// ビューボリュームの幅
		(float)m_viewport.Height,		// ビューボリュームの高さ
		MIN_VIEW_DIST,					// ビューボリュームの近平面までの距離
		MAX_VIEW_DIST);					// ビューボリュームの遠平面までの距離
#endif

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	// ビューマトリックスを作成
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,			// 視点
		&m_posR,			// 注視点
		&m_vecU);			// 上方向ベクトル

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);
}

//=============================================================================
// カメラの位置設定
//=============================================================================
void CCamera::SetPosition(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 rot, TYPE type)
{
	m_posV = posV;
	m_posVDest = posV;
	m_posR = posR;
	m_posRDest = posR;
	m_rot = rot;
	m_type = type;

	D3DXVECTOR3 dist = m_posV - m_posR;
	D3DXVECTOR2 distXZ = D3DXVECTOR2(dist.x, dist.z);
	float fDistXZ = D3DXVec2Length(&distXZ);

	m_fAngle = atan2f(dist.y, fDistXZ);
	m_fDistance = D3DXVec3Length(&dist);
}

//=============================================================================
// カメラの視野角設定処理
//=============================================================================
void CCamera::SetViewAngle(float fViewAngle)
{
	m_fMoveVA = fViewAngle;
}

//=============================================================================
// カメラの振動設定
//=============================================================================
void CCamera::SetShake(const float fValue)
{
	static float fShake = 0.0f;

	if (m_bShake == true && fShake == fValue)
	{// 同じ値は設定しない
		return;
	}

	m_posVOff.x = fValue;
	m_posVOff.z = fValue;
	fShake = fValue;		// 前回の値を記憶

	m_bShake = true;

	if (fValue == 0.0f)
	{// 0が設定された
		m_bShake = false;
	}
}

//=============================================================================
// 追従対象を設定
//=============================================================================
void CCamera::SetFollow(CObject* pFollow)
{
	if (pFollow == nullptr)
	{// NULLチェック
		return;
	}

	m_pFollow = pFollow;
}

//=============================================================================
// 視錐台の計算
//=============================================================================
void CCamera::CalcViewFrustum(void)
{
	// ローカル変数
	D3DXMATRIX matVF;		// 視錐台の計算用マトリックス

	// ビュー行列と射影行列を合成
	D3DXMatrixMultiply(&matVF, &m_mtxView, &m_mtxProjection);

	// 合成した行列の要素から、6つの平面の係数を抽出する
	// 左平面
	m_aPlane[0].a = matVF._14 + matVF._11;
	m_aPlane[0].b = matVF._24 + matVF._21;
	m_aPlane[0].c = matVF._34 + matVF._31;
	m_aPlane[0].d = matVF._44 + matVF._41;

	// 右平面
	m_aPlane[1].a = matVF._14 - matVF._11;
	m_aPlane[1].b = matVF._24 - matVF._21;
	m_aPlane[1].c = matVF._34 - matVF._31;
	m_aPlane[1].d = matVF._44 - matVF._41;

	// 上平面
	m_aPlane[2].a = matVF._14 - matVF._12;
	m_aPlane[2].b = matVF._24 - matVF._22;
	m_aPlane[2].c = matVF._34 - matVF._32;
	m_aPlane[2].d = matVF._44 - matVF._42;

	// 下平面
	m_aPlane[3].a = matVF._14 + matVF._12;
	m_aPlane[3].b = matVF._24 + matVF._22;
	m_aPlane[3].c = matVF._34 + matVF._32;
	m_aPlane[3].d = matVF._44 + matVF._42;

	// 手前平面
	m_aPlane[4].a = matVF._13;
	m_aPlane[4].b = matVF._23;
	m_aPlane[4].c = matVF._33;
	m_aPlane[4].d = matVF._43;

	// 奥平面
	m_aPlane[5].a = matVF._14 - matVF._13;
	m_aPlane[5].b = matVF._24 - matVF._23;
	m_aPlane[5].c = matVF._34 - matVF._33;
	m_aPlane[5].d = matVF._44 - matVF._43;

	// 全ての平面を正規化
	for (int nCntPlane = 0; nCntPlane < VIEWFRUSTUM_NUM; nCntPlane++)
	{
		D3DXPlaneNormalize(&m_aPlane[nCntPlane], &m_aPlane[nCntPlane]);
	}
}

//=============================================================================
// カメラ内に存在するかどうか
//=============================================================================
bool CCamera::IsInside(const D3DXVECTOR3 pos, const float fRadius)
{
	// 視錐台の計算
	CalcViewFrustum();

	for (int nCntPlane = 0; nCntPlane < VIEWFRUSTUM_NUM; nCntPlane++)
	{
		// 距離を計算
		float fDist = D3DXPlaneDotCoord(&m_aPlane[nCntPlane], &pos);

		if (fDist < -fRadius)
		{// 外側にいる
			return false;
		}
	}

	return true;
}

//=============================================================================
// 角度を修正
//=============================================================================
void CCamera::CorrectAngle(float* pAngle, float fAngle)
{
	// 向きを調整
	if (fAngle > D3DX_PI)
	{
		*pAngle -= D3DX_PI * 2.0f;
	}
	else if (fAngle < -D3DX_PI)
	{
		*pAngle += D3DX_PI * 2.0f;
	}
}