#include "undertow.h"

#define APPEAR                 1
#define HIDDEN                 0
#define ENABLED                0
#define DISABLED LEVEL_SUPREME+1


struct  command_data  cmd_table [] =
{
    { "north",         do_north,         POS_STANDING,  LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED},
    { "south",         do_south,         POS_STANDING,  LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "east",          do_east,          POS_STANDING,  LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "west",          do_west,          POS_STANDING,  LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "look",          do_look,          POS_FIGHTING,  LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "inventory",     do_inventory,     POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "get",           do_take,          POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "take",          do_take,          POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "who",           do_who,           POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "actions",       do_socials,       POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "afk",           do_afk,           POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "alarm",         do_alarm,         POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "alias",         do_alias,         POS_FIGHTING,  LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "ansi",          do_ansi,          POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "bug",           do_bug,           POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "bugs",          do_bugs,          POS_ANY,       LEVEL_IMMORTAL, LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "changes",       do_changes,       POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "changeadd",     do_changeadd,     POS_ANY,       LEVEL_SUPREME,  LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "changedel",     do_changedel,     POS_ANY,       LEVEL_SUPREME,  LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "chat",          do_chat,          POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "class",         do_class,         POS_STANDING,  LEVEL_AVATAR,   LOG_NORMAL, APPEAR, CLASS_NONE, ENABLED },
    { "color",         do_ansi,          POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "commands",      do_commands,      POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "config",        do_config,        POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "delay",         do_delay,         POS_ANY,       LEVEL_TRUSTEE,  LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "demtalk",       do_demtalk,       POS_ANY,       LEVEL_DEMIGOD,  LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "drop",          do_drop,          POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "email",         do_email,         POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "emote",         do_emote,         POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "enter",         do_enter,         POS_STANDING,  LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "equipment",     do_equipment,     POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "events",        do_events,        POS_ANY,       LEVEL_IMMORTAL, LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "finger",        do_finger,        POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "fingeracct",    do_fingeracct,    POS_ANY,       LEVEL_IMMORTAL, LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "flush",         do_flush,         POS_ANY,       LEVEL_DEMIGOD,  LOG_ALWAYS, APPEAR, CLASS_ANY,  ENABLED },
    { "fullansi",      do_fullansi,      POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "gold",          do_worth,         POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "help",          do_help,          POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "history",       do_history,       POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "hlist",         do_hlist,         POS_ANY,       LEVEL_IMMORTAL, LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "hosts",         do_hosts,         POS_ANY,       LEVEL_TRUSTEE,  LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "immtalk",       do_immtalk,       POS_ANY,       LEVEL_IMMORTAL, LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "identify",      do_identify,      POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "last",          do_last,          POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "learn",         do_learn,         POS_STANDING,  LEVEL_AVATAR,   LOG_NORMAL, APPEAR, CLASS_ONE,  ENABLED },
    { "mark",          do_mark,          POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "memory",        do_memory,        POS_ANY,       LEVEL_DEMIGOD,  LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "ocreate",       do_ocreate,       POS_ANY,       LEVEL_IMMORTAL, LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "plutos",        do_worth,         POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "prompt",        do_prompt,        POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "powers",        do_powers,        POS_SITTING,   LEVEL_AVATAR,   LOG_NORMAL, APPEAR, CLASS_ONE,  ENABLED },
    { "recall",        do_recall,        POS_STANDING,  LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "relevel",       do_relevel,       POS_ANY,       LEVEL_MORTAL,   LOG_ALWAYS, APPEAR, CLASS_ANY,  ENABLED },
    { "reply",         do_reply,         POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "replylock",     do_replylock,     POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "say",           do_say,           POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "sayto",         do_sayto,         POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "save",          do_save,          POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "score",         do_score,         POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "sit",           do_sit,           POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "sleep",         do_sleep,         POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "socials",       do_socials,       POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "spawn",         do_spawn,         POS_ANY,       LEVEL_SUPREME,  LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "stand",         do_stand,         POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "suggest",       do_suggest,       POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "suggestions",   do_suggestions,   POS_ANY,       LEVEL_IMMORTAL, LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "suptalk",       do_suptalk,       POS_ANY,       LEVEL_SUPREME,  LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "tell",          do_tell,          POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "time",          do_time,          POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "timer",         do_timer,         POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "train",         do_train,         POS_STANDING,  LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "trutalk",       do_trutalk,       POS_ANY,       LEVEL_TRUSTEE,  LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "typo",          do_typo,          POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "typos",         do_typos,         POS_ANY,       LEVEL_IMMORTAL, LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "users",         do_users,         POS_ANY,       LEVEL_TRUSTEE,  LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "whisper",       do_whisper,       POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "worth",         do_worth,         POS_SITTING,   LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },

    /* more drastic commands */
    { "qui",           do_full,          POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "quit",          do_quit,          POS_ANY,       LEVEL_MORTAL,   LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "copyove",       do_full,          POS_ANY,       LEVEL_SUPREME,  LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "copyover",      do_copyover,      POS_ANY,       LEVEL_SUPREME,  LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "delet",         do_full,          POS_ANY,       LEVEL_TRUSTEE,  LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "delete",        do_delete,        POS_ANY,       LEVEL_TRUSTEE,  LOG_ALWAYS, APPEAR, CLASS_ANY,  ENABLED },
    { "disabl",        do_full,          POS_ANY,       LEVEL_TRUSTEE,  LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "disable",       do_disable,       POS_ANY,       LEVEL_TRUSTEE,  LOG_NORMAL, APPEAR, CLASS_ANY,  ENABLED },
    { "invwip",        do_full,          POS_ANY,       LEVEL_TRUSTEE,  LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "invwipe",       do_invwipe,       POS_ANY,       LEVEL_TRUSTEE,  LOG_ALWAYS, APPEAR, CLASS_ANY,  ENABLED },
    { "shutdow",       do_full,          POS_ANY,       LEVEL_DEMIGOD,  LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "shutdown",      do_shutdown,      POS_ANY,       LEVEL_DEMIGOD,  LOG_ALWAYS, APPEAR, CLASS_ANY,  ENABLED },
    { "tou",           do_full,          POS_ANY,       LEVEL_TRUSTEE,  LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "tout",          do_tout,          POS_ANY,       LEVEL_TRUSTEE,  LOG_ALWAYS, APPEAR, CLASS_ANY,  ENABLED },
    { "undelet",       do_full,          POS_ANY,       LEVEL_TRUSTEE,  LOG_NORMAL, HIDDEN, CLASS_ANY,  ENABLED },
    { "undelete",      do_undelete,      POS_ANY,       LEVEL_TRUSTEE,  LOG_ALWAYS, APPEAR, CLASS_ANY,  ENABLED },

    /* always leave this here */
    { "",              NULL,             POS_ANY,                   0,   LOG_NEVER, HIDDEN, CLASS_ANY, ENABLED }
};

