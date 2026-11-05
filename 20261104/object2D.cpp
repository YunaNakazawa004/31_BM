//========================================================================
// 
// オブジェクト2D [ object2D.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "object2D.h"

#include "renderer.h"
#include "manager.h"
#include "texture.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define OBJECT_WIDTH	(180.0f)		// オブジェクトの幅
#define OBJECT_HEIGHT	(90.0f)			// オブジェクトの高さ

//========================================================================
// オブジェクト2Dクラスの生成処理
//========================================================================
CObject2D* CObject2D::Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight,
	const CObject::TYPE type, const char* pFilename, const int nPriority, const int posType)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return nullptr;
	}
#endif

	CObject2D* pObject2D = nullptr;

	if (pObject2D == nullptr)
	{// NULLチェック
		// オブジェクト2Dの生成
		pObject2D = new CObject2D(nPriority);
	}

	if (pObject2D != nullptr)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pObject2D->Init(pos, fWidth, fHeight, posType)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! オブジェクト2Dの初期化に失敗しました ! ! !\n");

			return nullptr;
		}

		// テクスチャを設定
		CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
		pObject2D->m_nIdxTexture = pTexture->Register(pFilename);

		// 種類を設定
		pObject2D->SetType(type);

		return pObject2D;
	}

	OutputDebugStringA("! ! ! オブジェクト2Dの生成に失敗しました ! ! !\n");

	return nullptr;
}

//========================================================================
// オブジェクト2Dクラスのコンストラクタ
//========================================================================
CObject2D::CObject2D(const int nPriority) :CObject(nPriority)
{
	// オブジェクト2Dクラスの値をクリア
	m_pVtxBuff = nullptr;
	m_nIdxTexture = -1;
	m_pos = DEFAULT_VECTOR3;
	m_rot = DEFAULT_VECTOR3;
	m_col = COLOR_WHITE;
	m_fAngle = 0.0f;
	m_fLength = 0.0f;
	m_bDisp = true;
}

//========================================================================
// オブジェクト2Dクラスのデストラクタ
//========================================================================
CObject2D::~CObject2D()
{
}

//========================================================================
// オブジェクト2Dクラスの初期化処理(オーバーロード)
//========================================================================
HRESULT CObject2D::Init(const D3DXVECTOR3 pos,
	const float fWidth, const float fHeight, const int posType)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	VERTEX_2D* pVtx;

	// クラスの値を初期化
	m_pos = pos;
	m_posType = posType;

	// 頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		nullptr)))
	{// もし失敗したら
		OutputDebugStringA("! ! ! 頂点バッファの生成に失敗しました ! ! !\n");

		return E_FAIL;
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	// 頂点座標の設定
	switch (m_posType)
	{
	case POS_CENTER:			// 真ん中
		// 角度
		m_fAngle = atan2f(fWidth, fHeight);

		// 対角線の長さ
		m_fLength = sqrtf(((fWidth * 2.0f) * (fWidth * 2.0f)) + ((fHeight * 2.0f) * (fHeight * 2.0f))) * 0.5f;

		pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_LEFT:				// 左
		// 角度
		m_fAngle = atan2f(fWidth * 2.0f, fHeight);

		// 対角線の長さ
		m_fLength = sqrtf(((fWidth * 2.0f * 2.0f) * (fWidth * 2.0f * 2.0f)) + ((fHeight * 2.0f) * (fHeight * 2.0f))) * 0.5f;

		pVtx[0].pos.x = m_pos.x;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_LEFT_TOP:			// 左上
		// 角度
		m_fAngle = atan2f(fWidth * 2.0f, fHeight * 2.0f);

		// 対角線の長さ
		m_fLength = sqrtf(((fWidth * 2.0f * 2.0f) * (fWidth * 2.0f * 2.0f)) + ((fHeight * 2.0f * 2.0f) * (fHeight * 2.0f * 2.0f))) * 0.5f;

		pVtx[0].pos.x = m_pos.x;
		pVtx[0].pos.y = m_pos.y;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_MID_BOTTOM:		// 真ん中下
		// 角度
		m_fAngle = atan2f(fWidth, fHeight * 2.0f);

		// 対角線の長さ
		m_fLength = sqrtf(((fWidth * 2.0f) * (fWidth * 2.0f)) + ((fHeight * 2.0f * 2.0f) * (fHeight * 2.0f * 2.0f))) * 0.5f;

		pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.y = m_pos.y;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y;
		pVtx[3].pos.z = 0.0f;

		break;
	}

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;
}

