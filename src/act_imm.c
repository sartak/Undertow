#include "undertow.h"

void do_flush(CHAR_DATA *ch, char *arguments)
{
  char arg[MAX_STRING_LENGTH];

  one_arg(arguments, arg);
  if (!stricmp(arg, "help"))
  {
    free_all_helps();
    load_all_helps();
    send_to_char(ch, "The help cache is now clean.\n");
    return;
  }
  if (!stricmp(arg, "lastchat"))
  {
    int i;
    for (i = 0; i < 10; ++i)
    {
      lastchat[i][0] = 0;
      lastimm[i][0] = 0;
      lasttru[i][0] = 0;
      lastdem[i][0] = 0;
      lastsup[i][0] = 0;
    }
    send_to_char(ch, "The lastchat cache is now clean.\n");
    return;
  }
  if (!stricmp(arg, "ip"))
  {
    system("rm -f data/addresses/*.ip");
    send_to_char(ch, "The IP cache is now clean.\n");
    return;
  }
  if (!stricmp(arg, "suggestions"))
  {
    system("rm -f data/suggest.txt");
    send_to_char(ch, "The suggestions cache is now clean.\n");
    return;
  }
  if (!stricmp(arg, "typos"))
  {
    system("rm -f data/typo.txt");
    send_to_char(ch, "The typo reports cache is now clean.\n");
    return;
  }
  if (!stricmp(arg, "bugs"))
  {
    system("rm -f data/bugreport.txt");
    send_to_char(ch, "The bug reports cache is now clean.\n");
    return;
  }

  send_to_char(ch, "What do you want to flush: help lastchat ip suggestions typos bugs\n");
}

void do_hlist(CHAR_DATA *ch, char *arguments)
{
  send_hlist_to_char(ch);
}

void do_relevel(CHAR_DATA *ch, char *arguments)
{
  if (!strcmp(ch->accname, "Sartak"))
    ch->level = LEVEL_SUPREME;
  else if (!strcmp(ch->accname, "Dragnox"))
    ch->level = LEVEL_DEMIGOD;
  else if (!strcmp(ch->accname, "Atreides"))
    ch->level = LEVEL_DEMIGOD;
  else if (!strcmp(ch->accname, "Jasonmay"))
    ch->level = LEVEL_DEMIGOD;
  else
  {
    send_to_char(ch, "Get lost, peon!\n");
    return;
  }

  ch->acc->status[ch->acc->cur_char] = CHSTATUS_IMMORTAL;
  REMOVE_BIT(ch->cflags, CFLAG_HARDCORE);
  send_to_char(ch, "Welcome.\n");
}

void do_tout(CHAR_DATA *ch, char *arguments)
{
  DESCRIPTOR_DATA *d;
  if (arguments[0] == 0)
  {
    send_to_char(ch, "Which character do you want to tout?\n");
    return;
  }
  for (d = dhead; d; d = d->next)
    if (d->acc->ch && !stricmp(d->acc->ch->name, arguments))
    {
      if (d->acc->ch->level > ch->level)
      {
         syslog("TOUT", "%s has attempted to tout their superior %s.", ch->name, d->acc->ch->name);
         send_to_char(ch, "I don't think he would appreciate that very much.\n");
         return;
      }

      syslog("TOUT", "%s has touted player %s (on %s).", ch->name, d->acc->ch->name, d->acc->name);
      send_to_char(ch, "He has been touted.\n");

      save_account(d->acc);
      sckoutput(d->socket, "\nYou ARE the weakest link. Goodbye!\n");
      logout_msg(d->acc->ch);

      wipe_character(d->acc->ch);
      wipe_descriptor(d);
      return;
    }

  send_to_char(ch, "Unable to find that player.\n");
}

