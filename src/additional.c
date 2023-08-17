#include "additional.h"

extern struct mosquitto *mosq;

void signal_handler(int signal)
{
    if (mosq)
        mosquitto_disconnect(mosq);
}