#include "undertow.h"

void do_users(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int num = 0;

  for (d = dhead; d; d = d->next)
  {
    ++num;
    snprintf(buf, MAX_STRING_LENGTH, "#1[#g%2d#1] ", d->socket);
    switch (d->state)
    {
      default:
        strlcpy(buf2, "#1[#rUnknown!  #1]", MAX_STRING_LENGTH);
      break;
      case STATE_LINKDEAD:
        strlcpy(buf2, "#1[#yLinkdead  #1]", MAX_STRING_LENGTH);
      break;
      case STATE_PLAYING:
        strlcpy(buf2, "#1[#3Playing   #1]", MAX_STRING_LENGTH);
      break;
      case STATE_VOID:
        strlcpy(buf2, "#1[#1Void      #1]", MAX_STRING_LENGTH);
      break;
      case STATE_GET_NAME:
        strlcpy(buf2, "#1[#GGetName   #1]", MAX_STRING_LENGTH);
      break;
      case STATE_CONFIRM_NAME:
        strlcpy(buf2, "#1[#GCfrmName  #1]", MAX_STRING_LENGTH);
      break;
      case STATE_NEW_PASS:
        strlcpy(buf2, "#1[#GNewPass   #1]", MAX_STRING_LENGTH);
      break;
      case STATE_CONFIRM_PASS:
        strlcpy(buf2, "#1[#GCfrmPass  #1]", MAX_STRING_LENGTH);
      break;
      case STATE_GET_ANSI:
        strlcpy(buf2, "#1[#GGetANSI   #1]", MAX_STRING_LENGTH);
      break;
      case STATE_GET_PASS:
        strlcpy(buf2, "#1[#GGetPass   #1]", MAX_STRING_LENGTH);
      break;
      case STATE_ACCT_MENU:
        strlcpy(buf2, "#1[#CAcctMenu  #1]", MAX_STRING_LENGTH);
      break;
      case STATE_NEW_CHAR:
        strlcpy(buf2, "#1[#CNewChar   #1]", MAX_STRING_LENGTH);
      break;
      case STATE_CONFIRM_CHAR:
        strlcpy(buf2, "#1[#CCfrmChar  #1]", MAX_STRING_LENGTH);
      break;
      case STATE_DELETE_CHAR:
        strlcpy(buf2, "#1[#CDelChar   #1]", MAX_STRING_LENGTH);
      break;
      case STATE_CONFIRM_DEL:
        strlcpy(buf2, "#1[#CDelChCfrm #1]", MAX_STRING_LENGTH);
      break;
      case STATE_GET_HARDCORE:
        strlcpy(buf2, "#1[#CChHardcore#1]", MAX_STRING_LENGTH);
      break;
      case STATE_CONFIG_MENU:
        strlcpy(buf2, "#1[#YConfig    #1]", MAX_STRING_LENGTH);
      break;
      case STATE_SET_EMAIL:
        strlcpy(buf2, "#1[#YSetEmail  #1]", MAX_STRING_LENGTH);
      break;
      case STATE_CHANGE_PASS1:
      case STATE_CHANGE_PASS2:
      case STATE_CHANGE_PASS3:
        strlcpy(buf2, "#1[#YChngPass  #1]", MAX_STRING_LENGTH);
      break;

      case STATE_MAIL_MENU:
        strlcpy(buf2, "#1[#MMailMenu  #1]", MAX_STRING_LENGTH);
      break;
      case STATE_MAIL_READ:
        strlcpy(buf2, "#1[#MMailRead  #1]", MAX_STRING_LENGTH);
      break;
      case STATE_MAIL_COMPOSE_REC:
      case STATE_MAIL_COMPOSE_SUB:
      case STATE_MAIL_COMPOSE_MSG:
      case STATE_MAIL_COMPOSE_CFRM:
        strlcpy(buf2, "#1[#MMailWrite #1]", MAX_STRING_LENGTH);
      break;
      case STATE_MAIL_DELETE:
      case STATE_MAIL_DELETE_CFRM:
        strlcpy(buf2, "#1[#MMailDelete#1]", MAX_STRING_LENGTH);
      break;
    }
    strlcat(buf, buf2, MAX_STRING_LENGTH);

    if (d->acc->ch)
    {
      char buf4[64];
      serial_time(d->acc->ch->idle, buf4);
      snprintf(buf2, MAX_STRING_LENGTH, " #R%-18s #1[#c%-8s#1]%s ", d->acc->ch->name, buf4, strlen(buf4) == 8 ? " " : "");
    }
    else if (d->acc->name[0])
    {
      char buf3[128];
      strlcpy(buf3, d->acc->name, 128);
      if (d->state == STATE_GET_PASS)
        strlcat(buf3, "#n(?)", 128);
      else
        strlcat(buf3, "#n   ", 128);
      snprintf(buf2, MAX_STRING_LENGTH, " #C%-28s ", buf3);
    }
    else
      snprintf(buf2, MAX_STRING_LENGTH, " #m%-30s ", d->ip);

    strlcat(buf, buf2, MAX_STRING_LENGTH);

    if (d->acc->ch)
    {
      snprintf(buf2, MAX_STRING_LENGTH, "#1[#R%d#1,#R%d#1][#n", d->acc->ch->x, d->acc->ch->y);
      switch(d->acc->ch->plane)
      {
        case PLANE_NONE:
          strlcat(buf2, "limbo#1]", MAX_STRING_LENGTH);
          break;
        case PLANE_NEWBIE:
          strlcat(buf2, "newbie#1]", MAX_STRING_LENGTH);
          break;
        case PLANE_NEXUS:
          strlcat(buf2, "nexus#1]", MAX_STRING_LENGTH);
          break;
        case PLANE_HNEWBIE:
          strlcat(buf2, "hnewbie#1]", MAX_STRING_LENGTH);
          break;
        case PLANE_HNEXUS:
          strlcat(buf2, "hnexus#1]", MAX_STRING_LENGTH);
          break;
        case PLANE_HEAVEN:
          strlcat(buf2, "heaven#1]", MAX_STRING_LENGTH);
          break;
        case PLANE_HELL:
          strlcat(buf2, "hell#1]", MAX_STRING_LENGTH);
          break;
        case PLANE_JAIL:
          strlcat(buf2, "jail#1]", MAX_STRING_LENGTH);
          break;
        case PLANE_INTERPLANE:
          strlcat(buf2, "interplane#1]", MAX_STRING_LENGTH);
          break;
        case PLANE_INTRAPLANE:
          strlcat(buf2, "intraplane#1]", MAX_STRING_LENGTH);
          break;
      }
      strlcat(buf, buf2, MAX_STRING_LENGTH);
    }

    strlcat(buf, "\n", MAX_STRING_LENGTH);
    send_to_char(ch, buf);
  }
}

