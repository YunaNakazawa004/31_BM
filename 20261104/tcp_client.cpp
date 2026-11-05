//========================================================================
// 
// TCP送受信 [ tcp_client.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "tcp_client.h"

//========================================================================
// TCP送受信クラスのコンストラクタ
//========================================================================
CTcpClient::CTcpClient()
{
	// 値をクリア
	m_socket = INVALID_SOCKET;
}

//========================================================================
// TCP送受信クラスのデストラクタ
//========================================================================
CTcpClient::~CTcpClient()
{
}

//========================================================================
// TCP送受信クラスの初期化処理
//========================================================================
HRESULT CTcpClient::Init(char* pStr, const int nPort)
{
	// ソケットを作成
	OutputDebugStringA("[通信用のソケットを作成]\n");
	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_socket == INVALID_SOCKET)
	{// ソケットの作成に失敗した場合
		OutputDebugStringA("[ソケットの作成に失敗しました]\n");

		LPVOID* errorString;

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER					// テキストのメモリ割り当てを要求する
			| FORMAT_MESSAGE_FROM_SYSTEM					// エラーメッセージはWindowsが用意しているものを使用
			| FORMAT_MESSAGE_IGNORE_INSERTS,				// 次の引数を無視してエラーコードに対するエラーメッセージを作成する
			nullptr,
			WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),		// 言語を指定
			(LPTSTR)&errorString,							// メッセージテキストが保存されるバッファへのポインタ
			0,
			nullptr);

		if (errorString != nullptr)
		{
			OutputDebugStringA("[エラーコード] : ");
			OutputDebugStringA((char*)errorString);
		}

		return E_FAIL;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	addr.sin_addr.S_un.S_addr = inet_addr(pStr);

	// サーバーに接続する
	if (connect(m_socket, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{// 接続に失敗した場合
		OutputDebugStringA("[接続に失敗しました]\n");

		return E_FAIL;
	}

	return S_OK;
}

//========================================================================
// TCP送受信クラスの初期化処理 ( オーバーロード )
//========================================================================
HRESULT CTcpClient::Init(void)
{
	// ソケットを作成
	OutputDebugStringA("[通信用のソケットを作成]\n");
	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_socket == INVALID_SOCKET)
	{// ソケットの作成に失敗した場合
		OutputDebugStringA("[ソケットの作成に失敗しました]\n");

		LPVOID* errorString;

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER					// テキストのメモリ割り当てを要求する
			| FORMAT_MESSAGE_FROM_SYSTEM					// エラーメッセージはWindowsが用意しているものを使用
			| FORMAT_MESSAGE_IGNORE_INSERTS,				// 次の引数を無視してエラーコードに対するエラーメッセージを作成する
			nullptr,
			WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),		// 言語を指定
			(LPTSTR)&errorString,							// メッセージテキストが保存されるバッファへのポインタ
			0,
			nullptr);

		if (errorString != nullptr)
		{
			OutputDebugStringA("[エラーコード] : ");
			OutputDebugStringA((char*)errorString);
		}

		return E_FAIL;
	}

	return S_OK;
}

//========================================================================
// TCP送受信クラスの終了処理
//========================================================================
void CTcpClient::Uninit(void)
{
	if (m_socket != INVALID_SOCKET)
	{// 使えるソケットであるか
		// サーバーとの接続を閉じる
		closesocket(m_socket);

		m_socket = INVALID_SOCKET;
	}
}

//========================================================================
// TCP送受信クラスの送信処理
//========================================================================
HRESULT CTcpClient::Send(unsigned char* data, const int nSize)
{
	if (m_socket != INVALID_SOCKET)
	{// 使えるソケットであるか
		if (send(m_socket, (const char*)data, nSize, 0) <= 0)
		{// エラー発生の場合
			// 接続を切断
			Uninit();

			return E_FAIL;
		}

		return S_OK;
	}

	return E_FAIL;
}

//========================================================================
// TCP送受信クラスの受信処理
//========================================================================
HRESULT CTcpClient::Recv(unsigned char* data, const int nSize)
{
	if (m_socket != INVALID_SOCKET)
	{// 使えるソケットであるか
		// データの受信
		char aRecvData[RECVDATA_LEN] = {};
		int nRecvByte = recv(m_socket, &aRecvData[0], sizeof(aRecvData), 0);

		if (nRecvByte <= 0)
		{// エラー発生の場合
			// 接続を切断
			Uninit();

			return E_FAIL;
		}

		if (nSize < nRecvByte)
		{// サイズが大きすぎる
			OutputDebugStringA("元データのサイズが足りませんでした\n");

			return E_FAIL;
		}

		memcpy(data, &aRecvData[0], nRecvByte);

		return S_OK;
	}

	return E_FAIL;
}