//========================================================================
// オブジェクト2Dクラスの終了処理
//========================================================================
void CObject2D::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// 自分自身を破棄
	CObject::Release();
}

//========================================================================
// オブジェクト2Dクラスの更新処理
//========================================================================
void CObject2D::Update(void)
{
}

//========================================================================
// オブジェクト2Dクラスの描画処理
//========================================================================
void CObject2D::Draw(void)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//========================================================================
// 位置設定
//========================================================================
void CObject2D::SetPosition(const D3DXVECTOR3 pos)
{
	VERTEX_2D* pVtx;

	// 位置を変更
	m_pos = pos;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	switch (m_posType)
	{
	case POS_CENTER:			// 真ん中
		pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_LEFT:				// 左
		pVtx[0].pos.x = m_pos.x;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_LEFT_TOP:			// 左上
		pVtx[0].pos.x = m_pos.x;
		pVtx[0].pos.y = m_pos.y;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_MID_BOTTOM:		// 真ん中下
		pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.y = m_pos.y;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y;
		pVtx[3].pos.z = 0.0f;

		break;
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// 位置設定 ( オーバーロード ) 
//========================================================================
void CObject2D::SetPosition(const D3DXVECTOR3 pos, const float fWidth, const float fHeight)
{
	VERTEX_2D* pVtx;

	// 位置を変更
	m_pos = pos;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	switch (m_posType)
	{
	case POS_CENTER:			// 真ん中
		// 角度
		m_fAngle = atan2f(fWidth, fHeight);

		// 対角線の長さ
		m_fLength = sqrtf(((fWidth * 2.0f) * (fWidth * 2.0f)) + ((fHeight * 2.0f) * (fHeight * 2.0f))) * 0.5f;

		pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_LEFT:				// 左
		// 角度
		m_fAngle = atan2f(fWidth * 2.0f, fHeight);

		// 対角線の長さ
		m_fLength = sqrtf(((fWidth * 2.0f * 2.0f) * (fWidth * 2.0f * 2.0f)) + ((fHeight * 2.0f) * (fHeight * 2.0f))) * 0.5f;

		pVtx[0].pos.x = m_pos.x;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_LEFT_TOP:			// 左上
		// 角度
		m_fAngle = atan2f(fWidth * 2.0f, fHeight * 2.0f);

		// 対角線の長さ
		m_fLength = sqrtf(((fWidth * 2.0f * 2.0f) * (fWidth * 2.0f * 2.0f)) + ((fHeight * 2.0f * 2.0f) * (fHeight * 2.0f * 2.0f))) * 0.5f;

		pVtx[0].pos.x = m_pos.x;
		pVtx[0].pos.y = m_pos.y;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_MID_BOTTOM:		// 真ん中下
		// 角度
		m_fAngle = atan2f(fWidth, fHeight * 2.0f);

		// 対角線の長さ
		m_fLength = sqrtf(((fWidth * 2.0f) * (fWidth * 2.0f)) + ((fHeight * 2.0f * 2.0f) * (fHeight * 2.0f * 2.0f))) * 0.5f;

		pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.y = m_pos.y;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y;
		pVtx[3].pos.z = 0.0f;

		break;
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// サイズ設定
//========================================================================
void CObject2D::SetSize(const float fLength)
{
	VERTEX_2D* pVtx;

	// 対角線の長さ
	m_fLength = fLength;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	switch (m_posType)
	{
	case POS_CENTER:			// 真ん中
		pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_LEFT:				// 左
		pVtx[0].pos.x = m_pos.x;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_LEFT_TOP:			// 左上
		pVtx[0].pos.x = m_pos.x;
		pVtx[0].pos.y = m_pos.y;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_MID_BOTTOM:		// 真ん中下
		pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.y = m_pos.y;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y;
		pVtx[3].pos.z = 0.0f;

		break;
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// 角度設定
//========================================================================
void CObject2D::SetRotation(const D3DXVECTOR3 rot)
{
	VERTEX_2D* pVtx;

	// 向きを変更
	m_rot = rot;

	// 向きを調整
	CorrectAngle(&m_rot.z, m_rot.z);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	switch (m_posType)
	{
	case POS_CENTER:			// 真ん中
		pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_LEFT:				// 左
		pVtx[0].pos.x = m_pos.x;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_LEFT_TOP:			// 左上
		pVtx[0].pos.x = m_pos.x;
		pVtx[0].pos.y = m_pos.y;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x;
		pVtx[2].pos.y = m_pos.y + cosf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y + cosf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.z = 0.0f;

		break;

	case POS_MID_BOTTOM:		// 真ん中下
		pVtx[0].pos.x = m_pos.x + sinf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.y = m_pos.y + cosf(D3DX_PI + m_fAngle + m_rot.z) * m_fLength;
		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.x = m_pos.x + sinf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.y = m_pos.y + cosf(D3DX_PI - m_fAngle + m_rot.z) * m_fLength;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.x = m_pos.x + sinf(-m_fAngle + m_rot.z) * m_fLength;
		pVtx[2].pos.y = m_pos.y;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.x = m_pos.x + sinf(m_fAngle + m_rot.z) * m_fLength;
		pVtx[3].pos.y = m_pos.y;
		pVtx[3].pos.z = 0.0f;

		break;
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// 頂点カラー設定
//========================================================================
void CObject2D::SetColor(const D3DXCOLOR col)
{
	m_col = col;

	VERTEX_2D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// テクスチャ座標設定
//========================================================================
void CObject2D::SetTexUV(const int nPatternAnim, const int nTexWidth, const int nTexHeight, const float fScrollX, const float fScrollY)
{
	VERTEX_2D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(nPatternAnim * (1.0f / (float)nTexWidth) + fScrollX, (nPatternAnim / nTexWidth) * (1.0f / (float)nTexHeight) + fScrollY);
	pVtx[1].tex = D3DXVECTOR2((nPatternAnim + 1) * (1.0f / (float)nTexWidth) + fScrollX, (nPatternAnim / nTexWidth) * (1.0f / (float)nTexHeight) + fScrollY);
	pVtx[2].tex = D3DXVECTOR2(nPatternAnim * (1.0f / (float)nTexWidth) + fScrollX, ((nPatternAnim / nTexWidth) + 1) * (1.0f / (float)nTexHeight) + fScrollY);
	pVtx[3].tex = D3DXVECTOR2((nPatternAnim + 1) * (1.0f / (float)nTexWidth) + fScrollX, ((nPatternAnim / nTexWidth) + 1) * (1.0f / (float)nTexHeight) + fScrollY);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// テクスチャ座標設定 ( オーバーロード )
//========================================================================
void CObject2D::SetTexUV(const float fStartX, const float fEndX, const float fStartY, const float fEndY)
{
	VERTEX_2D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(fStartX, fStartY);
	pVtx[1].tex = D3DXVECTOR2(fEndX, fStartY);
	pVtx[2].tex = D3DXVECTOR2(fStartX, fEndY);
	pVtx[3].tex = D3DXVECTOR2(fEndX, fEndY);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// マウスとの当たり判定
//========================================================================
bool CObject2D::MouseCollision(const POINT MousePos, const float fWidth, const float fHeight)
{
	switch (m_posType)
	{
	case POS_CENTER:			// 真ん中
		if (m_pos.x - fWidth < MousePos.x && m_pos.x + fWidth > MousePos.x &&
			m_pos.y - fHeight < MousePos.y && m_pos.y + fHeight > MousePos.y)
		{// 中にいる
			return true;
		}

		break;

	case POS_LEFT:				// 左
		if (m_pos.x < MousePos.x && m_pos.x + fWidth * 2.0f > MousePos.x &&
			m_pos.y - fHeight < MousePos.y && m_pos.y + fHeight > MousePos.y)
		{// 中にいる
			return true;
		}

		break;

	case POS_LEFT_TOP:			// 左上
		if (m_pos.x < MousePos.x && m_pos.x + fWidth * 2.0f > MousePos.x &&
			m_pos.y < MousePos.y && m_pos.y + fHeight * 2.0f > MousePos.y)
		{// 中にいる
			return true;
		}

		break;

	case POS_MID_BOTTOM:		// 真ん中下
		if (m_pos.x - fWidth < MousePos.x && m_pos.x + fWidth > MousePos.x &&
			m_pos.y - fHeight * 2.0f < MousePos.y && m_pos.y > MousePos.y)
		{// 中にいる
			return true;
		}

		break;
	}

	return false;
}