#include <stdio.h>
#include <syslog.h>
#include <stdbool.h>

#include "daemon.h"
#include "argp_for_daemon.h"
#include "mqtt.h"
#include "additional.h"
#include "uci_mqtt.h"
#include "sqlite_mqtt.h"
#include "email.h"

extern struct Arguments argpArguments; 
extern struct mosquitto *mosq;
extern sqlite3 *db;
extern bool connectedToTheBroker;
extern int retMqtt;
extern CURL *curl;

int main(int argc, char **argv)
{
    Topic *topics;
    int keepalive = 60, ret = 0;
    int amountOfTopics = 0;
    bool dbOpened = false;
   
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

    ret = mosquitto_init(keepalive);
    if (ret)
        goto cleanUp;

    topics = (Topic *)malloc(sizeof(Topic) * MAX_TOPICS);
    ret = load_topics_events(topics, &amountOfTopics);
    if (ret)
        goto cleanUp;

    subscribe_to_topics(topics, amountOfTopics);
    ret = email_sending_init();
    if (ret)
        goto cleanUp;

    ret = database_init(&dbOpened);
    if (ret)
        goto cleanUp;

    ret = mosquitto_loop_forever(mosq, -1, 1);
    if (ret || retMqtt)
        syslog(LOG_USER | LOG_ERR, "mosquitto_loop_forever failed: %s", mosquitto_strerror(ret? ret : retMqtt));

cleanUp:
    if (curl != NULL)
        curl_easy_cleanup(curl);
    
    if (dbOpened)
        sqlite3_close(db); 

    mosquitto_deinit(topics, amountOfTopics);
    free(topics);

    syslog(LOG_USER | LOG_INFO, "MQTT subscriber ended");
    closelog();

    return ret;
}