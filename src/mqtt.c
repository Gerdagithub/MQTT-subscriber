#include "mqtt.h"

struct mosquitto *mosq = NULL;
bool connectedToTheBroker = false;
int retMqtt = 0;

extern struct Arguments argpArguments; 

void on_connect_callback(struct mosquitto *mosq, void *userdata, int result) 
{
    if (result) {
        retMqtt = result;
        mosquitto_disconnect(mosq);
    }
}

int mosquitto_init(int keepalive)
{
    int ret = 0;

    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        syslog(LOG_USER | LOG_ERR, "Error: Out of memory");
        return 1;
    }

    if (strcmp(argpArguments.cafilePath, "")){
        bool useCertAndKey = (strcmp(argpArguments.certPath, "") && strcmp(argpArguments.keyPath, ""));

        ret = mosquitto_tls_set(
            mosq, argpArguments.cafilePath,
            NULL, useCertAndKey? argpArguments.certPath : NULL,
            useCertAndKey? argpArguments.keyPath : NULL, NULL
        );
    
        if (ret != MOSQ_ERR_SUCCESS){
            syslog(LOG_USER | LOG_ERR, "mosquitto_tls_set failed: %s", mosquitto_strerror(ret));
            return ret;
        }
    }

    if (strcmp(argpArguments.username, "") && strcmp(argpArguments.password, "")){
        ret = mosquitto_username_pw_set(mosq, argpArguments.username, argpArguments.password);
        if (ret != MOSQ_ERR_SUCCESS) {
            syslog(LOG_USER | LOG_ERR, "mosquitto_username_pw_set failed: %s", mosquitto_strerror(ret));
            return ret;
        }
    }

    mosquitto_connect_callback_set(mosq, on_connect_callback);
    
    ret = mosquitto_connect(mosq, argpArguments.host, argpArguments.port, keepalive);
    if (ret != MOSQ_ERR_SUCCESS) {
        syslog(LOG_USER | LOG_ERR, "mosquitto_connect failed: %s", mosquitto_strerror(ret));
        return ret;
    }
    else connectedToTheBroker = true;

    mosquitto_message_callback_set(mosq, message_callback);

    return ret;
}

void subscribe_to_topics(Topic *topics, int amountOfTopics)
{
    for (int i = 0; i < amountOfTopics; i++){
        if (mosquitto_subscribe(mosq, NULL, topics[i].topicName, topics[i].qos) != MOSQ_ERR_SUCCESS) {
            syslog(LOG_USER | LOG_ERR, "Unable to subscribe to topic");
        }
    }
}

void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    if(!is_json(message->payload))
        return;

    int ret = insert_topic_to_database(message->topic, message->payload);
    if (ret)
        syslog(LOG_USER | LOG_ERR, "Topic data was not inserted to DB");
}