#ifndef UCI_H
#define UCI_H

#include <stdio.h>
#include <uci.h>
#include <syslog.h>

#include "mqtt.h"

int uci_init(struct uci_context **ctx, struct uci_package **pkg, bool *uciCtxCreated, bool *uciIsLoaded);
int load_topics_events(Topic *topics, int *amountOfTopics);
void get_topics(struct uci_context *ctx, struct uci_package *pkg, Topic *topics, int *amountOfTopics);
void process_topic_info_from_uci(struct uci_section *section, Topic *topics, int *amountOfTopics);

#endif