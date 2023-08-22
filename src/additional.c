#include "additional.h"
#include "mqtt.h"

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
           localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    return currTime;
}

void get_json_value(cJSON *root, char parameterName[256], cJSON **jsonValue) {
    cJSON *dataObject = cJSON_GetObjectItem(root, "data");
    if (dataObject == NULL) {
        syslog(LOG_USER | LOG_ERR, "Error ocurred while retrieving data object.\n");
        return;
    }

    *jsonValue = cJSON_GetObjectItem(dataObject, parameterName);
    if (*jsonValue == NULL) {
        syslog(LOG_USER | LOG_ERR, "Error ocurred while retrieving jsonValue value.\n");
    }
}

bool value_meets_event_condition(cJSON *jsonValue, char valueType[20], int comparisonType,
     char comparisonValue[5])
{
    if (jsonValue && !strcmp(valueType, "decimal") && jsonValue->type == cJSON_Number) {
        char *endptr;
        double comparisonValDouble = strtod(comparisonValue, &endptr);

        if (*endptr != '\0') {
            syslog(LOG_USER | LOG_ERR, "Conversion from string to double failed.");
        }

        if (compare_number(jsonValue->valuedouble, comparisonType, comparisonValDouble)){
            return true;
        }

        return false;
    }

    if (jsonValue && !strcmp(valueType, "alphanumeric") && jsonValue->type == cJSON_String) {
        if (compare_string(jsonValue->valuestring, comparisonType, comparisonValue)){
            return true;
        }
    }

    return false;
}

bool compare_number(double number, int comparisonType, double comparisonValue)
{
    switch (comparisonType){
        case LESS:
            if (number < comparisonValue)
                return true;
            break;
        
        case GREATER:
            if (number > comparisonValue)
                return true;
            break;
        
        case LESS_EQUAL:
            if (number <= comparisonValue)
                return true;
            break;

        case GREATER_EQUAL:
            if (number >= comparisonValue)
                return true;
            break;

        case EQUAL:
            if (number == comparisonValue)
                return true;
            break;

        case NOT_EQUAL:
            if (number != comparisonValue)
                return true;
            break;

        default:
            return false;
    }

    return false;
}

bool compare_string(char value[256], int comparisonType, char comparisonValue[256])
{
    switch (comparisonType){
        case EQUAL:
            if (!strcmp(value, comparisonValue))
                return true;
            break;

        case NOT_EQUAL:
            if (strcmp(value, comparisonValue))
                return true;
            break;

        default:
            return false;
    }

    return false;
}

int send_email()
{
    
}
