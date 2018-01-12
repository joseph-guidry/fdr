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

#define MAXBUFFER 512
#define MAXSOCKETS 6

#define MULTIPLE

void SIGIOHandler(int signalType);
int create_IP4socket(int port_offset);
int create_IP6socket(int port_offset);

/* GLOBAL SERVER SOCKET FOR SIG HANDLER */
struct pollfd socket_array[MAXSOCKETS];

int main(void)
{
	struct sigaction handler;
	unsigned short servPortOffset[] = {0, 0, 1000, 1000, 2000, 2000};
	

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

#ifdef MULTIPLE
	for (unsigned int i = 0; i < MAXSOCKETS; i++)
	{
		if (i % 2 == 0)			// Allow for both IPv4 and IPv6 sockets to be created. with 3 ports = 6 sockets
		{						// 0, 2, 4 = IPv4
			printf("making IPv4\n");
			socket_array[i].fd = create_IP4socket(servPortOffset[i]);
			socket_array[i].events = POLLIN;
			socket_array[i].revents = 0;
		}
		else
		{
			printf("making IPv6\n");
			socket_array[i].fd = create_IP6socket(servPortOffset[i]);
			socket_array[i].events = POLLIN;
			socket_array[i].revents = 0;
		}
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
	printf("signal = %d\n", signalType);

	struct sockaddr_storage clientAddr;
	socklen_t clntLen = sizeof(clientAddr);
	int recvMsgSize;
	
	char msgBuffer[MAXBUFFER];
	memset(msgBuffer, 0, MAXBUFFER);

	int rv = poll(socket_array, MAXSOCKETS, 0);
	printf("RV = %d\n", rv);
	
	if (rv == 0)
	{
		perror("poll() error");
		exit(-10);
	}
	else
	{
		for(int i = 0; i < MAXSOCKETS; i++)
		{
			if ((socket_array[i].revents & POLLIN) == 0)
				continue;
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
				printf("Server received a datagram from host\n");
				if(conversions(msgBuffer, MAXBUFFER) < 0)
				{
					printf("ERROR");
	// 	insert error messgae: sendto(socket_array[i].fd, msgBuffer, strlen(msgBuffer), 0, (struct sockaddr *) &clientAddr, clntLen);			
				}
				//SUCCESS!!
				sendto(socket_array[i].fd, msgBuffer, strlen(msgBuffer), 0, (struct sockaddr *) &clientAddr, clntLen);			
			}
		}
	}
}

/* Eventually I could reduce the duplication of commmands */

int create_IP4socket(int port_offset)
{
    struct sockaddr_in serverINET;
	int servSocket;
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
		perror("bind() failure ip4");
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


int create_IP6socket(int port_offset)
{
    struct sockaddr_in6 serverINET6;
	int servSocket;
	int servPort;
	int sockFlags;

	servPort = getuid() + port_offset;
	printf("Server Port: [%u]\n", servPort);
	
	/* Create a IPv4 UDP socket on UID port */
	if ((servSocket = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror("Socket() failure");
		exit(-1);
	}

	/* Local Address info */
	memset(&serverINET6, 0, sizeof(struct sockaddr_in6));
	serverINET6.sin6_family = AF_INET6;
	serverINET6.sin6_flowinfo = 0;
	serverINET6.sin6_addr = in6addr_loopback;
	serverINET6.sin6_port = htons(servPort);

	/* Bind socket to address */
	if (bind(servSocket, (struct sockaddr *)&serverINET6, sizeof(struct sockaddr_in6)) < 0)
	{
		perror("bind() failure ip6");
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
