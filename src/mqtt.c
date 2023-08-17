#include "mqtt.h"

struct mosquitto *mosq = NULL;
bool connectedToTheBroker = false;
extern struct Arguments argpArguments; 
int retMqtt = 0;

void on_connect_callback(struct mosquitto *mosq, void *userdata, int result) 
{
    if (result) {
        retMqtt = result;
        mosquitto_disconnect(mosq);
    }
}

int moquitto_init(int keepalive)
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
    }
    else connectedToTheBroker = true;

    return ret;
}