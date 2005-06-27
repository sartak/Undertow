#include "undertow.h"

int RUNNING;
char str_boot_time[64];
char str_reset_time[64];
time_t boot_time;
time_t reset_time;
time_t current_time;
int last_socket = -1;

void greeting(DESCRIPTOR_DATA *d)
{
  char str_cur_time[64];
  int i = number_range(1, 2);
  char buf[MAX_STRING_LENGTH] = {(signed char)IAC, (signed char)WONT, (signed char)TELOPT_ECHO, '\0'};

  log_time(str_cur_time);

  switch (i)
  {
    case 1:
      strlcat(buf, "#1               ...\n"
                   "#1             ;::::;\n"
                   "#1           ;::::; :;     unknown\n"
                   "#1         ;:::::'   :;\n"
                   "#1        ;:::::;     ;.\n"
                   "#1       ,:::::'       ;          #3 OOO#y\\\n"
                   "#1       ::::::;       ;         #3 OOOOO#y\\\n"
                   "#1       ;:::::;       ;        #3 OOOOOOOO\n"
                   "#1      ,;::::::;     ;'       #y  / #3OOOOOOO\n"
                   "#1    ;:::::::::`. ,,,;.       #y /  / #3DOOOOOO\n"
                   "#1  .';:::::::::::::::::;,    #y /  /     #3DOOOO\n"
                   "#1 ,::::::;::::::;;;;::::;,   #y/  /       #3 DOOO\n"
                   "#1;`::::::`'::::::;;;::::: ,###y/  /        #3  DOOO\n"
                   "#1:`:::::::`;::::::;;::: ;::###y  /           #3 DOOO\n"
                   "#1::`:::::::`;:::::::: ;::::###y /            #3  DOO\n"
                   "#1`:`:::::::`;:::::: ;::::::###y/             #3  DOO\n"
                   "#1 :::`:::::::`;; ;:::::::::####             #3   OO\n"
                   "#1 ::::`:::::::`;::::::::;:::##              #3  OO\n"
                   "#1 `:::::`::::::::::::;'`:;::##             #3   O\n"
                   "#1  `:::::`::::::::;' #y/  / #1`:##\n"
                   "#1   ::::::`:::::;'  #y/  /  #1 `##\n", MAX_STRING_LENGTH);
    break;

    case 2:
      strlcat(buf, "#1                ..:::::::::..          \n"
                   "#1           ..:::#2aad8888888baa#1:::..      \n"
                   "#1        .::::#2d#1:#2?88888888888?#1::#28b#1::::.           \n"
                   "#1      .:::#2d8888#1:#2?88888888??a888888b#1:::.       \n"
                   "#1    .:::#2d8888888a8888888aa8888888888b#1:::.       \n"
                   "#1   ::::#2dP#1::::::::#288888888888#1::::::::#2Yb#1::::       \n"
                   "#1  ::::#2dP#1:::::::::#2Y888888888P#1:::::::::#2Yb#1::::         \n"
                   "#1 ::::#2d8#1:::::::::::#2Y8888888P#1:::::::::::#28b#1::::        \n"
                   "#1.::::#288#1::::::::::::#2Y88888P#1::::::::::::#288#1::::.      \n"
                   "#1:::::#2Y8baaaaaaaaaa88P#1:#2T#1:#2Y88aaaaaaaaaad8P#1:::::      \n"
                   "#1:::::::#2Y88888888888P#1::#2|#1::#2Y88888888888P#1:::::::      \n"
                   "#1::::::::::::::::#2888#1:::#2|#1:::#2888#1::::::::::::::::      \n"
                   "#1`:::::::::::::::#28888888888888b#1::::::::::::::'       \n"
                   "#1 :::::::::::::::#288888888888888#1:::::::::::::: alan \n"
                   "#1  :::::::::::::#2d88888888888888#1:::::::::::::   de   \n"
                   "#1   ::::::::::::#288#1::#288#1::#288#1:::#288#1::::::::::::   smet  \n"
                   "#1    `::::::::::#288#1::#288#1::#288#1:::#288#1::::::::::'          \n"
                   "#1      `::::::::#288#1::#288#1::#2P#1::::#288#1::::::::'          \n"
                   "#1        `::::::#288#1::#288#1:::::::#288#1::::::'    \n"
                   "#1           ``:::::::::::::::::::''     \n"
                   "#1                ``:::::::::''\n", MAX_STRING_LENGTH);
    break;
  }

  send_to_descriptor(d, buf);
  send_to_descriptor(d, "\n#nWelcome to #RU#rndertow#n!\n\n");

  {
    char buf2[128];

    time_words(current_time - boot_time, buf2);
    snprintf(buf, MAX_STRING_LENGTH, "Undertow was booted:     %s ago\n", buf2);
    send_to_descriptor(d, buf);

    time_words(current_time - reset_time, buf2);
    snprintf(buf, MAX_STRING_LENGTH, "Undertow was last reset: %s ago\n", buf2);
    send_to_descriptor(d, buf);

    snprintf(buf, MAX_STRING_LENGTH, "Current server time:     %s GMT\n", str_cur_time);
    send_to_descriptor(d, buf);
  }
}

