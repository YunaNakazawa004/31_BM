//========================================================================
// 
// ライト [ light.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "light.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"
#include "debugproc.h"
#include "object.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define LIGHTMOVE_ENABLE			// ライトを動かせるかどうか
#undef LIGHTMOVE_ENABLE

//========================================================================
// ライトクラスのコンストラクタ
//========================================================================
CLight::CLight()
{
	memset(&m_aLight[0], 0, sizeof m_aLight);
	m_fRotH = 0.0f;
	m_fRotV = 0.0f;
}

//========================================================================
// ライトクラスのデストラクタ
//========================================================================
CLight::~CLight()
{
}

//========================================================================
// ライトの初期化処理
//========================================================================
HRESULT CLight::Init(void)
{
	// ローカル変数
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXVECTOR3 vecDir;		// ライトの方向ベクトル

	// ライトの情報をクリアする
	ZeroMemory(&m_aLight[0], sizeof(D3DLIGHT9));
	ZeroMemory(&m_aLight[1], sizeof(D3DLIGHT9));
	ZeroMemory(&m_aLight[2], sizeof(D3DLIGHT9));

	// ライトの種類を設定
	m_aLight[0].Type = D3DLIGHT_DIRECTIONAL;
	m_aLight[1].Type = D3DLIGHT_DIRECTIONAL;
	m_aLight[2].Type = D3DLIGHT_DIRECTIONAL;

	// ライトの拡散光を設定
	m_aLight[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_aLight[1].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_aLight[2].Diffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);

	// ライトの方向を設定
	vecDir = D3DXVECTOR3(0.2f, -0.4f, 0.4f);
	D3DXVec3Normalize(&vecDir, &vecDir);		// ベクトルを正規化する
	m_aLight[0].Direction = vecDir;

	// ライトの角度
	D3DXVECTOR2 distXZ = D3DXVECTOR2(vecDir.x, vecDir.z);
	float fDistXZ = D3DXVec2Length(&distXZ);
	m_fRotH = atan2f(vecDir.x, vecDir.z);
	m_fRotV = atan2f(vecDir.y, fDistXZ);

	vecDir = D3DXVECTOR3(-0.2f, 0.3f, -0.3f);
	D3DXVec3Normalize(&vecDir, &vecDir);		// ベクトルを正規化する
	m_aLight[1].Direction = vecDir;

	vecDir = D3DXVECTOR3(0.0f, 0.8f, -0.2f);
	D3DXVec3Normalize(&vecDir, &vecDir);		// ベクトルを正規化する
	m_aLight[2].Direction = vecDir;

	// ライトを設定する
	pDevice->SetLight(0, &m_aLight[0]);
	pDevice->SetLight(1, &m_aLight[1]);
	pDevice->SetLight(2, &m_aLight[2]);

	// ライトを有効にする
	pDevice->LightEnable(0, TRUE);
	pDevice->LightEnable(1, TRUE);
	pDevice->LightEnable(2, TRUE);

	return S_OK;
}

//========================================================================
// ライトの終了処理
//========================================================================
void CLight::Uninit(void)
{
}

//========================================================================
// ライトの更新処理
//========================================================================
void CLight::Update(void)
{	
#ifdef LIGHTMOVE_ENABLE
	// ローカル変数
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CDebugProc* pDebug = CManager::GetDebugProc();
	D3DXVECTOR3 vecDir;		// ライトの方向ベクトル

	if (pInputKeyboard->GetPress(DIK_J) == true)
	{// ライト回転
		m_fRotH += 0.03f;
	}
	else if(pInputKeyboard->GetPress(DIK_L) == true)
	{// ライト回転
		m_fRotH -= 0.03f;
	}
	
	if (pInputKeyboard->GetPress(DIK_K) == true)
	{// ライト回転
		m_fRotV += 0.03f;
	}
	else if(pInputKeyboard->GetPress(DIK_I) == true)
	{// ライト回転
		m_fRotV -= 0.03f;
	}

	CObject::CorrectAngle(&m_fRotH, m_fRotH);
	CObject::CorrectAngle(&m_fRotV, m_fRotV);

	vecDir.x = sinf(m_fRotH) * sinf((D3DX_PI * 0.5f) - m_fRotV);
	vecDir.y = cosf((D3DX_PI * 0.5f) - m_fRotV);
	vecDir.z = cosf(m_fRotH) * sinf((D3DX_PI * 0.5f) - m_fRotV);
	D3DXVec3Normalize(&vecDir, &vecDir);		// ベクトルを正規化する
	m_aLight[0].Direction = vecDir;

	// ライトを設定する
	pDevice->SetLight(0, &m_aLight[0]);

	pDebug->Print("\n< LIGHT >\n");
	pDebug->Print("水平角度 [ J / L ] : ( %f )\n", m_fRotH);
	pDebug->Print("水平角度 [ I / K ] : ( %f )\n", m_fRotV);
#endif
}

//========================================================================
// ライトの色設定処理
//========================================================================
void CLight::SetColor(int nIdx, D3DXCOLOR col)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	m_aLight[nIdx].Diffuse = col;

	// ライトを設定する
	pDevice->SetLight(nIdx, &m_aLight[nIdx]);
}