#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#include <pthread.h>
#include <mcheck.h>
#include <errno.h>

extern void set_debug_traps();
extern void breakpoint();
extern void _catchException0();

#define LISTEN_SMTRACESERVER_PORT 22222
#define MAX_CONN 1

int smtraceServerSocket = 0;
int newSocket = 0;

extern void smtrace();
extern void smuntrace();

static void* thread_function(void* arg)
{
	struct sockaddr_in gdbSockAddr_in;
	struct sockaddr_in addr_in;
	smtraceServerSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (smtraceServerSocket < 0)
	{
		printf("socket creation failure, errno=%d\n", errno);
		return;
	}
	bzero(&gdbSockAddr_in, sizeof(gdbSockAddr_in));
	gdbSockAddr_in.sin_family = AF_INET;
	gdbSockAddr_in.sin_port = htons(LISTEN_SMTRACESERVER_PORT);
	gdbSockAddr_in.sin_addr.s_addr = INADDR_ANY;

	if (bind(smtraceServerSocket, (struct socketaddr*)&gdbSockAddr_in, sizeof(gdbSockAddr_in)) < 0)
	{
		printf("socket binding failure, errno=%d\n", errno);
		close(smtraceServerSocket);
		return;
	}

	char buff[20];
	struct sockaddr_in client_addr_in;
	int addrSize = 0;
	while(1)
	{
		bzero(&client_addr_in, sizeof(client_addr_in));
		if (recvfrom(smtraceServerSocket, buff, sizeof(buff), 0, (struct socketaddr*)&client_addr_in, &addrSize) < 0)
		{
			printf("socket receive failure, errno=%d\n", errno);
			close(smtraceServerSocket);
			return;
		}

		if (buff[0] == 'o')
		{
			printf("memory tracing switch on\n");
			smtrace();
		}
		else if (buff[0] == 'c')
		{
			printf("memory tracing switch off\n");
			smuntrace();
		}
	}
	return;
}

static void injectStub()
{
	pthread_t mythread;

	//sleep(10);

	if ( pthread_create( &mythread, NULL, thread_function, NULL) ) {
    		printf("error creating thread.");
    		abort();
  	}
	return;
}

void _init()
{
	injectStub();
	printf("Memory leak detection lib has been attached successfully\n");
}

void _fini()
{
	smuntrace();
	printf("Memory leak detection lib has been de-attached successfully\n");
}