void do_hosts(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  char loginbuf[64];
  char idlebuf[64];

  DESCRIPTOR_DATA *d;
  int num = 0;

  send_to_char(ch, "#R+#r-----------------------------------------------------------------#R+\n");
  send_to_char(ch, "#r| #2Character    #1Account      #2IP              #1     Login #2      Idle #r|\n");
  send_to_char(ch, "#R+#r-----------------------------------------------------------------#R+\n");
  for (d = dhead; d; d = d->next)
  {
    if (!d->acc->ch)
      continue;

    ++num;
    serial_time(d->acc->ch->idle, idlebuf);
    serial_time(d->acc->ch->logintime, loginbuf);
    snprintf(buf, MAX_STRING_LENGTH, "#r| #2%-12s #1%-12s #2%-15s #1%10s #2%10s #r|\n", d->acc->ch->name, d->acc->name, d->ip, loginbuf, idlebuf);
    send_to_char(ch, buf);
  }
  send_to_char(ch, "#R+#r-----------------------------------------------------------------#R+\n");

  if (num == 1)
    send_to_char(ch, "#2There is #3one#2 player connected to #RU#rndertow#2.\n");
  else
  {
    number_words(num, buf);

    send_to_char(ch, "#2There are #3");
    send_to_char(ch, buf);
    send_to_char(ch, "#2 players connected to #RU#rndertow#2.\n");
  }
}

