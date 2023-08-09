#include "additional.h"

volatile bool run = true;

void signal_handler(int signal)
{
    run = false;
}