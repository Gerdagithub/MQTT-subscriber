#ifndef SQLITE_H
#define SQLITE_H

#include <stdio.h>
#include <syslog.h>
#include <sqlite3.h>
#include <stdbool.h>

#define PATH_TO_DATABASE "/log/mqtt_subscriber_database.db"

int database_init(bool *dbOpened);
int open_database(char db_filename[126]);
int create_tables();
int insert_topic_to_database(char topicName[256], char data[256]);

#endif