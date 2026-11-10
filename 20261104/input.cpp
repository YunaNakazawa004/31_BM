//=============================================================================
// 
// 入力処理 [ input.cpp ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#include "input.h"

#include "manager.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define REPEAT_MAX			(40)		// リピートまでの最大フレーム数
#define REPEAT_COUNT		(10)		// リピート間隔

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
LPDIRECTINPUT8 CInput::m_pInput = nullptr;		// DirectInputオブジェクトへのポインタ

//=============================================================================
// 入力クラスのコンストラクタ
//=============================================================================
CInput::CInput()
{
	m_pInput = nullptr;
}

//=============================================================================
// 入力クラスのデストラクタ
//=============================================================================
CInput::~CInput()
{
}

//=============================================================================
// 入力クラスの初期化処理
//=============================================================================
HRESULT CInput::Init(HINSTANCE hInstance, HWND hWnd)
{
	if (m_pInput == nullptr)
	{// NULLチェック
		// DirectInputオブジェクトの生成
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, nullptr)))
		{// もし失敗したら
			return E_FAIL;
		}

		return S_OK;
	}

	return E_FAIL;
}

//=============================================================================
// 入力クラスの終了処理
//=============================================================================
void CInput::Uninit(void)
{
	// 入力デバイス(キーボード)の破棄
	if (m_pDevice != nullptr)
	{
		m_pDevice->Unacquire();			// キーボードへのアクセス権を破棄
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	// DirectInputオブジェクトの破棄
	if (m_pInput != nullptr)
	{
		m_pInput->Release();
		m_pInput = nullptr;
	}
}

//=============================================================================
// キーボードクラスのコンストラクタ
//=============================================================================
CInputKeyboard::CInputKeyboard()
{
	memset(&m_aKeyState[0], 0, sizeof(int[NUM_KEY_MAX]));
	memset(&m_aKeyStateTrigger[0], 0, sizeof(int[NUM_KEY_MAX]));
	memset(&m_aKeyStateRelease[0], 0, sizeof(int[NUM_KEY_MAX]));
	memset(&m_aKeyStateRepeat[0], 0, sizeof(int[NUM_KEY_MAX]));
	memset(&m_aRepeatCounter[0], 0, sizeof(int[NUM_KEY_MAX]));
	memset(&m_aRepeatCount[0], 0, sizeof(int[NUM_KEY_MAX]));
}

//=============================================================================
// キーボードクラスのデストラクタ
//=============================================================================
CInputKeyboard::~CInputKeyboard()
{
}

//=============================================================================
// キーボードの初期化処理
//=============================================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	// 入力クラスの初期化
	CInput::Init(hInstance, hWnd);

	if (m_pInput != nullptr)
	{// NULLチェック
		// 入力デバイス(キーボード)の生成
		if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, nullptr)))
		{// もし失敗したら
			return E_FAIL;
		}

		// データフォーマットを設定
		if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
		{// もし失敗したら
			return E_FAIL;
		}

		// 協調モードを設定
		if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
		{// もし失敗したら
			return E_FAIL;
		}

		// キーボードへのアクセス権を獲得
		m_pDevice->Acquire();

		return S_OK;
	}

	return E_FAIL;
}

//=============================================================================
// キーボードの終了処理
//=============================================================================
void CInputKeyboard::Uninit(void)
{
	// 入力クラスの終了処理
	CInput::Uninit();
}

