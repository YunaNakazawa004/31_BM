//=============================================================================
// 
// 入力ヘッダー [ input.h ]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _INPUT_H_		// このマクロ定義がされていなかったら
#define _INPUT_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_KEY_MAX			(256)		// キーの最大数
#define ONE_TICK			(120)		// マウスホイールの回転量

//*****************************************************************************
// 入力クラス
//*****************************************************************************
class CInput
{
public:
	CInput();
	virtual ~CInput();

	virtual HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;

protected:
	static LPDIRECTINPUT8 m_pInput;			// DirectInputオブジェクトへのポインタ
	LPDIRECTINPUTDEVICE8 m_pDevice;			// 入力デバイス(キーボード)へのポインタ
};

//*****************************************************************************
// キーボードクラス
//*****************************************************************************
class CInputKeyboard : public CInput
{
public:
	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);

	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetAnyTrigger(void);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey);

private:
	BYTE m_aKeyState[NUM_KEY_MAX];			// キーボードのプレス情報
	BYTE m_aKeyStateTrigger[NUM_KEY_MAX];	// キーボードのトリガー情報
	BYTE m_aKeyStateRelease[NUM_KEY_MAX];	// キーボードのリリース情報
	BYTE m_aKeyStateRepeat[NUM_KEY_MAX];	// キーボードのリピート情報
	int m_aRepeatCounter[NUM_KEY_MAX];		// 各キーごとのリピートカウンター
	int m_aRepeatCount[NUM_KEY_MAX];		// 各キーごとのリピート回数
};

//*****************************************************************************
// マウスクラス
//*****************************************************************************
class CInputMouse : public CInput
{
public:
	// マウスボタンの種類
	typedef enum
	{
		MOUSEBUTTON_LEFT = 0,		// マウス左ボタン
		MOUSEBUTTON_RIGHT,			// マウス右ボタン
		MOUSEBUTTON_CENTER,			// マウスホイールボタン
		MOUSEBUTTON_3,				// マウス空きボタン
		MOUSEBUTTON_4,				// マウス空きボタン
		MOUSEBUTTON_5,				// マウス空きボタン
		MOUSEBUTTON_6,				// マウス空きボタン
		MOUSEBUTTON_7,				// マウス空きボタン
		MOUSEBUTTON_MAX,
	}MOUSEBUTTON;

	CInputMouse();
	~CInputMouse();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);

	bool GetPress(const int nButton);
	bool GetTrigger(const int nButton);
	bool GetRelease(const int nButton);
	POINT GetPosition(void) { return m_cursorPos; }
	int GetWheel(void) { return m_nWheel; }
	void ResetWheel(void) { m_nWheel = 0; }

private:
	DIMOUSESTATE2 m_mouseState;			// マウスのプレス情報
	DIMOUSESTATE2 m_mouseStateTrigger;	// マウスのトリガー情報
	DIMOUSESTATE2 m_mouseStateRelease;	// マウスのリリース情報
	POINT m_cursorPos;					// マウスカーソルの位置
	int m_nWheel;						// マウスホイールの回転量
};

//*****************************************************************************
// ジョイパッドクラス
//*****************************************************************************
class CInputJoypad : public CInput
{
public:
	// ジョイパッドのキーの種類
	typedef enum
	{
		JOYKEY_UP = 0,				// [0]十字キー(上)
		JOYKEY_DOWN,				// [1]十字キー(下)
		JOYKEY_LEFT,				// [2]十字キー(左)
		JOYKEY_RIGHT,				// [3]十字キー(右)
		JOYKEY_START,				// [4]START
		JOYKEY_BACK,				// [5]BACK
		JOYKEY_LEFT_THUMB,			// [6]左スティック押し込み
		JOYKEY_RIGHT_THUMB,			// [7]右スティック押し込み
		JOYKEY_LEFT_SHOULDER,		// [8]左肩キー
		JOYKEY_RIGHT_SHOULDER,		// [9]右肩キー
		JOYKEY_A = 12,				// [12]Aキー
		JOYKEY_B,					// [13]Bキー
		JOYKEY_X,					// [14]Xキー
		JOYKEY_Y,					// [15]Yキー
		JOYKEY_LEFTSTICK,			// [16]左スティックの入力
		JOYKEY_LEFTSTICK_LEFT,		// [17]左スティックの左入力
		JOYKEY_LEFTSTICK_RIGHT,		// [18]左スティックの右入力
		JOYKEY_LEFTSTICK_UP,		// [19]左スティックの上入力
		JOYKEY_LEFTSTICK_DOWN,		// [20]左スティックの下入力
		JOYKEY_RIGHTSTICK,			// [21]右スティックの入力
		JOYKEY_RIGHTSTICK_LEFT,		// [22]右スティックの左入力
		JOYKEY_RIGHTSTICK_RIGHT,	// [23]右スティックの右入力
		JOYKEY_RIGHTSTICK_UP,		// [24]右スティックの上入力
		JOYKEY_RIGHTSTICK_DOWN,		// [25]右スティックの下入力
		JOYKEY_TRIGGER,				// [26]トリガー
		JOYKEY_LEFTTRIGGER,			// [27]左のトリガー
		JOYKEY_RIGHTTRIGGER,		// [28]右のトリガー
		JOYKEY_MAX
	}JOYKEY;

	CInputJoypad();
	~CInputJoypad();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	bool GetPress(int nIdx, JOYKEY key);
	bool GetTrigger(int nIdx, JOYKEY key);
	bool GetAnyTrigger(int nIdx);
	bool GetRelease(int nIdx, JOYKEY key);
	bool GetRepeat(int nIdx, JOYKEY key);
	bool GetStick(int nIdx, JOYKEY key, int* pValueH, int* pValueV);
	bool GetShoulder(int nIdx, JOYKEY key, int* pValueR, int* pValueL);
	bool GetStickSlow(int nIdx, int nCount);
	void SetVibration(int nIdx, WORD wLeftVibSpeed, WORD wRightVibSpeed, int nVibCount);
	void UpdateVibration(void);
	void UninitVibration(void);

private:
	XINPUT_STATE m_joykeyState[MAX_PLAYER];				// ジョイパッドのプレス情報
	XINPUT_STATE m_joykeyStateTrigger[MAX_PLAYER];		// ジョイパッドのトリガー情報
	XINPUT_STATE m_joykeyStateRelease[MAX_PLAYER];		// ジョイパッドのトリガー情報
	XINPUT_STATE m_joykeyStateRepeat[MAX_PLAYER];		// ジョイパッドのトリガー情報
	XINPUT_VIBRATION m_joykeyVibration[MAX_PLAYER];		// ジョイパッドの振動情報
	int m_joykeyRepeatCounter[MAX_PLAYER];				// リピートカウンター
	int m_joykeyRepeatCount[MAX_PLAYER];				// リピート間隔のカウント
	int m_joyStickSlowCount[MAX_PLAYER];				// スロウ間隔のカウント
	int m_EndVib[MAX_PLAYER];							// バイブレーションのカウンター
	bool m_bUseVib[MAX_PLAYER];							// バイブレーションの使用
};

#endif