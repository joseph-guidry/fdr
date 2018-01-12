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
#include <poll.h>

/* Used to take UDP message and convert based on input received. */
#include "conversions.h"

#define MAXBUFFER 32
#define MAXSOCKETS 3

#define MULTIPLE

void SIGIOHandler(int signalType);

/* GLOBAL SERVER SOCKET FOR SIG HANDLER */
int servSocket; // OLD 
struct pollfd socket_array[MAXSOCKETS];
#ifdef MULTIPLE

int create_socket(int port_offset)
{
    struct sockaddr_in serverINET;
	int servPort;
	int sockFlags;

	servPort = getuid() + port_offset;
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

	return servSocket;
}
#endif 

int main(void)
{
	struct sigaction handler;
	unsigned short servPortOffset[] = {0, 1000, 2000};
	

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

	for (unsigned int i = 0; i < MAXSOCKETS; i++)
	{
		socket_array[i].fd = create_socket(servPortOffset[i]);
		socket_array[i].events = POLLIN;
		socket_array[i].revents = 0;
	}


#ifdef SINGLE
    struct sockaddr_in serverINET;
	int servPort;
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
#endif

	for(;;)
	{
		printf(". ");
		sleep(10);
	}

    return 0;
}

void SIGIOHandler(int signalType)
{
	struct sockaddr_in clientAddr;
	int rv;
	unsigned int clntLen;
	int recvMsgSize; // Could check this for IPv6
	char msgBuffer[MAXBUFFER];

	memset(msgBuffer, 0, MAXBUFFER);

	printf("signal = %d\n", signalType);
	clntLen = sizeof(clientAddr);
	rv = poll(socket_array, MAXSOCKETS, 0);
	printf("RV = %d\n", rv);
	
	if (rv == 0)
	{
		perror("poll() error");
		exit(-10);
	}
	else
	{
	for(int i = 0; i < MAXSOCKETS; i++)
		if (( recvMsgSize = recvfrom(socket_array[i].fd, msgBuffer, MAXBUFFER, 0, 
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
			// This can be a call to my conversion main()...conversion(	msgBuffer )?	
			switch(msgBuffer[0]) 
			{
				default:
					write(2, "Server received unexpected datagram contents\n", 46);
					strcpy(msgBuffer, "Unsuccessful\n");
					sendto(socket_array[i].fd, msgBuffer, strlen(msgBuffer), 0, (struct sockaddr *) &clientAddr, clntLen);
				case 'D':
					sleep(1);
					conversions(msgBuffer);
					sendto(socket_array[i].fd, msgBuffer, strlen(msgBuffer), 0, (struct sockaddr *) &clientAddr, clntLen);
			}
		}
	}
}
