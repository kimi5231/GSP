#pragma once
#include "ExpOver.h"

class Player;

class Session
{
public:
	Session();
	~Session();

public:
	void Recv();
	void Send(int len, char* mess);

	SOCKET _clientSocket;
	int _id;
	bool _isConnected;
	ExpOver _recvOver;
	int _prevRecv;
	char username[MAX_NAME_LEN];
	Player* _player;
};