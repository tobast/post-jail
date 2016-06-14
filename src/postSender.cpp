#include "postSender.h"
#include <curl/curl.h>

bool sendChallenge(const char* header, const char* post, const char* url) {
	CURL* curl = curl_easy_init();
	if(!curl) {
		fprintf(stderr, "Cannot initialize cURL.\n");
		return false;
	}


	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);

	struct curl_slist* headList = curl_slist_append(NULL, header);
	if(headList == NULL) {
		fprintf(stderr, "Cannot allocate HTTP headers.\n");
		return false;
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headList);

	CURLcode res = curl_easy_perform(curl);
	if(res != CURLE_OK) {
		fprintf(stderr, "cURL error: %s\n", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
		return false;
	}

	curl_easy_cleanup(curl);
	return true;
}

