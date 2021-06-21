// Keep socket buffer large enough
#define SZBUFFER 65536

static void mainloop(int src, int dst)
{
	char input[SZBUFFER], output[SZBUFFER];

	fd_set fds;

	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(src, &fds);
		FD_SET(dst, &fds);

		struct timeval timeout;
		timeout.tv_usec = 10000;
		int activity = select(src > dst ? src + 1 : dst + 1, &fds, NULL, NULL, &timeout);

		switch (activity)
		{
		case -1 :
			fprintf(stderr, "Error -1 in select() at %s:%d\n", __FILE__, __LINE__);
			exit(-1);
		case 0 :
			// TODO Timeout
			break;
		default :
			if (FD_ISSET(src, &fds))
			{
				ssize_t szinput = recv(src, input, sizeof(input), 0);
				if (szinput == -1)
				{
					fprintf(stderr, "Error in recv(src) at %s:%d\n", __FILE__, __LINE__);
					exit(-1);
				}

				printf("Transferring %zu bytes from src to dst\n", (size_t)szinput);

				while (szinput)
				{
					ssize_t sent = send(dst, input, szinput, 0);
					if (sent == -1)
					{
						fprintf(stderr, "Error in send(dst) at %s:%d\n", __FILE__, __LINE__);
						exit(-1);
					}

					szinput -= sent;
				}
			}
			else if (FD_ISSET(dst, &fds))
			{
				ssize_t szoutput = recv(dst, output, sizeof(output), 0);
				if (szoutput == -1)
				{
					fprintf(stderr, "Error in recv(dst) at %s:%d\n", __FILE__, __LINE__);
					exit(-1);
				}

				printf("Transferring %zu bytes from dst to src\n", (size_t)szoutput);

				while (szoutput)
				{
					ssize_t received = send(src, output, szoutput, 0);
					if (received == -1)
					{
						fprintf(stderr, "Error in send(src) at %s:%d\n", __FILE__, __LINE__);
						exit(-1);
					}

					szoutput -= received;
				}
			}
			else
			{
				fprintf(stderr, "Unknown fd is set, unreachable is executed at %s:%d\n", __FILE__, __LINE__);
				exit(-1);
			}
		}
	}
}