void game_loop()
{
  struct timeval now_time;
  struct timeval last_time;
  long secDelta;
  long usecDelta;
  FILE *fp;
  /* initialization */

  if (!copyover)
    memset(&mem, 0, sizeof(MEMORY_DATA));
  sckcreate(port);
  srand ((unsigned int) time(NULL));
  gettimeofday(&now_time, NULL);
  current_time = (time_t) now_time.tv_sec;
  reset_time = current_time;
  log_time(str_reset_time);
  load_changes();
  load_all_helps();
  create_portal(1, NULL, 0, 0, PLANE_NEWBIE, 0, 0, PLANE_NEXUS);
  create_portal(1, NULL, 0, 0, PLANE_HNEWBIE, 0, 0, PLANE_HNEXUS);
  garbage_collector_init();

  if (!copyover)
  {
    strlcpy(str_boot_time, str_reset_time, 64);
    boot_time = reset_time;
    fp = fopen("data/boot.time", "w");
    fprintf(fp, "%s\n%d\n", str_boot_time, (int)boot_time);
    fflush(fp);
    fclose(fp);
  }
  else
  {
    fp = fopen("data/boot.time", "r");
    if (fp)
    {
      char buf[32];
      fread_line(fp, str_boot_time);
      fread_line(fp, buf);
      boot_time = atoi(buf);
      fclose(fp);
    }
    else
    {
      strlcpy(str_boot_time, str_reset_time, 64);
      boot_time = reset_time;
      fp = fopen("data/boot.time", "w");
      fprintf(fp, "%s\n%d\n", str_boot_time, (int)boot_time);
      fflush(fp);
      fclose(fp);
    }
  }

  last_time = now_time;

  while (RUNNING)
  {
    new_connection();
    input();
    update();
    output();

    gettimeofday(&now_time, NULL);
    usecDelta = ((int) last_time.tv_usec) - ((int) now_time.tv_usec) + 1000000 / TICKS_PER_SECOND;
    secDelta  = ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );

    while (usecDelta < 0)
    {
      usecDelta += 1000000;
      secDelta  -= 1;
    }

    while (usecDelta >= 1000000)
    {
      usecDelta -= 1000000;
      secDelta  += 1;
    }

    if (secDelta > 0 || (secDelta == 0 && usecDelta > 0) )
    {
      struct timeval stall_time;

      stall_time.tv_usec = usecDelta;
      stall_time.tv_sec  = secDelta;
      select(0, NULL, NULL, NULL, &stall_time);
    }
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;
  }

  shut_down();
}

void new_connection()
{
  DESCRIPTOR_DATA *d;
  int sck;
  char ip[32];
  char host[128];

  sck = sckaccept(ip, host);

  if (sck == SOCK_WOULDWAIT)
    return;

  d = new_descriptor();
  link_descriptor(d);
  strlcpy(d->ip, ip, 32);

  if (host[0] == 0)
    strlcpy(d->host, ip, 128);
  else
    strlcpy(d->host, host, 128);

  d->socket = sck;

  d->acc = new_account();
  link_account(d->acc);
  d->acc->d = d;

  SET_BIT(d->acc->aflags, AFLAG_ANSI);
  greeting(d);
  send_to_descriptor(d, "\nWhat do you want your name to be? ");

  check_address(ip, host);
  d->state = STATE_GET_NAME;

  setup_event_d(d, NULL, event_loginidle, "", TICKS_PER_SECOND * 60 * 5, EVENT_LOGINIDLE);

  if (host[0])
    syslog("CONNECT", "New connection from %s (%s). Possibly %s.", d->host, d->ip, host);
  else
    syslog("CONNECT", "New connection from %s (%s).", d->host, d->ip);
}

