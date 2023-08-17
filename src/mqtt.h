#ifndef MQTT_H
#define MQTT_H

#include <mosquitto.h>
#include <syslog.h>

#include "argp_for_daemon.h"

void on_connect_callback(struct mosquitto *mosq, void *userdata, int result);
int moquitto_init(int keepalive);

#endif