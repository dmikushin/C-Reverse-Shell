#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "mainloop.h"

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		return 0;
	}

	int hostSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int optval;
	if (setsockopt(hostSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		fprintf(stderr, "Error setting host socket options\n");
		exit(-1);
	}

	struct sockaddr_in hostSockAddr;
	hostSockAddr.sin_family = AF_INET;
	hostSockAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	hostSockAddr.sin_port = htons(atoi(argv[1]));
	bind(hostSock, (struct sockaddr *)&hostSockAddr, sizeof(hostSockAddr));
	listen(hostSock, 1);
	socklen_t hostSockAddrLength = sizeof(hostSockAddr);
	int hostSockAccepted = accept(hostSock, (struct sockaddr *)&hostSockAddr, &hostSockAddrLength);
	if (hostSockAccepted == -1)
	{
		fprintf(stderr, "Error accepting host connection\n");
		exit(-1);
	}

	printf("Connected to host %s:%d\n", inet_ntoa(hostSockAddr.sin_addr),
		ntohs(hostSockAddr.sin_port));

        int sshSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (setsockopt(sshSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        {
                fprintf(stderr, "Error setting host socket options\n");
                exit(-1);
        }

        struct sockaddr_in sshSockAddr;
        sshSockAddr.sin_family = AF_INET;
        sshSockAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
        sshSockAddr.sin_port = htons(atoi(argv[1]) + 1);
        bind(sshSock, (struct sockaddr *)&sshSockAddr, sizeof(sshSockAddr));
        listen(sshSock, 1);
        socklen_t sshSockAddrLength = sizeof(sshSockAddr);
        int sshSockAccepted = accept(sshSock, (struct sockaddr *)&sshSockAddr, &sshSockAddrLength);
        if (sshSockAccepted == -1)
        {
                fprintf(stderr, "Error accepting host connection\n");
                exit(-1);
        }

        printf("Connected to SSH client %s:%d\n", inet_ntoa(sshSockAddr.sin_addr),
		ntohs(sshSockAddr.sin_port));

        mainloop(sshSockAccepted, hostSockAccepted);

#ifdef _WIN32
        closesocket(hostSock); closesocket(hostSockAccepted);
        closesocket(sshSock); closesocket(sshSockAccepted);
        WSACleanup();
#else
        close(hostSock); close(hostSockAccepted);
        close(sshSock); close(sshSockAccepted);
#endif
        return 0;
}

