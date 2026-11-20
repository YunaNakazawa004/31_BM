//========================================================================
// 
// メッシュ [ mesh.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "mesh.h"

#include "manager.h"
#include "debugproc.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"

#include "objectX.h"
#include "meshwall.h"
#include "meshfield.h"
#include "meshring.h"
#include "meshcylinder.h"
#include "meshdome.h"

#include "effect_batching.h"
#include "bone.h"

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************

//========================================================================
// 生成処理
//========================================================================
CMesh* CMesh::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size, const int type, const bool bCalc)
{
	CMesh* pMesh = NULL;

	if (pMesh == NULL)
	{// NULLチェック
		// メッシュの生成
		switch (type)
		{
		case MESHWALL:			// メッシュウォール
			pMesh = CMeshWall::Create(pos, rot, block, size, nullptr, CObject::TYPE_MESH, 3);

			break;

		case MESHFIELD:			// メッシュフィールド
			pMesh = CMeshField::Create(pos, rot, block, size, CObject::TYPE_MESH, nullptr, 3);

			break;

		case MESHRING:			// メッシュリング
			pMesh = CMeshRing::Create(pos, DEFAULT_VECTOR3, rot, block, size, COLOR_WHITE, nullptr, CObject::TYPE_MESH, 3);

			break;

		case MESHCYLINDER:		// メッシュシリンダー
			pMesh = CMeshCylinder::Create(pos, rot, block, size, COLOR_WHITE, false, nullptr, CObject::TYPE_MESH, 3);

			break;

		case MESHDOME:			// メッシュドーム
			pMesh = CMeshDome::Create(pos, rot, block, size.x, nullptr, CObject::TYPE_MESH, 3);

			break;
		}
	}

	if (pMesh != NULL)
	{// NULLチェック
		pMesh->m_pos = pos;
		pMesh->m_rot = rot;
		pMesh->m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		pMesh->m_bCalc = bCalc;

		return pMesh;
	}

	OutputDebugStringA("! ! ! メッシュ生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// メッシュのコンストラクタ
//========================================================================
CMesh::CMesh(const int nPriority) :CObject(nPriority)
{
	// 値のクリア
	m_pos = DEFAULT_VECTOR3;
	m_rot = DEFAULT_VECTOR3;
	m_scale = DEFAULT_VECTOR3;
	m_nNumVtxAll = 0;
	m_nNumIdxAll = 0;
	m_nVtxIdx = 0;
	m_vnShadow.clear();
	m_nNumShadowVtx = 0;
	m_pPosOff = nullptr;
	m_pBoneRef = nullptr;
	m_pWeight = nullptr;
	m_nNumBone = 0;
	m_type = 0;
	m_bBoneDraw = true;
	m_bVtxDisp = true;
	m_bCalc = true;
}

//========================================================================
// メッシュのデストラクタ
//========================================================================
CMesh::~CMesh()
{
}

//========================================================================
// メッシュの初期化処理
//========================================================================
HRESULT CMesh::Init(void)
{
	return S_OK;
}

//========================================================================
// メッシュの終了処理
//========================================================================
void CMesh::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// インデックスバッファの破棄
	if (m_pIdxBuff != nullptr)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = nullptr;
	}

	// 頂点位置のオフセットの破棄
	if (m_pPosOff != nullptr)
	{
		delete[] m_pPosOff;
		m_pPosOff = nullptr;
	}

	// 影響を受けるボーンの破棄
	if (m_pBoneRef != nullptr)
	{
		delete[] m_pBoneRef;
		m_pBoneRef = nullptr;
	}

	// ウェイトの破棄
	if (m_pWeight != nullptr)
	{
		delete[] m_pWeight;
		m_pWeight = nullptr;
	}

	// 自分自身を破棄
	CObject::Release();
}

