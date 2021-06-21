#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

static int hostSockAccepted, sshSockAccepted;

static void shell()
{
        char input[1024];
        while (1)
        {
                ssize_t szinput = recv(sshSockAccepted, input, sizeof(input), 0);
                if (szinput == -1)
		{
			fprintf(stderr, "Error in recv(sshSockAccepted) at %s:%d\n", __FILE__, __LINE__);
			exit(-1);
		}

		printf("Transferring %zu bytes from SSH client to host\n", (size_t)szinput);

                while (szinput)
                {
                        ssize_t sent = send(hostSockAccepted, input, szinput, 0);
                        if (sent == -1)
			{
				fprintf(stderr, "Error in send(hostSockAccepted) at %s:%d\n", __FILE__, __LINE__);
				exit(-1);
			}

                        szinput -= sent;

                        char output[1024];
                        while (1)
                        {
                                ssize_t szoutput = recv(hostSockAccepted, output, sizeof(output), 0);
                                if (szoutput == -1)
				{
					fprintf(stderr, "Error in recv(hostSockAccepted) at %s:%d\n", __FILE__, __LINE__);
					exit(-1);
				}

                                if (szoutput == 0) break;

				printf("Transferring %zu bytes from host to SSH client\n", (size_t)szoutput);

                                while (szoutput)
                                {
                                        ssize_t received = send(sshSockAccepted, output, szoutput, 0);
                                        if (received == -1)
					{
						fprintf(stderr, "Error in send(sshSockAccepted) at %s:%d\n", __FILE__, __LINE__);
						exit(-1);
					}

                                        szoutput -= received;
                                }
                        }
                }
        }
}

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

	struct sockaddr_in hostSockAddress;
	hostSockAddress.sin_family = AF_INET;
	hostSockAddress.sin_addr.s_addr = inet_addr("0.0.0.0");
	hostSockAddress.sin_port = htons(atoi(argv[1]));
	bind(hostSock, (struct sockaddr *)&hostSockAddress, sizeof(hostSockAddress));
	listen(hostSock, 1);
	socklen_t hostSockAddressLength = sizeof(hostSockAddress);
	hostSockAccepted = accept(hostSock, (struct sockaddr *)&hostSockAddress, &hostSockAddressLength);
	if (hostSockAccepted == -1)
	{
		fprintf(stderr, "Error accepting host connection\n");
		exit(-1);
	}

	printf("Connected to host %s:%d\n", "a", 1);

        int sshSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (setsockopt(sshSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        {
                fprintf(stderr, "Error setting host socket options\n");
                exit(-1);
        }

        struct sockaddr_in sshSockAddress;
        sshSockAddress.sin_family = AF_INET;
        sshSockAddress.sin_addr.s_addr = inet_addr("0.0.0.0");
        sshSockAddress.sin_port = htons(atoi(argv[1]) + 1);
        bind(sshSock, (struct sockaddr *)&sshSockAddress, sizeof(sshSockAddress));
        listen(sshSock, 1);
        socklen_t sshSockAddressLength = sizeof(sshSockAddress);
        sshSockAccepted = accept(sshSock, (struct sockaddr *)&sshSockAddress, &sshSockAddressLength);
        if (sshSockAccepted == -1)
        {
                fprintf(stderr, "Error accepting host connection\n");
                exit(-1);
        }

        printf("Connected to SSH client %s:%d\n", "a", 1);

        shell();

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

