#ifndef EMAIL_H
#define EMAIL_H

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>

#include "mqtt.h"

#define FROM_MAIL_USERNAME ""
#define FROM_MAIL_PASSWORD ""
#define SERVER ""

struct upload_status {
  size_t bytesRead;
};
 
size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp);
int email_sending_init();
void send_email(char recipients[MAX_RECIPIENTS][MAX_MAIL_LENGTH], int amountOfRecipients);

#endif