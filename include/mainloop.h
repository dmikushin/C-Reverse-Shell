// Keep socket buffer large enough to avoid the hang at:
// debug1: expecting SSH2_MSG_KEX_ECDH_REPLY
// Althiugh this problem is attributed to VPN/MTU parameters,
// here we can run into a similar issue just due to a too
// small socket buffer.
#define SZBUFFER 65536

static void mainloop(int src, int dst)
{
        char input[SZBUFFER], output[SZBUFFER];

        while (1)
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

                        while (1)
                        {
                                ssize_t szoutput = recv(dst, output, sizeof(output), 0);
                                if (szoutput == -1)
				{
					fprintf(stderr, "Error in recv(dst) at %s:%d\n", __FILE__, __LINE__);
					exit(-1);
				}

                                if (szoutput == 0) break;

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
                }
        }
}

