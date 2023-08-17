#include "additional.h"

extern struct mosquitto *mosq;
extern bool connectedToTheBroker;

void signal_handler(int signal)
{
    if (mosq && connectedToTheBroker)
        mosquitto_disconnect(mosq);
}