void parse(DESCRIPTOR_DATA *d, char *input)
{
  CHAR_DATA *ch = d->acc->ch;
  char cmd[MAX_STRING_LENGTH];
  char args[MAX_STRING_LENGTH];
  char in[MAX_STRING_LENGTH];
  int i = 0;
  int x = 0;
  int bestmatch = -1;
  int matchcount = 0;
  EVENT_DATA *event;
  ALIAS_DATA *alias;

  event = find_event(ch, EVENT_IDLE);

  strlcpy(in, input, MAX_STRING_LENGTH);

  if (in[0] == 0)
  {
    send_to_char(ch, " ");
    if (event)
      event->ticks_left = IDLE_OUT;
    ch->idle = 0;
    return;
  }

  if (in[0] == '.' || in[0] == '0')
  {
    char *input2 = in;
    do
    {
      input2++;
    } while (input2[0] == ' ');
    strlcpy(in, input2, MAX_STRING_LENGTH);
    do_chat(ch, in);
    if (event)
      event->ticks_left = IDLE_OUT;
    ch->idle = 0;
    return;
  }

  if ((in[0] == ',' || in[0] == '1') && ch->level >= LEVEL_IMMORTAL)
  {
    char *input2 = in;
    do
    {
      input2++;
    } while (input2[0] == ' ');
    strlcpy(in, input2, MAX_STRING_LENGTH);
    do_immtalk(ch, in);
    if (event)
      event->ticks_left = IDLE_OUT;
    ch->idle = 0;
    return;
  }

  if ((in[0] == ';' || in[0] == '2') && ch->level >= LEVEL_TRUSTEE)
  {
    char *input2 = in;
    do
    {
      input2++;
    } while (input2[0] == ' ');
    strlcpy(in, input2, MAX_STRING_LENGTH);
    do_trutalk(ch, in);
    if (event)
      event->ticks_left = IDLE_OUT;
    ch->idle = 0;
    return;
  }

  if ((in[0] == '[' || in[0] == '3') && ch->level >= LEVEL_DEMIGOD)
  {
    char *input2 = in;
    do
    {
      input2++;
    } while (input2[0] == ' ');
    strlcpy(in, input2, MAX_STRING_LENGTH);
    do_demtalk(ch, in);
    if (event)
      event->ticks_left = IDLE_OUT;
    ch->idle = 0;
    return;
  }

  if ((in[0] == ']' || in[0] == '4') && ch->level >= LEVEL_SUPREME)
  {
    char *input2 = in;
    do
    {
      input2++;
    } while (input2[0] == ' ');
    strlcpy(in, input2, MAX_STRING_LENGTH);
    do_suptalk(ch, in);
    if (event)
      event->ticks_left = IDLE_OUT;
    ch->idle = 0;
    return;
  }

  if (in[0] == '\'')
  {
    char *input2 = in;
    do
    {
      input2++;
    } while (input2[0] == ' ');
    strlcpy(in, input2, MAX_STRING_LENGTH);
    do_say(ch, in);
    if (event)
      event->ticks_left = IDLE_OUT;
    ch->idle = 0;
    return;
  }

  strlcpy(args, in, MAX_STRING_LENGTH);
  one_arg(args, cmd);

  for (alias = ch->alias; alias; alias = alias->next_local)
    if (!stricmp(cmd, alias->in))
    {
      strlcpy(in, alias->out, MAX_STRING_LENGTH);
      if (args[0] != 0)
      {
        strlcat(in, " ", MAX_STRING_LENGTH);
        strlcat(in, args, MAX_STRING_LENGTH);
      }
      strlcpy(args, in, MAX_STRING_LENGTH);
      one_arg(args, cmd);
      break;
    }

  while (cmd_table[i].command[0] != 0)
  {
    if (cmd_table[i].minlevel > ch->level)
    {
      i++;
      continue;
    }

    if (cmd_table[i].class == CLASS_ANY ||
       (cmd_table[i].class == CLASS_ONE && ch->class != CLASS_NONE) ||
        cmd_table[i].class == ch->class)
    {
      x = beginning(cmd_table[i].command, cmd);
      if (x == strlen(cmd) && x > matchcount)
      {
        bestmatch = i;
        matchcount = x;
      }
    }
    i++;
  }

  if (cmd_table[bestmatch].log != LOG_NEVER &&
     (cmd_table[bestmatch].log == LOG_ALWAYS ||
      IS_SET(ch->cflags, CFLAG_LOGGED) ||
      IS_SET(d->acc->aflags, AFLAG_LOGGED)))
    syslog("COMMAND", "%s: %s", ch->name, in);

  if ((ch->plane == PLANE_INTERPLANE || ch->plane == PLANE_INTRAPLANE) && cmd_table[bestmatch].fun != do_look)
    send_to_char(ch, "#RMadness refuses to release you.#n\n");
  else if (ch->plane == PLANE_INTERPLANE || ch->plane == PLANE_INTRAPLANE)
    do_look(ch, args);
  else
  {
    if (bestmatch < 0)
    {
      if (parse_social(ch, cmd, args) == 0)
        send_to_char(ch, "Huh?\n");
    }
    else
    {
      if (ch->position > cmd_table[bestmatch].position)
        switch(ch->position)
        {
          case POS_SITTING:
            send_to_char(ch, "Not while sitting.\n");
            break;
          case POS_SLEEPING:
            send_to_char(ch, "Not while sleeping.\n");
            break;
          case POS_FIGHTING:
            send_to_char(ch, "Not while fighting.\n");
            break;
        }
      else if (ch->level < cmd_table[bestmatch].disabled)
        send_to_char(ch, "That command is currently disabled.\n");
      else
        (*cmd_table[bestmatch].fun) (ch, args);
    }
  }

  if (event)
    event->ticks_left = IDLE_OUT;
  ch->idle = 0;
}