void cut_connection(DESCRIPTOR_DATA *d)
{
  char ip[32];
  strlcpy(ip, d->ip, 32);
  close(d->socket);
  d->socket = -1;
  syslog("CUT", "%s@%s cut connection.", d->acc->name, ip);

  if (d->state != STATE_PLAYING) /* not worth holding onto */
  {
    if (d->acc->ch)
      wipe_character(d->acc->ch);
    wipe_descriptor(d);
    return;
  }

  d->state = STATE_LINKDEAD;
  d->linkdeath = 0;
  destroy_d_events(d, EVENT_ALL);
  destroy_all_events(d->acc->ch, EVENT_ALL);
  setup_event(d->acc->ch, NULL, event_linkdeath, "cut_connection", TICKS_PER_SECOND * 60 * 10, EVENT_LINKDEAD);
}

void input()
{
  DESCRIPTOR_DATA *d, *dnext;
  char buf[MAX_STRING_LENGTH];
  char *buf2;
  int ret;
  int len;
  int i, o;

  d = dhead;
  while (d)
  {
    dnext = d->next;

    if (d->state == STATE_LINKDEAD || d->state == STATE_VOID)
    {
      d = dnext;
      continue;
    }

    buf[0] = 0;
    ret = sckinput(d->socket, buf);

    if (ret == SOCK_FLOODING || (strlen(buf) + strlen(d->input)) > MAX_INPUT_LENGTH - 2)
    {
      sckoutput(d->socket, "\n\nIt would be wise to not flood the MUD.\n");
      d->input[0] = 0;
      d->output[0] = 0;
      cut_connection(d);
      break;
    }

    if (ret == SOCK_CUTCONNECTION)
    {
      cut_connection(d);
      break;
    }

    if (d->input[0] == 0 && (ret == SOCK_WOULDWAIT || ret == SOCK_NOINPUT))
    {
      d = dnext;
      continue;
    }

    if (ret != SOCK_WOULDWAIT && ret != SOCK_NOINPUT) /* we have new input to parse */
    {
      strlcat(d->input, buf, MAX_STRING_LENGTH);
      i = 0;
      o = 0;

      while (d->input[i] != '\0')
      {
        if (d->input[i] == '\r')
        {
          if (d->input[i+1] == '\n') /* \r\n becomes \n */
          {
            i++;
            buf[o++] = d->input[i++];
          }
          else
          {
            i++;
            buf[o++] = '\n'; /* \r becomes \n */
          }
        }
        else if (d->input[i] == '\n' && (d->input[i+1] == '\n' || d->input[i+1] == '\r'))
        {
          buf[o++] = d->input[i++]; /* \n\n becomes \n */
          i++;
        }
        else if (d->input[i] == 8)
        {
          if (i > 0 && o > 0 && buf[o-1] != '\n') /* we don't want to backspace into old data */
          {
            i++;
            o--;
          }
          else
            i++;
        }
        else
          buf[o++] = d->input[i++];
      }
      buf[o] = '\0';
      strlcpy(d->input, buf, MAX_STRING_LENGTH);
    }

    {
      EVENT_DATA *event;
      event = find_event_d(d, EVENT_LOGINIDLE);
      if (event)
        event->ticks_left = TICKS_PER_SECOND * 60 * 5;
    }

    len = strlen(d->input);
    d->input[len+2] = 0;

    for (ret = 0; ret < len; ret++)
    {
      if (d->input[ret] == '\n')
      {
        d->input[ret] = 0;
        if (d->state == STATE_PLAYING)
          parse(d, d->input);
        else
          nanny(d, d->input);

        buf2 = d->input;
        buf2 += ret + 1;
        strlcpy(d->input, buf2, MAX_INPUT_LENGTH);
        break;
      }
    }

    d = dnext;
  }
}

