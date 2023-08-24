#include "mqtt.h"
#include "additional.h"

Event events[MAX_EVENTS];
int amountOfEvents = 0;
struct mosquitto *mosq = NULL;
bool connectedToTheBroker = false;
int retMqtt = 0;

extern struct Arguments argpArguments;
extern char emailPayloadText[];

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
    int ret = 0;

    for (int i = 0; i < amountOfTopics; i++){
        ret = mosquitto_subscribe(mosq, NULL, topics[i].topicName, topics[i].qos);
        if (ret != MOSQ_ERR_SUCCESS) {
            syslog(LOG_USER | LOG_ERR, "Unable to subscribe to topic: %s", mosquitto_strerror(ret));
        }
    }
}

void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    int ret = insert_topic_to_database(message->topic, message->payload);
    if (ret)
        syslog(LOG_USER | LOG_ERR, "Topic data was not inserted to DB");

    if (!is_json(message->payload)){
        syslog(LOG_USER | LOG_WARNING, "Received message is not json");
        return;
    }

    trigger_events(message->topic, message->payload);
}

void trigger_events(char topic[256], char dataInJson[256])
{
    for (int i = 0; i < amountOfEvents; i++){
        if (!strcmp(events[i].topicName, topic)){
            cJSON *root = cJSON_Parse(dataInJson);
            if (root == NULL) {
                syslog(LOG_USER | LOG_ERR, "Error parsing JSON.\n");
                continue;
            }

            cJSON *jsonValue = NULL;
            get_json_value(root, events[i].parameterName, &jsonValue);

            if (jsonValue != NULL && value_meets_event_condition(jsonValue, events[i].valueType,
            events[i].comparisonType, events[i].comparisonValue)){
                change_email_payload(events[i].joinedRecipients, topic, dataInJson);
                send_email(events[i].recipients, events[i].amountOfRecipients);
            }

            cJSON_Delete(root);
        }
    }
}

void mosquitto_deinit(Topic *topics, int amountOfTopics)
{
    if (mosq && connectedToTheBroker)
        mosquitto_disconnect(mosq); 

    if (mosq)
        mosquitto_destroy(mosq);

    mosquitto_lib_cleanup();
}