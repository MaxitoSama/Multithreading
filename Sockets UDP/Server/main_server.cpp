#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8888

#define PAUSE_AND_EXIT() system("pause"); exit(-1)

void printWSErrorAndExit(const char *msg)
{
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "%s: %S\n", msg, s);
	LocalFree(s);
	PAUSE_AND_EXIT();
}

void server(int port)
{
	std::string error="";

	// TODO-1: Winsock init

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != NOERROR)
	{
		error = "Error initializing Winsock";
		printWSErrorAndExit(error.c_str());
	}

	// TODO-2: Create socket (IPv4, datagrams, UDP)

	SOCKET mySocket_server = socket(AF_INET, SOCK_DGRAM, 0);

	if (mySocket_server == INVALID_SOCKET)
	{
		error = "Error creating the socket";
		printWSErrorAndExit(error.c_str());
	}

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.S_un.S_addr = INADDR_ANY; 

	// TODO-3: Force address reuse

	int enable = 1;
	iResult = setsockopt(mySocket_server, SOL_SOCKET, SO_REUSEADDR, (const char *)& enable, sizeof(int));
	
	if (iResult == SOCKET_ERROR) 
	{ 
		error = "Error it's not possible to bind the given socket";
		printWSErrorAndExit(error.c_str());
	}

	// TODO-4: Bind to a local address

	iResult = bind(mySocket_server, (const struct sockaddr*)&bindAddr, sizeof(bindAddr));

	if (iResult != SOCKET_ERROR)
	{
		error = "Error initializing Winsock";
		printWSErrorAndExit(error.c_str());
	}

	while (true)
	{
		// TODO-5:
		// - Receive 'ping' packet from a remote host
		// - Answer with a 'pong' packet
		// - Control errors in both cases
	}

	// TODO-6: Close socket

	iResult = closesocket(mySocket_server);

	if (iResult == SOCKET_ERROR)
	{
		std::string error = "Error closing the socket";
		printWSErrorAndExit(error.c_str());
	}

	// TODO-7: Winsock shutdown

	iResult = WSACleanup();

	if (iResult != NOERROR)
	{
		std::string error = "Error";
		printWSErrorAndExit(error.c_str());
	}
}

int main(int argc, char **argv)
{
	server(SERVER_PORT);

	PAUSE_AND_EXIT();
}
