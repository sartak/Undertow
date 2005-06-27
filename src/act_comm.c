#include "undertow.h"

char lastchat[10][700];
char lastimm[10][700];
char lasttru[10][700];
char lastdem[10][700];
char lastsup[10][700];

void do_last(CHAR_DATA *ch, char *arguments)
{
  int i, tot = 0;
  char buf[MAX_STRING_LENGTH];
  one_arg(arguments, buf);

  if (ch->level < LEVEL_IMMORTAL || buf[0] == '\0' || !stricmp(buf, "chat"))
  {
    send_to_char(ch, "Here are the last few chats:\n");
    for (i = 9; i >= 0; --i)
      if (lastchat[i][0] != 0)
      {
        ++tot;
        snprintf(buf, MAX_STRING_LENGTH, "#1[#2%2d#1]: ", tot);
        if (buf[5] == ' ') buf[5] = '0';
        send_to_char(ch, buf);
        send_to_char(ch, lastchat[i]);
      }
    if (tot == 0)
      send_to_char(ch, "  #nNobody has chatted recently.\n");
  }
  else if (ch->level >= LEVEL_IMMORTAL && !stricmp(buf, "imm"))
  {
    send_to_char(ch, "Here are the last few immtalks:\n");
    for (i = 9; i >= 0; --i)
      if (lastimm[i][0] != 0)
      {
        ++tot;
        snprintf(buf, MAX_STRING_LENGTH, "#1[#2%2d#1]: ", tot);
        if (buf[5] == ' ') buf[5] = '0';
        send_to_char(ch, buf);
        send_to_char(ch, lastimm[i]);
      }
    if (tot == 0)
      send_to_char(ch, "  #nNobody has immtalked recently.\n");
  }
  else if (ch->level >= LEVEL_TRUSTEE && !stricmp(buf, "tru"))
  {
    send_to_char(ch, "Here are the last few trutalks:\n");
    for (i = 9; i >= 0; --i)
      if (lasttru[i][0] != 0)
      {
        ++tot;
        snprintf(buf, MAX_STRING_LENGTH, "#1[#2%2d#1]: ", tot);
        if (buf[5] == ' ') buf[5] = '0';
        send_to_char(ch, buf);
        send_to_char(ch, lasttru[i]);
      }
    if (tot == 0)
      send_to_char(ch, "  #nNobody has trutalked recently.\n");
  }
  else if (ch->level >= LEVEL_DEMIGOD && !stricmp(buf, "dem"))
  {
    send_to_char(ch, "Here are the last few demtalks:\n");
    for (i = 9; i >= 0; --i)
      if (lastdem[i][0] != 0)
      {
        ++tot;
        snprintf(buf, MAX_STRING_LENGTH, "#1[#2%2d#1]: ", tot);
        if (buf[5] == ' ') buf[5] = '0';
        send_to_char(ch, buf);
        send_to_char(ch, lastdem[i]);
      }
    if (tot == 0)
      send_to_char(ch, "  #nNobody has demtalked recently.\n");
  }
  else if (ch->level >= LEVEL_SUPREME && !stricmp(buf, "sup"))
  {
    send_to_char(ch, "Here are the last few suptalks:\n");
    for (i = 9; i >= 0; --i)
      if (lastsup[i][0] != 0)
      {
        ++tot;
        snprintf(buf, MAX_STRING_LENGTH, "#1[#2%2d#1]: ", tot);
        if (buf[5] == ' ') buf[5] = '0';
        send_to_char(ch, buf);
        send_to_char(ch, lastsup[i]);
      }
    if (tot == 0)
      send_to_char(ch, "  #nNobody has suptalked recently.\n");
  }
  else
  {
    snprintf(buf, MAX_STRING_LENGTH, "Invalid argument. Valid arguments are: chat imm%s%s%s\n",
                                     ch->level >= LEVEL_TRUSTEE ? " tru":"",
                                     ch->level >= LEVEL_DEMIGOD ? " dem":"",
                                     ch->level >= LEVEL_SUPREME ? " sup":"");
    send_to_char(ch, buf);
  }
}

