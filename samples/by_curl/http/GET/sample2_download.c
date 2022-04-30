#include <stdio.h>
#include <stdlib.h>

#include "curl/curl.h"

// refer to: https://blog.csdn.net/rong_toa/article/details/105689080
//   or curl-7.82.0/docs/examples/http-post.c 

size_t write_callback(const void *ptr, size_t size, size_t nmemb, void *stream)
{
	return fwrite(ptr, size, nmemb, (FILE *)stream);
}

int main(int argc, char *argv[])
{
	CURL *curl = NULL;
	CURLcode res;
	char *url = NULL;
	int port = 80; // default http server port
	char *filename = NULL;
	FILE *fpDownload = NULL;

	if(argc != 3 && argc != 4)
	{
		printf("Usage:\n"
			   "   %s <URL> <port, default=80> <save as filename>\n"
			   "Examples:\n"
			   "   %s www.baidu.com 80 index.html\n"
			   "   %s http://xzd.197946.com/sscom32.zip sscom32.zip\n", argv[0], argv[0], argv[0]);
		return -1;
	}
	url = argv[1];
	if(argc == 3)
	{
		filename = argv[2];
	}
	else
	{
		port = atoi(argv[2]);
		filename = argv[3];
	}
	
	fpDownload = fopen(filename, "wb");
	
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
	curl_easy_setopt(curl, CURLOPT_HTTPGET, url);

#if 1
	/* save to file */
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fpDownload);
#elif 0
	/* save to file */
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fpDownload);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
#elif 0
	/* print to stdout */
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, stdout);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
#endif

	/* Perform the request, res will get the return code */
	res = curl_easy_perform(curl);

	/* Check for errors */
	if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

exit:
	/* always cleanup */
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	if(fpDownload) fclose(fpDownload);

	return 0;
}

