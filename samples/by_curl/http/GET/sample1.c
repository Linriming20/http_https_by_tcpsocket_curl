#include <stdio.h>
#include <stdlib.h>

#include "curl/curl.h"

// refer to: https://blog.csdn.net/rong_toa/article/details/105689080
//   or curl-7.82.0/docs/examples/http-post.c 

int main(int argc, char *argv[])
{
	CURL *curl = NULL;
	CURLcode res;
	char *url = NULL;
	int port = 80; // default http server port

	if(argc != 2 && argc != 3)
	{
		printf("Usage:\n"
			   "   %s <URL> <port, default=80>\n", argv[0]);
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

