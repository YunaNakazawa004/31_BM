//========================================================================
// 
// パーツ [ part.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "part.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

#include "line.h"
#include "object.h"

//************************************************************************
// マクロ定義
//************************************************************************

//========================================================================
// パーツクラスのコンストラクタ
//========================================================================
CPart::CPart()
{
	// パーツクラスの値をクリア
	m_pos = DEFAULT_VECTOR3;
	m_rot = DEFAULT_VECTOR3;
	m_posOff = DEFAULT_VECTOR3;
	m_rotOff = DEFAULT_VECTOR3;
	m_posOffC = DEFAULT_VECTOR3;
	m_rotOffC = DEFAULT_VECTOR3;
	m_scale = DEFAULT_VECTOR3;
}

//========================================================================
// パーツクラスのデストラクタ
//========================================================================
CPart::~CPart()
{
}

//========================================================================
// パーツクラスの初期化処理
//========================================================================
HRESULT CPart::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	// パーツの情報の初期化
	m_pos = pos;
	m_rot = rot;
	m_posOff = pos;
	m_rotOff = rot;
	m_posOffC = pos;
	m_rotOffC = rot;
	m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	return S_OK;
}

//========================================================================
// パーツクラスの終了処理
//========================================================================
void CPart::Uninit(void)
{
}

//========================================================================
// パーツクラスの更新処理
//========================================================================
void CPart::Update(void)
{
}

//========================================================================
// パーツクラスの描画処理
//========================================================================
void CPart::Draw(void)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス
	D3DXMATRIX mtxParent;						// 親のマトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);
}

//========================================================================
// 位置設定
//========================================================================
void CPart::SetPosition(const D3DXVECTOR3 pos)
{
	// 位置を変更
	m_pos = pos;
}

//========================================================================
// 位置設定(オフセット)
//========================================================================
void CPart::SetPosOff(const D3DXVECTOR3 pos)
{
	// 位置を変更
	m_posOff = pos;
}

//========================================================================
// スケール設定
//========================================================================
void CPart::SetScale(const D3DXVECTOR3 scale)
{
	// スケールを変更
	m_scale = scale;
}

//========================================================================
// 角度設定
//========================================================================
void CPart::SetRotation(const D3DXVECTOR3 rot)
{
	// 向きを変更
	m_rot = rot;

	// X向きを調整
	CObject::CorrectAngle(&m_rot.x, m_rot.x);

	// Y向きを調整
	CObject::CorrectAngle(&m_rot.y, m_rot.y);

	// Z向きを調整
	CObject::CorrectAngle(&m_rot.z, m_rot.z);
}

//========================================================================
// 角度設定(オフセット)
//========================================================================
void CPart::SetRotOff(const D3DXVECTOR3 rot)
{
	// 向きを変更
	m_rotOff = rot;

	// X向きを調整
	CObject::CorrectAngle(&m_rotOff.x, m_rotOff.x);

	// Y向きを調整
	CObject::CorrectAngle(&m_rotOff.y, m_rotOff.y);

	// Z向きを調整
	CObject::CorrectAngle(&m_rotOff.z, m_rotOff.z);
}