void display_commands(CHAR_DATA *ch, int level, int indent)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[32];
  char color[3];
  int i;
  int o = 0;
  /* display the raw unformatted commands for a specific level */

  buf[0] = 0;

  for (i = 0; cmd_table[i].command[0]; ++i)
  {
    if (cmd_table[i].appear == HIDDEN)
      continue;

    if (cmd_table[i].minlevel > ch->level)
      continue;

    if ((level == -1 && cmd_table[i].minlevel < LEVEL_IMMORTAL) || (level == cmd_table[i].minlevel)) {}
    else
      continue;

    if (cmd_table[i].class == CLASS_ANY) {}
    else if (ch->class == cmd_table[i].class) {}
    else if (ch->class == CLASS_NONE && cmd_table[i].class == CLASS_NONE) {}
    else if (ch->class != CLASS_NONE && cmd_table[i].class == CLASS_ONE) {}
    else
      continue;

    if (o == 0)
    {
      strlcat(buf, "#r|", MAX_STRING_LENGTH);
      if (indent)
        strlcat(buf, "   ", MAX_STRING_LENGTH);
      else
        strlcat(buf, " ", MAX_STRING_LENGTH);
    }

    ++o;

    if (cmd_table[i].disabled && cmd_table[i].disabled < ch->level)
      strlcpy(color, "#R", 3);
    else if (cmd_table[i].disabled)
      strlcpy(color, "#r", 3);
    else if (o % 2 == 0)
      strlcpy(color, "#1", 3);
    else
      strlcpy(color, "#2", 3);

    snprintf(buf2, 32, "%s%-12s", color, cmd_table[i].command);
    strlcat(buf, buf2, MAX_STRING_LENGTH);

    if (o == 6)
    {
      if (indent)
        strlcat(buf, " ", MAX_STRING_LENGTH);
      else
        strlcat(buf, "   ", MAX_STRING_LENGTH);
      strlcat(buf, "#r|\n", MAX_STRING_LENGTH);
      o = 0;
    }
  }

  if (o > 0)
  {
    for (i = o; i < 6; ++i)
      strlcat(buf, "            ", MAX_STRING_LENGTH);
    if (indent)
      strlcat(buf, " ", MAX_STRING_LENGTH);
    else
      strlcat(buf, "   ", MAX_STRING_LENGTH);
    strlcat(buf, "#r|\n", MAX_STRING_LENGTH);
  }
  send_to_char(ch, buf);
}

