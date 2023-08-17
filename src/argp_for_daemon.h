#ifndef ARGP_FOR_DAEMON_H
#define ARGP_FOR_DAEMON_H

#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include <stdbool.h>

static struct argp_option options[] = {
  {"host",   'h', "HOST", 0,
   "Host" },

  {"port",    'p', "PORT", 0,
   "Port" },

  {"becomeDaemon", 'D', NULL, 0, 
   "Become daemon program" },

  {"cert", 'c', "CERT", 0, 
   "Path to client cert file" },

  {"key", 'k', "KEY", 0, 
   "Path to client key file" },

  {"cafile", 'a', "CAFILE", 0, 
   "Path to cafile file" },
  
  {"username", 'u', "USERNAME", 0, 
   "username" },

  {"password", 'P', "PASSWORD", 0, 
   "password" },

  { 0 }
};

struct Arguments
{
  char host[40];
  int port;
  bool becomeDaemon;
  char keyPath[126];
  char certPath[126];
  char cafilePath[126];
  char username[50];
  char password[50];
};

error_t parse_opt(int key, char *arg, struct argp_state *state);

static struct argp argp = { options, parse_opt, 0, 0 };

#endif