#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef _WIN32
#include <winsock2.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>
#else
#define _popen(...) popen(__VA_ARGS__)
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#define Sleep sleep
#endif
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "mainloop.h"

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
#else
#define __argc argc
#define __argv argv

int main(int argc, char* argv[])
#endif
{
	if (__argc != 3)
	{
		printf("Usage: %s <remote_ip> <remote_port>\n", __argv[0]);
		return 0;
	}

	char* remoteIP = __argv[1];
	int remotePort = atoi(__argv[2]);
	if ((remotePort < 0) || (remotePort > 65535))
	{
		fprintf(stderr, "Invalid port value \"%s\" must be in range 0..65535\n",
			__argv[2]);
		exit(-1);
	}
#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		fprintf(stderr, "Error initializing WSA\n");
		exit(-1);
	}
#endif
	int remoteSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in remoteSockAddr;
	memset(&remoteSockAddr, 0, sizeof(remoteSockAddr));
	remoteSockAddr.sin_family = AF_INET;
	remoteSockAddr.sin_addr.s_addr = inet_addr(remoteIP);
	remoteSockAddr.sin_port = htons((unsigned short)remotePort);

	printf("Waiting for connection...\n");

	while (1)
	{
		if (connect(remoteSock, (struct sockaddr*)&remoteSockAddr,
			sizeof(remoteSockAddr)) == 0)
			break;

		Sleep(1);
	}

	printf("Connected to remote %s:%d\n", inet_ntoa(remoteSockAddr.sin_addr),
		ntohs(remoteSockAddr.sin_port)); 

	int sshSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in sshSockAddr;
	memset(&sshSockAddr, 0, sizeof(sshSockAddr));
	sshSockAddr.sin_family = AF_INET;
	sshSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sshSockAddr.sin_port = htons((unsigned short)22);

	if (connect(sshSock, (struct sockaddr*)&sshSockAddr,
		sizeof(sshSockAddr)) != 0)
		exit(-1);

	printf("Connected to SSH server %s:%d\n", inet_ntoa(sshSockAddr.sin_addr),
		ntohs(sshSockAddr.sin_port));

	mainloop(remoteSock, sshSock);

#ifdef _WIN32
	closesocket(remoteSock);
	closesocket(sshSock);
	WSACleanup();
#else
	close(remoteSock);
	close(sshSock);
#endif
	return 0;
}