void do_commands(CHAR_DATA *ch, char *arguments)
{
  int level = 0;

       if (!stricmp(arguments, "supreme"))  level = LEVEL_SUPREME;
  else if (!stricmp(arguments, "sup"))      level = LEVEL_SUPREME;
  else if (!stricmp(arguments, "demigod"))  level = LEVEL_DEMIGOD;
  else if (!stricmp(arguments, "dgod"))     level = LEVEL_DEMIGOD;
  else if (!stricmp(arguments, "trustee"))  level = LEVEL_TRUSTEE;
  else if (!stricmp(arguments, "tru"))      level = LEVEL_TRUSTEE;
  else if (!stricmp(arguments, "immortal")) level = LEVEL_IMMORTAL;
  else if (!stricmp(arguments, "imm"))      level = LEVEL_IMMORTAL;
  else if (!stricmp(arguments, "mortal"))   level = -1;

  if (level > ch->level || ch->level < LEVEL_IMMORTAL)
    level = 0;

  /* level 0 means display all commands available to ch, otherwise display only a certain level's commands */

  send_to_char(ch, "#R+#r---------------------------------#1[#2Commands#1]#r---------------------------------#R+\n");
  if (level == 0)
  {
    if (ch->level >= LEVEL_SUPREME)
    {
      send_to_char(ch, "#r| #RSupreme:                                                                   #r|\n");
      display_commands(ch, LEVEL_SUPREME, 1);
      send_to_char(ch, "#R+#r----------------------------------------------------------------------------#R+\n");
    }
    if (ch->level >= LEVEL_DEMIGOD)
    {
      send_to_char(ch, "#r| #RDemigod:                                                                   #r|\n");
      display_commands(ch, LEVEL_DEMIGOD, 1);
      send_to_char(ch, "#R+#r----------------------------------------------------------------------------#R+\n");
    }
    if (ch->level >= LEVEL_TRUSTEE)
    {
      send_to_char(ch, "#r| #RTrustee:                                                                   #r|\n");
      display_commands(ch, LEVEL_TRUSTEE, 1);
      send_to_char(ch, "#R+#r----------------------------------------------------------------------------#R+\n");
    }
    if (ch->level >= LEVEL_IMMORTAL)
    {
      send_to_char(ch, "#r| #RImmortal:                                                                  #r|\n");
      display_commands(ch, LEVEL_IMMORTAL, 1);
      send_to_char(ch, "#R+#r----------------------------------------------------------------------------#R+\n");
      send_to_char(ch, "#r| #RMortal:                                                                    #r|\n");
    }
    display_commands(ch, -1, ch->level >= LEVEL_IMMORTAL);
  }
  else
  {
    switch (level)
    {
      case LEVEL_SUPREME:
        send_to_char(ch, "#r| #RSupreme:                                                                   #r|\n");
      break;
      case LEVEL_DEMIGOD:
        send_to_char(ch, "#r| #RDemigod:                                                                   #r|\n");
      break;
      case LEVEL_TRUSTEE:
        send_to_char(ch, "#r| #RTrustee:                                                                   #r|\n");
      break;
      case LEVEL_IMMORTAL:
        send_to_char(ch, "#r| #RImmortal:                                                                  #r|\n");
      break;
      default:
      case -1:
        send_to_char(ch, "#r| #RMortal:                                                                    #r|\n");
      break;
    }
    display_commands(ch, level, 1);
  }
  send_to_char(ch, "#R+#r----------------------------------------------------------------------------#R+\n");
}