void send_who(DESCRIPTOR_DATA *d)
{
  char imms[MAX_STRING_LENGTH];
  char avatars[MAX_STRING_LENGTH];
  char mortals[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *dx;
  CHAR_DATA *ch;
  int players = 0, imm = 0, ava = 0, mor = 0;
  int supreme = 0, demigod = 0, trustee = 0, immortal = 0;
  int loop = 0;
  buf2[0] = 0;
  imms[0] = 0;
  avatars[0] = 0;
  mortals[0] = 0;
  for (dx = dhead; dx; dx = dx->next)
  {
    if (dx->state != STATE_PLAYING && dx->state != STATE_LINKDEAD)
      continue;
    switch(dx->acc->ch->level)
    {
      case LEVEL_SUPREME:  supreme++;  break;
      case LEVEL_DEMIGOD:  demigod++;  break;
      case LEVEL_TRUSTEE:  trustee++;  break;
      case LEVEL_IMMORTAL: immortal++; break;
    }
    ++players;
  }

  while (imm+ava+mor < players)
  {
    for (dx = dhead; dx; dx = dx->next)
    {
      if (dx->state != STATE_PLAYING && dx->state != STATE_LINKDEAD)
        continue;
      ch = dx->acc->ch;

      if (supreme  && ch->level < LEVEL_SUPREME)
        continue;
      if (demigod  && ch->level < LEVEL_DEMIGOD)
        continue;
      if (trustee  && ch->level < LEVEL_TRUSTEE)
        continue;
      if (immortal && ch->level < LEVEL_IMMORTAL)
        continue;

      if (!supreme  && ch->level == LEVEL_SUPREME)
        continue;
      if (!demigod  && ch->level == LEVEL_DEMIGOD)
        continue;
      if (!trustee  && ch->level == LEVEL_TRUSTEE)
        continue;
      if (!immortal && ch->level == LEVEL_IMMORTAL)
        continue;
      if (loop > 1 && ch->level < LEVEL_IMMORTAL)
        continue;

      switch(ch->level)
      {
        case LEVEL_SUPREME:
          snprintf(buf, MAX_STRING_LENGTH, "#1[#CS#Supreme#1]  #2%s %s%s", ch->name, IS_SET(ch->tflags, CTFLAG_AFK) ? "#1[#RAFK#1] " : "#1#R#1", ch->acc->d->state == STATE_LINKDEAD ? "#1[#yLinkdead#1]" : "#1#y#1");
          snprintf(buf2, MAX_STRING_LENGTH, "#r| %-77s #r|\n", buf);
          strlcat(imms, buf2, MAX_STRING_LENGTH);
          --supreme;
          ++imm;
        break;
        case LEVEL_DEMIGOD:
          snprintf(buf, MAX_STRING_LENGTH, "#1[#YD#Semigod#1]  #2%s %s%s", ch->name, IS_SET(ch->tflags, CTFLAG_AFK) ? "#1[#RAFK#1] " : "#1#R#1", ch->acc->d->state == STATE_LINKDEAD ? "#1[#yLinkdead#1]" : "#1#y#1");
          snprintf(buf2, MAX_STRING_LENGTH, "#r| %-77s #r|\n", buf); strlcat(imms, buf2, MAX_STRING_LENGTH);
          --demigod;
          ++imm;
        break;
        case LEVEL_TRUSTEE:
          snprintf(buf, MAX_STRING_LENGTH, "#1[#GT#Srustee#1]  #2%s %s%s", ch->name, IS_SET(ch->tflags, CTFLAG_AFK) ? "#1[#RAFK#1] " : "#1#R#1", ch->acc->d->state == STATE_LINKDEAD ? "#1[#yLinkdead#1]" : "#1#y#1");
          snprintf(buf2, MAX_STRING_LENGTH, "#r| %-77s #r|\n", buf); strlcat(imms, buf2, MAX_STRING_LENGTH);
          --trustee;
          ++imm;
        break;
        case LEVEL_IMMORTAL:
          snprintf(buf, MAX_STRING_LENGTH, "#1[#BI#Smmortal#1] #2%s %s%s", ch->name, IS_SET(ch->tflags, CTFLAG_AFK) ? "#1[#RAFK#1] " : "#1#R#1", ch->acc->d->state == STATE_LINKDEAD ? "#1[#yLinkdead#1]" : "#1#y#1");
          snprintf(buf2, MAX_STRING_LENGTH, "#r| %-77s #r|\n", buf); strlcat(imms, buf2, MAX_STRING_LENGTH);
          --immortal;
          ++imm;
        break;
        case LEVEL_AVATAR:
          snprintf(buf, MAX_STRING_LENGTH, "#1[#RAvatar#1]   %s%s %s%s", IS_SET(ch->cflags, CFLAG_HARDCORE) ? "#R":"#2", ch->name, IS_SET(ch->tflags, CTFLAG_AFK) ? "#1[#RAFK#1] " : "#1#R#1", ch->acc->d->state == STATE_LINKDEAD ? "#1[#yLinkdead#1]" : "#1#y#1");
          snprintf(buf2, MAX_STRING_LENGTH, "#r| %-75s #r|\n", buf); strlcat(avatars, buf2, MAX_STRING_LENGTH);
          ++ava;
        break;
        case LEVEL_MORTAL:
          snprintf(buf, MAX_STRING_LENGTH, "#1[#MMortal#1]   %s%s %s%s", IS_SET(ch->cflags, CFLAG_HARDCORE) ? "#R":"#2", ch->name, IS_SET(ch->tflags, CTFLAG_AFK) ? "#1[#RAFK#1] " : "#1#R#1", ch->acc->d->state == STATE_LINKDEAD ? "#1[#yLinkdead#1]" : "#1#y#1");
          snprintf(buf2, MAX_STRING_LENGTH, "#r| %-75s #r|\n", buf); strlcat(mortals, buf2, MAX_STRING_LENGTH);
          ++mor;
        break;
      }
    }
  }

  send_to_descriptor(d, "\n#R+#r-----------------------#1[#2User List#1]#r-----------------------#R+\n");
  if (players == 0)
  {
    send_to_descriptor(d, "#r|                #3No one is on at Undertow!                #r|\n");
    send_to_descriptor(d, "#R+#r---------------------------------------------------------#R+#n\n");
    return;
  }

  if (imm > 0)
  {
    send_to_descriptor(d, "#r|                        #3Immortals                        #r|\n");
    send_to_descriptor(d, "#r|                        #1---------                        #r|\n");
    send_to_descriptor(d, imms);
  }
  if (ava > 0)
  {
    if (imm > 0)
      send_to_descriptor(d, "#r|                         #1-------                         #r|\n");

    send_to_descriptor(d, "#r|                         #3Avatars                         #r|\n");
    send_to_descriptor(d, "#r|                         #1-------                         #r|\n");
    send_to_descriptor(d, avatars);
  }
  if (mor > 0)
  {
    if (imm > 0 || ava > 0)
      send_to_descriptor(d, "#r|                         #1-------                         #r|\n");
    send_to_descriptor(d, "#r|                         #3Mortals                         #r|\n");
    send_to_descriptor(d, "#r|                         #1-------                         #r|\n");
    send_to_descriptor(d, mortals);
  }

  send_to_descriptor(d, "#R+#r---------------------------------------------------------#R+#n\n");

  number_words(players, buf2);
  if (players == 1)
    strlcpy(buf, "is #3one#2 player on #RU#rndertow#2.", MAX_STRING_LENGTH);
  else
    snprintf(buf, MAX_STRING_LENGTH, "are #3%s#2 players on #RU#rndertow#2.", buf2);
  snprintf(buf2, MAX_STRING_LENGTH, "#r| #2There %-59s #r|\n", buf);
  send_to_descriptor(d, buf2);
  send_to_descriptor(d, "#R+#r---------------------------------------------------------#R+#n\n");
}

void do_who(CHAR_DATA *ch, char *arguments)
{
  send_who(ch->d);
}

void do_worth(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  snprintf(buf, MAX_STRING_LENGTH, "You are carrying #R%d#n pluto%s.\n", ch->plutos, ch->plutos == 1 ? "" : "s");
  send_to_char(ch, buf);
}

void do_time(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  char str_cur_time[64];

  time_words_short(current_time - boot_time, str_cur_time);
  snprintf(buf, MAX_STRING_LENGTH, "Undertow was booted:     %s GMT (%s ago)\n", str_boot_time, str_cur_time);
  send_to_char(ch, buf);

  time_words_short(current_time - reset_time, str_cur_time);
  snprintf(buf, MAX_STRING_LENGTH, "Undertow was last reset: %s GMT (%s ago)\n", str_reset_time, str_cur_time);
  send_to_char(ch, buf);

  log_time(str_cur_time);
  snprintf(buf, MAX_STRING_LENGTH, "Current server time:     %s GMT\n", str_cur_time);
  send_to_char(ch, buf);
}

void do_history(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];

  snprintf(buf, MAX_STRING_LENGTH, "History info for:     %s\n", ch->name);
  send_to_char(ch, buf);

  time_words(ch->time, buf2);
  snprintf(buf, MAX_STRING_LENGTH, "Total time on:        %s\n", buf2);
  send_to_char(ch, buf);

  snprintf(buf, MAX_STRING_LENGTH, "First time on (acct): %s\n", ch->acc->firsttime[0] ? ch->acc->firsttime : "Before I started keeping track");
  send_to_char(ch, buf);

  snprintf(buf, MAX_STRING_LENGTH, "First time on (char): %s\n", ch->acc->chfirsttime[ch->acc->cur_char][0] ? ch->acc->chfirsttime[ch->acc->cur_char] : "Before I started keeping track");
  send_to_char(ch, buf);
}

