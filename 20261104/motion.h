//========================================================================
// 
// モーションヘッダー [ motion.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _MOTION_H_		// このマクロ定義がされていなかったら
#define _MOTION_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

//************************************************************************
// 前方宣言
//************************************************************************
class CModel;

//************************************************************************
// マクロ定義
//************************************************************************
#define MAX_NUMMODEL		(20)				// モデルの最大数
#define MAX_KEY				(100)				// キーの最大数
#define MAX_MOTION			(20)				// モーションの最大数

//************************************************************************
// モーションクラス
//************************************************************************
class CMotion
{
public:
	// キー要素の構造体
	typedef struct
	{
		float fPosX;		// 位置X
		float fPosY;		// 位置Y
		float fPosZ;		// 位置Z
		float fRotX;		// 向きX
		float fRotY;		// 向きY
		float fRotZ;		// 向きZ
	}KEY;

	// キー情報の構造体
	typedef struct
	{
		int nFrame;					// 再生フレーム
		KEY aKey[MAX_NUMMODEL];		// 各パーツのキー要素	
	}KEY_INFO;

	// モーション情報の構造体
	typedef struct
	{
		bool bLoop;						// ループするかどうか
		int nNumKey;					// キーの総数
		KEY_INFO aKeyInfo[MAX_KEY];		// キー情報
	}INFO;

	CMotion();
	virtual ~CMotion();

	static CMotion* Create(void);
	HRESULT Init(void);
	void Uninit(void);

	void SetInfo(const INFO info);
	void SetModel(CModel** ppModel, const int nNumModel);

	void Set(const int nType, bool bBlend, int nFrameBlend);
	void Update(void);

	int GetType(void) { return m_nType; }
	bool IsFinish(void) { return m_bFinish; }

private:
	INFO m_aInfo[MAX_MOTION];		// モーション情報
	int m_nNumAll;					// モーションの総数
	CModel** m_ppModel;				// モデルへのポインタ
	int m_nNumModel;				// モデルの総数

	int m_nType;					// 現在のモーションの種類
	bool m_bLoop;					// 現在のループするかどうか
	int m_nNumKey;					// 現在のキーの総数
	int m_nKey;						// 現在のキーNo.
	int m_nCounter;					// 現在のモーションのカウンター
	bool m_bFinish;					// 現在のモーションが終了しているかどうか

	bool m_bBlend;					// ブレンドモーションがあるかどうか
	int m_nTypeBlend;				// ブレンドモーションの種類
	bool m_bLoopBlend;				// ブレンドモーションがループするかどうか
	int m_nNumKeyBlend;				// ブレンドモーションのキーの総数
	int m_nKeyBlend;				// ブレンドモーションの現在のキーNo.
	int m_nCounterBlend;			// ブレンドモーションのカウンター
	int m_nFrameBlend;				// ブレンドフレーム数
	int m_nCounterFrameBlend;		// ブレンドフレームカウンター
};

#endif