#include "uci_mqtt.h"

int uci_init(struct uci_context **ctx, struct uci_package **pkg, bool *uciCtxCreated, bool *uciIsLoaded)
{
    *ctx = uci_alloc_context();
    if (ctx == NULL){
        syslog(LOG_USER | LOG_ERR, "Failed to allocate uci context");
        return 1;
    }

    uciCtxCreated = true;

    if (uci_load(*ctx, "mqtt_subscriber", pkg) != UCI_OK) {
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
    bool uciCtxCreated = false, uciIsLoaded = false;
    int ret = 0;

    ret = uci_init(&ctx, &pkg, &uciCtxCreated, &uciIsLoaded);
    if (ret)
        goto cleanUp;   

    get_topics(ctx, pkg, topics, amountOfTopics);

cleanUp:
    if (uciCtxCreated && uciIsLoaded)
        ret = uci_unload(ctx, pkg);

    if (ret)
        syslog(LOG_USER | LOG_ERR, "Failed to unload uci");

    if (uciCtxCreated)
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