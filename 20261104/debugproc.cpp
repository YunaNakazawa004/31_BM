//=============================================================================
// 
// デバッグ表示 [ debugproc.cpp ]
// Author : 中澤優奈
// 
//=============================================================================
#include "debugproc.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"

#include "object.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FONT_SIZE			(26)			// フォントの大きさ

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
LPD3DXFONT CDebugProc::m_pFont = nullptr;				// フォントへのポインタ
char CDebugProc::m_aStrDebug[MAX_DEBUGSTRING] = {};	// 文字列(デバッグ情報)を格納するバッファ
bool CDebugProc::m_bDebug = true;					// デバッグ表示のON/OFF

//=============================================================================
// デバッグ表示クラスのコンストラクタ
//=============================================================================
CDebugProc::CDebugProc()
{
	// デバッグ表示クラスの値をクリア
	m_pFont = nullptr;
	memset(&m_aStrDebug[0], 0, sizeof m_aStrDebug);
	m_bDebug = true;
}

//=============================================================================
// デバッグ表示クラスのデストラクタ
//=============================================================================
CDebugProc::~CDebugProc()
{
}

//=============================================================================
// デバッグ表示の初期化処理
//=============================================================================
HRESULT CDebugProc::Init(void)
{
	// ローカル変数
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ

	// デバッグ表示用フォントの生成
	D3DXCreateFont(pDevice, FONT_SIZE, 0, 0, 0, FALSE,
		SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH,
		"Terminal", &m_pFont);

	// デバッグ情報バッファのクリア
	memset(&m_aStrDebug[0], 0, sizeof m_aStrDebug);

	// デバッグ表示ON/OFFを初期化
#ifdef _DEBUG
	m_bDebug = true;
#else
	m_bDebug = false;
#endif

	return S_OK;
}

//=============================================================================
// デバッグ表示の終了処理
//=============================================================================
void CDebugProc::Uninit(void)
{
	// デバッグ表示用フォントの破棄
	if (m_pFont != nullptr)
	{
		m_pFont->Release();
		m_pFont = nullptr;
	}
}

//=============================================================================
// デバッグ表示の更新処理
//=============================================================================
void CDebugProc::Update(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得

	if (pInputKeyboard->GetTrigger(DIK_F1) == true)
	{// デバッグ表示切り替え
		(m_bDebug == false) ? (m_bDebug = true) : (m_bDebug = false);
	}
}

//=============================================================================
// デバッグ表示の描画処理
//=============================================================================
void CDebugProc::Draw(void)
{
	RECT rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };

	if (m_bDebug == true)
	{// 表示するときだけ
		// テキストの描画
		m_pFont->DrawText(nullptr, &m_aStrDebug[0], -1, &rect, DT_LEFT, COLOR_WHITE);
	}

	// デバッグ情報バッファのクリア
	memset(&m_aStrDebug[0], 0, sizeof m_aStrDebug);
}

//=============================================================================
// デバッグ情報を登録
//=============================================================================
void CDebugProc::Print(const char* fmt, ...)
{
	va_list ap;
	char aString[MAX_DEBUGSTRING] = "\0";

	va_start(ap, fmt);

	for (const char* pChara = fmt; *pChara != '\0'; ++pChara)
	{
		if (*pChara == '%')
		{// 型変換
			pChara++;

			switch (*pChara)
			{
			case 'd':		// int
				sprintf(&aString[0], "%d", va_arg(ap, int));

				break;

			case 'f':		// double
				sprintf(&aString[0], "%.3f", va_arg(ap, double));

				break;

			case 'c':		// char
				sprintf(&aString[0], "%c", va_arg(ap, char));

				break;

			case 's':		// char*
				sprintf(&aString[0], "%s", va_arg(ap, const char*));

				break;
			}
		}
		else
		{// 文字列
			sprintf(&aString[0], "%c", *pChara);
		}

		strcat(&m_aStrDebug[0], &aString[0]);
	}

	va_end(ap);
}