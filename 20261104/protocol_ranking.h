//========================================================================
// 
// ランキング用のプロトコルヘッダー [ protocol_ranking.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _PROTOCOL_RANKING_H_
#define _PROTOCOL_RANKING_H_

//**************************************************************************
// ランキングに関するコマンドの列挙型定義
//**************************************************************************
typedef enum
{
	COMMAND_TYPE_NONE = 0,			// なし
	COMMAND_TYPE_SET_RANKING,		// ランキング設定
	COMMAND_TYPE_GET_RANKING,		// ランキング取得
	COMMAND_TYPE_MAX
}COMMAND_TYPE;

#endif