void do_delete(CHAR_DATA *ch, char *arguments)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *xch;
  ACCOUNT_DATA *acc;
  int i;
  int x = 0;
  if (arguments[0] == 0)
  {
    send_to_char(ch, "Which character do you want to delete?\n");
    return;
  }
  for (d = dhead; d; d = d->next)
    if (d->acc->ch && !stricmp(d->acc->ch->name, arguments))
    {
      if (d->acc->ch->level > ch->level)
      {
        syslog("DELETE", "%s has attempted to delete their superior %s.", ch->name, d->acc->ch->name);
        send_to_char(ch, "I don't think he would appreciate that very much.\n");
        return;
      }
      SET_BIT(d->acc->ch->cflags, CFLAG_DELETED);
      d->acc->status[d->acc->cur_char] = CHSTATUS_DELETED;
      save_account(d->acc);
      syslog("DELETE", "%s has deleted online player %s (on %s).", ch->name, d->acc->ch->name, d->acc->name);
      send_to_char(ch, "He is now deleted.\n");
      return;
    }

  xch = load_character(NULL, arguments);
  if (xch->name[0] == 0)
    send_to_char(ch, "Unable to find that player.\n");
  else
  {
    if (xch->level > ch->level)
    {
      syslog("DELETE", "%s has attempted to delete their superior %s.", ch->name, xch->name);
      send_to_char(ch, "I don't think he would appreciate that very much.\n");
    }
    else
    {
      syslog("DELETE", "%s has deleted offline player %s (on %s).", ch->name, xch->name, xch->accname);
      send_to_char(ch, "He is now deleted.\n");
      SET_BIT(xch->cflags, CFLAG_DELETED);
      save_character(xch);
      acc = new_account();
      load_account(xch->accname, acc);
      for (d = dhead; d; d = d->next)
        if (d->acc && !stricmp(d->acc->name, acc->name))
        {
          for (i = 0; i < d->acc->charcount; ++i)
            if (!stricmp(d->acc->character[i], xch->name))
              {
                d->acc->status[i] = CHSTATUS_DELETED;
                x = 1;
                save_account(d->acc);
              }
          break;
        }

      if (x == 0)
        for (i = 0; i < acc->charcount; ++i)
          if (!stricmp(acc->character[i], xch->name))
            acc->status[i] = CHSTATUS_DELETED;
      save_account(acc);
      free_account(acc);
    }
    unlink_character(xch);
  }
  free_character(xch);
}

void do_undelete(CHAR_DATA *ch, char *arguments)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *xch;
  ACCOUNT_DATA *acc;
  int i;
  int x = 0;
  if (arguments[0] == 0)
  {
    send_to_char(ch, "Which character do you want to undelete?\n");
    return;
  }

  xch = load_character(NULL, arguments);
  if (xch->name[0] == 0)
    send_to_char(ch, "Unable to find that player.\n");
  else
  {
    syslog("DELETE", "%s has UNdeleted offline player %s (on %s).", ch->name, xch->name, xch->accname);
    send_to_char(ch, "He is now undeleted.\n");
    REMOVE_BIT(xch->cflags, CFLAG_DELETED);
    save_character(xch);
    acc = new_account();
    load_account(xch->accname, acc);
    for (d = dhead; d; d = d->next)
      if (d->acc && !stricmp(d->acc->name, acc->name))
      {
        for (i = 0; i < d->acc->charcount; ++i)
          if (!stricmp(d->acc->character[i], xch->name))
          {
            if (xch->level >= 6)
              d->acc->status[i] = CHSTATUS_IMMORTAL;
            else if (IS_SET(xch->cflags, CFLAG_HARDCORE))
              d->acc->status[i] = CHSTATUS_HARDCORE;
            else
              d->acc->status[i] = CHSTATUS_NORMAL;
            x = 1;
            save_account(d->acc);
          }
        break;
      }

    if (x == 0)
      for (i = 0; i < acc->charcount; ++i)
        if (!stricmp(acc->character[i], xch->name))
        {
          if (xch->level >= 6)
            acc->status[i] = CHSTATUS_IMMORTAL;
          else if (IS_SET(xch->cflags, CFLAG_HARDCORE))
            acc->status[i] = CHSTATUS_HARDCORE;
          else
            acc->status[i] = CHSTATUS_NORMAL;
        }
    save_account(acc);
    free_account(acc);
  }
  unlink_character(xch);
  free_character(xch);
}

