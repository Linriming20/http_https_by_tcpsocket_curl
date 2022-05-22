#include <stdio.h>
#include <stdlib.h>

#include "curl/curl.h"

// refer to: https://blog.csdn.net/rong_toa/article/details/105689080
//   or curl-7.82.0/docs/examples/http-post.c 

int debug_callback(CURL *curl, curl_infotype type, char *data, size_t size, void *userptr)
{
	(void)curl; /* prevent compiler warning */
	(void)size;
	(void)userptr;

	char *typeString = NULL;

	switch(type)
	{
		case CURLINFO_TEXT:         typeString = "CURLINFO_TEXT";         break;
		case CURLINFO_HEADER_IN:    typeString = "CURLINFO_HEADER_IN";    break;
		case CURLINFO_HEADER_OUT:   typeString = "CURLINFO_HEADER_OUT";   break;
		case CURLINFO_DATA_IN:      typeString = "CURLINFO_DATA_IN";      break;
		case CURLINFO_DATA_OUT:     typeString = "CURLINFO_DATA_OUT";     break;
		case CURLINFO_SSL_DATA_IN:  typeString = "CURLINFO_SSL_DATA_IN";  break;
		case CURLINFO_SSL_DATA_OUT: typeString = "CURLINFO_SSL_DATA_OUT"; break;
		default: return -1;
	}
	fprintf(stdout, "\e[32m[%s] >> %s \e[0m", typeString, data);

	return 0;
}

int main(int argc, char *argv[])
{
	CURL *curl = NULL;
	CURLcode res;
	char *url = NULL;
	int port = 443; // default https server port

	if(argc != 2 && argc != 3)
	{
		printf("Usage:\n"
			   "   %s <URL> <port, default=443>\n"
			   "Examples: \n"
			   "   %s http://www.baidu.com 80\n"
			   "   %s https://www.baidu.com 443\n", argv[0], argv[0], argv[0]);
		return -1;
	}
	url = argv[1];
	if(argc == 3)
		port = atoi(argv[2]);

	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	curl = curl_easy_init();
	if(curl == NULL)
	{
		fprintf(stderr, "[%s: %d] curl_easy_init failed!\n", __FUNCTION__, __LINE__);
		goto exit;
	}

	#if 1 // refer to: curl-7.82.0/docs/examples/debug.c
	curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);

	/* the DEBUGFUNCTION has no effect until we enable VERBOSE */
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	#endif

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_PORT, port);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, NULL);

	/* Perform the request, res will get the return code */
	res = curl_easy_perform(curl);

	/* Check for errors */
	if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

exit:
	/* always cleanup */
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return 0;
}

