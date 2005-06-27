#include "undertow.h"

void do_full(CHAR_DATA *ch, char *arguments)
{
  send_to_char(ch, "If you want to do that, you have to type the full command.\n");
}

void do_quit(CHAR_DATA *ch, char *arguments)
{
  DESCRIPTOR_DATA *d = ch->d;

  syslog("LOGOUT", "%s (on %s@%s) has moved from the game to account menu.", ch->name, ch->accname, d->ip);
  save_account(d->acc);
  logout_msg(ch);

  wipe_character(ch);

  setup_event_d(d, NULL, event_loginidle, "", TICKS_PER_SECOND * 60 * 5, EVENT_LOGINIDLE);
  d->state = STATE_ACCT_MENU;
  show_account_menu(d, 1);
}

void do_config(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  send_to_char(ch, "#R+#r--------------------#1[#2Config#1]#r--------------------#R+\n");

  if (IS_SET(ch->acc->aflags, AFLAG_ANSI))
    send_to_char(ch, "#r| #2ANSI Color:                               #1[#GOn#1] #r|\n");
  else
    send_to_char(ch, "#r| #2ANSI Color:                              #1[#ROff#1] #r|\n");

  if (IS_SET(ch->acc->aflags, AFLAG_ANSI_SPARSE))
    send_to_char(ch, "#r| #2Full ANSI Color:                         #1[#ROff#1] #r|\n");
  else
    send_to_char(ch, "#r| #2Full ANSI Color:                          #1[#GOn#1] #r|\n");

  if (IS_SET(ch->acc->aflags, AFLAG_REPLYLOCK))
    send_to_char(ch, "#r| #2Reply Lock:                               #1[#GOn#1] #r|\n");
  else
    send_to_char(ch, "#r| #2Reply Lock:                              #1[#ROff#1] #r|\n");

  if (!strcmp(ch->acc->email, "Unset"))
    send_to_char(ch, "#r| #2Email Address:                         #1[#RUnset#1] #r|\n");
  else
  {
    strlcpy(buf2, "#1[#GSet#1]", MAX_STRING_LENGTH);
    snprintf(buf, MAX_STRING_LENGTH, "#r| #2Email Address:                  %20s #r|\n", buf2);
    send_to_char(ch, buf);
    snprintf(buf2, MAX_STRING_LENGTH, "#1[#G%s#1]", ch->acc->email);
    snprintf(buf, MAX_STRING_LENGTH, "#r| %52s #r|\n", buf2);
    send_to_char(ch, buf);
  }
  if (IS_SET(ch->acc->aflags, AFLAG_PUBLIC_EMAIL))
    send_to_char(ch, "#r| #2Public Email:                             #1[#GOn#1] #r|\n");
  else
    send_to_char(ch, "#r| #2Public Email:                            #1[#ROff#1] #r|\n");

  send_to_char(ch, "#R+#r------------------------------------------------#R+\n");
}

void do_ansi(CHAR_DATA *ch, char *arguments)
{
  TOGGLE_BIT(ch->acc->aflags, AFLAG_ANSI);
  if (IS_SET(ch->acc->aflags, AFLAG_ANSI))
  {
    if (IS_SET(ch->acc->aflags, AFLAG_ANSI_SPARSE))
      send_to_char(ch, "Your prompts will now be colored. If you want full color, turn Full ANSI Color on.\n");
    else
      send_to_char(ch, "Welcome to the wonderful world of #Rc#Yo#Gl#Co#Mr#n!\n");
  }
  else
    send_to_char(ch, "ANSI color turned off.\n");
}

void do_fullansi(CHAR_DATA *ch, char *arguments)
{
  TOGGLE_BIT(ch->acc->aflags, AFLAG_ANSI_SPARSE);
  if (!IS_SET(ch->acc->aflags, AFLAG_ANSI))
    send_to_char(ch, "You would probably notice a subsequent color level change if you had color on.\n");
  else if (IS_SET(ch->acc->aflags, AFLAG_ANSI_SPARSE))
    send_to_char(ch, "You will now only see colored prompts.\n");
  else
    send_to_char(ch, "You will now see full color.\n");
}