//=============================================================================
// キーボードの更新処理
//=============================================================================
void CInputKeyboard::Update(void)
{
	// ローカル変数宣言
	BYTE aKeyState[NUM_KEY_MAX];		// キーボードの入力情報
	int nCntKey;

	// 入力デバイスからデータを取得(ブレークポイントが効かない！！！)
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			m_aKeyStateTrigger[nCntKey] = ~m_aKeyState[nCntKey] & aKeyState[nCntKey];			// キーボードのトリガー情報を保存
			m_aKeyStateRelease[nCntKey] = m_aKeyState[nCntKey] & ~aKeyState[nCntKey];			// キーボードのリリース情報を保存
			m_aKeyState[nCntKey] = aKeyState[nCntKey];			// キーボードのプレス情報を保存

			if (GetRelease(nCntKey))
			{// キーボードが離されたらリピートをリセット
				m_aRepeatCounter[nCntKey] = 0;
				m_aRepeatCount[nCntKey] = 0;
				m_aKeyStateRepeat[nCntKey] = 0;
			}
			else if (GetPress(nCntKey))
			{// キーボードが押されている間
				if (m_aKeyStateTrigger[nCntKey])
				{// 押された瞬間
					m_aKeyStateRepeat[nCntKey] = 0x80;
					m_aRepeatCounter[nCntKey] = 0;
					m_aRepeatCount[nCntKey] = 0;
				}
				else
				{// 押されている間
					m_aRepeatCounter[nCntKey]++;

					if (m_aRepeatCounter[nCntKey] > REPEAT_MAX)
					{// カウントが一定値を越えた
						m_aRepeatCount[nCntKey]++;

						if (m_aRepeatCount[nCntKey] > REPEAT_COUNT)
						{// 一定間隔ごとにカウントをリセット
							m_aRepeatCount[nCntKey] = 0;
							m_aKeyStateRepeat[nCntKey] = 0x80;
						}
						else
						{// カウント中は待機
							m_aKeyStateRepeat[nCntKey] = 0;
						}
					}
					else
					{// カウント中は待機
						m_aKeyStateRepeat[nCntKey] = 0;
					}
				}
			}
			else
			{// カウント中は待機
				m_aKeyStateRepeat[nCntKey] = 0;
			}
		}
	}
	else
	{// デバイスがアクセス権を持っていない場合
		m_pDevice->Acquire();					// キーボードへのアクセス権を獲得
	}
}

//=============================================================================
// キーボードのプレス情報を取得
//=============================================================================
bool CInputKeyboard::GetPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのトリガー情報を取得
//=============================================================================
bool CInputKeyboard::GetTrigger(int nKey)
{
	return (m_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//=============================================================================
// キーボードの全てのトリガー情報を取得
//=============================================================================
bool CInputKeyboard::GetAnyTrigger(void)
{
	for (int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
	{
		if (m_aKeyStateTrigger[nCntKey] & 0x80 && 
			nCntKey != DIK_ESCAPE && nCntKey != DIK_BACKSPACE && nCntKey != DIK_F1 && nCntKey != DIK_F11)
		{// いずれかのキーが押された(他機能キー以外)
			return true;
		}
	}

	return false;
}

//=============================================================================
// キーボードのリリース情報を取得
//=============================================================================
bool CInputKeyboard::GetRelease(int nKey)
{
	return (m_aKeyStateRelease[nKey] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのリピート情報を取得
//=============================================================================
bool CInputKeyboard::GetRepeat(int nKey)
{
	return (m_aKeyStateRepeat[nKey] & 0x80) ? true : false;
}

//=============================================================================
// マウスクラスのコンストラクタ
//=============================================================================
CInputMouse::CInputMouse()
{
	memset(&m_mouseState, 0, sizeof m_mouseState);
	memset(&m_mouseStateTrigger, 0, sizeof m_mouseStateTrigger);
	memset(&m_mouseStateRelease, 0, sizeof m_mouseStateRelease);
	memset(&m_cursorPos, 0, sizeof m_cursorPos);
}

//=============================================================================
// マウスクラスのデストラクタ
//=============================================================================
CInputMouse::~CInputMouse()
{
}

//=============================================================================
// マウスの初期化処理
//=============================================================================
HRESULT CInputMouse::Init(HINSTANCE hInstance, HWND hWnd)
{
	// 入力クラスの初期化
	CInput::Init(hInstance, hWnd);

	if (m_pInput != nullptr)
	{// NULLチェック
		// 入力デバイス(マウス)の生成
		if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pDevice, nullptr)))
		{// もし失敗したら
			return E_FAIL;
		}

		// データフォーマットを設定
		if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse2)))
		{// もし失敗したら
			return E_FAIL;
		}

		// 協調モードを設定
		if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
		{// もし失敗したら
			return E_FAIL;
		}

		// マウスへのアクセス権を獲得
		m_pDevice->Acquire();

		return S_OK;
	}

	return E_FAIL;
}