//========================================================================
// メッシュの更新処理
//========================================================================
void CMesh::Update(void)
{
	if (m_bCalc == false)
	{// 何もしない
		return;
	}

	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CInputMouse* pInputMouse = CManager::GetInputMouse();				// マウス入力の取得
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	CEffectBatching* pEffectBatching = CManager::GetEffectBatching();	// バッチ描画エフェクトの取得
	VERTEX_3D* pVtx;						// 頂点情報へのポインタ

	if (m_pVtxBuff == nullptr)
	{// NULLチェック
		return;
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntVtx = 0; nCntVtx < m_nNumVtxAll; nCntVtx++)
	{
		D3DXCOLOR col = COLOR_RED;
		D3DXVECTOR3 pos;
		D3DXVec3TransformCoord(&pos, &pVtx[nCntVtx].pos, &m_mtxWorld);

		for (int nCntShadow = 0; nCntShadow < m_nNumShadowVtx; nCntShadow++)
		{
			if (nCntVtx == m_vnShadow.at(nCntShadow))
			{// 影の頂点
				col = COLOR_YELLOW;
			}
		}

		if (m_nVtxIdx == nCntVtx)
		{// 操作している頂点
			col = COLOR_GREEN;
		}
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// メッシュの描画処理
//========================================================================
void CMesh::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxScale, mtxTrans, mtxBone;			// 計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	if (m_pBoneRef != nullptr && m_pWeight != nullptr && m_bBoneDraw == true)
	{// NULLチェック
		//D3DXVECTOR3* pPosCorrect = new D3DXVECTOR3[m_nNumVtxAll];
		//VERTEX_3D* pVtx;

		//// 初期化
		//for (int nCntVtx = 0; nCntVtx < m_nNumVtxAll; nCntVtx++)
		//{
		//	pPosCorrect[nCntVtx] = DEFAULT_VECTOR3;
		//}

		//for (int nCntBone = 0; nCntBone < pConsole->GetNumBone(); nCntBone++)
		//{
		//	// ボーンの描画（マトリックスの計算）
		//	pConsole->GetBone(nCntBone)->Draw();

		//	// ボーンのマトリックスを取得
		//	mtxBone = pConsole->GetBone(nCntBone)->GetMtxWorld();

		//	// 全頂点データの算出 
		//	for (int nCntVtx = 0; nCntVtx < m_nNumVtxAll; nCntVtx++)
		//	{
		//		if (m_pBoneRef[0 * m_nNumVtxAll + nCntVtx] != nCntBone && m_pBoneRef[1 * m_nNumVtxAll + nCntVtx] != nCntBone)
		//		{// 全く影響しない頂点は飛ばす
		//			continue;
		//		}

		//		// 計算した位置
		//		D3DXVECTOR3 pos = DEFAULT_VECTOR3;

		//		// 頂点座標に変換
		//		D3DXVec3TransformCoord(&pos, &m_pPosOff[nCntBone * m_nNumVtxAll + nCntVtx], &mtxBone);

		//		// 頂点データの合計値を算出
		//		if (m_pBoneRef[0 * m_nNumVtxAll + nCntVtx] == nCntBone)
		//		{// 1つ目のボーン番号
		//			pPosCorrect[nCntVtx] += pos * m_pWeight[0 * m_nNumVtxAll + nCntVtx];
		//		}
		//		else if (m_pBoneRef[1 * m_nNumVtxAll + nCntVtx] == nCntBone)
		//		{// 2つ目のボーン番号
		//			pPosCorrect[nCntVtx] += pos * m_pWeight[1 * m_nNumVtxAll + nCntVtx];
		//		}
		//	}
		//}

		//// 頂点バッファをロックし、頂点情報へのポインタを取得
		//m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		//// 頂点座標を設定
		//for (int nCntVtx = 0; nCntVtx < m_nNumVtxAll; nCntVtx++)
		//{
		//	pVtx[nCntVtx].pos = pPosCorrect[nCntVtx];
		//}

		//if (pPosCorrect != nullptr)
		//{// NULLチェック
		//	delete[] pPosCorrect;
		//	pPosCorrect = nullptr;
		//}

		//// 頂点バッファをアンロックする
		//m_pVtxBuff->Unlock();
	}

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);
}

