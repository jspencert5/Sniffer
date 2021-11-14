#include <curl/curl.h>

int main(){

    CURL *curl = curl_easy_init(); // initilze library
    CURLcode res;

    struct curl_slist *headers = NULL;

    curl_easy_setopt(curl, CURLOPT_URL, "127.0.0.1:3000/");

    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{ \"hello\": \"darkness\"}");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);

    curl_easy_cleanup(curl); // cleanup

    return 0;

}