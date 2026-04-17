#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "WS2_32.LIB")

using namespace std;

const char* SERVER_ADDR = "127.0.0.1";
const short SERVER_PORT = 4000;
const int BUFSIZE = 256;

int main()
{
	wcout.imbue(locale("korean"));

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	WSAConnect(s_socket, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(server_addr), NULL, NULL, NULL, NULL);
	
	for (;;)
	{
		char buf[BUFSIZE];
		cout << "Enter Message : "; cin.getline(buf, BUFSIZE);

		DWORD sent_size;
		WSABUF was_buf{ strlen(buf + 1), buf};
		WSASend(234, &was_buf, 1, &sent_size, 0, nullptr, nullptr);

		char recv_buf[BUFSIZE];
		WSABUF recv_wsa_buf{ BUFSIZE, recv_buf };
		DWORD recv_size = 0;
		DWORD recv_flag = 0;
		WSARecv(s_socket, &recv_wsa_buf, 1, &recv_size, &recv_flag, nullptr, nullptr);
		
		cout << "Server Sent [" << recv_size << "bytes] : " << recv_buf << endl;
	}

	WSACleanup();
}