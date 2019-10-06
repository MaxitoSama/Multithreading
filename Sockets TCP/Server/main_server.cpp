#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

#define LISTEN_PORT 8888

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
	// TODO-1: Winsock init
	std::string Error = "error";
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != NOERROR)
	{
		Error = "Server Error initializing Winsock";
		printWSErrorAndExit(Error.c_str());
	}

	// TODO-2: Create socket (IPv4, stream, TCP)
	SOCKET mySocket_server = socket(AF_INET, SOCK_STREAM, 0);

	if (mySocket_server == INVALID_SOCKET)
	{
		Error = "Server Error creating the TCP Socket";
		printWSErrorAndExit(Error.c_str());
	}

	// TODO-3: Configure socket for address reuse
	int enable = 1;
	iResult = setsockopt(mySocket_server, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));

	if (iResult == SOCKET_ERROR)
	{
		Error = "Server Error configuring address reuse";
		printWSErrorAndExit(Error.c_str());
	}

	// TODO-4: Create an address object with any local address
	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	// TODO-5: Bind socket to the local address
	iResult = bind(mySocket_server, (const sockaddr*)&bindAddr, sizeof(bindAddr));

	if (iResult == SOCKET_ERROR)
	{
		Error = "Server Error binding";
		printWSErrorAndExit(Error.c_str());
	}

	// TODO-6: Make the socket enter into listen mode�
	iResult = listen(mySocket_server, 1);
	
	if (iResult == SOCKET_ERROR)
	{
		Error = "Server Error entering listen mode";
		printWSErrorAndExit(Error.c_str());
	}

	// TODO-7: Accept a new incoming connection from a remote host
	// Note that once a new connection is accepted, we will have
	// a new socket directly connected to the remote host.

	SOCKET acceptedScocket;
	int size = sizeof(mySocket_server);
	acceptedScocket = accept(mySocket_server, (struct sockaddr*)&mySocket_server, &size);

	while (true)
	{
		// TODO-8:
		// - Wait a 'ping' packet from the client
		// - Send a 'pong' packet to the client
		// - Control errors in both cases
	}

	// TODO-9: Close socket
	iResult = closesocket(mySocket_server);

	if (iResult == SOCKET_ERROR)
	{
		Error = "Server Error clossing Socket";
		printWSErrorAndExit(Error.c_str());
	}

	// TODO-10: Winsock shutdown
	iResult = WSACleanup();

	if (iResult != NOERROR)
	{
		Error = "Server Error initializing Winsock";
		printWSErrorAndExit(Error.c_str());
	}
}

int main(int argc, char **argv)
{
	server(LISTEN_PORT);

	PAUSE_AND_EXIT();
}