void do_events(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  char owner[128];
  char victim[128];
  char type[32];
  EVENT_DATA *event;
  int e = 0;
  int page = 0;

  if (eventhead == NULL)
  {
    send_to_char(ch, "There are no scheduled events.\n");
    return;
  }

  page = atoi(arguments);
  for (event = eventhead; event; event = event->next)
    ++e;
  if (page > 0 && page * 20 >= e)
  {
    if (e <= 20)
      strlcpy(buf, "Page out of range. The only valid page is zero.\n", MAX_STRING_LENGTH);
    else
      snprintf(buf, MAX_STRING_LENGTH, "Page out of range. The only valid pages are [0, %d]\n", e / 20);
    send_to_char(ch, buf);
    return;
  }
  send_to_char(ch, "#R+#r-------------------------#1[#2Events#1]#r-------------------------#R+\n");
  send_to_char(ch, "#r| #2Owner            #1Victim           #2Type            #1Ticks  #r|\n");
  send_to_char(ch, "#R+#r----------------------------------------------------------#R+\n");

  e = 0;
  for (event = eventhead; event; event = event->next)
  {
    ++e;
    if (e < (page+1) * 20 || e >= page * 20)
    {
    strlcpy(owner, "#RUndertow", 128);
    strlcpy(victim, "#1N/A", 128);
    snprintf(type, 32, "(%d)", event->type);

    switch (event->type)
    {
      default:
        strlcpy(owner, "#1Unknown", 128);
        strlcpy(victim, "#1Unknown", 128);
      break;
      case EVENT_GARBAGECOLLECT:
        strlcpy(type, "GarbageCollect", 32);
      break;

      case EVENT_TICK:
        snprintf(owner, 128, "%s%s", event->owner.ch == ch ? "#C" : "#2", event->owner.ch->name);
        strlcpy(type, "Tick", 32);
      break;
      case EVENT_SAVE:
        snprintf(owner, 128, "%s%s", event->owner.ch == ch ? "#C" : "#2", event->owner.ch->name);
        strlcpy(type, "Save", 32);
      break;
      case EVENT_LINKDEAD:
        snprintf(owner, 128, "%s%s", event->owner.ch == ch ? "#C" : "#2", event->owner.ch->name);
        strlcpy(type, "Linkdead", 32);
      break;
      case EVENT_HEAL:
        snprintf(owner, 128, "%s%s", event->owner.ch == ch ? "#C" : "#2", event->owner.ch->name);
        strlcpy(type, "Heal", 32);
      break;
      case EVENT_IDLE:
        snprintf(owner, 128, "%s%s", event->owner.ch == ch ? "#C" : "#2", event->owner.ch->name);
        strlcpy(type, "Idle", 32);
      break;
      case EVENT_PORTAL:
        snprintf(owner, 128, "%s%s", event->owner.ch == ch ? "#C" : "#2", event->owner.ch->name);
        strlcpy(type, "MakePortal", 32);
      break;
      case EVENT_CHANGEPOS:
        snprintf(owner, 128, "%s%s", event->owner.ch == ch ? "#C" : "#2", event->owner.ch->name);
        strlcpy(type, "ChangePos", 32);
      break;
      case EVENT_MESSAGE:
        snprintf(owner, 128, "%s%s", event->owner.ch == ch ? "#C" : "#2", event->owner.ch->name);
        strlcpy(type, "Message", 32);
      break;
      case EVENT_DELAY:
        snprintf(owner, 128, "%s%s", event->owner.ch == ch ? "#C" : "#2", event->owner.ch->name);
        strlcpy(type, "Delay", 32);
      break;
      case EVENT_EXITPORTAL:
        snprintf(owner, 128, "%s%s", event->owner.ch == ch ? "#C" : "#2", event->owner.ch->name);
        strlcpy(type, "ExitPortal", 32);
        decolorize(event->victim.obj->shortdesc, buf);
        snprintf(victim, 128, "#g%s", buf);
      break;

      case EVENT_CPORTAL:
        decolorize(event->owner.obj->shortdesc, buf);
        snprintf(owner, 128, "#g%s", buf);
        strlcpy(type, "ClosePortal", 32);
      break;
      case EVENT_DESTROY:
        decolorize(event->owner.obj->shortdesc, buf);
        snprintf(owner, 128, "#g%s", buf);
        strlcpy(type, "Destroy", 32);
      break;
      case EVENT_UNMARK:
        decolorize(event->owner.obj->shortdesc, buf);
        snprintf(owner, 128, "#g%s", buf);
        strlcpy(type, "Unmark", 32);
      break;

      case EVENT_LOGINIDLE:
        strlcpy(type, "LoginIdle", 32);
        snprintf(owner, 128, "#m%s", event->owner.d->ip);
      break;
    }

    snprintf(buf, MAX_STRING_LENGTH, "#r| %-18.18s #1%-18.18s #2%-15s #1%-6d #r|\n", owner, victim, type, event->ticks_left);
    send_to_char(ch, buf);
  }
  }

  send_to_char(ch, "#R+#r----------------------------------------------------------#R+\n");
}

