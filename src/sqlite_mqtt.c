#include "sqlite_mqtt.h"
#include "additional.h"

sqlite3 *db;

int database_init(bool *dbOpened)
{
    int ret = 0;
    ret = open_database(PATH_TO_DATABASE);
    if (ret)
        return ret;
    *dbOpened = true;

    ret = create_tables();
    return ret;
}

int open_database(char db_filename[126]) 
{
    int rc = sqlite3_open(db_filename, &db);
    if (rc != SQLITE_OK) {
        syslog(LOG_USER | LOG_ERR, "Cannot open database: %s", sqlite3_errmsg(db));
        return 1;
    }
    return 0;
}

int create_tables()
{
    int ret = 0;

    ret = sqlite3_exec(db, 
        "CREATE TABLE IF NOT EXISTS topics ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "topic TEXT NOT NULL, "
        "data TEXT NOT NULL, "
        "datetime DATETIME);",
        0, 0, 0);

    if (ret != SQLITE_OK) {
        syslog(LOG_USER | LOG_ERR, "SQL error: %s", sqlite3_errmsg(db));
        return 1;
    }

    return 0;
}

int insert_topic_to_database(char topicName[256], char data[256])
{
    char command[256];
    char *time = current_time();

    sprintf(command,
    "INSERT INTO topics (topic, data, datetime) VALUES "
    "('%s', '%s', '%s');", topicName, data, time);

    free(time);

    return sqlite3_exec(db, command, 0, 0, 0);
}