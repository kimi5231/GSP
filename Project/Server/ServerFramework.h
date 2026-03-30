#pragma once
class ServerObject;

class ServerFramework
{
public:
	ServerFramework();
	~ServerFramework();

public:
	void Update();
	Vector ProcessMove(Dir dir);

public:
	void Recv();
	void Send();
	static void CALLBACK RecvCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags);
	static void CALLBACK SendCallback(DWORD err, DWORD byteNum, LPWSAOVERLAPPED over, DWORD flags);

public:
	char* GetRecvBuffer() { return _recvBuffer; }
	void SetSendBuffer(char* sendBuffer) { _sendBuffer = sendBuffer; }

private:
	SOCKET _listenSocket;
	SOCKET _clientSocket;

	WSABUF _wsaRecvBuffer;
	WSABUF _wsaSendBuffer;
	char _recvBuffer[BuffSize];
	char* _sendBuffer;
	WSAOVERLAPPED _over;

	ServerObject* _pawn;
};