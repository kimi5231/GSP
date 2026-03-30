#include "pch.h"
#include "ServerFramework.h"
#include "Global.h"
#include "ServerObject.h"

ServerFramework::ServerFramework()
{
	// 윈속 초기화
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 0), &wsa);

	// listenSocket 생성
	_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	
	// bind
	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(7777);
	bind(_listenSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));

	// listen
	listen(_listenSocket, SOMAXCONN);

	// accept
	int addrLen = sizeof(addr);
	_clientSocket = WSAAccept(_listenSocket, reinterpret_cast<sockaddr*>(&addr), &addrLen, 0, 0);

	std::cout << "Client Connect" << std::endl;

	_pawn = new ServerObject();

	Recv();
}

ServerFramework::~ServerFramework()
{
	closesocket(_listenSocket);
	WSACleanup();
}

void ServerFramework::Update()
{
	SleepEx(100, true);
}

Vector ServerFramework::ProcessMove(Dir dir)
{
	return _pawn->Move(dir);
}

void ServerFramework::Recv()
{
	_wsaRecvBuffer.buf = _recvBuffer;
	_wsaRecvBuffer.len = BuffSize;
	DWORD recvFlag = 0;
	ZeroMemory(&_over, sizeof(_over));
	WSARecv(_clientSocket, &_wsaRecvBuffer, 1, 0, &recvFlag, &_over, RecvCallback);
}

void ServerFramework::Send()
{
	_wsaSendBuffer.buf = _sendBuffer;
	_wsaSendBuffer.len = sizeof(Vector);
	ZeroMemory(&_over, sizeof(_over));
	WSASend(_clientSocket, &_wsaSendBuffer, 1, 0, 0, &_over, SendCallback);
}

void ServerFramework::RecvCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags)
{
	Dir dir;
	memcpy(&dir, g_serverFramework->GetRecvBuffer(), sizeof(Dir));
	Vector pos = g_serverFramework->ProcessMove(dir);

	g_serverFramework->SetSendBuffer(reinterpret_cast<char*>(&pos));
	g_serverFramework->Send();

	// Recv 다시 걸기
	g_serverFramework->Recv();
}

void ServerFramework::SendCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags)
{
}