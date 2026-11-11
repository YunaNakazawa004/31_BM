//========================================================================
// 
// 鳥 [ bird.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "bird.h"

#include "renderer.h"
#include "manager.h"
#include "debugproc.h"
#include "input.h"
#include "sound.h"
#include "texture.h"

#include "camera.h"
#include "model.h"
#include "bone.h"
#include "motion.h"

#include "effect3D.h"
#include "particle3D.h"
#include "object3D.h"

#include <iostream>
#include <fstream>
#include <string>

//************************************************************************
// マクロ定義
//************************************************************************
#define VTX_NUM			(10)			// 頂点数

//========================================================================
// 鳥クラスの生成処理
//========================================================================
CBird* CBird::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return NULL;
	}
#endif

	CBird* pBird = NULL;

	if (pBird == NULL)
	{// NULLチェック
		// 鳥の生成
		pBird = new CBird;
	}

	if (pBird != NULL)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pBird->Init(pos, rot)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! 鳥の初期化に失敗しました ! ! !\n");

			return NULL;
		}

		// 種類を設定
		pBird->SetType(TYPE_PLAYER);

		return pBird;
	}

	OutputDebugStringA("! ! ! 鳥の生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// 鳥クラスのコンストラクタ
//========================================================================
CBird::CBird(const int nPriority) :CObject(nPriority)
{
	// 鳥クラスの値をクリア
	m_pVtxBuff = nullptr;
	m_vpBone.clear();
	m_pPosOff = nullptr;
	m_pIdxRef = nullptr;
	m_pWeightValue = nullptr;
	m_nNumBone = 0;
	m_nNumVtx = 0;
	m_nNumPoly = 0;
	m_pMotion = nullptr;
	m_pos = DEFAULT_VECTOR3;
	m_posOld = DEFAULT_VECTOR3;
	m_move = DEFAULT_VECTOR3;
	m_rot = DEFAULT_VECTOR3;
	m_rotDest = DEFAULT_VECTOR3;
	m_scale = DEFAULT_VECTOR3;
	m_col = COLOR_WHITE;
	m_fRadius = 0.0f;
	m_fHeight = 0.0f;
	m_state = STATE_NONE;
	m_nCounterState = 0;
	m_bDisp = true;
}

//========================================================================
// 鳥クラスのデストラクタ
//========================================================================
CBird::~CBird()
{
}

//========================================================================
// 鳥クラスの初期化処理(オーバーロード)
//========================================================================
HRESULT CBird::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	CRenderer* pRenderer = CManager::GetRenderer();				// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ

	// 鳥の情報の初期化
	m_pos = pos;
	m_posOld = pos;
	m_rot = rot;
	m_rotDest = rot;
	m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_state = STATE_APPEAR;

	// モーションを生成/初期化
	if (m_pMotion == NULL)
	{// NULLチェック
		m_pMotion = CMotion::Create();
	}

	// 鳥を設定
	if (FAILED(SetBone("data\\bird.txt")))
	{// もし失敗したら
		OutputDebugStringA("! ! ! 鳥の設定に失敗しました ! ! !\n");

		return E_FAIL;
	}

	// モーションに必要なボーンの設定
	m_pMotion->SetPart((CPart**)&m_vpBone.at(0), m_nNumBone);

	// 初期モーションの設定
	m_pMotion->Set(MOTIONTYPE_NEUTRAL, false, 0);

	return S_OK;
}