//========================================================================
// メッシュの全てを破棄
//========================================================================
void CMesh::UninitAll(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// インデックスバッファの破棄
	if (m_pIdxBuff != nullptr)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = nullptr;
	}

	// 頂点位置のオフセットの破棄
	if (m_pPosOff != nullptr)
	{
		delete[] m_pPosOff;
		m_pPosOff = nullptr;
	}

	// 影響を受けるボーンの破棄
	if (m_pBoneRef != nullptr)
	{
		delete[] m_pBoneRef;
		m_pBoneRef = nullptr;
	}

	// ウェイトの破棄
	if (m_pWeight != nullptr)
	{
		delete[] m_pWeight;
		m_pWeight = nullptr;
	}

	// 自分自身を破棄
	CObject::Release();
}

//========================================================================
// 位置設定
//========================================================================
void CMesh::SetPosition(const D3DXVECTOR3 pos)
{
	// 位置を変更
	m_pos = pos;
}

//========================================================================
// 角度設定
//========================================================================
void CMesh::SetRotation(const D3DXVECTOR3 rot)
{
	// X向きを調整
	CorrectAngle(&m_rot.x, m_rot.x);

	// Y向きを調整
	CorrectAngle(&m_rot.y, m_rot.y);

	// Z向きを調整
	CorrectAngle(&m_rot.z, m_rot.z);

	// 向きを変更
	m_rot = rot;
}

//========================================================================
// スケール設定
//========================================================================
void CMesh::SetScale(const D3DXVECTOR3 scale)
{
	// スケールを変更
	m_scale = scale;
}

//========================================================================
// 頂点バッファを生成
//========================================================================
HRESULT CMesh::CreateVtxBuff(const int nValue)
{
	if (nValue < 0)
	{// 範囲外の数値
		return E_FAIL;
	}

	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// 頂点バッファの生成
	HRESULT hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * nValue,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点数を保存
	m_nNumVtxAll = nValue;
	m_nVtxIdx = 0;

	if (m_bCalc == false)
	{// 一足先に抜ける
		return hr;
	}

	return hr;
}

//========================================================================
// 頂点バッファを生成
//========================================================================
HRESULT CMesh::CreateVtxBuffMulti(const int nValue)
{
	if (nValue < 0)
	{// 範囲外の数値
		return E_FAIL;
	}

	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// 頂点バッファの生成
	HRESULT hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_3D_MULTI) * nValue,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D_MULTI,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点数を保存
	m_nNumVtxAll = nValue;
	m_nVtxIdx = 0;

	return hr;
}