//=============================================================================
// マウスの終了処理
//=============================================================================
void CInputMouse::Uninit(void)
{
	// 入力クラスの終了処理
	CInput::Uninit();
}

//=============================================================================
// マウスの更新処理
//=============================================================================
void CInputMouse::Update(void)
{
	// ローカル変数宣言
	DIMOUSESTATE2 mouseState;		// マウスの入力情報
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得

	// 入力デバイスからデータを取得(ブレークポイントが効かない！！！)
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(mouseState), &mouseState)))
	{
		for (int nCntMouseButton = 0; nCntMouseButton < MOUSEBUTTON_MAX; nCntMouseButton++)
		{
			m_mouseStateTrigger.rgbButtons[nCntMouseButton] =
				~m_mouseState.rgbButtons[nCntMouseButton]
				& mouseState.rgbButtons[nCntMouseButton];			// マウスのトリガー情報を保存

			m_mouseStateRelease.rgbButtons[nCntMouseButton] =
				m_mouseState.rgbButtons[nCntMouseButton]
				& ~mouseState.rgbButtons[nCntMouseButton];			// マウスのリリース情報を保存
		}

		m_mouseState = mouseState;			// マウスのプレス情報を保存

		// カーソルの座標を保存
		GetCursorPos(&m_cursorPos);
		ScreenToClient(FindWindowA("WindowClass", nullptr), &m_cursorPos);

		// ホイールの回転量を保存
		m_nWheel += mouseState.lZ;
	}
	else
	{// デバイスがアクセス権を持っていない場合
		m_pDevice->Acquire();					// マウスへのアクセス権を獲得
	}
}

//=============================================================================
// マウスのプレス情報を取得
//=============================================================================
bool CInputMouse::GetPress(const int nButton)
{
	return (m_mouseState.rgbButtons[nButton] & 0x80) ? true : false;
}

//=============================================================================
// マウスのトリガー情報を取得
//=============================================================================
bool CInputMouse::GetTrigger(const int nButton)
{
	return (m_mouseStateTrigger.rgbButtons[nButton] & 0x80) ? true : false;
}

//=============================================================================
// マウスのリリース情報を取得
//=============================================================================
bool CInputMouse::GetRelease(const int nButton)
{
	return (m_mouseStateRelease.rgbButtons[nButton] & 0x80) ? true : false;
}

//=============================================================================
// ジョイパッドクラスのコンストラクタ
//=============================================================================
CInputJoypad::CInputJoypad()
{
	// メモリのクリア
	memset(&m_joykeyState[0], 0, sizeof(m_joykeyState));
	memset(&m_joykeyStateTrigger[0], 0, sizeof(m_joykeyStateTrigger));
	memset(&m_joykeyStateRelease[0], 0, sizeof(m_joykeyStateRelease));
	memset(&m_joykeyStateRepeat[0], 0, sizeof(m_joykeyStateRepeat));
	memset(&m_joykeyVibration[0], 0, sizeof(m_joykeyVibration));
	memset(&m_joykeyRepeatCounter[0], 0, sizeof(m_joykeyRepeatCounter));
	memset(&m_joykeyRepeatCount[0], 0, sizeof(m_joykeyRepeatCount));
	memset(&m_joyStickSlowCount[0], 0, sizeof(m_joyStickSlowCount));
	memset(&m_EndVib[0], 0, sizeof(m_EndVib));
	memset(&m_bUseVib[0], false, sizeof(m_bUseVib));
}

