#include "pch.h"
#include "ServerFramework.h"
#include "ServerObject.h"

ServerFramework::ServerFramework()
{
	// 윈속 초기화
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 0), &wsa);

	// listenSocket 생성
	_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	
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
}

ServerFramework::~ServerFramework()
{
	WSACleanup();
}

void ServerFramework::Update()
{
	char recvbuffer[BuffSize];
	WSABUF buffer_recv;
	buffer_recv.buf = recvbuffer;
	buffer_recv.len = BuffSize;
	DWORD recv_byte;
	DWORD recv_flag = 0;
	WSARecv(_clientSocket, &buffer_recv, 1, &recv_byte, &recv_flag, 0, 0);

	Dir dir;
	memcpy(&dir, recvbuffer, sizeof(Dir));
	Vector pos = _pawn->Move(dir);

	WSABUF buffer_send;
	buffer_send.buf = reinterpret_cast<char*>(&pos);
	buffer_send.len = sizeof(Vector);
	DWORD send_byte;
	WSASend(_clientSocket, &buffer_send, 1, &send_byte, 0, 0, 0);
}