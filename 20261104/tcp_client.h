//========================================================================
// 
// TCP送受信ヘッダー [ tcp_client.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "main.h"

//************************************************************************
// マクロ定義
//************************************************************************
#define RECVDATA_LEN		(256)				// 受け取るデータの大きさ

//************************************************************************
// TCP送受信クラス
//************************************************************************
class CTcpClient
{
public:
	CTcpClient();
	virtual ~CTcpClient();

	HRESULT Init(char* pStr, const int nPort);
	HRESULT Init(void);
	void Uninit(void);
	HRESULT Send(unsigned char* data, const int nSize);
	HRESULT Recv(unsigned char* data, const int nSize);

	void SetSocket(const SOCKET socket) { m_socket = socket; }

private:
	SOCKET m_socket;		// 通信用ソケット
};

#endif