void prompt(DESCRIPTOR_DATA *d, char *buf)
{
  CHAR_DATA *ch;
  char xbuf[32];
  char *c;
  char *pbuf;
  int i;
  if (d->acc == NULL || (ch = d->acc->ch) == NULL)
    return;

  /* pwipe: make prompts use $ instead of % */
  pbuf = buf;
  xbuf[0] = 0;
  for (c = ch->prompt; *c != 0; c++)
    if (*c != '%')
      *(pbuf++) = *c;
    else
    {
      switch (*(++c))
      {
        default:
          *(pbuf++) = *c;
          break;
        case '/':
          *(pbuf++) = '\n';
          break;
        case 'h':
          snprintf(xbuf, 32, "%d", ch->hp);
          break;
        case 'H':
          snprintf(xbuf, 32, "%d", ch->maxhp);
          break;
        case 'm':
          snprintf(xbuf, 32, "%d", ch->mana);
          break;
        case 'M':
          snprintf(xbuf, 32, "%d", ch->maxmana);
          break;
        case 'v':
          snprintf(xbuf, 32, "%d", ch->move);
          break;
        case 'V':
          snprintf(xbuf, 32, "%d", ch->maxmove);
          break;
        case 'p':
          snprintf(xbuf, 32, "%d", ch->primal);
          break;
        case 'g':
        case 'G':
          snprintf(xbuf, 32, "%d", ch->plutos);
          break;
        case 'r':
        case 'R':
          snprintf(xbuf, 32, "%d", ch->resistance);
          break;
        case 'n':
          strlcpy(xbuf, ch->name, 32);
          break;
        case 'N':
          strlcpy(xbuf, ch->acc->name, 32);
          break;
        case 'x':
        case 'X':
          snprintf(xbuf, 32, "%d", ch->x);
          break;
        case 'y':
        case 'Y':
          snprintf(xbuf, 32, "%d", ch->y);
          break;
        case 't':
        case 'T':
          log_timeofday(xbuf);
          break;
        case 'P':
          switch(ch->plane)
          {
            default:
              strlcpy(xbuf, "Limbo", 32);
              break;
            case PLANE_NEWBIE:
              strlcpy(xbuf, "Newbie", 32);
              break;
            case PLANE_NEXUS:
              strlcpy(xbuf, "Nexus", 32);
              break;
            case PLANE_HNEWBIE:
              strlcpy(xbuf, "Hardcore Newbie", 32);
              break;
            case PLANE_HNEXUS:
              strlcpy(xbuf, "Hardcore Nexus", 32);
              break;
            case PLANE_HEAVEN:
              strlcpy(xbuf, "Heaven", 32);
              break;
            case PLANE_HELL:
              strlcpy(xbuf, "Hell", 32);
              break;
            case PLANE_JAIL:
              strlcpy(xbuf, "Jail", 32);
              break;
            case PLANE_INTERPLANE:
              strlcpy(xbuf, "Interplanar Void", 32);
              break;
            case PLANE_INTRAPLANE:
              strlcpy(xbuf, "Intraplanar Void", 32);
              break;
          }
          break;
      }
      if (xbuf[0] != 0)
      {
        for (i = 0; xbuf[i] != 0; ++i)
          *(pbuf++) = xbuf[i];
        xbuf[0] = 0;
      }
    }
  *(pbuf++) = 0;
}

void output()
{
  DESCRIPTOR_DATA *d;
  static char buf[MAX_STRING_LENGTH];
  static char buf2[MAX_STRING_LENGTH];
  static char promptbuf[512];

  for (d = dhead; d; d = d->next)
  {
    if ((d->state == STATE_LINKDEAD && d->linkdeath == 0) || d->socket <= 0)
      continue;
    if (d->output[0] == 0)
      continue;

    d->linkdeath = 0;

    if (d->state != STATE_LINKDEAD)
    {
      if (IS_SET(d->acc->aflags, AFLAG_ANSI) && !IS_SET(d->acc->aflags, AFLAG_ANSI_SPARSE))
        colorize(d->output, buf, MAX_STRING_LENGTH);
      else
        decolorize(d->output, buf);

      d->output[0] = 0;
    }

    prompt(d, promptbuf);
    if (d->state == STATE_PLAYING)
    {
      if (IS_SET(d->acc->aflags, AFLAG_ANSI))
        colorize(promptbuf, buf2, MAX_STRING_LENGTH);
      else
        decolorize(promptbuf, buf2);
      strlcat(buf, "\n", MAX_STRING_LENGTH);
      strlcat(buf, buf2, MAX_STRING_LENGTH);
    }
    else if (d->state == STATE_LINKDEAD)
    {
      strlcat(d->output, "\n", MAX_STRING_LENGTH);
      strlcat(d->output, promptbuf, MAX_STRING_LENGTH);
    }

    if (d->state != STATE_LINKDEAD)
    {
      last_socket = d->socket;
      sckoutput(d->socket, buf);
      if (last_socket == -2) /* SIGPIPE */
      {
        cut_connection(d);
        break;
      }
    }
  }
}