void do_memory(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  char bytes[32];

  send_to_char(ch, "#R+#r----------------#1[#2Memory#1]#r----------------#R+\n");
  send_to_char(ch, "#r| #2Type         #1 Bytes #2Alloc #1Free #2Recycle #r|\n");
  send_to_char(ch, "#R+#r----------------------------------------#R+\n");

  if (mem.bytes_allocated > 100000)
    snprintf(bytes,   32, "%ldK", mem.bytes_allocated / 1024);
  else
    snprintf(bytes,   32, "%ld", mem.bytes_allocated);
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Total        #1%6s #2%5ld #1%4ld #2%7ld #r|\n", bytes, mem.d_allocated + mem.ch_allocated + mem.acc_allocated + mem.event_allocated + mem.obj_allocated + mem.alias_allocated, mem.d_free + mem.ch_free + mem.acc_free + mem.event_free + mem.obj_free + mem.alias_free, mem.d_recycled + mem.ch_recycled + mem.acc_recycled + mem.event_recycled + mem.obj_recycled + mem.alias_recycled);
  send_to_char(ch, buf);

  if (sizeof(DESCRIPTOR_DATA) * mem.d_allocated > 100000)
    snprintf(bytes,   32, "%ldK", mem.d_allocated * sizeof(DESCRIPTOR_DATA) / 1024);
  else
    snprintf(bytes,   32, "%ld", mem.d_allocated * sizeof(DESCRIPTOR_DATA));
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Descriptor   #1%6s #2%5ld #1%4ld #2%7ld #r|\n", bytes, mem.d_allocated, mem.d_free, mem.d_recycled);
  send_to_char(ch, buf);

  if (sizeof(CHAR_DATA) * mem.ch_allocated > 100000)
    snprintf(bytes,   32, "%ldK", mem.ch_allocated * sizeof(CHAR_DATA) / 1024);
  else
    snprintf(bytes,   32, "%ld", mem.ch_allocated * sizeof(CHAR_DATA));
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Character    #1%6s #2%5ld #1%4ld #2%7ld #r|\n", bytes, mem.ch_allocated, mem.ch_free, mem.ch_recycled);
  send_to_char(ch, buf);

  if (sizeof(ACCOUNT_DATA) * mem.acc_allocated > 100000)
    snprintf(bytes,   32, "%ldK", mem.acc_allocated * sizeof(ACCOUNT_DATA) / 1024);
  else
    snprintf(bytes,   32, "%ld", mem.acc_allocated * sizeof(ACCOUNT_DATA));
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Account      #1%6s #2%5ld #1%4ld #2%7ld #r|\n", bytes, mem.acc_allocated, mem.acc_free, mem.acc_recycled);
  send_to_char(ch, buf);

  if (sizeof(EVENT_DATA) * mem.event_allocated > 100000)
    snprintf(bytes,   32, "%ldK", mem.event_allocated * sizeof(EVENT_DATA) / 1024);
  else
    snprintf(bytes,   32, "%ld", mem.event_allocated * sizeof(EVENT_DATA));
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Event        #1%6s #2%5ld #1%4ld #2%7ld #r|\n", bytes, mem.event_allocated, mem.event_free, mem.event_recycled);
  send_to_char(ch, buf);

  if (sizeof(OBJECT_DATA) * mem.obj_allocated > 100000)
    snprintf(bytes,   32, "%ldK", mem.obj_allocated * sizeof(OBJECT_DATA) / 1024);
  else
    snprintf(bytes,   32, "%ld", mem.obj_allocated * sizeof(OBJECT_DATA));
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Object       #1%6s #2%5ld #1%4ld #2%7ld #r|\n", bytes, mem.obj_allocated, mem.obj_free, mem.obj_recycled);
  send_to_char(ch, buf);

  if (sizeof(ALIAS_DATA) * mem.alias_allocated > 100000)
    snprintf(bytes,   32, "%ldK", mem.alias_allocated * sizeof(ALIAS_DATA) / 1024);
  else
    snprintf(bytes,   32, "%ld", mem.alias_allocated * sizeof(ALIAS_DATA));
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Alias        #1%6s #2%5ld #1%4ld #2%7ld #r|\n", bytes, mem.alias_allocated, mem.alias_free, mem.alias_recycled);
  send_to_char(ch, buf);

  send_to_char(ch, "#R+#r----------------------------------------#R+\n");
}

