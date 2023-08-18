#include "additional.h"

extern struct mosquitto *mosq;
extern bool connectedToTheBroker;

void signal_handler(int signal)
{
    if (mosq && connectedToTheBroker){
        mosquitto_disconnect(mosq);
        connectedToTheBroker = false;
    }
}

bool is_json(const char json_string[256]) 
{
    cJSON *json = cJSON_Parse(json_string);
    if (json == NULL) {
        cJSON_Delete(json);
        return false;
    }

    cJSON_Delete(json);
    return true; 
}

char* current_time()
{
    time_t currentTime;
    struct tm *localTime;
    char *currTime = (char *)malloc(20 * sizeof(char));

    time(&currentTime);
    localTime = localtime(&currentTime);

    sprintf(currTime, "%04d-%02d-%02d %02d:%02d:%02d",
           localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
           localTime->tm_hour + 3, localTime->tm_min, localTime->tm_sec);

    return currTime;
}