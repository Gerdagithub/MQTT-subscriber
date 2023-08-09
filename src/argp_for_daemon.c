#include "argp_for_daemon.h"

error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  struct Arguments *arguments = state->input;

  switch (key)
  {
    case 'D':
      arguments->becomeDaemon = true;
  }
    
  return 0;
}