void do_chat(CHAR_DATA *ch, char *arguments)
{
  DESCRIPTOR_DATA *d;
  char buf[700];
  int i;

  if (arguments[0] == 0)
  {
    send_to_char(ch, "What do you want to chat?\n");
    return;
  }

  if (strlen(arguments) > 500)
  {
    arguments[500] = '\0';
    strlcat(arguments, "#G...", MAX_STRING_LENGTH);
    send_to_char(ch, "[Your chat was truncated]\n");
  }

  snprintf(buf, 700, "#1[#2Chat#1] #R%s#1: #2%s\33[0m\n", ch->name, arguments);

  for (i = 9; i > 0; --i)
    strlcpy(lastchat[i], lastchat[i-1], 700);
  strlcpy(lastchat[0], buf, 700);

  for (d = dhead; d; d = d->next)
    if (d->state == STATE_PLAYING || d->state == STATE_LINKDEAD)
    {
      if (d != ch->d)
        send_to_descriptor(d, "\n");
      send_to_descriptor(d, buf);
    }
}

void do_immtalk(CHAR_DATA *ch, char *arguments)
{
  DESCRIPTOR_DATA *d;
  char buf[700];
  int i;

  if (arguments[0] == 0)
  {
    send_to_char(ch, "What do you want to immortalchat?\n");
    return;
  }

  if (strlen(arguments) > 500)
  {
    arguments[500] = '\0';
    strlcat(arguments, "#G...", MAX_STRING_LENGTH);
    send_to_char(ch, "[Your immtalk was truncated]\n");
  }

  snprintf(buf, 700, "#1[#BImmortal#1] #R%s#1: #2%s\33[0m\n", ch->name, arguments);

  for (i = 9; i > 0; --i)
    strlcpy(lastimm[i], lastimm[i-1], 700);
  strlcpy(lastimm[0], buf, 700);

  for (d = dhead; d; d = d->next)
    if (d->acc->ch && (d->state == STATE_PLAYING || d->state == STATE_LINKDEAD))
    {
      if (d->acc->ch->level < LEVEL_IMMORTAL)
        continue;
      if (d != ch->d)
        send_to_descriptor(d, "\n");
      send_to_descriptor(d, buf);
    }
}

void do_trutalk(CHAR_DATA *ch, char *arguments)
{
  DESCRIPTOR_DATA *d;
  char buf[700];
  int i;

  if (arguments[0] == 0)
  {
    send_to_char(ch, "What do you want to trusteechat?\n");
    return;
  }

  if (strlen(arguments) > 500)
  {
    arguments[500] = '\0';
    strlcat(arguments, "#G...", MAX_STRING_LENGTH);
    send_to_char(ch, "[Your trutalk was truncated]\n");
  }

  snprintf(buf, 700, "#1[#gTrustee#1] #R%s#1: #2%s\33[0m\n", ch->name, arguments);

  for (i = 9; i > 0; --i)
    strlcpy(lasttru[i], lasttru[i-1], 700);
  strlcpy(lasttru[0], buf, 700);

  for (d = dhead; d; d = d->next)
    if (d->acc->ch && (d->state == STATE_PLAYING || d->state == STATE_LINKDEAD))
    {
      if (d->acc->ch->level < LEVEL_TRUSTEE)
        continue;
      if (d != ch->d)
        send_to_descriptor(d, "\n");
      send_to_descriptor(d, buf);
    }
}

void do_demtalk(CHAR_DATA *ch, char *arguments)
{
  DESCRIPTOR_DATA *d;
  char buf[700];
  int i;

  if (arguments[0] == 0)
  {
    send_to_char(ch, "What do you want to demigodchat?\n");
    return;
  }

  if (strlen(arguments) > 500)
  {
    arguments[500] = '\0';
    strlcat(arguments, "#G...", MAX_STRING_LENGTH);
    send_to_char(ch, "[Your demtalk was truncated]\n");
  }

  snprintf(buf, 700, "#1[#yDemigod#1] #R%s#1: #2%s\33[0m\n", ch->name, arguments);

  for (i = 9; i > 0; --i)
    strlcpy(lastdem[i], lastdem[i-1], 700);
  strlcpy(lastdem[0], buf, 700);

  for (d = dhead; d; d = d->next)
    if (d->acc->ch && (d->state == STATE_PLAYING || d->state == STATE_LINKDEAD))
    {
      if (d->acc->ch->level < LEVEL_DEMIGOD)
        continue;
      if (d != ch->d)
        send_to_descriptor(d, "\n");
      send_to_descriptor(d, buf);
    }
}

