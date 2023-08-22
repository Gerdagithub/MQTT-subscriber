#ifndef MQTT_H
#define MQTT_H

#include <mosquitto.h>
#include <syslog.h>
#include <cJSON.h>

#include "argp_for_daemon.h"
#include "sqlite_mqtt.h"

#define MAX_RECIPIENTS 20
#define MAX_TOPICS 50
#define MAX_EVENTS 50

#define LESS      1
#define GREATER   2
#define LESS_EQUAL 3
#define GREATER_EQUAL 4
#define EQUAL     5
#define NOT_EQUAL 6

typedef struct{
    char topicName[256];
    short int qos;
} Topic;

typedef struct{
    char topicName[256];
    char parameterName[256];
    char valueType[20];
    int comparisonType;
    char comparisonValue[256];
} Event;

void on_connect_callback(struct mosquitto *mosq, void *userdata, int result);
int mosquitto_init(int keepalive);
void subscribe_to_topics(Topic *topics, int amountOfTopics);
void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
void trigger_events(char topic[256], char dataInJson[256]);
void mosquitto_deinit(Topic *topics, int amountOfTopics);

#endif