#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>


static int getIpAddrByDomainName(char *hostname, char *ip);


int main(int argc, char *argv[])
{
	char *domainName = NULL;
	char ipAddr[32] = {0};

	if(argc != 2)
	{
		printf("Usage:\n"
			   "    %s  <domain name>\n", argv[0]);
		return -1;
	}

	domainName = argv[1];

	if(0 == getIpAddrByDomainName(domainName, ipAddr))
	{
		printf("[%s] has a ip address was [%s]\n", domainName, ipAddr);
	}
	else
	{
		printf("getIpAddrByHostname failed!\n");
		return -1;
	}
	
	return 0;
}



/* function: get ip address by domain name.
 * arguments£º
 *      domain_name[in];
 *      ip[out].
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
			break;
	}

	return 0;
}