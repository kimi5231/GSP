#pragma once
class GameNetwork
{
public:
	GameNetwork();
	~GameNetwork();

public:
	POINT SendMove(WPARAM wParam);

private:
	SOCKET _clientSocket;
	std::string _serverIP;
};