void do_replylock(CHAR_DATA *ch, char *arguments)
{
  TOGGLE_BIT(ch->acc->aflags, AFLAG_REPLYLOCK);
  if (IS_SET(ch->acc->aflags, AFLAG_REPLYLOCK))
    send_to_char(ch, "You will now reply only to the last person to whom you sent a tell.\n");
  else
    send_to_char(ch, "You will now reply to the person who most recently sent you a tell.\n");
}

void do_email(CHAR_DATA *ch, char *arguments)
{
  char arg[MAX_STRING_LENGTH];

  one_arg(arguments, arg);

  if (!stricmp(arg, "public"))
  {
    TOGGLE_BIT(ch->acc->aflags, AFLAG_PUBLIC_EMAIL);
    if (IS_SET(ch->acc->aflags, AFLAG_PUBLIC_EMAIL))
      send_to_char(ch, "Your email address is now public.\n");
    else
      send_to_char(ch, "Your email address is now private.\n");
  }
  else if (!stricmp(arg, "unset"))
  {
    strlcpy(ch->acc->email, "Unset", 128);
    send_to_char(ch, "Your email address is now unset.\n");
  }
  else if (!stricmp(arg, "set"))
  {
    if (arguments[0] == '\0')
    {
      strlcpy(ch->acc->email, "Unset", 128);
      send_to_char(ch, "Your email address is now unset.\n");
    }
    else if (check_string(arguments, -1, 47, 0) == 2)
    {
        send_to_char(ch, "Your email can contain at most 47 characters.\n");
    }
    else
    {
      replace(arguments, arg, "%", "", MAX_STRING_LENGTH);
      replace(arg, arguments, "~", "-", MAX_STRING_LENGTH);
      strlcpy(ch->acc->email, arguments, 128);
      snprintf(arg, MAX_STRING_LENGTH, "Your email address is now %s.\n", arguments);
      send_to_char(ch, arg);
    }
  }
  else
    send_to_char(ch, "The proper syntax is:\n  email public\n  email unset\n  email set [new address]\n");
}

void do_timer(CHAR_DATA *ch, char *arguments)
{
  char arg[MAX_STRING_LENGTH];
  int ticks;
  EVENT_DATA *event;

  one_arg(arguments, arg);
  ticks = TICKS_PER_SECOND * atoi(arg);

  if (strlen(arguments) > 500)
  {
    send_to_char(ch, "That message is too long. Maximum is 500 characters, which should be more than plenty, jerk!\n");
    return;
  }
  if (strlen(arguments) == 0 || ticks <= 0)
  {
    send_to_char(ch, "Syntax is: timer [seconds] [message]\n");
    return;
  }

  event = setup_event(ch, NULL, event_message, arguments, ticks, EVENT_MESSAGE);
  strlcpy(event->argument, arguments, 512);
  strlpre(event->argument, "\n", 512);
  strlcat(event->argument, "\n", 512);
  send_to_char(ch, "Timer set.\n");
}

void do_alarm(CHAR_DATA *ch, char *arguments)
{
  char arg[MAX_STRING_LENGTH];
  int ticks;
  EVENT_DATA *event;

  one_arg(arguments, arg);
  ticks = TICKS_PER_SECOND * atoi(arg);

  if (strlen(arguments) > 500)
  {
    send_to_char(ch, "That message is too long. Maximum is 500 characters, which should be more than plenty, jerk!\n");
    return;
  }
  if (strlen(arguments) == 0 || ticks <= 0)
  {
    send_to_char(ch, "Syntax is: alarm [seconds] [message]\n");
    return;
  }

  event = setup_event(ch, NULL, event_message, arguments, ticks, EVENT_MESSAGE);
  strlcpy(event->argument, arguments, 512);
  strlpre(event->argument, "\n\a", 512);
  strlcat(event->argument, "\n", 512);
  send_to_char(ch, "Alarm set.\n");
}

void do_save(CHAR_DATA *ch, char *arguments)
{
  save_account(ch->acc);
  send_to_char(ch, "You have been saved.\n");
}

int stat_cost(int cur)
{
  if (cur == 0)
    return 1;
  return cur;
}

