#pragma once
#include "Packets.h"
#include "Session.h"

class ServerObject;

class ServerFramework
{
public:
	ServerFramework();
	~ServerFramework();

public:
	void Update();

public:
	void ProcessMove(C_Move_Packet packet, int playerID);

public:
	static void CALLBACK RecvCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags);
	static void CALLBACK SendCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags);

public:
	std::unordered_map<int, Session>& GetClients() { return _clients; }

private:
	SOCKADDR_IN _addr;
	SOCKET _listenSocket;
	SOCKET _clientSocket;

	std::unordered_map<int, Session> _clients;
	int _generateClientID;
	int _generatePlayerID;

	std::vector<ServerObject> _players;
};