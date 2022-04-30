#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

// refer to: https://blog.csdn.net/u010835747/article/details/119149276

int httpDownloadSample(char *download_url);


int main(int argc, char *argv[])
{
	char *download_url = NULL;

	if(argc != 2)
	{
		printf("Usage:\n"
			   "    %s <download URL>\n"
			   "Examples:\n"
			   "    %s http://xzd.197946.com/sscom32.zip\n", argv[0], argv[0]);
		return -1;
	}
	download_url = argv[1];

	httpDownloadSample(download_url);

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

/* function: send data(http request) to http server  by tcp socket.
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

/* function: parse HTTP download URL.(such as: http://xzd.197946.com/sscom32.zip)
 * arguments:
 *      download_url[in]: as it name;
 *      domain_name[out]: http server domain name;
 *      port[out]: http server port;
 *      filename[out]: will be downloaded filename.
 * return:  0: success, -1: failed
 */
int parseHttpDownloadUrl(char *download_url, char *domain_name, int *port, char *filename)
{
	char *httpPrefix = "http://";
	char *httpsPrefix = "https://";
	int defaultServerPort = 80;
	char *pDomainName = NULL;
	char *pPort = NULL;
	char *pFileName = NULL;

	if(!download_url || !domain_name || !port || !filename)
	{
		fprintf(stderr, "[%s: %d] Invaild params!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if((pDomainName = strstr(download_url, httpPrefix)) != NULL)
	{
		if((pPort = strchr(download_url + strlen(httpPrefix), ':')) != NULL)
		{
			memcpy(domain_name,
				   pDomainName + strlen(httpPrefix),
				   pPort - (pDomainName + strlen(httpPrefix)));
			sscanf(pPort, ":%d/%s", port, filename);
		}
		else
		{
			if((pFileName = strchr(download_url + strlen(httpPrefix), '/')) != NULL)
			{
				memcpy(domain_name,
					   pDomainName + strlen(httpPrefix),
					   pFileName - (pDomainName + strlen(httpPrefix)));
				sscanf(pFileName, "/%s", filename);
			}
			*port = defaultServerPort;
		}
	}
	else if(strstr(download_url, "https://"))
	{
		fprintf(stderr, "[%s: %d] Donn't support HTTPS profile now!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}


/* function: print download progress bar.
 * arguments:
 *      percent : as it name(0~100).
 * return: none.
 */
void printDownloadProgressBar(int percent)
{
	int i = 0;
	char bar[50+1] = {0};
	static int last_percent = 0;

	if(percent == last_percent)
    {
        return;
    }

	i = percent / 2;
	if(i > 50)
		i = 50;
	memset(bar, '=', i);

	printf("\r%d%%[%s]", percent, bar);

	last_percent = percent;

    return;
}

/* function: sample of http download file.
 * arguments:
 *      download_url[in]: as it name;
 * return:  0: success, -1: failed
 */
int httpDownloadSample(char *download_url)
{
	char domainName[128] = {0};
	char filename[128] = {0};
	int serverPort = -1;
	int socketFd = -1;
	int ret = -1;
	int sendBufSize = 1024;
	int recvBufSize = 1024;
	char *sendBuf = (char *)calloc(sendBufSize, 1);
	char *recvBuf = (char *)calloc(recvBufSize, 1);
	char ch = 0;
	int recvIndex = 0;
	FILE *fpDownload = NULL;
	unsigned long long curDownloadLength = 0;
	int statusCode = 0;
	unsigned long long contentLength = 0;

	if(!download_url)
	{
		fprintf(stderr, "[%s: %d] Invaild params!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	ret = parseHttpDownloadUrl(download_url, domainName, &serverPort, filename);
	if(ret < 0)
	{
		fprintf(stderr, "[%s: %d] tcpSocketConnectToHttpServer failed!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	socketFd = tcpSocketConnectToHttpServer(domainName, serverPort);
	if(socketFd < 0)
	{
		fprintf(stderr, "[%s: %d] tcpSocketConnectToHttpServer failed!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	fpDownload = fopen(filename, "wb");
	if(fpDownload == NULL)
	{
		fprintf(stderr, "[%s: %d] create file(%s) failed!\n", __FUNCTION__, __LINE__, filename);
		return -1;
	}

	#if 1 // optional
	int timeout_s = 2;
	ret = tcpSocketSetupTransTimeOut(socketFd, timeout_s);
	if(ret < 0)
	{
		fprintf(stderr, "[%s: %d] tcpSocketSetupTransTimeOut failed!\n", __FUNCTION__, __LINE__);
		goto exit;
	}
	#endif

	snprintf(sendBuf, sendBufSize,
			"GET %s HTTP/1.1\r\n"
			"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
			"User-Agent:Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537(KHTML, like Gecko) Chrome/47.0.2526Safari/537.36\r\n"
			"Host:%s:%d\r\n"
			"Connection:close\r\n"
			"\r\n",
			download_url/**/, domainName, serverPort);

	printf("\e[34mlocal machine ---------------> remote http server(%s:%d)\n\e[0m", domainName, serverPort);
	printf("%s", sendBuf);

	ret = tcpSocketSendHttpRequest(socketFd, sendBuf, strlen(sendBuf));
	if(ret < 0)
	{
		fprintf(stderr, "[%s: %d] tcpSocketSetupTransTimeOut failed!\n", __FUNCTION__, __LINE__);
		goto exit;
	}

	/* get response header */
	while((ret = tcpSocketRecvHttpResponse(socketFd, &ch, sizeof(ch)/* =1 */)) > 0)
	{
		recvBuf[recvIndex] = ch;

		if(recvIndex >= 3 &&
		   recvBuf[recvIndex - 3] == '\r'&&
		   recvBuf[recvIndex - 2] == '\n' &&
		   recvBuf[recvIndex - 1] == '\r' &&
		   recvBuf[recvIndex] == '\n')
		{
			char *p = NULL;

			printf("\e[32mlocal machine <--------------- remote http server(%s:%d)\n\e[0m", domainName, serverPort);
			printf("%s", recvBuf);

			if(p = strstr(recvBuf, "HTTP/"))
				sscanf(p, "%*s %d", &statusCode);
			if(statusCode != 200)
			{
				fprintf(stderr, "\e[31m[%s: %d] Could not download [%s]!\n\e[0m", __FUNCTION__, __LINE__, filename);
				goto exit;
			}

			if(p = strstr(recvBuf, "Content-Length:"))
				sscanf(p, "%*s %lld", &contentLength);

			break;
		}

		recvIndex++;

		#if 0 // expand memory size
		if(recvIndex >= recvBufSize)
		{
			recvBufSize *= 2;
			recvBuf = (char *)realloc(recvBuf, recvBufSize);
		}
		#endif
	}

	/* get response body(file data) */
	while((ret = tcpSocketRecvHttpResponse(socketFd, recvBuf, recvBufSize)) > 0)
	{
		fwrite(recvBuf, ret, 1, fpDownload);
		curDownloadLength += ret;
		printDownloadProgressBar(100*curDownloadLength/contentLength);
	}
	fprintf(stdout, "\e[32m\nDownload [%s] successfully!\n\e[0m", filename);

exit:
	if(socketFd) tcpSocketDisconnectFromHttpServer(socketFd);
	if(sendBuf) free(sendBuf);
	if(recvBuf) free(recvBuf);
	if(fpDownload) fclose(fpDownload);

	return 0;
}
