#include "pch.h"
#include "ServerFramework.h"
#include "Global.h"
#include "Packets.h"
#include "ServerObject.h"

ServerFramework::ServerFramework()
{
	// 윈속 초기화
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 0), &wsa);

	// listenSocket 생성
	_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	
	// bind
	ZeroMemory(&_addr, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(7777);
	bind(_listenSocket, reinterpret_cast<sockaddr*>(&_addr), sizeof(_addr));

	// listen
	listen(_listenSocket, SOMAXCONN);

	_generateClientID = 1;
	_generatePlayerID = 1;
}

ServerFramework::~ServerFramework()
{
	_clients.clear();
	closesocket(_listenSocket);
	WSACleanup();
}

void ServerFramework::Update()
{
	// accept
	int addrLen = sizeof(_addr);
	SOCKET clientSocket = WSAAccept(_listenSocket, reinterpret_cast<sockaddr*>(&_addr), &addrLen, 0, 0);
	_clients.try_emplace(_generateClientID, _generateClientID, clientSocket);;
	_clients[_generateClientID++].Recv();
	std::cout << "Client Connect" << std::endl;

	// Client에 대응되는 Player 생성
	_players.emplace_back(_generatePlayerID);

	// 현재 접속되어 있는 모든 Client에게 새로 생성된 Player 정보를 전달
	Header header{ S_AddObject, sizeof(S_AddObject_Packet) };
	S_AddObject_Packet packetData{ _generatePlayerID, _players[_generatePlayerID++ - 1].GetPos() };
	std::vector<char> sendBuffer(sizeof(Header) + header.dataSize);
	memcpy(sendBuffer.data(), &header, sizeof(Header));
	memcpy(sendBuffer.data() + sizeof(Header), &packetData, header.dataSize);

	for (auto& item : _clients)
	{
		item.second.Send(sendBuffer);
	}
		
	SleepEx(10, true);
}

void ServerFramework::ProcessMove(C_Move_Packet packet, int playerID)
{
	Vector pos = _players[playerID - 1].Move(packet.dir);

	// 현재 접속되어 있는 모든 Client에게 이동 알림
	Header header{ S_Move, sizeof(S_Move_Packet) };
	S_Move_Packet packetData{ playerID, pos };
	std::vector<char> sendBuffer(sizeof(Header) + header.dataSize);
	memcpy(sendBuffer.data(), &header, sizeof(Header));
	memcpy(sendBuffer.data() + sizeof(Header), &packetData, header.dataSize);

	for (auto& item : _clients)
		item.second.Send(sendBuffer);

	_clients[playerID].Recv();
}

void ServerFramework::RecvCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags)
{
	int id = reinterpret_cast<int>(over->hEvent);
	std::unordered_map<int, Session>& clients = g_serverFramework->GetClients();
	
	Header header;
	memcpy(&header, clients[id].GetRecvBuffer(), sizeof(header));

	switch (header.id)
	{
	case C_Move:
		C_Move_Packet packet;
		memcpy(&packet, clients[id].GetRecvBuffer() + sizeof(header), sizeof(C_Move_Packet));
		g_serverFramework->ProcessMove(packet, id);
		break;
	}
}

void ServerFramework::SendCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags)
{
}