void do_suptalk(CHAR_DATA *ch, char *arguments)
{
  DESCRIPTOR_DATA *d;
  char buf[700];
  int i;

  if (arguments[0] == 0)
  {
    send_to_char(ch, "What do you want to supremechat?\n");
    return;
  }

  if (strlen(arguments) > 500)
  {
    arguments[500] = '\0';
    strlcat(arguments, "#G...", MAX_STRING_LENGTH);
    send_to_char(ch, "[Your suptalk was truncated]\n");
  }

  snprintf(buf, 700, "#1[#cSupreme#1] #R%s#1: #2%s\33[0m\n", ch->name, arguments);

  for (i = 9; i > 0; --i)
    strlcpy(lastsup[i], lastsup[i-1], 700);
  strlcpy(lastsup[0], buf, 700);

  for (d = dhead; d; d = d->next)
    if (d->acc->ch && (d->state == STATE_PLAYING || d->state == STATE_LINKDEAD))
    {
      if (d->acc->ch->level < LEVEL_SUPREME)
        continue;
      if (d != ch->d)
        send_to_descriptor(d, "\n");
      send_to_descriptor(d, buf);
    }
}

void do_reply(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  if (ch->reply[0] == 0)
  {
    send_to_char(ch, "Reply to what, exactly?\n");
    return;
  }
  if (arguments[0] == 0)
  {
    send_to_char(ch, "What do you want to reply?\n");
    return;
  }

  snprintf(buf, MAX_STRING_LENGTH, "%s %s", ch->reply, arguments);

  do_tell(ch, buf);
}

