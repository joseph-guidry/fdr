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
#include <signal.h>
#include <fcntl.h>
#include <errno.h>


#define maxBufferSize 32

/* GLOBAL SERVER SOCKET FOR SIG HANDLER */
int servSocket;

void SIGIOHandler(int signalType)
{
	struct sockaddr_in clientAddr;
	unsigned int clntLen;
	int recvMsgSize; // Could check this for IPv6
	char msgBuffer[maxBufferSize];

	printf("signal = %d\n", signalType);

	clntLen = sizeof(clientAddr);
	if (( recvMsgSize = recvfrom(servSocket, msgBuffer, maxBufferSize, 0, 
								(struct sockaddr *) &clientAddr, &clntLen)) < 0)
	{
		if (errno != EWOULDBLOCK)
		{
			perror("recvfrom() failed");
			exit(-7);
		}
	}
	else
	{
		printf("Server received a datagram from host %x port %d\n", ntohl(clientAddr.sin_addr.s_addr), ntohs(clientAddr.sin_port));
		switch(msgBuffer[0]) 
		{
			default:
				write(2, "Server received unexpected datagram contents\n", 46);
				exit(2);
			case 'D':
				sleep(1);
				strcpy(msgBuffer, "Successful\n");
				printf("recv size = %d \n", recvMsgSize);
				sendto(servSocket, msgBuffer, strlen(msgBuffer), 0, (struct sockaddr *) &clientAddr, clntLen);
			
		}
	}
}

int main(void)
{
    /* Way to declare functions without including the whole header file */

    struct sockaddr_in serverINET; //, client;
	struct sigaction handler;
	//unsigned int connectionLen;
	//char msgBuffer[maxBufferSize];
	unsigned short servPort;
	//int recvMsgSize;
	int sockFlags;

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
	
	/* SIG HANDLER */
	handler.sa_handler = SIGIOHandler;
	if (sigfillset(&handler.sa_mask) < 0)
	{
		perror("sigfill() failed");
		exit(-7);
	}
	handler.sa_flags = 0;

	if (sigaction(SIGIO, &handler, NULL) < 0)
	{
		perror("sigaction() failed");
		exit(-8);
	}

	/* NON BLOCKING */

	if ( fcntl(servSocket, F_SETOWN, getpid()) < 0)
	{
		perror("process owner change fail");
		exit(-5);
	}
	
	sockFlags = fcntl(servSocket, F_GETFL);

	if ( fcntl(servSocket, F_SETFL, sockFlags | O_NONBLOCK | FASYNC ) < 0 )
	{
		perror("Async/NonBlock error");
		exit(-6);
	}

	for(;;)
	{
		printf(".\n");
		sleep(10);
	}

    return 0;
}
