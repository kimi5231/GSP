#include "pch.h"
#include "GameNetwork.h"

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
	_clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);

	// connect
	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7777);
	inet_pton(AF_INET, _serverIP.c_str(), &addr.sin_addr);
	connect(_clientSocket, (sockaddr*)&addr, sizeof(addr));
}

GameNetwork::~GameNetwork()
{
	WSACleanup();
}

POINT GameNetwork::SendMove(WPARAM wParam)
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

	WSABUF buffer_send;
	buffer_send.buf = reinterpret_cast<char*>(&dir);
	buffer_send.len = sizeof(dir);
	DWORD send_byte;
	WSASend(_clientSocket, &buffer_send, 1, &send_byte, 0, 0, 0);

	char recvbuffer[BuffSize];
	WSABUF buffer_recv;
	buffer_recv.buf = recvbuffer;
	buffer_recv.len = BuffSize;
	DWORD recv_byte;
	DWORD recv_flag = 0;
	WSARecv(_clientSocket, &buffer_recv, 1, &recv_byte, &recv_flag, 0, 0);

	POINT pos;
	memcpy(&pos, recvbuffer, sizeof(pos));
	return pos;
}