//========================================================================
// 鳥クラスの終了処理
//========================================================================
void CBird::Uninit(void)
{
	// ボーンの破棄
	for (int nCntBone = 0; nCntBone < m_nNumBone; nCntBone++)
	{
		if (m_vpBone.at(nCntBone) != nullptr)
		{// NULLチェック
			// 終了処理
			m_vpBone.at(nCntBone)->Uninit();

			delete[] m_vpBone.at(nCntBone);
			m_vpBone.at(nCntBone) = nullptr;
		}
	}

	// オフセット位置の破棄
	if (m_pPosOff != nullptr)
	{// NULLチェック
		delete[] m_pPosOff;
		m_pPosOff = nullptr;
	}

	// ボーン番号の破棄
	if (m_pIdxRef != nullptr)
	{// NULLチェック
		delete[] m_pIdxRef;
		m_pIdxRef = nullptr;
	}

	// ウェイトの破棄
	if (m_pWeightValue != nullptr)
	{// NULLチェック
		delete[] m_pWeightValue;
		m_pWeightValue = nullptr;
	}

	// モーションの破棄
	if (m_pMotion != nullptr)
	{// NULLチェック
		// 終了処理
		m_pMotion->Uninit();

		delete m_pMotion;
		m_pMotion = nullptr;
	}

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
// 鳥クラスの更新処理
//========================================================================
void CBird::Update(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	D3DXVECTOR3 rot0 = m_vpBone.at(0)->GetRotation();
	D3DXVECTOR3 rot1 = m_vpBone.at(1)->GetRotation();

	if (pInputKeyboard->GetPress(DIK_T))
	{
		rot0.x += 0.03f;
	}
	else if (pInputKeyboard->GetPress(DIK_G))
	{
		rot0.x -= 0.03f;
	}

	if (pInputKeyboard->GetPress(DIK_Y))
	{
		rot1.x += 0.03f;
	}
	else if (pInputKeyboard->GetPress(DIK_H))
	{
		rot1.x -= 0.03f;
	}

	m_vpBone.at(0)->SetRotation(rot0);
	m_vpBone.at(1)->SetRotation(rot1);

	// モーションの更新処理
	m_pMotion->Update();
}

//========================================================================
// 鳥クラスの描画処理
//========================================================================
void CBird::Draw(void)
{
	if (m_bDisp == false)
	{// 表示しない場合
		return;
	}

	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	CTexture* pTexture = CManager::GetTexture();			// テクスチャへのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxScale, mtxBone;			// 計算用マトリックス
	VERTEX_3D* pVtx;

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

	D3DXVECTOR3 posCorrect[VTX_NUM];
	memset(&posCorrect[0], 0, sizeof posCorrect);

	for (int nCntBone = 0; nCntBone < 2; nCntBone++)
	{
		// ボーンの描画（マトリックスの計算）
		m_vpBone.at(nCntBone)->Draw();

		// ボーンのマトリックスを取得
		mtxBone = m_vpBone.at(nCntBone)->GetMtxWorld();

		// 全頂点データの算出
		for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
		{
			if (m_pIdxRef[0 * m_nNumVtx + nCntVtx] != nCntBone && m_pIdxRef[1 * m_nNumVtx + nCntVtx] != nCntBone)
			{// 全く影響しない頂点は飛ばす
				continue;
			}

			// 計算した位置
			D3DXVECTOR3 pos = DEFAULT_VECTOR3;

			// 頂点座標に変換
			D3DXVec3TransformCoord(&pos, &m_pPosOff[nCntBone * m_nNumVtx + nCntVtx], &mtxBone);

			// 頂点データの合計値を算出
			if (m_pIdxRef[0 * m_nNumVtx + nCntVtx] == nCntBone)
			{// 1つ目のボーン番号
				posCorrect[nCntVtx] += pos * m_pWeightValue[0 * m_nNumVtx + nCntVtx];
			}
			else if (m_pIdxRef[1 * m_nNumVtx + nCntVtx] == nCntBone)
			{// 2つ目のボーン番号
				posCorrect[nCntVtx] += pos * m_pWeightValue[1 * m_nNumVtx + nCntVtx];
			}
		}
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標を設定
	for (int nCntVtx = 0; nCntVtx < m_nNumVtx; nCntVtx++)
	{
		pVtx[nCntVtx].pos = posCorrect[nCntVtx];
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, nullptr);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,		// 描画する最初の頂点インデックス
		m_nNumPoly);		// 描画するプリミティブ数
}

//========================================================================
// 位置設定
//========================================================================
void CBird::SetPosition(const D3DXVECTOR3 pos)
{
	// 位置を変更
	m_pos = pos;
}

//========================================================================
// スケール設定
//========================================================================
void CBird::SetScale(const D3DXVECTOR3 scale)
{
	// スケールを変更
	m_scale = scale;
}

//========================================================================
// 角度設定
//========================================================================
void CBird::SetRotation(const D3DXVECTOR3 rot)
{
	// 向きを変更
	m_rot = rot;
	m_rotDest = rot;

	// X向きを調整
	CorrectAngle(&m_rot.x, m_rot.x);

	// Y向きを調整
	CorrectAngle(&m_rot.y, m_rot.y);

	// Z向きを調整
	CorrectAngle(&m_rot.z, m_rot.z);
}

//========================================================================
// 頂点カラー設定
//========================================================================
void CBird::SetColor(const D3DXCOLOR col)
{
	// 色を変更
	m_col = col;
}

//========================================================================
// 外部ファイルからボーンデータを読み込む
//========================================================================
HRESULT CBird::SetBone(const char* pFilename)
{
	// ローカル変数宣言
	CRenderer* pRenderer = CManager::GetRenderer();				// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスへのポインタ
	VERTEX_3D* pVtx;
	std::string filename = pFilename;	// 読み込むファイル名
	std::fstream file(filename);		// ファイル
	std::string line = {};				// テキストを読み込む

	// テクスチャ読み込み用の変数
	int nNumTexture = 0;				// テクスチャ総数
	int nCntTexture = 0;				// 
	std::string TextureName = {};		// テクスチャの名前

	// ボーンセット用の変数
	int nNumBone = 0;					// 読み込むボーン数
	int nIdxParent = -1;				// 親のボーン番号
	D3DXVECTOR3 pos = DEFAULT_VECTOR3;	// 位置
	D3DXVECTOR3 rot = DEFAULT_VECTOR3;	// 向き

	// 頂点設定用の変数
	int nNumVtx = 0;						// 頂点の数
	int nNumPoly = 0;						// プリミティブ数
	D3DXVECTOR3 posVtx = DEFAULT_VECTOR3;	// 位置
	int nBoneRef[2] = {};					// 影響を受けるボーン番号
	float fWeightValue[2] = {};				// ボーンごとのウェイト
	D3DXVECTOR2 texVtx = DEFAULT_VECTOR2;	// テクスチャ座標

	// モーションセット用の変数
	CMotion::INFO info = {};	// モーション情報
	int nLoop = 0;				// ループするかどうか
	int nNumKey = 0;			// キーの総数
	int nFrame = 0;				// 再生フレーム数
	int nCntMotion = 0;			// モーション番号
	CMotion::KEY key = {};		// キー要素

	if (file.is_open() == true)
	{// ファイルが開けた
		// ファイルの内容を読み込む
		while (file >> line)
		{
			if (line.find("#") != std::string::npos)
			{// コメント
				std::getline(file, line);

				continue;
			}

			if (line.find("NUM_TEXTURE") != std::string::npos)
			{// テクスチャ数
				file >> line;				// =
				file >> nNumTexture;		// 数字

				continue;
			}

			if (line.find("TEXTURE_FILENAME") != std::string::npos && nCntTexture < nNumTexture)
			{// テクスチャの名前
				file >> line;				// =
				file >> TextureName;		// テクスチャの名前

				nCntTexture++;		// 読み込んだテクスチャ数をカウントアップ

				continue;
			}

			if (line.find("NUM_BONE") != std::string::npos)
			{// モデル数
				file >> line;				// =
				file >> nNumBone;		// 数字

				m_nNumBone = nNumBone;

				continue;
			}

			if (line.find("BONESET") != std::string::npos)
			{// キャラクターの設定
				file >> line;				// 次の情報

				if (line.find("RADIUS") != std::string::npos)
				{// キャラクターの半径
					file >> line;				// =
					file >> m_fRadius;			// 半径

					file >> line;				// 次の文字列

					if (line.find("#") != std::string::npos)
					{// コメント
						std::getline(file, line);

						// 次の文字列を読み込む
						file >> line;			// 次の文字列
					}
				}

				if (line.find("HEIGHT") != std::string::npos)
				{// キャラクターの高さ
					file >> line;				// =
					file >> m_fHeight;			// 高さ

					file >> line;				// 次の文字列

					if (line.find("#") != std::string::npos)
					{// コメント
						std::getline(file, line);

						// 次の文字列を読み込む
						file >> line;			// 次の文字列
					}
				}

				// ボーン読み込み
				for (int nCntBone = 0; nCntBone < nNumBone; nCntBone++)
				{
					if (line.find("BONE") != std::string::npos)
					{// BONE
						file >> line;				// ボーン設定
					}

					if (line.find("PARENT") != std::string::npos)
					{// 親のボーン番号
						file >> line;				// =
						file >> nIdxParent;			// 親のボーン番号

						file >> line;				// 次の文字列

						if (line.find("#") != std::string::npos)
						{// コメント
							std::getline(file, line);

							// 次の文字列を読み込む
							file >> line;			// 次の文字列
						}
					}

					if (line.find("POS") != std::string::npos)
					{// ボーンの位置(オフセット)
						file >> line;				// =
						file >> pos.x;				// 位置(オフセット)x
						file >> pos.y;				// 位置(オフセット)y
						file >> pos.z;				// 位置(オフセット)y

						file >> line;				// 次の文字列

						if (line.find("#") != std::string::npos)
						{// コメント
							std::getline(file, line);

							// 次の文字列を読み込む
							file >> line;			// 次の文字列
						}
					}

					if (line.find("ROT") != std::string::npos)
					{// ボーンの向き
						file >> line;				// =
						file >> rot.x;				// 向き(オフセット)x
						file >> rot.y;				// 向き(オフセット)y
						file >> rot.z;				// 向き(オフセット)y

						file >> line;				// 次の文字列

						if (line.find("#") != std::string::npos)
						{// コメント
							std::getline(file, line);

							// 次の文字列を読み込む
							file >> line;			// 次の文字列
						}
					}

					if (line.find("END_BONE") != std::string::npos)
					{// END_BONE
						file >> line;				// 次の文字列
					}

					// ボーンを生成
					m_vpBone.push_back(CBone::Create(pos, rot));

					// 親の設定
					if (m_vpBone.at(nCntBone) != nullptr)
					{// NULLチェック
						if (nIdxParent < 0 || nIdxParent >= nNumBone)
						{// 親がいない場合
							m_vpBone.at(nCntBone)->SetParent(nullptr);
						}
						else
						{// 親がいる場合
							m_vpBone.at(nCntBone)->SetParent(m_vpBone.at(nCntBone));
						}
					}
				}

				continue;
			}

			if (line.find("VERTEXSET") != std::string::npos)
			{// 頂点設定
				file >> line;				// 次の情報

				if (line.find("NUM_VERTEX") != std::string::npos)
				{// 頂点数
					file >> line;				// =
					file >> nNumVtx;			// 数字

					m_nNumVtx = nNumVtx;

					// 頂点に関連する情報を生成
					m_pPosOff = new D3DXVECTOR3[m_nNumBone * m_nNumVtx];
					m_pIdxRef = new int[m_nNumBone * m_nNumVtx];
					m_pWeightValue = new float[m_nNumBone * m_nNumVtx];

					if (m_pVtxBuff == nullptr)
					{// NULLチェック
						// 頂点バッファを生成
						if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * nNumVtx,
							D3DUSAGE_WRITEONLY,
							FVF_VERTEX_3D,
							D3DPOOL_MANAGED,
							&m_pVtxBuff,
							nullptr)))
						{// もし失敗したら
							OutputDebugStringA("! ! ! 頂点バッファの生成に失敗しました ! ! !\n");

							continue;
						}
					}

					file >> line;				// 次の情報
				}

				if (line.find("NUM_PRIMITIVE") != std::string::npos)
				{// プリミティブ数
					file >> line;				// =
					file >> nNumPoly;			// 数字

					m_nNumPoly = nNumPoly;

					file >> line;				// 次の情報
				}

				// 頂点バッファをロックし、頂点情報へのポインタを取得
				m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

				// 頂点情報読み込み
				for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
				{
					if (line.find("VERTEX") != std::string::npos)
					{// VERTEX
						file >> line;				// 頂点情報

						if (line.find("#") != std::string::npos)
						{// コメント
							std::getline(file, line);

							// 次の文字列を読み込む
							file >> line;			// 次の文字列
						}
					}

					if (line.find("POS") != std::string::npos)
					{// 頂点の位置
						file >> line;				// =
						file >> pos.x;				// 頂点の位置x
						file >> pos.y;				// 頂点の位置y
						file >> pos.z;				// 頂点の位置y

						file >> line;				// 次の文字列

						if (line.find("#") != std::string::npos)
						{// コメント
							std::getline(file, line);

							// 次の文字列を読み込む
							file >> line;			// 次の文字列
						}
					}

					if (line.find("BONE_REF") != std::string::npos)
					{// 影響を受けるボーン番号
						file >> line;				// =
						file >> nBoneRef[0];		// 影響を受けるボーン番号
						file >> nBoneRef[1];		// 影響を受けるボーン番号

						file >> line;				// 次の文字列

						if (line.find("#") != std::string::npos)
						{// コメント
							std::getline(file, line);

							// 次の文字列を読み込む
							file >> line;			// 次の文字列
						}
					}

					if (line.find("WEIGHT_VALUE") != std::string::npos)
					{// ボーンごとのウェイト
						file >> line;				// =
						file >> fWeightValue[0];	// ボーンごとのウェイト
						file >> fWeightValue[1];	// ボーンごとのウェイト

						file >> line;				// 次の文字列

						if (line.find("#") != std::string::npos)
						{// コメント
							std::getline(file, line);

							// 次の文字列を読み込む
							file >> line;			// 次の文字列
						}
					}

					if (line.find("TEX") != std::string::npos)
					{// テクスチャ座標
						file >> line;				// =
						file >> texVtx.x;			// テクスチャ座標
						file >> texVtx.y;			// テクスチャ座標

						file >> line;				// 次の文字列

						if (line.find("#") != std::string::npos)
						{// コメント
							std::getline(file, line);

							// 次の文字列を読み込む
							file >> line;			// 次の文字列
						}
					}

					if (line.find("END_VERTEX") != std::string::npos)
					{// END_VERTEX
						file >> line;				// 次の文字列
					}

					// 頂点情報を設定
					pVtx[nCntVtx].pos = pos;
					pVtx[nCntVtx].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
					pVtx[nCntVtx].col = COLOR_WHITE;
					pVtx[nCntVtx].tex = texVtx;

					// 各頂点の追加情報を設定
					for (int nCntBone = 0; nCntBone < nNumBone; nCntBone++)
					{
						m_pPosOff[nCntBone * m_nNumVtx + nCntVtx] = pos - m_vpBone.at(nCntBone)->GetPosition();
						m_pIdxRef[nCntBone * m_nNumVtx + nCntVtx] = nBoneRef[nCntBone];
						m_pWeightValue[nCntBone * m_nNumVtx + nCntVtx] = fWeightValue[nCntBone];
					}
				}

				// 頂点バッファをアンロックする
				m_pVtxBuff->Unlock();

				continue;
			}

			if (line.find("MOTIONSET") != std::string::npos)
			{// モーションの設定
				file >> line;				// 次の文字列

				if (line.find("#") != std::string::npos)
				{// コメント
					std::getline(file, line);

					// 次の文字列を読み込む
					file >> line;			// 次の文字列
				}

				if (line.find("LOOP") != std::string::npos)
				{// ループするかどうか
					file >> line;				// =
					file >> nLoop;				// ループするかどうか

					// ループの真偽を代入
					if (nLoop == 0)
					{// false
						info.bLoop = false;
					}
					else if (nLoop == 1)
					{// true
						info.bLoop = true;
					}

					file >> line;				// 次の文字列

					if (line.find("#") != std::string::npos)
					{// コメント
						std::getline(file, line);

						// 次の文字列を読み込む
						file >> line;			// 次の文字列
					}
				}

				if (line.find("NUM_KEY") != std::string::npos)
				{// キーの総数
					file >> line;				// =
					file >> nNumKey;			// キーの総数

					// キーの総数を代入
					info.nNumKey = nNumKey;

					file >> line;				// 次の文字列

					if (line.find("#") != std::string::npos)
					{// コメント
						std::getline(file, line);

						// 次の文字列を読み込む
						file >> line;			// 次の文字列
					}
				}

				// キー読み込み
				for (int nCntKey = 0; nCntKey < nNumKey; nCntKey++)
				{
					if (line.find("KEYSET") != std::string::npos)
					{// KEYSET
						file >> line;				// 次の文字列

						if (line.find("#") != std::string::npos)
						{// コメント
							std::getline(file, line);

							// 次の文字列を読み込む
							file >> line;			// 次の文字列
						}
					}

					if (line.find("FRAME") != std::string::npos)
					{// 再生フレーム数
						file >> line;				// =
						file >> nFrame;				// 再生フレーム数

						info.aKeyInfo[nCntKey].nFrame = nFrame;

						file >> line;				// 次の文字列

						if (line.find("#") != std::string::npos)
						{// コメント
							std::getline(file, line);

							// 次の文字列を読み込む
							file >> line;			// 次の文字列
						}
					}

					// パーツごとに読み込み
					for (int nCntBoneKey = 0; nCntBoneKey < nNumBone; nCntBoneKey++)
					{
						if (line.find("KEY") != std::string::npos)
						{// KEY
							file >> line;				// 次の文字列

							if (line.find("#") != std::string::npos)
							{// コメント
								std::getline(file, line);

								// 次の文字列を読み込む
								file >> line;			// 次の文字列
							}
						}

						if (line.find("POS") != std::string::npos)
						{// パーツの位置(オフセット)
							file >> line;				// =
							file >> key.fPosX;			// 位置(オフセット)x
							file >> key.fPosY;			// 位置(オフセット)y
							file >> key.fPosZ;			// 位置(オフセット)y

							file >> line;				// 次の文字列

							if (line.find("#") != std::string::npos)
							{// コメント
								std::getline(file, line);

								// 次の文字列を読み込む
								file >> line;			// 次の文字列
							}
						}

						if (line.find("ROT") != std::string::npos)
						{// パーツの向き
							file >> line;				// =
							file >> key.fRotX;			// 向き(オフセット)x
							file >> key.fRotY;			// 向き(オフセット)y
							file >> key.fRotZ;			// 向き(オフセット)y

							file >> line;				// 次の文字列

							if (line.find("#") != std::string::npos)
							{// コメント
								std::getline(file, line);

								// 次の文字列を読み込む
								file >> line;			// 次の文字列
							}
						}

						if (line.find("END_KEY") != std::string::npos)
						{// END_KEY
							file >> line;				// 次の文字列
						}

						// モーション情報に代入
						info.aKeyInfo[nCntKey].aKey[nCntBoneKey] = key;
					}

					if (line.find("END_KEYSET") != std::string::npos)
					{// END_KEYSET
						file >> line;				// 次の文字列
					}
				}

				nCntMotion++;

				if (m_pMotion != nullptr)
				{// NULLチェック
					// モーション情報を設定
					m_pMotion->SetInfo(info);
				}

				continue;
			}

			if (line.find("END_SCRIPT") != std::string::npos)
			{// ファイルの読み込みが完了
				break;
			}
		}

		// ファイルを閉じる
		file.close();
	}
	else
	{// ファイルが開けなかった
		OutputDebugStringA("! ! ! ファイルを開けませんでした ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}