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
	SessionState _state;
	ExpOver _recvOver;
	int _prevRecv;
	char _userName[MAX_NAME_LEN];
	std::unordered_set<int> _viewList;
	std::mutex _viewLock;
};