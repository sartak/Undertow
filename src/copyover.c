#include "undertow.h"

void initiate_copyover()
{
  FILE *fp;
  char buf[10];
  DESCRIPTOR_DATA *d;

  for (d = dhead; d; d = d->next)
    if (d->acc && d->state == STATE_PLAYING)
      save_account(d->acc);

  fp = fopen("copyover.txt", "w");

  if (fp == NULL)
  {
    for (d = dhead; d; d = d->next)
      sckoutput(d->socket, "\n\nA copyover was attempted but failed. Your character was saved, so fear not. The MUD should be back up shortly. We apologize for the inconvenience.\n");
    error("Copyover failed.");
    exit(1);
  }

  fprintf(fp, "disabled\n");
  autodisable(0, "", fp);
  fprintf(fp, "-eod\n");

  for (d = dhead; d; d = d->next)
  {
    if (d->state == STATE_LINKDEAD)
      continue;
    else if (d->state == STATE_PLAYING)
      send_to_descriptor(d, "\n#1[#2Copyover#1] #RArthur#1: #2Ford, stop it. You're turning into a penguin.#n\n");
    else
    {
      send_to_descriptor(d, "\nWe apologize for the inconvenience, but we are rebooting now. Please reconnect in five seconds.\n");
      continue;
    }

    fprintf(fp, "%d\n%s\n%s\n%d\n%s\n%s\n%d\n%d\n%d\n", d->socket, d->acc->name, d->acc->ch->name, d->acc->cur_char, d->ip, d->host, IS_SET(d->acc->ch->tflags, CTFLAG_AFK) ? 1:0, d->acc->ch->idle, d->acc->ch->logintime);
  }
  fprintf(fp, "EOF~\n");
  fflush(fp);
  fclose(fp);
  output(); /* I wouldn't recommend calling this anywhere else. Bad things could happen. */
  shut_down();
  snprintf(buf, 10, "%d", port);
  fflush(fsyslog);
  fclose(fsyslog);
  execl("ut", "./ut", "-copyover", buf, 0);
  error("This post-copyover message should never display. But it's okay because we're closing up shop anyway.");
}

void recover_copyover()
{
  FILE *fp;
  DESCRIPTOR_DATA *d;
  char buf[50];
  char accountname[32];
  char charname[32];
  char ipbuf[32];
  char hostbuf[128];
  int i;
  int sock;
  int cur_char;
  int afk;
  int idle;
  int logintime;

  fp = fopen("copyover.txt", "r");
  if (fp == NULL)
    error("Unable to read copyover file. I hope you aren't dicking around!");

  fgets(buf, 50, fp);
  for (i = 0; i < 50; ++i)
    if (buf[i] == '\n')
    {
      buf[i] = 0;
      break;
    }
  if (!stricmp(buf, "disabled"))
  {
    while (!feof(fp))
    {
      fgets(buf, 50, fp);
      for (i = 0; i < 50; ++i)
        if (buf[i] == '\n')
        {
          buf[i] = 0;
          break;
        }
      if (!stricmp(buf, "-eod"))
        break;
      autodisable(1, buf, fp);
    }
  }

  while (!feof(fp))
  {
    fgets(buf, 10, fp);

    for (i = 0; i < 10; ++i)
      if (buf[i] == '\n')
      {
        buf[i] = 0;
        break;
      }

    if (!strcmp(buf, "EOF~"))
      break;

    sock = atoi(buf);

    fgets(accountname, 32, fp);
    for (i = 0; i < 32; ++i)
      if (accountname[i] == '\n')
      {
        accountname[i] = 0;
        break;
      }

    fgets(charname, 32, fp);
    for (i = 0; i < 32; ++i)
      if (charname[i] == '\n')
      {
        charname[i] = 0;
        break;
      }

    fgets(buf, 10, fp);
    for (i = 0; i < 10; ++i)
      if (buf[i] == '\n')
      {
        buf[i] = 0;
        break;
      }
    cur_char = atoi(buf);

    fgets(ipbuf, 32, fp);
    for (i = 0; i < 32; ++i)
      if (ipbuf[i] == '\n')
      {
        ipbuf[i] = 0;
        break;
      }

    fgets(hostbuf, 128, fp);
    for (i = 0; i < 128; ++i)
      if (hostbuf[i] == '\n')
      {
        hostbuf[i] = 0;
        break;
      }
    fgets(buf, 10, fp);
    for (i = 0; i < 10; ++i)
      if (buf[i] == '\n')
      {
        buf[i] = 0;
        break;
      }
    afk = atoi(buf);

    fgets(buf, 10, fp);
    for (i = 0; i < 10; ++i)
      if (buf[i] == '\n')
      {
        buf[i] = 0;
        break;
      }
    idle = atoi(buf);

    fgets(buf, 10, fp);
    for (i = 0; i < 10; ++i)
      if (buf[i] == '\n')
      {
        buf[i] = 0;
        break;
      }
    logintime = atoi(buf);

    d = new_descriptor();
    link_descriptor(d);
    d->socket = sock;
    d->state = STATE_PLAYING;
    strlcpy(d->ip, ipbuf, 32);
    strlcpy(d->host, hostbuf, 128);

    d->acc = new_account();
    link_account(d->acc);
    load_account(accountname, d->acc);
    d->acc->d = d;

    d->acc->ch = load_character(d->acc, charname);
    d->acc->ch->acc = d->acc;
    d->acc->ch->d = d;
    d->acc->cur_char = cur_char;
    setup_char_events(d->acc->ch);

    if (afk)
      SET_BIT(d->acc->ch->tflags, CTFLAG_AFK);
    d->acc->ch->idle = idle;
    d->acc->ch->logintime = logintime;
    find_event(d->acc->ch, EVENT_IDLE)->ticks_left = idle * 8 < IDLE_OUT ? IDLE_OUT - idle * 8 : IDLE_OUT;

    send_to_descriptor(d, "\nWe have normality. Anything you still can't cope with is your own problem.\n\n");
    do_look(d->acc->ch, "");
  }

  fclose(fp);
  if (!DEBUG)
    unlink("copyover.txt");
}
