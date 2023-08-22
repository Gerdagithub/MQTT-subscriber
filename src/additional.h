#ifndef ADDITIONAL_H
#define ADDITIONAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <cJSON.h>
#include <syslog.h>
#include <time.h>

void signal_handler(int signal);
bool is_json(const char json_string[256]);
char* current_time();
void get_json_value(cJSON *root, char parameterName[256], cJSON **jsonValue);
bool value_meets_event_condition(cJSON *jsonValue, char valueType[20], int comparisonType,
                                 char comparisonValue[5]);
bool compare_number(double number, int comparisonType, double comparisonValue);
bool compare_string(char value[256], int comparisonType, char comparisonValue[256]);
int send_email();

#endif