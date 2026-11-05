//=============================================================================
// 
// テクスチャ [ texture.cpp ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#include "texture.h"

#include "renderer.h"
#include "manager.h"

//=============================================================================
// 静的メンバ変数宣言
//=============================================================================
int CTexture::m_nNumAll = 0;			// テクスチャの使用数
const char* CTexture::c_apFilername[TYPE_MAX] = 			// 固定テクスチャファイル名
{
	"data\\TEXTURE\\field000.jpg",
};

//=============================================================================
// テクスチャクラスのコンストラクタ
//=============================================================================
CTexture::CTexture()
{
	// テクスチャクラスの値をクリア
	memset(&m_apTexture[0], NULL, sizeof m_apTexture);
	memset(&m_pFilename[0], '\0', sizeof m_pFilename);
	memset(&m_aCopyFilename[0][0], '\0', sizeof m_aCopyFilename);
	m_nNumAll = 0;
}

//=============================================================================
// テクスチャクラスのデストラクタ
//=============================================================================
CTexture::~CTexture()
{
}

//=============================================================================
// 規定テクスチャの読み込み処理
//=============================================================================
HRESULT CTexture::Load(void)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ

	for (int nCntTex = 0; nCntTex < TYPE_MAX; nCntTex++)
	{
		// テクスチャの読み込み
		if (FAILED(D3DXCreateTextureFromFile(pDevice, c_apFilername[nCntTex], &m_apTexture[nCntTex])))
		{// 失敗した場合
			OutputDebugStringA("! ! ! 規定テクスチャの読み込みに失敗しました ! ! !\n");

			return E_FAIL;
		}

		// ファイル名を保存
		m_pFilename[nCntTex] = c_apFilername[nCntTex];

		// 総数を増やす
		m_nNumAll++;
	}

	return S_OK;
}

//=============================================================================
// テクスチャの破棄
//=============================================================================
void CTexture::Unload(void)
{
	// 全テクスチャの破棄
	for (int nCntTex = 0; nCntTex < MAX_TEXTURE; nCntTex++)
	{
		if (m_apTexture[nCntTex] != nullptr)
		{// NULLチェック
			m_apTexture[nCntTex]->Release();
			m_apTexture[nCntTex] = nullptr;
		}
	}
}

//=============================================================================
// テクスチャの設定
//=============================================================================
int CTexture::Register(const char* pFilename)
{
	if (m_nNumAll >= MAX_TEXTURE)
	{// テクスチャが満杯
		OutputDebugStringA("! ! ! テクスチャがいっぱいです ! ! !\n");

		return -1;
	}

	if (pFilename == nullptr)
	{// NULLだったら
		return -1;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ

	// 重複していないか確認
	for (int nCntTex = 0; nCntTex < m_nNumAll; nCntTex++)
	{
		if (strcmp(m_pFilename[nCntTex], pFilename) == 0)
		{// 同じファイル名があった場合
			// その番号を返す
			return nCntTex;
		}
	}

	// テクスチャのインデックスを保存
	int nIdx = m_nNumAll;

	// テクスチャの読み込み
	if (FAILED(D3DXCreateTextureFromFile(pDevice, pFilename, &m_apTexture[nIdx])))
	{// 失敗した場合
		OutputDebugStringA("! ! ! テクスチャの読み込みに失敗しました ! ! !\n");

		return -1;
	}

	// ファイル名をコピー
	strcpy(&m_aCopyFilename[nIdx][0], pFilename);

	// ファイル名を保存
	m_pFilename[nIdx] = &m_aCopyFilename[nIdx][0];

	// 総数を増やす
	m_nNumAll++;

	return nIdx;
}

//=============================================================================
// テクスチャのポインタを取得
//=============================================================================
LPDIRECT3DTEXTURE9 CTexture::GetAddress(const int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_TEXTURE)
	{// 範囲外のインデックス
		return nullptr;
	}

	return m_apTexture[nIdx];
}