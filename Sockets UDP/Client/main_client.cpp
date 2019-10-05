#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDRESS "127.0.0.1"

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

void client(const char *serverAddrStr, int port)
{
	bool close = false;
	std::string error = "";

	// TODO-1: Winsock init

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != NOERROR)
	{
		error="Error initializing Winsock";
		printWSErrorAndExit(error.c_str());
	}

	// TODO-2: Create socket (IPv4, datagrams, UDP)

	SOCKET mySocket_client = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (mySocket_client == INVALID_SOCKET)
	{
		error = "Error creating the socket";
		printWSErrorAndExit(error.c_str());
	}

	// TODO-3: Create an address object with the server address
	
	struct sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;							//To indicate that we are using a the IPv4 adress family.
	remoteAddr.sin_port = htons(port);							//Transforms the port address to the network order.
	inet_pton(AF_INET, serverAddrStr, &remoteAddr.sin_addr);	//transforming the string into the appropriate library's IP.

	while (true)
	{
		// TODO-4:
		// - Send a 'ping' packet to the server
		// - Receive 'pong' packet from the server
		// - Control errors in both cases
	}

	// TODO-5: Close socket
	
	iResult = closesocket(mySocket_client);

	if (iResult == SOCKET_ERROR)
	{
		std::string error = "Error closing the socket";
		printWSErrorAndExit(error.c_str());
	}

	// TODO-6: Winsock shutdown

	iResult = WSACleanup();
	
	if (iResult != NOERROR)
	{
		std::string error = "Error";
		printWSErrorAndExit(error.c_str());
	}
}

int main(int argc, char **argv)
{
	client(SERVER_ADDRESS, SERVER_PORT);

	PAUSE_AND_EXIT();
}