void do_train(CHAR_DATA *ch, char *arguments)
{
  char arg[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int stat;
  int amount;
  int x;
  int gained = 0;
  one_arg(arguments, arg);

  if (!stricmp(arg, "hp"))
    stat = 1, x = ch->maxhp;
  else if (!stricmp(arg, "mana"))
    stat = 2, x = ch->maxmana;
  else if (!stricmp(arg, "move"))
    stat = 3, x = ch->maxmove;
  else if (!stricmp(arg, "radiation") || !stricmp(arg, "tolerance"))
    stat = 4, x = ch->resistance;
  else
  {
    send_to_char(ch, "\n#R+#r-------------------#1[#2Training#1]#r-------------------#R+\n");
    snprintf(arg, MAX_STRING_LENGTH, "#1[#2%d#1]", stat_cost(ch->maxhp));
    snprintf(buf, MAX_STRING_LENGTH, "#r| #2HP:   %-46s #r|\n", arg);
    send_to_char(ch, buf);
    snprintf(arg, MAX_STRING_LENGTH, "#1[#2%d#1]", stat_cost(ch->maxmana));
    snprintf(buf, MAX_STRING_LENGTH, "#r| #2Mana: %-46s #r|\n", arg);
    send_to_char(ch, buf);
    snprintf(arg, MAX_STRING_LENGTH, "#1[#2%d#1]", stat_cost(ch->maxmove));
    snprintf(buf, MAX_STRING_LENGTH, "#r| #2Move: %-46s #r|\n", arg);
    send_to_char(ch, buf);
    send_to_char(ch, "#R+#r------------------------------------------------#R+#n\n");
    snprintf(arg, MAX_STRING_LENGTH, "#1[#2%d#1]", 10000*stat_cost(ch->resistance));
    snprintf(buf, MAX_STRING_LENGTH, "#r| #2Radiation Tolerance: %-31s #r|\n", arg);
    send_to_char(ch, buf);
    send_to_char(ch, "#R+#r------------------------------------------------#R+#n\n");
    return;
  }

  one_arg(arguments, arg);
  amount = atoi(arg);
  if (arg[0] == 0)
    amount = 1;
  if (amount <= 0)
    amount = 0;
  if (!strcmp(arg, "all"))
    amount = -1;
  if (amount == 0)
  {
    send_to_char(ch, "How much do you want to train?\n");
    return;
  }

  if (stat == 4)
  {
    while (ch->primal > 10000*stat_cost(x))
    {
      if (amount > 0 && gained >= amount)
        break;
      ch->primal -= 10000*stat_cost(x);
      ++x;
      ++gained;
    }
    ch->resistance = x;
    snprintf(buf, MAX_STRING_LENGTH, "Your radiation tolerance increases by %d.\n", gained);
  }
  else
  {
    while (ch->primal > stat_cost(x))
    {
      if (amount > 0 && gained >= amount)
        break;
      ch->primal -= stat_cost(x);
      ++x;
      ++gained;
    }

    switch (stat)
    {
      case 1:
        ch->maxhp = x;
        snprintf(buf, MAX_STRING_LENGTH, "You gain %d HP.\n", gained);
        break;
      case 2:
        ch->maxmana = x;
        snprintf(buf, MAX_STRING_LENGTH, "You gain %d mana.\n", gained);
        break;
      case 3:
        ch->maxmove = x;
        snprintf(buf, MAX_STRING_LENGTH, "You gain %d move.\n", gained);
        break;
    }
  }

  if (gained == 0)
    send_to_char(ch, "You don't have enough primal.\n");
  else
  {
    send_to_char(ch, buf);
    setup_event(ch, NULL, event_heal, "", TICKS_PER_SECOND * 3, EVENT_HEAL);
  }
}

void do_prompt(CHAR_DATA *ch, char *arguments)
{
  int i, tilde = 0;
  if (arguments[0] == 0)
  {
    strlcpy(ch->prompt, "#1[#2%pP#1] #1[#2%h#1/#2%HH %m#1/#2%MM %v#1/#2%VV#1]#n> ", 256);
    send_to_char(ch, "You now have the default prompt.\n");
    return;
  }

  if (strlen(arguments) > 200)
  {
    send_to_char(ch, "There is a 200 character max on prompts.\n");
    return;
  }

  for (i = 0; i < strlen(arguments); ++i)
    if (arguments[i] == '~')
    {
      arguments[i] = '-';
      tilde++;
    }

  if (tilde)
    send_to_char(ch, "Warning, you cannot have a tilde in your prompt. Converting it to a hyphen.\n");

  strlcpy(ch->prompt, arguments, 256);
  send_to_char(ch, "Enjoy your new prompt.\n");
}

void do_suggest(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  FILE *fp;
  if (arguments[0] == 0)
  {
    send_to_char(ch, "What do you want to suggest?\n");
    return;
  }
  strlcpy(buf, ch->name, MAX_STRING_LENGTH);
  strlcat(buf, " suggests: ", MAX_STRING_LENGTH);
  strlcat(buf, arguments, MAX_STRING_LENGTH);
  syslog("SUGGEST", buf);
  fp = fopen("data/suggest.txt", "a");
  strlcat(buf, "\n", MAX_STRING_LENGTH);
  fputs(buf, fp);
  fflush(fp);
  fclose(fp);
}

void do_typo(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  FILE *fp;
  if (arguments[0] == 0)
  {
    send_to_char(ch, "What typo do you want to report?\n");
    return;
  }
  strlcpy(buf, ch->name, MAX_STRING_LENGTH);
  strlcat(buf, " reports typo: ", MAX_STRING_LENGTH);
  strlcat(buf, arguments, MAX_STRING_LENGTH);
  syslog("TYPO", buf);
  fp = fopen("data/typo.txt", "a");
  strlcat(buf, "\n", MAX_STRING_LENGTH);
  fputs(buf, fp);
  fflush(fp);
  fclose(fp);
}

void do_bug(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  FILE *fp;
  if (arguments[0] == 0)
  {
    send_to_char(ch, "What bug do you want to report?\n");
    return;
  }
  strlcpy(buf, ch->name, MAX_STRING_LENGTH);
  strlcat(buf, " reports bug: ", MAX_STRING_LENGTH);
  strlcat(buf, arguments, MAX_STRING_LENGTH);
  syslog("BUGREPORT", buf);
  fp = fopen("data/bugreport.txt", "a");
  strlcat(buf, "\n", MAX_STRING_LENGTH);
  fputs(buf, fp);
  fflush(fp);
  fclose(fp);
}

void do_suggestions(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  FILE *fp;
  buf[0] = 0;
  fp = fopen("data/suggest.txt", "r");
  if (fp)
  {
    buf[fread(buf, 1, MAX_STRING_LENGTH, fp)] = 0;
    fclose(fp);
  }
  if (!fp || strlen(buf) == 0)
    send_to_char(ch, "There are no suggestions.\n");
  else
    send_to_char(ch, buf);
}

void do_typos(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  FILE *fp;
  buf[0] = 0;
  fp = fopen("data/typo.txt", "r");
  if (fp)
  {
    buf[fread(buf, 1, MAX_STRING_LENGTH, fp)] = 0;
    fclose(fp);
  }
  if (!fp || strlen(buf) == 0)
    send_to_char(ch, "There are no typo reports.\n");
  else
    send_to_char(ch, buf);
}

void do_bugs(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  FILE *fp;
  buf[0] = 0;
  fp = fopen("data/bugreport.txt", "r");
  if (fp)
  {
    buf[fread(buf, 1, MAX_STRING_LENGTH, fp)] = 0;
    fclose(fp);
  }
  if (!fp || strlen(buf) == 0)
    send_to_char(ch, "There are no bug reports.\n");
  else
    send_to_char(ch, buf);
}

void do_alias(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[32];
  ALIAS_DATA *alias;
  ALIAS_DATA *tempalias;
  CHAR_DATA *vic = ch;
  int aliascount = 0;
  one_arg(arguments, buf);

  if (ch->level < LEVEL_IMMORTAL || buf[0] == '\0')
    vic = ch;
  else if (stricmp(buf, "add") && stricmp(buf, "del") && stricmp(buf, "clear"))
  {
    vic = find_char_world(ch, buf);

    if (vic == NULL)
    {
      send_to_char(ch, "I'm afraid I cannot find that player.\n");
      return;
    }
  }

  if (buf[0] == '\0' || (stricmp(buf, "add") && stricmp(buf, "del") && stricmp(buf, "clear")))
  {
    if (ch != vic)
    {
      send_to_char(ch, vic->name);
      send_to_char(ch, "\'s aliases:\n");
    }
    send_to_char(ch, "#R+#r----------------#1[#2Aliases#1]#r----------------#R+\n");
    if (vic->alias == NULL)
      send_to_char(ch, "#r|  #3None!                                  #r|\n");
    else
      for (alias = vic->alias; alias; alias = alias->next_local)
      {
        snprintf(buf2, 32, "%s#1:", alias->in);
        snprintf(buf, MAX_STRING_LENGTH, "#r| #R%-13s #2%-27s #r|\n", buf2, alias->out);
        send_to_char(ch, buf);
      }
    send_to_char(ch, "#R+#r-----------------------------------------#R+#n\n");
    send_to_char(ch, "Add new aliases with: alias add [command] [result]\n");
    send_to_char(ch, "Remove aliases with: alias del [command]\n");
    send_to_char(ch, "Remove all aliases with: alias clear\n");
    if (ch->level >= LEVEL_IMMORTAL)
      send_to_char(ch, "Look at someone else's aliases: alias [character]\n");
    return;
  }

  if (!stricmp(buf, "clear"))
  {
    alias = ch->alias;
    if (alias == NULL)
    {
      send_to_char(ch, "You have no aliases to clear.\n");
      return;
    }
    while (alias)
    {
      tempalias = alias->next_local;
      unlink_alias(alias);
      free_alias(alias);
      alias = tempalias;
    }
    ch->alias = NULL;
    send_to_char(ch, "All aliases cleared.\n");
  }
  else if (!stricmp(buf, "add"))
  {
     for (alias = ch->alias; alias; alias = alias->next_local)
       ++aliascount;
     if (aliascount >= 20)
     {
       send_to_char(ch, "You can only have twenty aliases.\n");
       return;
     }
    one_arg(arguments, buf);
    if (arguments[0] == 0)
    {
      send_to_char(ch, "The syntax is: alias add [command] [result]\n");
      return;
    }

    if (strlen(buf) > 10)
    {
      send_to_char(ch, "The maximum length for an alias command is ten characters.\n");
      return;
    }

    if (strlen(arguments) > 25)
    {
      send_to_char(ch, "The maximum length for an alias expansion is twenty-five characters.\n");
      return;
    }

    to_lower(buf);
    if (!strcmp(buf, "alias"))
    {
      send_to_char(ch, "I would recommend against overriding the alias command.\n");
      return;
    }
    for (alias = ch->alias; alias; alias = alias->next_local)
      if (!strcmp(alias->in, buf))
      {
        send_to_char(ch, "You already have an alias set for that command.\n");
        return;
      }

    alias = new_alias();
    link_alias(alias);

    if (ch->alias == NULL)
      ch->alias = alias;
    else
    {
      for (tempalias = ch->alias; tempalias->next_local; tempalias = tempalias->next_local);
      tempalias->next_local = alias;
    }
    strlcpy(alias->in, buf, 16);
    strlcpy(alias->out, arguments, 128);
    send_to_char(ch, "New alias set.\n");
    return;
  }
  else
  {
    one_arg(arguments, buf);
    if (buf[0] == 0)
    {
      send_to_char(ch, "The syntax is: alias del [command]\n");
      return;
    }

    tempalias = ch->alias;

    for (alias = ch->alias; alias; alias = alias->next_local)
    {
      if (!stricmp(alias->in, buf))
      {
        if (alias == ch->alias)
          ch->alias = ch->alias->next_local;
        else
          tempalias->next_local = alias->next_local;
        unlink_alias(alias);
        free_alias(alias);
        send_to_char(ch, "Alias removed.\n");
        return;
      }
      tempalias = alias;
    }
    send_to_char(ch, "Unable to find that alias.\n");
    return;
  }
}

void do_afk(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  if (IS_SET(ch->tflags, CTFLAG_AFK))
    snprintf(buf, MAX_STRING_LENGTH, "%s has returned to his keyboard.\n", ch->name);
  else
    snprintf(buf, MAX_STRING_LENGTH, "%s is now away from keyboard.\n", ch->name);
  info(ch, buf, !strcmp(arguments, " ") ? 1:0);
  TOGGLE_BIT(ch->tflags, CTFLAG_AFK);
}