//========================================================================
// インデックスバッファを生成
//========================================================================
HRESULT CMesh::CreateIdxBuff(const int nValue)
{
	if (nValue < 0)
	{// 範囲外の数値
		return E_FAIL;
	}

	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// インデックスバッファの生成
	HRESULT hr = pDevice->CreateIndexBuffer(sizeof(DWORD) * nValue,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	// インデックス数を保存
	m_nNumIdxAll = nValue;

	return hr;
}

//========================================================================
// 位置のオフセットを生成
//========================================================================
void CMesh::CreatePosOff(const int nNumBone)
{
	if (m_pPosOff == nullptr)
	{// NULLチェック
		m_pPosOff = new D3DXVECTOR3[nNumBone * m_nNumVtxAll];

		if (m_pPosOff != nullptr)
		{// NULLチェック
			for (int nCnt = 0; nCnt < nNumBone * m_nNumVtxAll; nCnt++)
			{
				m_pPosOff[nCnt] = DEFAULT_VECTOR3;
			}
		}
	}
}

//========================================================================
// ボーン関連を生成
//========================================================================
void CMesh::CreateBoneInfo(const int nNumBone)
{
	if (m_pBoneRef == nullptr)
	{// NULLチェック
		m_pBoneRef = new int[nNumBone * m_nNumVtxAll];

		if (m_pBoneRef != nullptr)
		{// NULLチェック
			for (int nCnt = 0; nCnt < nNumBone * m_nNumVtxAll; nCnt++)
			{
				m_pBoneRef[nCnt] = 0;
			}
		}
	}

	if (m_pWeight == nullptr)
	{// NULLチェック
		m_pWeight = new float[nNumBone * m_nNumVtxAll];

		if (m_pWeight != nullptr)
		{// NULLチェック
			for (int nCnt = 0; nCnt < nNumBone * m_nNumVtxAll; nCnt++)
			{
				m_pWeight[nCnt] = 0.0f;
			}
		}
	}

	m_nNumBone = nNumBone;
}

//========================================================================
// 影の頂点を生成
//========================================================================
void CMesh::CreateShadowVtx(const int nValue)
{
	if (nValue < 0 || nValue >= m_nNumVtxAll)
	{// 範囲外の数値
		return;
	}

	// 生成
	m_vnShadow.push_back(nValue);

	m_nNumShadowVtx++;
}

//========================================================================
// 頂点位置のオフセットを設定
//========================================================================
void CMesh::SetPosOff(const int nIdxBone, const D3DXVECTOR3 posBone)
{
	if (m_pPosOff == nullptr)
	{// NULLチェック
		return;
	}

	VERTEX_3D* pVtx;						// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < m_nNumVtxAll; nCnt++)
	{
		m_pPosOff[nCnt + nIdxBone * m_nNumVtxAll] = pVtx[nCnt].pos - posBone;
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// 選択された頂点の座標
//========================================================================
D3DXVECTOR3 CMesh::GetVtxPos(void)
{
	D3DXVECTOR3 pos = DEFAULT_VECTOR3;
	VERTEX_3D* pVtx;						// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pos = pVtx[m_nVtxIdx].pos;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return pos;
}

//========================================================================
// メッシュの書き込み処理
//========================================================================
HRESULT CMesh::WriteTxtData(const char* pFilename)
{
	// ファイルオープン
	std::ofstream filetxt(pFilename);

	if (filetxt.is_open() == true)
	{// ファイルが開けた
		VERTEX_3D* pVtx;
		DWORD* pIdx;
		std::string str;		// 情報書き込み用

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtx = 0; nCntVtx < m_nNumVtxAll; nCntVtx++)
		{
			D3DXVECTOR3 pos;
			D3DXVec3TransformCoord(&pos, &pVtx[nCntVtx].pos, &m_mtxWorld);

			str.clear();

			str = "\tVERTEX\n";
			str += "\t\tPOS = ";
			str += std::to_string(pos.x);
			str += " ";
			str += std::to_string(pos.y);
			str += " ";
			str += std::to_string(pos.z);
			str += "\n";

			if (m_pBoneRef != nullptr && m_pWeight != nullptr)
			{// NULLチェック
				str += "\t\tBONE_REF = ";
				str += std::to_string(m_pBoneRef[nCntVtx]);
				str += " ";
				str += std::to_string(m_pBoneRef[m_nNumVtxAll + nCntVtx]);
				str += "\n";
				str += "\t\tWEIGHT_VALUE = ";
				str += std::to_string(m_pWeight[nCntVtx]);
				str += " ";
				str += std::to_string(m_pWeight[m_nNumVtxAll + nCntVtx]);
				str += "\n";
			}
			else
			{// もしNULLだったら
				str += "\t\tBONE_REF = 0 1\n";
				str += "\t\tWEIGHT_VALUE = 1.0 0.0\n";
			}

			str += "\t\tTEX = ";
			str += std::to_string(pVtx[nCntVtx].tex.x);
			str += " ";
			str += std::to_string(pVtx[nCntVtx].tex.y);
			str += "\n";
			str += "\tEND_VERTEX\n\n";

			filetxt.write(str.c_str(), str.size());
		}

		// 頂点バッファをアンロックする
		GetVtxBuff()->Unlock();

		// インデックスバッファをロックし、頂点番号データへのポインタを取得
		GetIdxBuff()->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntIdx = 0; nCntIdx < m_nNumIdxAll; nCntIdx++)
		{
			str.clear();
			str = std::to_string(pIdx[nCntIdx]);
			str += "\n";

			filetxt.write(str.c_str(), str.size());
		}

		// インデックスバッファをアンロックする
		GetIdxBuff()->Unlock();

		// 影に関する書き込み
		str.clear();
		str = "NUM_SHADOW_VTX = ";
		str += std::to_string(m_nNumShadowVtx);
		str += "\n";

		filetxt.write(str.c_str(), str.size());

		for (int nCntShadow = 0; nCntShadow < m_nNumShadowVtx; nCntShadow++)
		{
			str.clear();
			str += "\tSHADOW_VTX = ";
			str += std::to_string(m_vnShadow.at(nCntShadow));
			str += "\n";

			filetxt.write(str.c_str(), str.size());
		}

		// 現在時刻を取得
		std::ostringstream oss;
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);

		// ローカル時刻にする
		std::tm tm_now{};
		localtime_s(&tm_now, &now_c);		// Windowsの場合

		// フォーマット
		oss << std::put_time(&tm_now, "%Y/%m/%d %H:%M:%S");

		// 現在時刻を入れておく
		str.clear();
		str = "#";
		str += oss.str();
		str += "\n";
		filetxt.write(str.c_str(), str.size());

		// ファイルを閉じる
		filetxt.close();
	}
	else
	{// ファイルが開けなかった
		OutputDebugStringA("! ! ! ファイルを開けませんでした ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}

//========================================================================
// メッシュの書き込み処理
//========================================================================
HRESULT CMesh::WriteBinData(const char* pFilename)
{
	// ファイルオープン
	std::ofstream filebin(pFilename, std::ios_base::out | std::ios_base::binary);

	if (filebin.is_open() == true)
	{// ファイルが開けた
		VERTEX_3D* pVtx;
		DWORD* pIdx;

		// 書き出す
		filebin.write((const char*)&m_type, sizeof(m_type));
		filebin.write((const char*)&m_pos, sizeof(m_pos));
		filebin.write((const char*)&m_rot, sizeof(m_rot));
		filebin.write((const char*)&m_block, sizeof(m_block));
		filebin.write((const char*)&m_size, sizeof(m_size));
		filebin.write((const char*)&m_nNumVtxAll, sizeof(m_nNumVtxAll));
		filebin.write((const char*)&m_nNumIdxAll, sizeof(m_nNumIdxAll));

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtx = 0; nCntVtx < m_nNumVtxAll; nCntVtx++)
		{
			filebin.write((const char*)&pVtx[nCntVtx], sizeof(*pVtx));
		}

		// 頂点バッファをアンロックする
		GetVtxBuff()->Unlock();

		// インデックスバッファをロックし、頂点番号データへのポインタを取得
		GetIdxBuff()->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntIdx = 0; nCntIdx < m_nNumIdxAll; nCntIdx++)
		{
			filebin.write((const char*)&pIdx[nCntIdx], sizeof(*pIdx));
		}

		// インデックスバッファをアンロックする
		GetIdxBuff()->Unlock();

		// ボーン関係のものを書き込み
		if (m_pBoneRef != nullptr && m_pWeight != nullptr)
		{// NULLチェック
			filebin.write((const char*)&m_nNumBone, sizeof(m_nNumBone));

			for (int nCntBoneVtx = 0; nCntBoneVtx < m_nNumBone * m_nNumVtxAll; nCntBoneVtx++)
			{
				filebin.write((const char*)&m_pBoneRef[nCntBoneVtx], sizeof(int));
			}

			for (int nCntBoneVtx = 0; nCntBoneVtx < m_nNumBone * m_nNumVtxAll; nCntBoneVtx++)
			{
				filebin.write((const char*)&m_pWeight[nCntBoneVtx], sizeof(float));
			}
		}

		// 影に関する書き込み
		filebin.write((const char*)&m_nNumShadowVtx, sizeof(m_nNumShadowVtx));

		for (int nCntShadow = 0; nCntShadow < m_nNumShadowVtx; nCntShadow++)
		{
			filebin.write((const char*)&m_vnShadow.at(nCntShadow), sizeof(m_vnShadow.at(nCntShadow)));
		}

		// ファイルを閉じる
		filebin.close();
	}
	else
	{// ファイルが開けなかった
		OutputDebugStringA("! ! ! ファイルを開けませんでした ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}