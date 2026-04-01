#pragma once
class Session
{
public:
	Session() {};
	Session(int id, SOCKET socket);
	~Session();
	
public:
	void Recv();
	void Send(std::vector<char>& sendBuffer);

public:
	char* GetRecvBuffer() { return _recvBuffer; }

private:
	int _id;
	WSABUF _wsaRecvBuffer;
	WSABUF _wsaSendBuffer;
	char _recvBuffer[BuffSize];
	WSAOVERLAPPED _over;
	SOCKET _socket;
};