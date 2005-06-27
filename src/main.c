#include "undertow.h"

int port = PORT;
int DEBUG = 0;
int copyover = 0;
FILE *fsyslog;

static void catch_sigint(int signo)
{
  syslog("SIGNAL", "Caught SIGINT. Shutting down somewhat normally.");
  RUNNING = 0;
}

static void catch_sighup(int signo)
{
  syslog("SIGNAL", "Caught SIGHUP. Continuing normally.");
}

static void catch_sigpipe(int signo)
{
  syslog("SIGNAL", "Caught SIGPIPE from socket %d. Continuing normally.", last_socket);
  last_socket = -2;
}

int main(int argc, char *argv[])
{
  if (argc >= 3)
  {
    if (!strcmp(argv[2], "-debug"))
      DEBUG = 1;
    else if (!strcmp(argv[2], "-copyover"))
      copyover = 1;
    else
      port = atoi(argv[2]);
  }

  if (argc >= 2)
  {
    if (!strcmp(argv[1], "-debug"))
      DEBUG = 1;
    else if (!strcmp(argv[1], "-copyover"))
      copyover = 1;
    else
      port = atoi(argv[1]);
  }

  fsyslog = fopen("data/syslog.txt", "a");
  if (fsyslog == NULL)
  {
    printf("Unable to open syslog.");
    return 0;
  }

  if (copyover)
    recover_copyover();

  if (port < 1024 || port > 32000)
  {
    error("The port should be between 1024 and 32000.");
    return 0;
  }

  RUNNING = 1;

  signal(SIGHUP, catch_sighup);
  signal(SIGPIPE, catch_sigpipe);

  if (DEBUG)
    signal(SIGINT, catch_sigint);
  else
    if (fork())
      exit(0);

  if (DEBUG)
    syslog("BOOT", "%s (%i) is ready to rock on port %i. \33[1;32m(DEBUG MODE ON)\33[0m", MUD_NAME, getpid(), port);
  else
    syslog("BOOT", "%s (%i) is ready to rock on port %i.", MUD_NAME, getpid(), port);

  game_loop();

  syslog("SHUTDOWN", "%s shutdown successfully.", MUD_NAME);
  return 0;
}

void shut_down()
{
  syslog("SHUTDOWN", "Initiating denitialization sequence.");
  free_all();
  sckclose();
  fflush(fsyslog);
  fclose(fsyslog);
}
