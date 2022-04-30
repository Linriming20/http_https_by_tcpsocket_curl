#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>

// refer to: https://blog.csdn.net/weixin_37569048/article/details/91047343

int httpPostSample(void);


int main(int argc, char *argv[])
{
	httpPostSample();

	return 0;
}



/* function: get ip address by domain name.
 * arguments:
 *      domain_name[in];
 *      ip[out].(must alloc memory before the function called.)
 * return:  0: success, -1: failed
 */
static int getIpAddrByDomainName(char *domain_name, char *ip)
{
	char **pp = NULL;
	struct hostent *pHostent = NULL;
	const char *ipPtr = NULL;
	char ipStr[32] = {0}; 

	if(!domain_name || !ip)
	{
		fprintf(stderr, "[%s: %d] Invaild params!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if((pHostent = gethostbyname(domain_name)) == NULL)
	{
		fprintf(stderr, "[%s: %d] gethostbyname fialed!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	switch(pHostent->h_addrtype)
	{
		case AF_INET:
		case AF_INET6:
			#if 0 /* print something */
			pp = pHostent->h_addr_list;
			for(; *pp != NULL; pp++)
			{
				fprintf(stdout, "[%s: %d] IP address: %s\n", __FUNCTION__, __LINE__,
											inet_ntop(pHostent->h_addrtype, *pp, ipStr, sizeof(ipStr)));
			}
			#endif

			/* just get the first ip address. */
			ipPtr = inet_ntop(pHostent->h_addrtype, pHostent->h_addr, ipStr, sizeof(ipStr));
			if(ipPtr)
				memcpy(ip, ipStr, sizeof(ipStr));
			break;
		default:
			fprintf(stderr, "[%s: %d] Unknown address type!\n", __FUNCTION__, __LINE__);
			return -1;
	}

	return 0;
}

/* function: connect to http server(domain name) by tcp socket.
 * arguments:
 *      domain_name[in]: http server domain name.
 *      server_port: as it name.
 * return:  0: success, -1: failed
 */
static int tcpSocketConnectToHttpServer(char *domain_name, int server_port)
{
	int socketFd = -1;
	struct sockaddr_in serverAddr = {};
	int ret = -1;
	char ipAddr[32] = {0};

	if(!domain_name || !server_port)
	{
		fprintf(stderr, "[%s: %d] Invaild params!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ret = getIpAddrByDomainName(domain_name, ipAddr);
	if(ret < 0)
	{
		fprintf(stderr, "[%s: %d] getIpAddrByDomainName failed!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketFd < 0)
	{
		fprintf(stderr, "[%s: %d] socket error!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(server_port);
	if(0 == inet_aton(ipAddr, &serverAddr.sin_addr))
	{
		fprintf(stderr, "[%s: %d] invalid server ip!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ret = connect(socketFd, (const struct sockaddr *)&serverAddr, sizeof(struct sockaddr));
	if(ret < 0)
	{
		fprintf(stderr, "[%s: %d] connect failed!\n", __FUNCTION__, __LINE__);
		return -1;
	}
	
	return socketFd;
}

/* function: set up tcp socket timeout of send/recieve.(optional)
 * arguments:
 *      socketFd: socket handler;
 *      timeout_s[in]: timeout in seconds;
 * return:  0: success, -1: failed
 */
int tcpSocketSetupTransTimeOut(int socketFd, int timeout_s)
{
	struct timeval timeout = {0}; 

	if(socketFd < 0 || timeout_s <= 0)
	{
		fprintf(stderr, "[%s: %d] Invaild params!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	timeout.tv_sec = timeout_s;
	timeout.tv_usec = 0;

	setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
	setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

	return 0;
}

/* function: send data(http request) to http server by tcp socket.
 * arguments:
 *      socketFd: socket handler;
 *      request_playload[in]: http request data;
 *      playload_size: size of http request playload data size. 
 * return:  0: success, -1: failed
 */
static int tcpSocketSendHttpRequest(int socketFd, char *request_playload, int playload_size)
{
	int ret = -1;

	if(socketFd < 0 || !request_playload || !playload_size)
	{
		fprintf(stderr, "[%s: %d] Invaild params!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ret = send(socketFd, request_playload, playload_size, 0);
	if(ret < 0)
	{
		fprintf(stderr, "[%s: %d] send http request failed!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

/* function: recieve data form http server response by socket.
 * arguments:
 *      socketFd: socket handler;
 *      response_playload[out]: http response data;
 *              (must alloc memory before the function called.)
 *      playload_maxsize: max size of response_playload. 
 * return:  0: size of recieve data, -1: failed
 */
static int tcpSocketRecvHttpResponse(int socketFd, char *response_playload, int playload_maxsize)
{
	int recv_size = -1;

	if(socketFd < 0 || !response_playload || !playload_maxsize)
	{
		fprintf(stderr, "[%s: %d] Invaild params!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	recv_size = recv(socketFd, response_playload, playload_maxsize, 0);
	if(recv_size < 0)
	{
		fprintf(stderr, "[%s: %d] recvieve http response failed!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	return recv_size;
}

/* function: tcp socket disconnect from Http server(close socket).
 * arguments:
 *      socketFd: socket handler;
 * return: none.
 */
void tcpSocketDisconnectFromHttpServer(int socketFd)
{
	close(socketFd);
}

/* function: sample of http POST.
 * arguments: none.
 * return:  0: success, -1: failed
 */
int httpPostSample(void)
{
	char *domainName = "www.webxml.com.cn";
	char *uri = "/webservices/qqOnlineWebService.asmx/qqCheckOnline";
	int serverPort = 80; /* default port */
	int socketFd = -1;
	int ret = -1;
	int timeout_s = 2;
	int tmpBufSize = 1024;
	char *tmpBuf = (char *)malloc(tmpBufSize);
	char postPacketBody[64] = "qqCode=2607950424";
	int postPacketBodyLen = strlen(postPacketBody);

	socketFd = tcpSocketConnectToHttpServer(domainName, serverPort);
	if(socketFd < 0)
	{
		printf("tcpSocketConnectToHttpServer failed!\n");
		return -1;
	}

	ret = tcpSocketSetupTransTimeOut(socketFd, timeout_s);
	if(ret < 0)
	{
		printf("tcpSocketSetupTransTimeOut failed!\n");
		return -1;
	}

	snprintf(tmpBuf, tmpBufSize,
		"POST %s HTTP/1.1\r\n"
		"Host: %s:%d\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: %d\r\n"
		"Connection: keep-alive\r\n"
		"\r\n"
		"%s"
		"\r\n",
		uri, domainName, serverPort, postPacketBodyLen, postPacketBody);

	printf("\e[34mlocal machine ---------------> remote http server(%s:%d)\n\e[0m", domainName, serverPort);
	printf("%s", tmpBuf);

	ret = tcpSocketSendHttpRequest(socketFd, tmpBuf, strlen(tmpBuf));
	if(ret < 0)
	{
		printf("tcpSocketSetupTransTimeOut failed!\n");
		return -1;
	}

	memset(tmpBuf, 0, tmpBufSize);
	ret = tcpSocketRecvHttpResponse(socketFd, tmpBuf, tmpBufSize);
	if(ret < 0)
	{
		printf("tcpSocketSetupTransTimeOut failed!\n");
		return -1;
	}
	else if(ret > 0)
	{
		printf("\e[32mlocal machine <--------------- remote http server(%s:%d)\n\e[0m", domainName, serverPort);
		printf("%s\n", tmpBuf);
	}

	tcpSocketDisconnectFromHttpServer(socketFd);
	free(tmpBuf);
	return 0;
}
