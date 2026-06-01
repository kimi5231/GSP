#include "pch.h"
#include "Session.h"
#include "ExpOver.h"
#include "Player.h"

Session::Session()
{
	_clientSocket = INVALID_SOCKET;
	_isConnected = false;
	_id = -1;
	_recvOver._ioType = IOType::Recv;
	_prevRecv = 0;
	_player = nullptr;
}

Session::~Session()
{
	if (_isConnected)
		closesocket(_clientSocket);
}

void Session::Recv()
{
	DWORD recvFlag = 0;
	DWORD bytesReceived = 0;
	memset(&_recvOver._over, 0, sizeof(_recvOver._over));
	_recvOver._wsaBuffer.buf = _recvOver._buffer.data();
	_recvOver._wsaBuffer.len = _recvOver._buffer.size();
	int result = WSARecv(_clientSocket, &_recvOver._wsaBuffer, 1, &bytesReceived, &recvFlag, &_recvOver._over, nullptr);
}

void Session::Send(int len, char* mess)
{
	ExpOver* over = new ExpOver(IOType::Send);
	over->_wsaBuffer.len = len;
	memcpy(over->_buffer.data(), mess, len);
	WSASend(_clientSocket, &over->_wsaBuffer, 1, 0, 0, &over->_over, nullptr);
}