//=============================================================================
// ジョイパッドクラスのデストラクタ
//=============================================================================
CInputJoypad::~CInputJoypad()
{
}

//=============================================================================
// ジョイパッドの初期化処理
//=============================================================================
HRESULT CInputJoypad::Init(void)
{
	// メモリのクリア
	memset(&m_joykeyState[0], 0, sizeof(m_joykeyState));
	memset(&m_joykeyStateTrigger[0], 0, sizeof(m_joykeyStateTrigger));
	memset(&m_joykeyStateRelease[0], 0, sizeof(m_joykeyStateRelease));
	memset(&m_joykeyStateRepeat[0], 0, sizeof(m_joykeyStateRepeat));
	memset(&m_joykeyVibration[0], 0, sizeof(m_joykeyVibration));
	memset(&m_joykeyRepeatCounter[0], 0, sizeof(m_joykeyRepeatCounter));
	memset(&m_joykeyRepeatCount[0], 0, sizeof(m_joykeyRepeatCount));
	memset(&m_joyStickSlowCount[0], 0, sizeof(m_joyStickSlowCount));
	memset(&m_EndVib[0], 0, sizeof(m_EndVib));
	memset(&m_bUseVib[0], false, sizeof(m_bUseVib));

	// Xinputのステートを設定(有効にする)
	XInputEnable(true);

	return S_OK;
}

//=============================================================================
// ジョイパッドの終了処理
//=============================================================================
void CInputJoypad::Uninit(void)
{
	// Xinputのステートを設定(無効にする)
	XInputEnable(false);

	// 振動の終了処理
	UninitVibration();
}

//=============================================================================
// ジョイパッドの更新処理
//=============================================================================
void CInputJoypad::Update(void)
{
	// ローカル変数宣言
	XINPUT_STATE joykeyState;		// ジョイパッドの入力情報

	for (int nCntJoypad = 0; nCntJoypad < MAX_PLAYER; nCntJoypad++)
	{
		// ジョイパッドの状態を取得
		if (XInputGetState(nCntJoypad, &joykeyState) == ERROR_SUCCESS)
		{
			m_joykeyStateTrigger[nCntJoypad].Gamepad.wButtons =
				(m_joykeyState[nCntJoypad].Gamepad.wButtons ^ joykeyState.Gamepad.wButtons)
				& joykeyState.Gamepad.wButtons;			// ジョイパッドのトリガー情報を保存

			m_joykeyStateRelease[nCntJoypad].Gamepad.wButtons =
				m_joykeyState[nCntJoypad].Gamepad.wButtons & ~joykeyState.Gamepad.wButtons;			// ジョイパッドのリリース情報を保存

			m_joykeyState[nCntJoypad] = joykeyState;									// ジョイパッドのプレス情報を保存

			if (m_joykeyState[nCntJoypad].Gamepad.wButtons != 0)
			{// ボタンが押されているとき
				m_joykeyStateRepeat[nCntJoypad].Gamepad.wButtons = m_joykeyStateTrigger[nCntJoypad].Gamepad.wButtons;

				m_joykeyRepeatCounter[nCntJoypad]++;
				m_joykeyRepeatCount[nCntJoypad]++;
			}

			if (m_joykeyRepeatCounter[nCntJoypad] > REPEAT_MAX)
			{// カウンターが一定の値を超えた
				if (m_joykeyRepeatCount[nCntJoypad] % REPEAT_COUNT == 0)
				{
					m_joykeyStateRepeat[nCntJoypad].Gamepad.wButtons = joykeyState.Gamepad.wButtons;			// ジョイパッドのリピート情報を保存
				}

				if (m_joykeyStateRelease[nCntJoypad].Gamepad.wButtons != 0)
				{
					m_joykeyRepeatCounter[nCntJoypad] = 0;
				}
			}

			m_joyStickSlowCount[nCntJoypad]++;
		}
	}

	// 振動の更新
	UpdateVibration();
}

