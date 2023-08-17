#include <stdio.h>
#include <syslog.h>
#include <stdbool.h>

#include "daemon.h"
#include "argp_for_daemon.h"
#include "mqtt.h"
#include "additional.h"

extern struct Arguments argpArguments; 
extern struct mosquitto *mosq;
extern bool connectedToTheBroker;
extern int retMqtt;

int main(int argc, char **argv)
{
    int keepalive = 60, ret = 0;
   
    argpArguments.becomeDaemon = false;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    argp_parse(&argp, argc, argv, 0, 0, &argpArguments);

    if (argpArguments.becomeDaemon)
        ret = become_daemon(0);

    if (argpArguments.port == 0)
        argpArguments.port = 1883;

    const char *LOGNAME = "MQTT subscriber";
    openlog(LOGNAME, LOG_PID, LOG_USER);
    syslog(LOG_USER | LOG_INFO, "MQTT subscriber started");

    if (ret){
        syslog(LOG_USER | LOG_ERR, "An error occurred while trying to make the program a daemon process");
        goto cleanUp;
    }

    ret = moquitto_init(keepalive);
    if (ret)
        goto cleanUp;

    ret = mosquitto_loop_forever(mosq, -1, 1);
    if (ret || retMqtt)
        syslog(LOG_USER | LOG_ERR, "mosquitto_loop_forever failed: %s", mosquitto_strerror(ret? ret : retMqtt));

cleanUp:
    if (mosq){
        if (connectedToTheBroker)
            mosquitto_disconnect(mosq);
        
        mosquitto_destroy(mosq);
    }
    mosquitto_lib_cleanup();

    syslog(LOG_USER | LOG_INFO, "MQTT subscriber ended");
    closelog();

    return ret;
}