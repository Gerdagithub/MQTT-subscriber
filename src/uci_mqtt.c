#include "uci_mqtt.h"

extern Event events[];
extern int amountOfEvents;

int uci_init(struct uci_context **ctx, struct uci_package **pkg, bool *uciIsLoaded)
{
    *ctx = uci_alloc_context();
    if (ctx == NULL){
        syslog(LOG_USER | LOG_ERR, "Failed to allocate uci context");
        return 1;
    }

    if (uci_load(*ctx, "mqtt_subscriber.config", pkg) != UCI_OK) {
        syslog(LOG_USER | LOG_ERR, "Failed to load UCI package");
        return 1;
    }

    uciIsLoaded = true;

    return 0;
}

int load_topics_events(Topic *topics, int *amountOfTopics)
{
    struct uci_context *ctx;
    struct uci_package *pkg;
    bool uciIsLoaded = false;
    int ret = 0;

    ret = uci_init(&ctx, &pkg, &uciIsLoaded);
    if (ret)
        goto cleanUp;   

    get_topics(ctx, pkg, topics, amountOfTopics);
    get_events(ctx, pkg);
 
 cleanUp:
    if (uciIsLoaded && uci_unload(ctx, pkg))
        syslog(LOG_USER | LOG_ERR, "Failed to unload uci");

    uci_free_context(ctx);

    return ret;
}

void get_topics(struct uci_context *ctx, struct uci_package *pkg, Topic *topics, int *amountOfTopics)
{
    struct uci_element *element;
    uci_foreach_element(&pkg->sections, element) {
        struct uci_section *section = uci_to_section(element);
        if (strcmp(section->type, "topic") == 0 && *amountOfTopics < MAX_TOPICS) {
            process_topic_info_from_uci(uci_to_section(element), topics, amountOfTopics);
        }
    }
}

void get_events(struct uci_context *ctx, struct uci_package *pkg)
{
    struct uci_element *element;
    uci_foreach_element(&pkg->sections, element) {
        struct uci_section *section = uci_to_section(element);
        if (strcmp(section->type, "event") == 0 && amountOfEvents < MAX_EVENTS) {
            process_event_info_from_uci(uci_to_section(element));
        }
    }
}

void process_topic_info_from_uci(struct uci_section *section, Topic *topics, int *amountOfTopics)
{
    struct uci_element *optionAsUciElement;
    struct uci_option *option;
    char temp[5];

    uci_foreach_element(&section->options, optionAsUciElement) {
        option = uci_to_option(optionAsUciElement);
        if (!strcmp(option->e.name, "topic_name")){    
            strncpy(topics[*amountOfTopics].topicName, option->v.string, sizeof(topics[*amountOfTopics].topicName));
        }

        if (!strcmp(option->e.name, "qos")){
            strncpy(temp, option->v.string, sizeof(temp));
            topics[*amountOfTopics].qos = atoi(temp);
        }
    }

    (*amountOfTopics)++;
}

void process_event_info_from_uci(struct uci_section *section)
{
    struct uci_element *optionAsUciElement;
    struct uci_option *option;

    uci_foreach_element(&section->options, optionAsUciElement) {
        option = uci_to_option(optionAsUciElement);
        if (!strcmp(option->e.name, "topic")){
            strncpy(events[amountOfEvents].topicName, option->v.string,
                    sizeof(events[amountOfEvents].topicName));
        }
        if (!strcmp(option->e.name, "parameterName")){
            strncpy(events[amountOfEvents].parameterName, option->v.string, 
                    sizeof(events[amountOfEvents].parameterName));
        }
        if (!strcmp(option->e.name, "valueType")){
            strncpy(events[amountOfEvents].valueType, option->v.string, 
                    sizeof(events[amountOfEvents].valueType));
        }
        if (!strcmp(option->e.name, "comparisonType")){
            char temp[2];
            strncpy(temp, option->v.string, sizeof(temp));
            events[amountOfEvents].comparisonType = atoi(temp);
        }
        if (!strcmp(option->e.name, "comparisonValue")){
            strncpy(events[amountOfEvents].comparisonValue, option->v.string, 
                    sizeof(events[amountOfEvents].comparisonValue));
        }
    }

    amountOfEvents++;
}