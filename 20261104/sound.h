//=============================================================================
// 
// サウンド処理 [ sound.h ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"

//*****************************************************************************
// サウンドクラス
//*****************************************************************************
class CSound
{
public:
	// サウンド一覧
	typedef enum
	{
		BGM_TITLE,					// タイトルBGM
		BGM_GAME,					// ゲームBGM
		SE_ENTER,					// 選択
		SE_PAUSE,					// ポーズ
		SE_BALLOON,					// 風船
		SE_LAND,					// 着地
		SE_MOVE,					// 移動
		SE_TIMER,					// タイマー
		SE_C,						// シェパードトーンC
		SE_D,						// シェパードトーンD
		SE_E,						// シェパードトーンE
		SE_F,						// シェパードトーンF
		SE_G,						// シェパードトーンG
		SE_A,						// シェパードトーンA
		SE_B,						// シェパードトーンB
		SOUND_MAX,
	} SOUND_LABEL;

	CSound();
	~CSound();

	HRESULT Init(HWND hWnd);
	void Uninit(void);
	HRESULT PlaySound(SOUND_LABEL label);
	void StopSound(SOUND_LABEL label);
	void StopSound(void);

private:
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

	static IXAudio2* m_pXAudio2;								// XAudio2オブジェクトへのインターフェイス
	static IXAudio2MasteringVoice* m_pMasteringVoice;			// マスターボイス
	static IXAudio2SourceVoice* m_apSourceVoice[SOUND_MAX];		// ソースボイス
	static BYTE* m_apDataAudio[SOUND_MAX];						// オーディオデータ
	static DWORD m_aSizeAudio[SOUND_MAX];						// オーディオデータサイズ

	// サウンド情報の構造体定義
	typedef struct
	{
		const char* pFilename;	// ファイル名
		int nCntLoop;			// ループカウント
	} SOUNDINFO;

	static SOUNDINFO m_aSoundInfo[SOUND_MAX];					// サウンドの情報
};

#endif