void do_score(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char buf3[MAX_STRING_LENGTH];
  CHAR_DATA *vic;

  if (ch->level < LEVEL_IMMORTAL || arguments[0] == '\0')
    vic = ch;
  else
  {
    vic = find_char_world(ch, arguments);

    if (vic == NULL)
    {
      send_to_char(ch, "I'm afraid I cannot find that player.\n");
      return;
    }
  }

  if (ch != vic)
    send_to_char(ch, "\n#R+#r--------------------------#1[#2Score#1]#r-------------------------#R+\n");
  else
    send_to_char(ch, "\n#R+#r-----------------------#1[#2Your Score#1]#r-----------------------#R+\n");
  snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Character: #R%s#1]", vic->name);
  snprintf(buf3, MAX_STRING_LENGTH, "#1[#2Account: #R%s#1]", vic->accname);
  snprintf(buf, MAX_STRING_LENGTH, "#r| %-35s %-35s  #r|\n", buf2, buf3);
  send_to_char(ch, buf);

  serial_time(vic->time, buf3);
  snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Time: #R%s#1]", buf3);
  snprintf(buf3, MAX_STRING_LENGTH, "#1[#2Class: #RNone#1]");
  snprintf(buf, MAX_STRING_LENGTH, "#r| %-35s %-35s  #r|\n", buf2, buf3);
  send_to_char(ch, buf);

  snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Primal: #R%d#1]", vic->primal);
  snprintf(buf3, MAX_STRING_LENGTH, "#1[#2HP: #R%d#1/#R%d#1]", vic->hp, vic->maxhp);
  snprintf(buf, MAX_STRING_LENGTH, "#r| %-35s %-39s  #r|\n", buf2, buf3);
  send_to_char(ch, buf);

  snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Mana: #R%d#1/#R%d#1]", vic->mana, vic->maxmana);
  snprintf(buf3, MAX_STRING_LENGTH, "#1[#2Move: #R%d#1/#R%d#1]", vic->move, vic->maxmove);
  snprintf(buf, MAX_STRING_LENGTH, "#r| %-39s %-39s  #r|\n", buf2, buf3);
  send_to_char(ch, buf);

  snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Tolerance: #R%d#1]", vic->resistance);
  snprintf(buf3, MAX_STRING_LENGTH, "#1[#2Plutos: #R%d#1]", vic->plutos);
  snprintf(buf, MAX_STRING_LENGTH, "#r| %-35s %-35s  #r|\n", buf2, buf3);
  send_to_char(ch, buf);
  send_to_char(ch, "#R+#r----------------------------------------------------------#R+#n\n");
}

