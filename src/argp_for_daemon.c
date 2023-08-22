#include "argp_for_daemon.h"

struct Arguments argpArguments; 

error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  struct Arguments *arguments = state->input;

  switch (key)
  {
    case 'h':
      strncpy(arguments->host, arg, sizeof(arguments->host)); 
      break;
        
    case 'p':
      arguments->port = atoi(arg);
      break;

    case 'D':
      arguments->becomeDaemon = true;
      break;

    case 'a':
      strncpy(arguments->cafilePath, arg, sizeof(arguments->cafilePath));
      break;

    case 'c':
      strncpy(arguments->certPath, arg, sizeof(arguments->certPath));
      break;
    
    case 'k':
      strncpy(arguments->keyPath, arg, sizeof(arguments->keyPath));
      break;

    case 'u':
      strncpy(arguments->username, arg, sizeof(arguments->username));
      break;

    case 'P':
      strncpy(arguments->password, arg, sizeof(arguments->password));
      break;

    case ARGP_KEY_END:
      if (!strcmp(arguments->host, "") ||
      !(strcmp(arguments->cafilePath, "") && strcmp(arguments->certPath, "") && strcmp(arguments->keyPath, "")) &&
      (strcmp(arguments->cafilePath, "") || strcmp(arguments->certPath, "") || strcmp(arguments->keyPath, "")) ||
      !(strcmp(arguments->username, "") && strcmp(arguments->password, "")) && 
      (strcmp(arguments->username, "") || strcmp(arguments->password, "")))
        argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
      break;
        
    default:
      return ARGP_ERR_UNKNOWN;
  }
    
  return 0;
}