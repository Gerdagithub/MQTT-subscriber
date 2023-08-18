#ifndef ADDITIONAL_H
#define ADDITIONAL_H

#include <stdbool.h>
#include <signal.h>
#include <cJSON.h>
#include <time.h>

void signal_handler(int signal);
bool is_json(const char json_string[256]);
char* current_time();

#endif