void send_to_descriptor(DESCRIPTOR_DATA *d, char *buf)
{
  if (d->state == STATE_LINKDEAD)
    d->linkdeath = 1;
  if (strlen(d->output) + strlen(buf) + 50 >= MAX_STRING_LENGTH)
  {
    strlcat(d->output, "\n\n#ROutput truncated.", MAX_STRING_LENGTH);
    return;
  }
  strlcat(d->output, buf, MAX_STRING_LENGTH);
}

void send_to_char(CHAR_DATA *ch, char *buf)
{
  if (IS_NPC(ch))
    return;
  if (ch->d->state == STATE_LINKDEAD)
    ch->d->linkdeath = 1;
  if (strlen(ch->d->output) + strlen(buf) + 50 >= MAX_STRING_LENGTH)
  {
    strlcat(ch->d->output, "\n\n#ROutput truncated.", MAX_STRING_LENGTH);
    return;
  }
  strlcat(ch->d->output, buf, MAX_STRING_LENGTH);
}

void colorize(char *in, char *out, int size)
{
  int i = 0; /* iterator, keeps track of position of in */
  int o = 0; /* keeps track of position of out */
  int x = 0; /* used for the random number generator */
  int last = 0;

/*
  rRcCyYgGbBmMpP
  t    random (rcygbm)
  T    random (RCYGBM)
  s    random (rRcCyYgGbBmM)
  nN   clear
  0123 shades of grey
  8    random number 1-9
  9    random number 0-9
*/

  while (i <= strlen(in)) {
    if (o + 100 > size)
    {
      out[o++] = '\33'; out[o++] = '[';  out[o++] = '1'; out[o++] = ';';
      out[o++] = '3';   out[o++] = '1';  out[o++] = 'm';
      out[o++] = '\n';  out[o++] = '\n'; out[o++] = 'B'; out[o++] = 'u';
      out[o++] = 'f';   out[o++] = 'f';  out[o++] = 'e'; out[o++] = 'r';
      out[o++] = ' ';   out[o++] = 't';  out[o++] = 'r'; out[o++] = 'u';
      out[o++] = 'n';   out[o++] = 'c';  out[o++] = 'a'; out[o++] = 't';
      out[o++] = 'e';   out[o++] = 'd';  out[o++] = '.'; out[o++] = '\n';
      out[o++] = '\n';  out[o] = '\0';
      return;
    }

    if (in[i] == '#') {
      switch (in[++i]) {
        case '#':
          out[o++] = '#';
          break;
        case 'N':
        case 'n':
          if (last == 0)
            break;
          last = 0;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = 'm';
          break;
        case '0':
          if (last == 1)
            break;
          last = 1;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '0';   out[o++] = 'm';
          break;
        case '1':
          if (last == 2)
            break;
          last = 2;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '0';   out[o++] = 'm';
          break;
        case '2':
          if (last == 3)
            break;
          last = 3;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '7';   out[o++] = 'm';
          break;
        case '3':
          if (last == 4)
            break;
          last = 4;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '7';   out[o++] = 'm';
          break;
        case 'r':
          if (last == 5)
            break;
          last = 5;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '1';   out[o++] = 'm';
          break;
        case 'R':
          if (last == 6)
            break;
          last = 6;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '1';   out[o++] = 'm';
          break;
        case 'c':
          if (last == 7)
            break;
          last = 7;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '6';   out[o++] = 'm';
          break;
        case 'C':
          if (last == 8)
            break;
          last = 8;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '6';   out[o++] = 'm';
          break;
        case 'y':
        case 'o':
          if (last == 9)
            break;
          last = 9;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '3';   out[o++] = 'm';
          break;
        case 'Y':
          if (last == 10)
            break;
          last = 10;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '3';   out[o++] = 'm';
          break;
        case 'g':
          if (last == 11)
            break;
          last = 11;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '2';   out[o++] = 'm';
          break;
        case 'G':
          if (last == 12)
            break;
          last = 12;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '2';   out[o++] = 'm';
          break;
        case 'b':
          if (last == 13)
            break;
          last = 13;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '4';   out[o++] = 'm';
          break;
        case 'B':
          if (last == 14)
            break;
          last = 14;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '4';   out[o++] = 'm';
          break;
        case 'p':
        case 'm':
          if (last == 15)
            break;
          last = 15;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '5';   out[o++] = 'm';
          break;
        case 'M':
        case 'P':
          if (last == 16)
            break;
          last = 16;
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '5';   out[o++] = 'm';
          break;

        case '8':
          x = number_range(1, 9);
          out[o++] = '0' + x;
          break;

        case '9':
          x = number_range(0, 9);
          out[o++] = '0' + x;
          break;

        case 't':
          x = number_range(1, 6);
          switch (x)
          {
            case 1: /* dark red */
              if (last == 5)
                break;
              last = 5;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '1';   out[o++] = 'm';
              break;
            case 2: /* dark cyan */
              if (last == 7)
                break;
              last = 7;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '6';   out[o++] = 'm';
              break;
            case 3: /* dark yellow */
              if (last == 9)
                break;
              last = 9;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '3';   out[o++] = 'm';
              break;
            case 4: /* dark green */
              if (last == 11)
                break;
              last = 11;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '2';   out[o++] = 'm';
              break;
            case 5: /* dark blue */
              if (last == 13)
                break;
              last = 13;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '4';   out[o++] = 'm';
              break;
            case 6: /* dark purple */
              if (last == 15)
                break;
              last = 15;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '5';   out[o++] = 'm';
              break;
          }
          break;

        case 'T':
          x = number_range(1, 6);
          switch (x)
          {
            case 1: /* bright red */
              if (last == 6)
                break;
              last = 6;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '1';   out[o++] = 'm';
              break;
            case 2: /* bright cyan */
              if (last == 8)
                break;
              last = 8;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '6';   out[o++] = 'm';
              break;
            case 3: /* bright yellow */
              if (last == 10)
                break;
              last = 10;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '3';   out[o++] = 'm';
              break;
            case 4: /* bright green */
              if (last == 12)
                break;
              last = 12;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '2';   out[o++] = 'm';
              break;
            case 5: /* bright blue */
              if (last == 14)
                break;
              last = 14;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '4';   out[o++] = 'm';
              break;
            case 6: /* bright purple */
              if (last == 16)
                break;
              last = 16;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '5';   out[o++] = 'm';
              break;
          }
          break;

        case 's':
          x = number_range(1, 10);
          switch (x)
          {
            case 1: /* dark red */
              if (last == 5)
                break;
              last = 5;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '1';   out[o++] = 'm';
              break;
            case 2: /* bright red */
              if (last == 6)
                break;
              last = 6;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '1';   out[o++] = 'm';
              break;
            case 3: /* dark cyan */
              if (last == 7)
                break;
              last = 7;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '6';   out[o++] = 'm';
              break;
            case 4: /* bright cyan */
              if (last == 8)
                break;
              last = 8;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '6';   out[o++] = 'm';
              break;
            case 5: /* dark yellow */
              if (last == 9)
                break;
              last = 9;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '3';   out[o++] = 'm';
              break;
            case 6: /* bright yellow */
              if (last == 10)
                break;
              last = 10;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '3';   out[o++] = 'm';
              break;
            case 7: /* dark green */
              if (last == 11)
                break;
              last = 11;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '2';   out[o++] = 'm';
              break;
            case 8: /* bright green */
              if (last == 12)
                break;
              last = 12;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '2';   out[o++] = 'm';
              break;
            case 9: /* dark blue */
              if (last == 13)
                break;
              last = 13;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '4';   out[o++] = 'm';
              break;
            case 10: /* bright blue */
              if (last == 14)
                break;
              last = 14;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '4';   out[o++] = 'm';
              break;
            case 11: /* dark purple */
              if (last == 15)
                break;
              last = 15;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '5';   out[o++] = 'm';
              break;
            case 12: /* bright purple */
              if (last == 16)
                break;
              last = 16;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '5';   out[o++] = 'm';
              break;
          }
          break;

        case 'S':
          switch (last)
          {
            case 0:
            default:
              break;
            case 1:
              last = 2;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '0';   out[o++] = 'm';
              break;
            case 2:
              last = 1;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '0';   out[o++] = 'm';
              break;
            case 3:
              last = 4;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '7';   out[o++] = 'm';
              break;
            case 4:
              last = 3;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '7';   out[o++] = 'm';
              break;
            case 5:
              last = 6;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '1';   out[o++] = 'm';
              break;
            case 6:
              last = 5;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '1';   out[o++] = 'm';
              break;
            case 7:
              last = 8;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '6';   out[o++] = 'm';
              break;
            case 8:
              last = 7;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '6';   out[o++] = 'm';
              break;
            case 9:
              last = 10;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '3';   out[o++] = 'm';
              break;
            case 10:
              last = 9;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '3';   out[o++] = 'm';
              break;
            case 11:
              last = 12;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '2';   out[o++] = 'm';
              break;
            case 12:
              last = 11;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '2';   out[o++] = 'm';
              break;
            case 13:
              last = 14;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '4';   out[o++] = 'm';
              break;
            case 14:
              last = 13;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '4';   out[o++] = 'm';
              break;
            case 15:
              last = 16;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '5';   out[o++] = 'm';
              break;
            case 16:
              last = 15;
              out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
              out[o++] = '3';   out[o++] = '5';   out[o++] = 'm';
              break;
          }
          break;

        default:
          out[o++] = '#';
          out[o++] = in[i];
          break;
      }
    }
    else if (in[i] == '\n')
    {
      /* after a linebreak resend the last color because gmud sometimes 'forgets' */
      ++i;
      out[o++] = '\n';
      switch (last)
      {
        case 0:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = 'm';
          break;
        case 1:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '0';   out[o++] = 'm';
          break;
        case 2:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '0';   out[o++] = 'm';
          break;
        case 3:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '7';   out[o++] = 'm';
          break;
        case 4:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '7';   out[o++] = 'm';
          break;
        case 5:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '1';   out[o++] = 'm';
          break;
        case 6:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '1';   out[o++] = 'm';
          break;
        case 7:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '6';   out[o++] = 'm';
          break;
        case 8:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '6';   out[o++] = 'm';
          break;
        case 9:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '3';   out[o++] = 'm';
          break;
        case 10:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '3';   out[o++] = 'm';
          break;
        case 11:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '2';   out[o++] = 'm';
          break;
        case 12:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '2';   out[o++] = 'm';
          break;
        case 13:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '4';   out[o++] = 'm';
          break;
        case 14:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '4';   out[o++] = 'm';
          break;
        case 15:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '5';   out[o++] = 'm';
          break;
        case 16:
          out[o++] = '\33';  out[o++] = '[';   out[o++] = '1'; out[o++] = ';';
          out[o++] = '3';   out[o++] = '5';   out[o++] = 'm';
          break;
      }
      continue;
    }
    else
      out[o++] = in[i];

    i++;
  }

  out[o++] = '\33';  out[o++] = '[';   out[o++] = '0'; out[o++] = 'm'; out[o++] = '\0';
}

