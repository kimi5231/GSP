#include "pch.h"
#include "Session.h"
#include "Global.h"

Session::Session(int id, SOCKET socket) 
	: _id(id), _socket(socket)
{

}

Session::~Session()
{
	closesocket(_socket);
}

void Session::Recv()
{
	_wsaRecvBuffer.buf = _recvBuffer;
	_wsaRecvBuffer.len = BuffSize;
	DWORD recvFlag = 0;
	ZeroMemory(&_over, sizeof(_over));
	_over.hEvent = reinterpret_cast<HANDLE>(_id);
	WSARecv(_socket, &_wsaRecvBuffer, 1, 0, &recvFlag, &_over, g_serverFramework->RecvCallback);
}

void Session::Send(std::vector<char>& sendBuffer)
{
	_wsaSendBuffer.buf = sendBuffer.data();
	_wsaSendBuffer.len = sendBuffer.size();
	ZeroMemory(&_over, sizeof(_over));
	_over.hEvent = reinterpret_cast<HANDLE>(_id);
	WSASend(_socket, &_wsaSendBuffer, 1, 0, 0, &_over, g_serverFramework->SendCallback);
}