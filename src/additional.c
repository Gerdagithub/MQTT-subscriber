#include "additional.h"

volatile bool run = true;
extern struct mosquitto *mosq;

void signal_handler(int signal)
{
    run = false;

    if (mosq)
        mosquitto_disconnect(mosq);
}