//=============================================================================
// ジョイパッドのプレス情報を取得
//=============================================================================
bool CInputJoypad::GetPress(int nIdx, JOYKEY key)
{
	return (m_joykeyState[nIdx].Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//=============================================================================
// ジョイパッドのトリガー情報を取得
//=============================================================================
bool CInputJoypad::GetTrigger(int nIdx, JOYKEY key)
{
	return (m_joykeyStateTrigger[nIdx].Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//=============================================================================
// ジョイパッドの全てのトリガー情報を取得
//=============================================================================
bool CInputJoypad::GetAnyTrigger(int nIdx)
{
	for (int nCntKey = 0; nCntKey < JOYKEY_LEFTSTICK; nCntKey++)
	{
		if (m_joykeyStateTrigger[nIdx].Gamepad.wButtons & (0x01 << nCntKey))
		{// いずれかのキーが押された
			return true;
		}
	}

	return false;
}

//=============================================================================
// ジョイパッドのリリース情報を取得
//=============================================================================
bool CInputJoypad::GetRelease(int nIdx, JOYKEY key)
{
	return (m_joykeyStateRelease[nIdx].Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//=============================================================================
// ジョイパッドのリピート情報を取得
//=============================================================================
bool CInputJoypad::GetRepeat(int nIdx, JOYKEY key)
{
	return (m_joykeyStateRepeat[nIdx].Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//=============================================================================
// ジョイパッドのスティックプレス情報を取得(true : 動く	false : 動かない)
//=============================================================================
bool CInputJoypad::GetStick(int nIdx, JOYKEY key, int* pValueH, int* pValueV)
{
	if (key == JOYKEY_LEFTSTICK)
	{// 左スティック
		if (pValueH != nullptr)
		{// 水平方向
			*pValueH = m_joykeyState[nIdx].Gamepad.sThumbLX;
		}

		if (pValueV != nullptr)
		{// 垂直方向
			*pValueV = m_joykeyState[nIdx].Gamepad.sThumbLY;
		}

		if (m_joykeyState[nIdx].Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
			m_joykeyState[nIdx].Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
			m_joykeyState[nIdx].Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
			m_joykeyState[nIdx].Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{// 入力がある
			return true;
		}
		else
		{// 入力がない
			return false;
		}
	}

	if (key == JOYKEY_RIGHTSTICK)
	{// 右スティック
		if (pValueH != nullptr)
		{// 水平方向
			*pValueH = m_joykeyState[nIdx].Gamepad.sThumbRX;
		}

		if (pValueV != nullptr)
		{// 垂直方向
			*pValueV = m_joykeyState[nIdx].Gamepad.sThumbRY;
		}

		if (m_joykeyState[nIdx].Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
			m_joykeyState[nIdx].Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
			m_joykeyState[nIdx].Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
			m_joykeyState[nIdx].Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{// 入力がある
			return true;
		}
		else
		{// 入力がない
			return false;
		}
	}

	// ローカル変数宣言
	float fLX = m_joykeyState[nIdx].Gamepad.sThumbLX;
	float fLY = m_joykeyState[nIdx].Gamepad.sThumbLY;
	float fRX = m_joykeyState[nIdx].Gamepad.sThumbRX;
	float fRY = m_joykeyState[nIdx].Gamepad.sThumbRY;

	if (key == JOYKEY_LEFTSTICK_LEFT)
	{// 左スティックの左入力
		return (fLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_LEFTSTICK_RIGHT)
	{// 左スティックの右入力
		return (fLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_LEFTSTICK_UP)
	{// 左スティックの上入力
		return (fLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_LEFTSTICK_DOWN)
	{// 左スティックの下入力
		return (fLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}

#if 1
	if (key == JOYKEY_RIGHTSTICK_LEFT)
	{// 右スティックの左入力
		return (fRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_RIGHTSTICK_RIGHT)
	{// 右スティックの右入力
		return (fRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_RIGHTSTICK_UP)
	{// 右スティックの上入力
		return (fRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_RIGHTSTICK_DOWN)
	{// 右スティックの下入力
		return (fRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
#endif

	return false;
}

//=============================================================================
// ジョイパッドの肩トリガー情報を取得(true : 動く	false : 動かない)
//=============================================================================
bool CInputJoypad::GetShoulder(int nIdx, JOYKEY key, int* pValueR, int* pValueL)
{
	if (key == JOYKEY_TRIGGER)
	{// 両方
		if (pValueL != nullptr)
		{// 左トリガー
			*pValueL = m_joykeyState[nIdx].Gamepad.bLeftTrigger;
		}

		if (pValueR != nullptr)
		{// 右トリガー
			*pValueR = m_joykeyState[nIdx].Gamepad.bRightTrigger;
		}

		if (m_joykeyState[nIdx].Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ||
			m_joykeyState[nIdx].Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{// 入力がある
			return true;
		}
		else
		{// 入力がない
			return false;
		}
	}

	if (key == JOYKEY_LEFTTRIGGER)
	{// 左トリガー
		if (pValueL != nullptr)
		{// 左トリガー
			*pValueL = m_joykeyState[nIdx].Gamepad.bLeftTrigger;
		}

		if (m_joykeyState[nIdx].Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{// 入力がある
			return true;
		}
		else
		{// 入力がない
			return false;
		}
	}

	if (key == JOYKEY_RIGHTTRIGGER)
	{// 右トリガー
		if (pValueR != nullptr)
		{// 右トリガー
			*pValueR = m_joykeyState[nIdx].Gamepad.bRightTrigger;
		}

		if (m_joykeyState[nIdx].Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{// 入力がある
			return true;
		}
		else
		{// 入力がない
			return false;
		}
	}

	return false;
}

//=============================================================================
// ジョイパッドのスティックスロウ情報を取得(true : 動く	false : 動かない)
//=============================================================================
bool CInputJoypad::GetStickSlow(int nIdx, int nCount)
{
	return ((m_joyStickSlowCount[nIdx] % nCount) == 0) ? true : false;
}

//=============================================================================
// ジョイパッドの振動設定処理
//=============================================================================
void CInputJoypad::SetVibration(int nIdx, WORD wLeftVibSpeed, WORD wRightVibSpeed, int nVibCount)
{
	m_joykeyVibration[nIdx].wLeftMotorSpeed = wLeftVibSpeed;
	m_joykeyVibration[nIdx].wRightMotorSpeed = wRightVibSpeed;

	// 振動時間の設定
	m_EndVib[nIdx] = nVibCount;
	m_bUseVib[nIdx] = true;

	// 振動
	XInputSetState(nIdx, &m_joykeyVibration[nIdx]);
}

//=============================================================================
// ジョイパッドの振動更新処理
//=============================================================================
void CInputJoypad::UpdateVibration(void)
{
	for (int nCntJoypad = 0; nCntJoypad < MAX_PLAYER; nCntJoypad++)
	{
		m_EndVib[nCntJoypad]--;

		if (m_bUseVib[nCntJoypad] == true && m_EndVib[nCntJoypad] < 0)
		{// 振動を終わる時になった
			// 振動を止める
			m_joykeyVibration[nCntJoypad] = { 0 };
			XInputSetState(nCntJoypad, &m_joykeyVibration[nCntJoypad]);
			m_bUseVib[nCntJoypad] = false;
		}
	}
}

//=============================================================================
// ジョイパッドの振動終了処理
//=============================================================================
void CInputJoypad::UninitVibration(void)
{
	for (int nCntJoypad = 0; nCntJoypad < MAX_PLAYER; nCntJoypad++)
	{
		m_joykeyVibration[nCntJoypad] = {};
	}
}