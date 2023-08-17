#include "mqtt.h"

struct mosquitto *mosq = NULL;
extern struct Arguments argpArguments; 

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

    ret = mosquitto_connect(mosq, argpArguments.host, argpArguments.port, keepalive);
    if (ret != MOSQ_ERR_SUCCESS) {
        syslog(LOG_USER | LOG_ERR, "mosquitto_connect failed: %s", mosquitto_strerror(ret));
    }

    return ret;
}