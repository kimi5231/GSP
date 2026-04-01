#include "pch.h"
#include "GameNetwork.h"
#include "Global.h"
#include "../Server/Packets.h"

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

void GameNetwork::Send(std::vector<char>& sendBuffer)
{
	_wsaSendBuffer.buf = sendBuffer.data();
	_wsaSendBuffer.len = sendBuffer.size();
	ZeroMemory(&_sendOver, sizeof(_sendOver));
	WSASend(_clientSocket, &_wsaSendBuffer, 1, 0, 0, &_sendOver, SendCallback);
}

void GameNetwork::RecvCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags)
{
	Header header;
	memcpy(&header, g_gameNetwork->GetRecvBuffer(), sizeof(header));

	switch (header.id)
	{
	case S_AddObject:
	{
		S_AddObject_Packet packet;
		memcpy(&packet, g_gameNetwork->GetRecvBuffer() + sizeof(header), sizeof(S_AddObject_Packet));
		g_gameFramework->ProcessAddObjectPacket(packet);
		break;
	}
	case S_Move:
	{
		S_Move_Packet packet;
		memcpy(&packet, g_gameNetwork->GetRecvBuffer() + sizeof(header), sizeof(S_Move_Packet));
		g_gameFramework->ProcessMovePacket(packet);
		break;
	}
	}

	// Recv 다시 걸기
	g_gameNetwork->Recv();
}

void GameNetwork::SendCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags)
{
}

void GameNetwork::SendMovePacket(Dir dir)
{
	Header header{ C_Move, sizeof(C_Move_Packet) };
	C_Move_Packet packetData{ dir };
	std::vector<char> sendBuffer(sizeof(Header) + header.dataSize);
	memcpy(sendBuffer.data(), &header, sizeof(Header));
	memcpy(sendBuffer.data() + sizeof(Header), &packetData, header.dataSize);
	Send(sendBuffer);
}