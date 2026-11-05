//========================================================================
// 
// ファイル [ file.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "file.h"

#include "manager.h"
#include "player.h"
#include "wind.h"
#include "field.h"
#include "wallMM.h"

#include <iostream>
#include <fstream>
#include <string>

//========================================================================
// ファイルクラスのコンストラクタ
//========================================================================
CFile::CFile()
{
}

//========================================================================
// ファイルクラスのデストラクタ
//========================================================================
CFile::~CFile()
{
}

//========================================================================
// オブジェクト配置
//========================================================================
CObject* CFile::SetObject(const char* pFilename, const CObject::TYPE type)
{
	// ローカル変数宣言
	std::string filename = pFilename;	// 読み込むファイル名
	std::fstream file(filename);		// ファイル
	std::string line = {};				// テキストを読み込む
	CObject* pObject = nullptr;			// 生成するオブジェクト

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

			if (line.find("FIELDSET") != std::string::npos && (type == CObject::TYPE_FIELD || type == CObject::TYPE_WALL))
			{// フィールドを設定
				std::string pFieldTexture = "";			// 地面のテクスチャ
				D3DXVECTOR3 pos = DEFAULT_VECTOR3;		// 初期位置
				D3DXVECTOR3 size = DEFAULT_VECTOR3;		// サイズ
				D3DXVECTOR2 block = DEFAULT_VECTOR2;	// 分割数

				file >> line;					// 次の文字

				if (line.find("FIELDTEXTURE") != std::string::npos)
				{// 地面のテクスチャ
					file >> line;				// =
					file >> pFieldTexture;		// 地面のテクスチャ

					file >> line;				// 次の文字
				}

				if (line.find("POS") != std::string::npos)
				{// 初期位置
					file >> line;				// =
					file >> pos.x;				// 位置
					file >> pos.y;				// 位置
					file >> pos.z;				// 位置

					file >> line;				// 次の文字
				}

				if (line.find("SIZE") != std::string::npos)
				{// サイズ
					file >> line;				// =
					file >> size.x;				// サイズ
					file >> size.y;				// サイズ
					file >> size.z;				// サイズ

					file >> line;				// 次の文字
				}
				
				if (line.find("BLOCK") != std::string::npos)
				{// 分割数(1メッシュあたりのサイズ)
					file >> line;				// =
					file >> block.x;			// 分割数
					file >> block.y;			// 分割数

					file >> line;				// 次の文字
				}

				if (line.find("END_FIELDSET") != std::string::npos)
				{// フィールド設定を終わる
					CField::Load(pFieldTexture.c_str());

					// フィールドを生成
					CField::Create(pos, size.x * 0.5f, size.z * 0.5f);
				
					// 移動限界を設定
					CManager::SetLimit(D3DXVECTOR2(size.x * 0.5f, size.z * 0.5f));

					continue;
				}
			}

			if (line.find("PLAYERSET") != std::string::npos && type == CObject::TYPE_PLAYER)
			{// プレイヤーを設定
				std::string pModelFile = "";			// モデルファイル名
				D3DXVECTOR3 pos = DEFAULT_VECTOR3;		// 初期位置
				float fRadius = 0.0f, fJump = 0.0f;		// 半径、ジャンプ量
				float fSpeed = 0.0f, fInertia = 0.0f;	// 回転速度、回転減衰

				file >> line;					// 次の文字

				if (line.find("MODELFILE") != std::string::npos)
				{// モデルファイル
					file >> line;				// =
					file >> pModelFile;			// モデルファイル名

					file >> line;				// 次の文字
				}
				
				if (line.find("POS") != std::string::npos)
				{// 初期位置
					file >> line;				// =
					file >> pos.x;				// 位置
					file >> pos.y;				// 位置
					file >> pos.z;				// 位置

					file >> line;				// 次の文字
				}

				if (line.find("RADIUS") != std::string::npos)
				{// 半径
					file >> line;				// =
					file >> fRadius;			// 半径

					file >> line;				// 次の文字
				}

				if (line.find("JUMP") != std::string::npos)
				{// ジャンプ
					file >> line;				// =
					file >> fJump;				// ジャンプ

					file >> line;				// 次の文字
				}
				
				if (line.find("SPEED") != std::string::npos)
				{// 回転速度
					file >> line;				// =
					file >> fSpeed;				// 回転速度

					file >> line;				// 次の文字
				}
				
				if (line.find("INERTIA") != std::string::npos)
				{// 回転減衰
					file >> line;				// =
					file >> fInertia;			// 回転減衰

					file >> line;				// 次の文字
				}

				if (line.find("END_PLAYERSET") != std::string::npos)
				{// プレイヤー設定を終わる
					CPlayer::Load(pModelFile.c_str());
					pObject = CPlayer::Create(pos, fRadius, fJump, fSpeed, fInertia);

					continue;
				}
			}
			
			if (line.find("WINDSET") != std::string::npos && type == CObject::TYPE_WIND)
			{// 風を設定
				std::string pModelFile = "";			// モデルファイル名
				D3DXVECTOR3 pos = DEFAULT_VECTOR3;		// 初期位置
				D3DXVECTOR3 vec = DEFAULT_VECTOR3;		// 風の向き
				float fStrength = 0.0f;					// 強さ

				file >> line;					// 次の文字

				if (line.find("MODELFILE") != std::string::npos)
				{// モデルファイル
					file >> line;				// =
					file >> pModelFile;			// モデルファイル名

					file >> line;				// 次の文字
				}
				
				if (line.find("POS") != std::string::npos)
				{// 初期位置
					file >> line;				// =
					file >> pos.x;				// 位置
					file >> pos.y;				// 位置
					file >> pos.z;				// 位置

					file >> line;				// 次の文字
				}
				
				if (line.find("VEC") != std::string::npos)
				{// 風の向き
					file >> line;				// =
					file >> vec.x;				// 風の向き
					file >> vec.y;				// 風の向き
					file >> vec.z;				// 風の向き

					file >> line;				// 次の文字
				}

				if (line.find("STRENGTH") != std::string::npos)
				{// 強さ
					file >> line;				// =
					file >> fStrength;			// 強さ

					file >> line;				// 次の文字
				}

				if (line.find("END_WINDSET") != std::string::npos)
				{// 風設定を終わる
					CWind::Load(pModelFile.c_str());
					pObject = CWind::Create(pos, vec, fStrength);

					continue;
				}
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

		return nullptr;
	}

	return pObject;
}