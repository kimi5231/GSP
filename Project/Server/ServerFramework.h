#pragma once
class ServerObject;

class ServerFramework
{
public:
	ServerFramework();
	~ServerFramework();

public:
	void Update();

private:
	SOCKET _listenSocket;
	SOCKET _clientSocket;

	ServerObject* _pawn;
};