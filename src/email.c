#include "email.h"

CURL *curl;
struct upload_status uploadCtx = { 0 };
char emailPayloadText[1500];

extern Event events[];

size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{  
    struct upload_status *uploadCtx = (struct upload_status *)userp;
    const char *data;
    size_t room = size * nmemb;
 
    if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
        return 0;
    }
 
    data = &emailPayloadText[uploadCtx->bytesRead];

    if (data) {
        size_t len = strlen(data);
        if(room < len)
        len = room;
        memcpy(ptr, data, len);
        uploadCtx->bytesRead += len;
 
        return len;
    }
 
    return 0;
}

int email_sending_init()
{   
    CURLcode ret = CURLE_OK;
    curl = curl_easy_init();

    ret = curl_easy_setopt(curl, CURLOPT_USERNAME, FROM_MAIL_USERNAME);
    if (ret){
        syslog(LOG_USER | LOG_ERR, "curl_easy_setopt() that sets username failed: %s", curl_easy_strerror(ret)); 
        return ret;
    }
    ret = curl_easy_setopt(curl, CURLOPT_PASSWORD, FROM_MAIL_PASSWORD);
    if (ret){
        syslog(LOG_USER | LOG_ERR, "curl_easy_setopt() that sets password, failed: %s", curl_easy_strerror(ret)); 
        return ret;
    }
    ret = curl_easy_setopt(curl, CURLOPT_URL, SERVER);
    if (ret){
        syslog(LOG_USER | LOG_ERR, "curl_easy_setopt() that sets server, failed: %s", curl_easy_strerror(ret)); 
        return ret;
    }
    ret = curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    if (ret){
        syslog(LOG_USER | LOG_ERR, "curl_easy_setopt() for secure connections, failed: %s", curl_easy_strerror(ret)); 
        return ret;
    }
    
    ret = curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM_MAIL_USERNAME);
    if (ret){
        syslog(LOG_USER | LOG_ERR, "curl_easy_setopt() that sets sender's username, failed: %d", ret); 
        return ret;
    }

    ret = curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    if (ret){
        syslog(LOG_USER | LOG_ERR, "curl_easy_setopt() that sets payload function, failed: %d", ret); 
        return ret;
    }
    ret = curl_easy_setopt(curl, CURLOPT_READDATA, &uploadCtx);
    if (ret){
        syslog(LOG_USER | LOG_ERR, "curl_easy_setopt() that sets uploadCtx, failed: %d", ret); 
        return ret;
    }
    ret = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    if (ret)
        syslog(LOG_USER | LOG_ERR, "curl_easy_setopt() that sets CURLOPT_UPLOAD, failed: %d", ret); 

    return ret;
}

void send_email(char recipients[MAX_RECIPIENTS][MAX_MAIL_LENGTH], int amountOfRecipients)
{
    int ret = 0;
    uploadCtx.bytesRead = 0;

    struct curl_slist *recipientsCurlList = NULL;
    for (int i = 0; i < amountOfRecipients; i++){
        recipientsCurlList = curl_slist_append(recipientsCurlList, 
            recipients[i]);
    }
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipientsCurlList);

    ret = curl_easy_perform(curl);
    if (ret != CURLE_OK)
        syslog(LOG_USER | LOG_ERR, "Email is not sent: %s", curl_easy_strerror(ret));

    curl_slist_free_all(recipientsCurlList);
}