void decolorize(char *in, char *out)
{
  int i = 0; /* iterator, keeps track of position of in */
  int o = 0; /* keeps track of position of out */
  int x;

/*
  rRcCyYgGbBmMpP
  t    random (rcygbm)
  T    random (RCYGBM)
  s    random (rRcCyYgGbBmM)
  nN   clear
  0123 shades of grey
  8    random number 1-9
  9    random number 0-9
*/

  while (i <= strlen(in)) {
    if (in[i] == '#') {
      switch (in[++i]) {
        case '#':
          out[o++] = '#';
          break;
        case 'N':
        case 'n':
        case '0':
        case '1':
        case '2':
        case '3':
        case 'r':
        case 'R':
        case 'c':
        case 'C':
        case 'y':
        case 'Y':
        case 'g':
        case 'G':
        case 'b':
        case 'B':
        case 'p':
        case 'm':
        case 'M':
        case 'P':
        case 't':
        case 'T':
        case 's':
        case 'S':
          break;

        case '8':
          x = number_range(1, 9);
          out[o++] = '0' + x;
          break;

        case '9':
          x = number_range(0, 9);
          out[o++] = '0' + x;
          break;

        default:
          out[o++] = '#';
          out[o++] = in[i];
          break;
      }
    } else
      out[o++] = in[i];

    i++;
  }

  out[o++] = '\0';
}
