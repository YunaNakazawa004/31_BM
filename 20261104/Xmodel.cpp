//=============================================================================
// 
// モデル管理 [ Xmodel.cpp ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#include "Xmodel.h"

#include "renderer.h"
#include "manager.h"

//=============================================================================
// 静的メンバ変数宣言
//=============================================================================
int CXModel::m_nNumAll = 0;			// モデル管理の使用数

//=============================================================================
// モデル管理クラスのコンストラクタ
//=============================================================================
CXModel::CXModel()
{
	// モデル管理クラスの値をクリア
	memset(&m_apMesh[0], NULL, sizeof m_apMesh);
	memset(&m_apBuffMat[0], NULL, sizeof m_apBuffMat);
	memset(&m_adwNumMat[0], NULL, sizeof m_adwNumMat);
	memset(&m_pFilename[0], '\0', sizeof m_pFilename);
	memset(&m_aCopyFilename[0][0], '\0', sizeof m_aCopyFilename);
	m_nNumAll = 0;
}

//=============================================================================
// モデル管理クラスのデストラクタ
//=============================================================================
CXModel::~CXModel()
{
}

//=============================================================================
// 規定モデル管理の読み込み処理
//=============================================================================
HRESULT CXModel::Load(void)
{
	return S_OK;
}

//=============================================================================
// モデル管理の破棄
//=============================================================================
void CXModel::Unload(void)
{
	// 全モデル管理の破棄
	for (int nCntTex = 0; nCntTex < MAX_MODEL; nCntTex++)
	{
		// メッシュの破棄
		if (m_apMesh[nCntTex] != nullptr)
		{// NULLチェック
			m_apMesh[nCntTex]->Release();
			m_apMesh[nCntTex] = nullptr;
		}

		// マテリアルの破棄
		if (m_apBuffMat[nCntTex] != nullptr)
		{// NULLチェック
			m_apBuffMat[nCntTex]->Release();
			m_apBuffMat[nCntTex] = nullptr;
		}
	}
}

//=============================================================================
// モデル管理の設定
//=============================================================================
int CXModel::Register(const char* pFilename)
{
	if (m_nNumAll >= MAX_MODEL)
	{// モデルが満杯
		OutputDebugStringA("! ! ! モデルがいっぱいです ! ! !\n");

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

	// モデル管理のインデックスを保存
	int nIdx = m_nNumAll;

	// Xファイルの読み込み
	if (FAILED(D3DXLoadMeshFromX(pFilename,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		nullptr,
		&m_apBuffMat[nIdx],
		nullptr,
		&m_adwNumMat[nIdx],
		&m_apMesh[nIdx])))
	{// ファイルの読み込みに失敗
		OutputDebugStringA("! ! ! Xファイルの読み込みに失敗しました ! ! !\n");

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
// モデルのメッシュを取得
//=============================================================================
LPD3DXMESH CXModel::GetMesh(const int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_MODEL)
	{// 範囲外のインデックス
		return nullptr;
	}

	return m_apMesh[nIdx];
}

//=============================================================================
// モデルのマテリアルを取得
//=============================================================================
LPD3DXBUFFER CXModel::GetBuffMat(const int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_MODEL)
	{// 範囲外のインデックス
		return nullptr;
	}

	return m_apBuffMat[nIdx];
}

//=============================================================================
// モデルのマテリアルの数を取得
//=============================================================================
DWORD CXModel::GetNumMat(const int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_MODEL)
	{// 範囲外のインデックス
		return -1;
	}

	return m_adwNumMat[nIdx];
}