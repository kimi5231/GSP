#include "pch.h"
#include "GameNetwork.h"
#include "Global.h"

GameNetwork::GameNetwork()
{
	// IP 주소 입력용 콘솔창 생성
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
	std::cout << "Server IP: "; std::cin >> _serverIP;

	// 윈속 초기화
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 0), &wsa);
	
	// socket 생성
	_clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	// connect
	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7777);
	inet_pton(AF_INET, _serverIP.c_str(), &addr.sin_addr);
	WSAConnect(_clientSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr), 0, 0, 0, 0);

	// Recv 걸어두기
	Recv();
}

GameNetwork::~GameNetwork()
{
	closesocket(_clientSocket);
	WSACleanup();
}

void GameNetwork::Update()
{
	SleepEx(10, true);
}

void GameNetwork::Recv()
{
	_wsaRecvBuffer.buf = _recvBuffer;
	_wsaRecvBuffer.len = BuffSize;
	DWORD recvFlag = 0;
	ZeroMemory(&_recvOver, sizeof(_recvOver));
	WSARecv(_clientSocket, &_wsaRecvBuffer, 1, 0, &recvFlag, &_recvOver, RecvCallback);
}

void GameNetwork::Send()
{
	_wsaSendBuffer.buf = _sendBuffer;
	_wsaSendBuffer.len = sizeof(Dir);
	ZeroMemory(&_sendOver, sizeof(_sendOver));
	WSASend(_clientSocket, &_wsaSendBuffer, 1, 0, 0, &_sendOver, SendCallback);
}

void GameNetwork::RecvCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags)
{
	// 이동 처리
	POINT pos;
	memcpy(&pos, g_gameNetwork->GetRecvBuffer(), sizeof(pos));
	g_gameFramework->ProcessMove(pos);

	// Recv 다시 걸기
	g_gameNetwork->Recv();
}

void GameNetwork::SendCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags)
{
}

void GameNetwork::SendMove(WPARAM wParam)
{
	Dir dir;

	switch (wParam)
	{
	case VK_UP:
		dir = UP;
		break;
	case VK_RIGHT:
		dir = RIGHT;
		break;
	case VK_DOWN:
		dir = DOWN;
		break;
	case VK_LEFT:
		dir = LEFT;
		break;
	}

	_sendBuffer = reinterpret_cast<char*>(&dir);
	Send();
}