void do_finger(CHAR_DATA *ch, char *arguments)
{
  ACCOUNT_DATA *acc;
  CHAR_DATA *xch;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char buf3[MAX_STRING_LENGTH];
  int email = 0;
  if (arguments[0] == 0)
  {
    send_to_char(ch, "Whom would you like to finger?\n");
    return;
  }

  xch = load_character(NULL, arguments);
  if (xch->name[0] == 0)
    send_to_char(ch, "Unable to finger that player.\n");
  else
  {
    acc = new_account();
    load_account(xch->accname, acc);

    send_to_char(ch, "\n#R+#r------------------------------#1[#2Finger#1]#r------------------------------#R+\n");
    snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Character: #R%s#1]", xch->name);
    snprintf(buf3, MAX_STRING_LENGTH, "#1[#2Class: #RNone#1]");
    snprintf(buf, MAX_STRING_LENGTH, "#r| %-40s %-40s  #r|\n", buf2, buf3);
    send_to_char(ch, buf);
    time_words(xch->time, buf3);
    snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Time: #R%s#1]", buf3);
    snprintf(buf, MAX_STRING_LENGTH, "#r| %-74s #r|\n", buf2);
    send_to_char(ch, buf);

    if (IS_SET(acc->aflags, AFLAG_PUBLIC_EMAIL))
    {
      email = 1;
      snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Email: #R%s#1]", acc->email);
      snprintf(buf, MAX_STRING_LENGTH, "#r| %-74s #r|\n", buf2);
      send_to_char(ch, buf);
    }

    if (ch->level == LEVEL_SUPREME)
    {
      send_to_char(ch, "#R+#r-----------------------------#1[#2Imm Info#1]#r-----------------------------#R+\n");

      if (email == 0)
      {
        snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Email: #R%s#1]", acc->email);
        snprintf(buf, MAX_STRING_LENGTH, "#r| %-74s #r|\n", buf2);
        send_to_char(ch, buf);
      }

      snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Account: #R%s#1]", xch->accname);
      snprintf(buf, MAX_STRING_LENGTH, "#r| %-74s #r|\n", buf2);
      send_to_char(ch, buf);

      if (acc->lasttime == 0)
        snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Lastsave: #RNever!#1]");
      else
        snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Lastsave: #R%s#1]", acc->lasttime);
      snprintf(buf, MAX_STRING_LENGTH, "#r| %-74s #r|\n", buf2);
      send_to_char(ch, buf);

      if (acc->lasthost[0] == 0)
        snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Host: #RN/A#1]");
      else
        snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Host: #R%s#1]", acc->lasthost);
      snprintf(buf, MAX_STRING_LENGTH, "#r| %-74s #r|\n", buf2);
      send_to_char(ch, buf);
    }

    send_to_char(ch, "#R+#r--------------------------------------------------------------------#R+#n\n");
    unlink_character(xch);
    free_account(acc);
  }
  free_character(xch);
}

