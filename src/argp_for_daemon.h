#ifndef ARGP_FOR_DAEMON_H
#define ARGP_FOR_DAEMON_H

#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <stdbool.h>

static struct argp_option options[] = {
    {"becomeDaemon", 'D', NULL, 0, 
    "Become daemon program" },
  { 0 }
};

struct Arguments
{
  bool becomeDaemon;
};

error_t parse_opt(int key, char *arg, struct argp_state *state);

static struct argp argp = { options, parse_opt, 0, 0 };

#endif