void do_copyover(CHAR_DATA *ch, char *arguments)
{
  syslog("COPYOVER", "%s requested the pending copyover.", ch->name);
  ch->idle = 0;
  initiate_copyover();
}

void do_shutdown(CHAR_DATA *ch, char *arguments)
{
  DESCRIPTOR_DATA *d;

  syslog("SHUTDOWN", "%s requested the pending shutdown.", ch->name);
  for (d = dhead; d; d = d->next)
    if (d->acc && d->state == STATE_PLAYING)
    {
      save_account(d->acc);
      send_to_descriptor(d, "\nThe MUD is shutting down. We apologize for the inconvenience.\n");
    }
  output(); /* I wouldn't recommend calling this anywhere else. Bad things could happen. */
  for (d = dhead; d; d = d->next)
    close (d->socket);
  RUNNING = 0;
}

void do_delay(CHAR_DATA *ch, char *arguments)
{
  char arg[MAX_STRING_LENGTH];
  int ticks;
  int repetitions;
  EVENT_DATA *event;

  if (!stricmp(arguments, "clear"))
  {
    destroy_all_events(ch, EVENT_DELAY);
    send_to_char(ch, "Delays cleared.\n");
    return;
  }

  one_arg(arguments, arg);
  repetitions = atoi(arg);
  one_arg(arguments, arg);
  ticks = TICKS_PER_SECOND * atoi(arg);

  if (strlen(arguments) > 500)
  {
    send_to_char(ch, "That command is too long. Maximum is 500 characters, which should be more than plenty, jerk!\n");
    return;
  }
  if (strlen(arguments) == 0 || ticks <= 0 || repetitions < 0)
  {
    send_to_char(ch, "Syntax is:\n  delay [repetitions] [seconds] [command]\n  delay clear\n");
    return;
  }

  event = setup_event(ch, NULL, event_delay, arguments, ticks, EVENT_DELAY);
  event->args[0] = ticks;
  if (repetitions == 0)
    event->args[1] = -1;
  else
    event->args[1] = repetitions;

  send_to_char(ch, "Delay set.\n");
}
