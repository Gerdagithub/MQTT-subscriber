#ifndef MQTT_H
#define MQTT_H

#include <mosquitto.h>
#include <syslog.h>

#include "argp_for_daemon.h"

int moquitto_init(int keepalive);

#endif