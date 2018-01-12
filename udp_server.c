#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>


#define maxBufferSize 32
/*
void write_time(int output_file)
{
  time_t x;
  register char *s;
  register int l;
  extern int write(int, const void *, unsigned int);

  x = time( (time_t *)0 );
  s = ctime(&x);
  l = strlen(s);
  write(output_file, s, l);
}
*/

void send_time(int output, struct sockaddr_in * from, int size)
{
    time_t x;

    x = time(NULL);
    x = htonl(x);
    if ( sendto(output, (void *)&x, sizeof(x), 0, (struct sockaddr *)from, size) != sizeof(x))
    {
	perror("sendto() error");
	exit(-1);
    }
}

int main(void)
{
    /* Way to declare functions without including the whole header file */
	int servSocket;
    struct sockaddr_in serverINET, client;
	unsigned int connectionLen;
	char msgBuffer[maxBufferSize];
	unsigned short servPort;
	int recvMsgSize;

	servPort = getuid();
	printf("Server Port: [%u]\n", servPort);
	
	/* Create a IPv4 UDP socket on UID port */
	if ((servSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror("Socket() failure");
		exit(-1);
	}

	/* Local Address info */
	memset(&serverINET, 0, sizeof(struct sockaddr_in));
	serverINET.sin_family = AF_INET;
	serverINET.sin_addr.s_addr = htonl(INADDR_ANY);
	serverINET.sin_port = htons(servPort);

	/* Bind socket to address */
	if (bind(servSocket, (struct sockaddr *)&serverINET, sizeof(struct sockaddr_in)) < 0)
	{
		perror("bind() failure");
		exit(-2);
	}
	
	for(;;)
	{
		connectionLen = sizeof(client);
	
		if ((recvMsgSize = recvfrom(servSocket, msgBuffer, maxBufferSize, 0, 
									(struct sockaddr *) &client, &connectionLen)) < 0)
		{
			perror("recvfrom() failed");
			exit(-3);
		}
	
		printf("Server received a datagram from host %x port %d\n", ntohl(client.sin_addr.s_addr), ntohs(client.sin_port));
		switch(msgBuffer[0]) 
		{
			default:
				write(2, "Server received unexpected datagram contents\n", 46);
				exit(2);
			case 'D':
				sleep(1);
				strcpy(msgBuffer, "Successful\n");
				printf("recv size = %d \n", recvMsgSize);
				sendto(servSocket, msgBuffer, strlen(msgBuffer), 0, (struct sockaddr *) &client, connectionLen);
				
		}
	}

    return 0;
}