void do_fingeracct(CHAR_DATA *ch, char *arguments)
{
  ACCOUNT_DATA *acc;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char buf3[MAX_STRING_LENGTH];
  char buf4[128];
  int i, j, len = 0;

  if (arguments[0] == 0)
  {
    send_to_char(ch, "Whom would you like to finger?\n");
    return;
  }

  acc = new_account();
  load_account(arguments, acc);
  if (acc->name[0] == 0)
  {
    free_account(acc);
    send_to_char(ch, "Unable to finger that account.\n");
    return;
  }

  send_to_char(ch, "\n#R+#r--------------------------#1[#2Finger Account#1]#r--------------------------#R+\n");
  snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Account: #R%s#1]", acc->name);
  snprintf(buf3, MAX_STRING_LENGTH, "#1[#2Logins: #R%d#1]", acc->logins);
  snprintf(buf, MAX_STRING_LENGTH, "#r| %-40s %-40s  #r|\n", buf2, buf3);
  send_to_char(ch, buf);

  serial_time(acc->time, buf4);
  if (acc->lasttime == 0)
    snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Lastsave: #RNever!#1]");
  else
    snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Lastsave: #R%s#1]", acc->lasttime);
  snprintf(buf3, MAX_STRING_LENGTH, "#1[#2Time: #R%s#1]", buf4);
  snprintf(buf, MAX_STRING_LENGTH, "#r| %-40s %-40s  #r|\n", buf2, buf3);
  send_to_char(ch, buf);

  for (i = 0; i < acc->charcount; ++i)
    if (strlen(acc->character[i]) > len)
      len = strlen(acc->character[i]);

  for (i = 0; i < acc->charcount; ++i)
  {
    for (j = 0; j < len - strlen(acc->character[i]); ++j)
      buf[j] = ' ';
    buf[j] = '\0';

    switch (acc->status[i])
    {
      default:
      case CHSTATUS_NORMAL:
        strlcat(buf, "#1#R#1", MAX_STRING_LENGTH); /* gets around colors not displaying */
        break;
      case CHSTATUS_HARDCORE:
        strlcat(buf, "#1[#RH#1]", MAX_STRING_LENGTH);
        break;
      case CHSTATUS_DELETED:
        strlcat(buf, "#1[#1D#1]", MAX_STRING_LENGTH);
        break;
      case CHSTATUS_IMMORTAL:
        strlcat(buf, "#1[#YI#1]", MAX_STRING_LENGTH);
        break;
    }
    snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Char %d: #R%s#1] %s", i+1, acc->character[i], buf);
    serial_time(acc->chtime[i], buf4);
    snprintf(buf3, MAX_STRING_LENGTH, "#1[#2Time: #R%s#1]", buf4);
    snprintf(buf, MAX_STRING_LENGTH, "#r| %-46s %-40s  #r|\n", buf2, buf3);
    send_to_char(ch, buf);
  }

  snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Email: #R%s#1]", acc->email);
  snprintf(buf, MAX_STRING_LENGTH, "#r| %-74s #r|\n", buf2);
  send_to_char(ch, buf);

  if (acc->lasthost[0] == 0)
    snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Host: #RN/A#1]");
  else
    snprintf(buf2, MAX_STRING_LENGTH, "#1[#2Host: #R%s#1]", acc->lasthost);
  snprintf(buf, MAX_STRING_LENGTH, "#r| %-74s #r|\n", buf2);
  send_to_char(ch, buf);

  send_to_char(ch, "#R+#r--------------------------------------------------------------------#R+#n\n");
  free_account(acc);
}