/* warning: this function accepts NULL for the character argument for copyover purposes */
void do_disable(CHAR_DATA *ch, char *arguments)
{
  int level;
  int i = 0;
  int x = 0;
  int bestmatch = -1;
  int matchcount = 0;
  char buf[MAX_STRING_LENGTH];

  if (arguments[0] == '\0')
  {
    for (i = 0; cmd_table[i].command[0]; ++i)
    {
      if (cmd_table[i].minlevel > ch->level)
        continue;
      if (cmd_table[i].disabled == 0)
        continue;
      ++x;
      if (x == 1)
        send_to_char(ch, "Disabled commands:\n");
      snprintf(buf, MAX_STRING_LENGTH, "%-15s for levels < %d\n", cmd_table[i].command, cmd_table[i].disabled);
      send_to_char(ch, buf);
    }
    if (x == 0)
      send_to_char(ch, "No commands are currently disabled.\n");
    return;
  }

  one_arg(arguments, buf);
  level = atoi(arguments);

  if (buf[0] == '\0')
  {
    if (ch)
      send_to_char(ch, "Syntax is: disable [command] [level]\n");
    return;
  }

  while (cmd_table[i].command[0] != 0)
  {
    if (ch && cmd_table[i].minlevel > ch->level)
    {
      i++;
      continue;
    }
    x = beginning(cmd_table[i].command, buf);
    if (x == strlen(buf) && x > matchcount)
    {
      bestmatch = i;
      matchcount = x;
    }
    i++;
  }

  if (bestmatch < 0)
  {
    if (ch)
      send_to_char(ch, "Could not find that command.\n");
    else
      syslog("WARNING", "Unable to autodisable command [%s] (404)\n", buf);
    return;
  }
  if (cmd_table[bestmatch].fun == do_disable)
  {
    if (ch)
      send_to_char(ch, "I don't think that would be very wise..\n");
    else
      syslog("WARNING", "Unable to autodisable command [%s] (cannot disable disable)\n", buf);
    return;
  }
  if (ch && level >= ch->level)
  {
    send_to_char(ch, "You can only disable commands for players of lower level than you.\n");
    return;
  }
  if (ch && cmd_table[bestmatch].disabled >= ch->level)
  {
    send_to_char(ch, "Nice try!\n");
    return;
  }
  if (cmd_table[bestmatch].disabled == level && level == 0)
  {
    if (ch)
    {
      snprintf(buf, MAX_STRING_LENGTH, "%s is already enabled.\n", cmd_table[bestmatch].command);
      buf[0] = UPPER(buf[0]);
      send_to_char(ch, buf);
    }
    else
      syslog("WARNING", "Autodisabler: %s already marked as enabled.\n", cmd_table[bestmatch].command, level);
    return;
  }
  if (cmd_table[bestmatch].disabled == level)
  {
    if (ch)
    {
      snprintf(buf, MAX_STRING_LENGTH, "%s is already disabled to level %d.\n", cmd_table[bestmatch].command, level);
      buf[0] = UPPER(buf[0]);
      send_to_char(ch, buf);
    }
    else
      syslog("WARNING", "Autodisabler: %s already marked as disabled to level %d.\n", cmd_table[bestmatch].command, level);
    return;
  }

  for (i = 0; cmd_table[i].command[0]; ++i)
    if (cmd_table[i].fun == cmd_table[bestmatch].fun)
      cmd_table[i].disabled = level;

  if (level == 0)
  {
    syslog("DISABLE", "%s has enabled [%s] command.", ch ? ch->name : "Copyover", cmd_table[bestmatch].command);
    snprintf(buf, MAX_STRING_LENGTH, "%s is now enabled for all levels.\n", cmd_table[bestmatch].command);
  }
  else
  {
    syslog("DISABLE", "%s has disabled [%s] command for levels below %d.", ch ? ch->name : "Copyover", cmd_table[bestmatch].command, level);
    snprintf(buf, MAX_STRING_LENGTH, "%s is now disabled all levels below %d.\n", cmd_table[bestmatch].command, level);
  }
  buf[0] = UPPER(buf[0]);
  if (ch)
    send_to_char(ch, buf);
}

void autodisable(int read, char *cmdlev, FILE *fp)
{
  int i;

  if (read)
    do_disable(NULL, cmdlev);
  else
    for (i = 0; cmd_table[i].command[0]; ++i)
      if (cmd_table[i].disabled > 0)
        fprintf(fp, "%s %d\n", cmd_table[i].command, cmd_table[i].disabled);
}
