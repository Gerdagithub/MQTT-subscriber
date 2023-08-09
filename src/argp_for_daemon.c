#include "argp_for_daemon.h"

error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  struct Arguments *arguments = state->input;

  switch (key)
  { 
    case 'h':
      strncpy(arguments->host, arg, strlen(arg) + 1); 
      break;
        
    case 'p':
      arguments->port = atoi(arg);
      break;

    case 'D':
      arguments->becomeDaemon = true;

    case ARGP_KEY_END:
      if (!arguments->host || !arguments->port)
        argp_usage (state);
      break;
        
    default:
      return ARGP_ERR_UNKNOWN;
  }
    
  return 0;
}