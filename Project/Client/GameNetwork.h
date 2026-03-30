#pragma once
class GameNetwork
{
public:
	GameNetwork();
	~GameNetwork();

public:
	void Update();

public:
	void Recv();
	void Send();
	static void CALLBACK RecvCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags);
	static void CALLBACK SendCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags);
	
public:
	void SendMove(WPARAM wParam);
	
public:
	char* GetRecvBuffer() { return _recvBuffer; }

private:
	SOCKET _clientSocket;
	std::string _serverIP;
	WSABUF _wsaRecvBuffer;
	WSABUF _wsaSendBuffer;
	char _recvBuffer[BuffSize];
	char* _sendBuffer;
	WSAOVERLAPPED _recvOver;
	WSAOVERLAPPED _sendOver;
};