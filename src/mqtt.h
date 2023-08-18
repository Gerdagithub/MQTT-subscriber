#ifndef MQTT_H
#define MQTT_H

#include <mosquitto.h>
#include <syslog.h>

#include "argp_for_daemon.h"
#include "sqlite_mqtt.h"

#define MAX_RECIPIENTS 20
#define MAX_TOPICS 50
#define MAX_EVENTS 50

typedef struct{
    char topicName[256];
    short int qos;
} Topic;

void on_connect_callback(struct mosquitto *mosq, void *userdata, int result);
int mosquitto_init(int keepalive);
void subscribe_to_topics(Topic *topics, int amountOfTopics);
void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
void save_topic_payload(char topic[256], char payload[256]);

#endif