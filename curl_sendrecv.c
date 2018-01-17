
#include "../../core/sr_module.h"
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

CURL *curl;

void init_curl(void) {

}

size_t curl_recv(void *buffer, size_t size, size_t nmemb, void *userp) {
	str *recv_data = (str *) userp;
	int chunk_size = nmemb*size;
	int new_size = recv_data->len + chunk_size;
	LM_INFO("recv_bytes[%d]\n", chunk_size);

	recv_data->s = (char*) pkg_realloc(recv_data->s, new_size + 1);
	if (recv_data->s == NULL) {
		LM_ERR("no pkg memory left\n");
		return -1;
	}
	memcpy(recv_data->s + recv_data->len, buffer, chunk_size);
	recv_data->len = new_size;
	recv_data->s[recv_data->len] = '\0';

	size_t recv_bytes = size * nmemb;
	LM_INFO("recv_bytes[%ld][%s]\n", recv_bytes, (char*) buffer);
	return recv_bytes;
}

int curl_send(const char* uri, char *post_data, str *recv_data){
	LM_INFO("sending to[%s]", uri);
	CURL *curl_handle;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();

	char curl_error[CURL_ERROR_SIZE + 1];
	curl_error[CURL_ERROR_SIZE] = '\0';

	if (curl_handle) {
		res = curl_easy_setopt(curl_handle, CURLOPT_URL, uri);
		if (res != CURLE_OK)
			LM_INFO("Error: %s\n", curl_easy_strerror(res));
		// curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_recv);
		if (res != CURLE_OK)
			LM_INFO("Error: %s\n", curl_easy_strerror(res));
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) recv_data);
		if (res != CURLE_OK)
			LM_INFO("Error: %s\n", curl_easy_strerror(res));
		curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, curl_error);
		if (res != CURLE_OK)
			LM_INFO("Error: %s\n", curl_easy_strerror(res));
		if (post_data) {
			res = curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_data);
			if (res != CURLE_OK)
				LM_INFO("Error: %s\n", curl_easy_strerror(res));
		}

		struct curl_slist *list = NULL;
		list = curl_slist_append(list, "Expect:");
		list = curl_slist_append(list, "Content-Type: application/json");
		res = curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, list);
		if (res != CURLE_OK)
			LM_INFO("Error: %s\n", curl_easy_strerror(res));

		res = curl_easy_perform(curl_handle);
		if (res != CURLE_OK)
			LM_INFO("Error: %s\n", curl_easy_strerror(res));

		LM_INFO("sent/receive completed  !\n");
		curl_easy_cleanup(curl_handle);
		// curl_slist_free_all(list); /* free the list again */
		curl_global_cleanup();
	}
	return 1;
}