void do_tell(CHAR_DATA *ch, char *arguments)
{
  char arg[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *vic;

  if (arguments[0] == 0)
  {
    send_to_char(ch, "To whom do you want to send a tell?\n");
    return;
  }

  one_arg(arguments, arg);
  vic = find_char_world(ch, arg);

  if (vic == NULL)
  {
    send_to_char(ch, "I'm afraid I cannot find that player.\n");
    return;
  }
  if (vic == ch)
  {
    send_to_char(ch, "Talking to yourself is highly irregular.\n");
    return;
  }
  if (IS_NPC(vic))
  {
    send_to_char(ch, "I'm afraid they cannot understand you.\n");
    return;
  }
  if (arguments[0] == 0)
  {
    send_to_char(ch, "What do you want to tell them?\n");
    return;
  }
  if (strlen(arguments) > 500)
  {
    arguments[500] = '\0';
    strlcat(arguments, "#G...", MAX_STRING_LENGTH);
    send_to_char(ch, "[Your tell was truncated]\n");
  }
  if (vic->acc->d->state == STATE_LINKDEAD)
    send_to_char(ch, "[They are linkdead and may not receive your message]\n");
  else if (IS_SET(vic->tflags, CTFLAG_AFK))
    send_to_char(ch, "[They are marked as AFK and may not receive your message]\n");
  if (IS_SET(ch->tflags, CTFLAG_AFK))
    send_to_char(ch, "[Note: people get an annoying message when they send you tells because you're AFK]\n");

  snprintf(buf, MAX_STRING_LENGTH, "\n#M%s#n tells you, \'#M%s\33[0m\'\n", ch->name, arguments);
  send_to_char(vic, buf);
  snprintf(buf, MAX_STRING_LENGTH, "You tell %s, \'#M%s\33[0m\'\n", vic->name, arguments);
  send_to_char(ch, buf);

  strlcpy(ch->reply, vic->name, 32);

  if (!IS_SET(vic->acc->aflags, AFLAG_REPLYLOCK) || vic->reply[0] == '\0')
    strlcpy(vic->reply, ch->name, 32);
}

void do_whisper(CHAR_DATA *ch, char *arguments)
{
  char arg[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *vic;

  if (arguments[0] == 0)
  {
    send_to_char(ch, "To whom do you want to whisper?\n");
    return;
  }

  one_arg(arguments, arg);
  vic = find_char_world(ch, arg);

  if (vic == NULL)
  {
    send_to_char(ch, "I'm afraid I cannot find that player.\n");
    return;
  }
  if (vic == ch)
  {
    send_to_char(ch, "Talking to yourself is highly irregular.\n");
    return;
  }
  if (IS_NPC(vic))
  {
    send_to_char(ch, "I'm afraid they cannot understand you.\n");
    return;
  }
  if (arguments[0] == 0)
  {
    send_to_char(ch, "What do you want to whisper to them?\n");
    return;
  }
  if (!is_local(ch->x, ch->y, ch->plane, vic->x, vic->y, vic->plane, ch->sightradius))
  {
    send_to_char(ch, "They're too far away. Try the tell command instead.\n");
    return;
  }
  if (strlen(arguments) > 500)
  {
    arguments[500] = '\0';
    strlcat(arguments, "#G...", MAX_STRING_LENGTH);
    send_to_char(ch, "[Your whisper was truncated]\n");
  }
  snprintf(buf, MAX_STRING_LENGTH, "\n#C%s#n whispers to you, \'#C%s\33[0m\'\n", ch->name, arguments);
  send_to_char(vic, buf);
  snprintf(buf, MAX_STRING_LENGTH, "You whisper to %s, \'#C%s\33[0m\'\n", vic->name, arguments);
  send_to_char(ch, buf);

  snprintf(buf, MAX_STRING_LENGTH, "%s whispers something to %s.\n", ch->name, vic->name);
  put_sense_except2(ch, vic, ch->x, ch->y, ch->plane, SENSE_HEARING, buf);
}

void do_sayto(CHAR_DATA *ch, char *arguments)
{
  char arg[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *vic;

  if (arguments[0] == 0)
  {
    send_to_char(ch, "To whom do you want to say something?\n");
    return;
  }

  one_arg(arguments, arg);
  vic = find_char_world(ch, arg);

  if (vic == NULL)
  {
    send_to_char(ch, "I'm afraid I cannot find that player.\n");
    return;
  }
  if (vic == ch)
  {
    send_to_char(ch, "Talking to yourself is highly irregular.\n");
    return;
  }
  if (IS_NPC(vic))
  {
    send_to_char(ch, "I'm afraid they cannot understand you.\n");
    return;
  }
  if (arguments[0] == 0)
  {
    send_to_char(ch, "What do you want to say to them?\n");
    return;
  }
  if (!is_local(ch->x, ch->y, ch->plane, vic->x, vic->y, vic->plane, ch->sightradius))
  {
    send_to_char(ch, "They're too far away. Try the chat line instead.\n");
    return;
  }
  if (strlen(arguments) > 500)
  {
    arguments[500] = '\0';
    strlcat(arguments, "#G...", MAX_STRING_LENGTH);
    send_to_char(ch, "[Your sayto was truncated]\n");
  }
  snprintf(buf, MAX_STRING_LENGTH, "\n#C%s#n says to you, \'#C%s\33[0m\'\n", ch->name, arguments);
  send_to_char(vic, buf);
  snprintf(buf, MAX_STRING_LENGTH, "You say to %s, \'#C%s\33[0m\'\n", vic->name, arguments);
  send_to_char(ch, buf);

  snprintf(buf, MAX_STRING_LENGTH, "#C%s#n says to #C%s#n, \'#C%s\33[0m\'\n", ch->name, vic->name, arguments);
  put_sense_except2(ch, vic, ch->x, ch->y, ch->plane, SENSE_HEARING, buf);
}

void do_emote(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  if (arguments[0] == 0)
  {
    send_to_char(ch, "What do you want to emote?\n");
    return;
  }

  if (strlen(arguments) > 500)
  {
    arguments[500] = '\0';
    strlcat(arguments, "#G...", MAX_STRING_LENGTH);
    send_to_char(ch, "[Your emote was truncated]\n");
  }
  snprintf(buf, MAX_STRING_LENGTH, "%s %s\33[0m\n", ch->name, arguments);
  put_sense(ch, SENSE_SIGHT, buf);
  put_sense_except(ch, ch->x, ch->y, ch->plane, SENSE_SIGHT, buf);
}

void do_say(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  if (arguments[0] == 0)
  {
    send_to_char(ch, "What do you want to say\n");
    return;
  }

  if (strlen(arguments) > 500)
  {
    arguments[500] = '\0';
    strlcat(arguments, "#G...", MAX_STRING_LENGTH);
    send_to_char(ch, "[Your say was truncated]\n");
  }

  snprintf(buf, MAX_STRING_LENGTH, "#R%s#n says, '#R%s\33[0m\'\n", ch->name, arguments);
  put_sense_except(ch, ch->x, ch->y, ch->plane, SENSE_HEARING, buf);
  snprintf(buf, MAX_STRING_LENGTH, "You say, '#R%s\33[0m\'\n", arguments);
  send_to_char(ch, buf);
}
