#include <stdio.h>
#include <syslog.h>
#include <stdbool.h>


#include "daemon.h"
#include "argp_for_daemon.h"
#include "mqtt.h"
#include "additional.h"

extern struct mosquitto *mosq;
extern volatile bool run;

int main(int argc, char **argv)
{
    struct Arguments argpArguments; 
    int keepalive = 60, ret = 0;
   
    argpArguments.becomeDaemon = false;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    argp_parse(&argp, argc, argv, 0, 0, &argpArguments);

    if (argpArguments.becomeDaemon)
        ret = become_daemon(0);

    const char *LOGNAME = "MQTT subscriber";
    openlog(LOGNAME, LOG_PID, LOG_USER); 
    syslog(LOG_USER | LOG_INFO, "MQTT subscriber started");

    if (ret)
        syslog(LOG_USER | LOG_ERR, "An error occurred while trying to make the program a daemon process");
        goto cleanUp;
    
    


cleanUp:
    syslog(LOG_USER | LOG_INFO, "MQTT subscriber ended");
    closelog();

    return ret;
}
