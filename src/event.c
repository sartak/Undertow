#include "undertow.h"

int event_ticks(EVENT_DATA *event)
{
  event->owner.ch->acc->chtime[event->owner.ch->acc->cur_char] = ++event->owner.ch->time;
  event->owner.ch->acc->time++;
  event->owner.ch->idle++;
  event->owner.ch->logintime++;

  event->ticks_left = TICKS_PER_SECOND;
  return 0;
}

int event_save(EVENT_DATA *event)
{
  save_account(event->owner.ch->acc);

  event->ticks_left = TICKS_PER_SECOND * 120;
  return 0;
}

int event_linkdeath(EVENT_DATA *event)
{
  DESCRIPTOR_DATA *d = event->owner.ch->d;

  syslog("DISCONNECT", "%s (on %s@%s) has been booted because of prolonged linkdeath.", d->acc->ch->name, d->acc->name, d->ip);
  save_account(d->acc);
  logout_msg(d->acc->ch);

  wipe_character(d->acc->ch);
  wipe_descriptor(d);
  return 1;
}

int event_heal(EVENT_DATA *event)
{
  CHAR_DATA *ch = event->owner.ch;
  int x;

  if (ch->hp != ch->maxhp)
  {
    x = ch->hp + ch->maxhp / number_range(15, 25);
    ch->hp = MIN(ch->maxhp, x);
  }
  if (ch->mana != ch->maxmana)
  {
    x = ch->mana + ch->maxmana / number_range(15, 25);
    ch->mana = MIN(ch->maxmana, x);
  }
  if (ch->move != ch->maxmove)
  {
    x = ch->move + ch->maxmove / number_range(15, 25);
    ch->move = MIN(ch->maxmove, x);
  }

  if (ch->hp == ch->maxhp && ch->mana == ch->maxmana && ch->move == ch->maxmove)
  {
    destroy_all_events(ch, EVENT_HEAL);
    return 1;
  }
  else
  {
    event->ticks_left = TICKS_PER_SECOND * 2;
    return 0;
  }
}

int event_idle(EVENT_DATA *event)
{
  DESCRIPTOR_DATA *d = event->owner.ch->d;
  if (d->acc->ch->level >= LEVEL_IMMORTAL) /* imms don't idle off, they go afk */
  {
    event->ticks_left = IDLE_OUT;
    if (!IS_SET(d->acc->ch->tflags, CTFLAG_AFK))
      do_afk(d->acc->ch, " ");
    else
    {
      char buf[512];
      time_words_long(event->owner.ch->idle, buf);
      strlpre(buf, "\n#cYou have been idle for ", 512);
      strlcat(buf, ".\n", 512);
      send_to_char(event->owner.ch, buf);
    }
    return 0;
  }

  syslog("DISCONNECT", "%s (on %s@%s) has been booted because of prolonged inactivity.", d->acc->ch->name, d->acc->name, d->ip);
  sckoutput(d->socket, "\n\33[1;31mYou have been idle for too long.\n");
  save_account(d->acc);
  logout_msg(d->acc->ch);

  wipe_character(d->acc->ch);
  wipe_descriptor(d);
  return 1;
}

int event_portal(EVENT_DATA *event)
{
  CHAR_DATA *ch = event->owner.ch;
  int args[3];
  args[0] = event->args[0];
  args[1] = event->args[1];
  args[2] = event->args[2];
  destroy_all_events(ch, EVENT_PORTAL);
  create_portal(0, ch, args[0], args[1], args[2], 0, 0, args[2]);
  return 1;
}

int event_cportal(EVENT_DATA *event)
{
  char buf[MAX_STRING_LENGTH];
  OBJECT_DATA *obj = event->owner.obj;

  destroy_obj_events(obj, EVENT_ALL);

  strlcpy(buf, "A #Rred#n portal closes with a pop.\n", MAX_STRING_LENGTH);
  put_sense_area(obj->x, obj->y, obj->plane, SENSE_SIGHT, buf);

  unlink_object(obj);
  free_object(obj);
  return 1;
}

int event_destroy(EVENT_DATA *event)
{
  char buf[MAX_STRING_LENGTH];
  OBJECT_DATA *obj = event->owner.obj;

  destroy_obj_events(obj, EVENT_ALL);

  strlcpy(buf, obj->shortdesc, MAX_STRING_LENGTH);
  buf[0] = UPPER(buf[0]);
  strlcat(buf, " vanishes with a pop.\n", MAX_STRING_LENGTH);
  put_sense_area(obj->x, obj->y, obj->plane, SENSE_SIGHT, buf);

  unlink_object(obj);
  free_object(obj);
  return 1;
}

int event_unmark(EVENT_DATA *event)
{
  char buf[MAX_STRING_LENGTH];
  OBJECT_DATA *obj = event->owner.obj;

  destroy_obj_events(obj, EVENT_UNMARK);

  if (obj->holder != NULL)
  {
    snprintf(buf, MAX_STRING_LENGTH, "The markings on %s fade away.\n", obj->shortdesc);
    put_sense(obj->holder, SENSE_SIGHT, buf);
  }

  REMOVE_BIT(obj->tflags, OTFLAG_MARKED);
  return 1;
}

int event_loginidle(EVENT_DATA *event)
{
  DESCRIPTOR_DATA *d = event->owner.d;

  syslog("DISCONNECT", "%s has been booted because of prolonged inactivity while not logged in.", d->ip);
  sckoutput(d->socket, "\n\33[1;31mYou have been idle for too long.\n");
  close(d->socket);

  if (d->acc->logins > 0) /* aka is this account fully created */
    save_account(d->acc);

  wipe_descriptor(d);
  return 1;
}

int event_changepos(EVENT_DATA *event)
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *ch = event->owner.ch;
  int args[2];
  args[0] = event->args[0]; /* oldpos */
  args[1] = event->args[1]; /* newpos */

  destroy_all_events(ch, EVENT_CHANGEPOS);

  if (ch->position == POS_FIGHTING) /* someone hit him in the middle of sitting or whatever */
    return 1;

  switch (args[0])
  {
    case POS_STANDING:
      switch (args[1])
      {
        case POS_SITTING:
          send_to_char(ch, "\nYou assume the lotus position.\n");
          snprintf(buf, MAX_STRING_LENGTH, "%s sits down.\n", ch->name);
        break;
        case POS_SLEEPING:
          send_to_char(ch, "\nYou lie down and begin sleeping.\n");
          snprintf(buf, MAX_STRING_LENGTH, "%s lies down and begins sleeping.\n", ch->name);
        break;
      }
    break;
    case POS_SITTING:
      switch (args[1])
      {
        case POS_STANDING:
          send_to_char(ch, "\nYou stand up.\n");
          snprintf(buf, MAX_STRING_LENGTH, "%s stands up.\n", ch->name);
        break;
        case POS_SLEEPING:
          send_to_char(ch, "\nYou tip over and start sleeping.\n");
          snprintf(buf, MAX_STRING_LENGTH, "%s tips over and starts sleeping.\n", ch->name);
        break;
      }
    break;
    case POS_SLEEPING:
      switch (args[1])
      {
        case POS_SITTING:
          send_to_char(ch, "\nYou wake up and move to a sitting position.\n");
          snprintf(buf, MAX_STRING_LENGTH, "%s wakes up and moves to a sitting position..\n", ch->name);
        break;
        case POS_STANDING:
          send_to_char(ch, "\nYou wake up and jump to your feet.\n");
          snprintf(buf, MAX_STRING_LENGTH, "%s wakes up and jumps to his feet.\n", ch->name);
        break;
      }
    break;
  }

  put_sense_except(ch, ch->x, ch->y, ch->plane, SENSE_SIGHT, buf);
  ch->position = args[1];
  return 1;
}

/* event_garbagecollect() is in mem.c */

int event_message(EVENT_DATA *event)
{
  send_to_char(event->owner.ch, event->argument);
  destroy_one_event(event->owner.ch, event);
  return 1;
}

int event_delay(EVENT_DATA *event)
{
  strlcat(event->owner.ch->d->output, "\n", MAX_STRING_LENGTH);
  parse(event->owner.ch->d, event->argument);

  if (event->args[1] == 1)
  {
    destroy_one_event(event->owner.ch, event);
    return 1;
  }

  event->args[1]--;
  event->ticks_left = event->args[0];
  return 1; /* this *could* create another event with the command that's parsed, and it's best to be conservative about it */
}

int event_exitportal(EVENT_DATA *event)
{
  CHAR_DATA *ch = event->owner.ch;
  OBJECT_DATA *obj = event->victim.obj;
  int args[3];

  args[0] = event->args[0]; /* newx */
  args[1] = event->args[1]; /* newy */
  args[2] = event->args[2]; /* newplane */

  destroy_all_events(ch, EVENT_EXITPORTAL);

  ch->x = args[0];
  ch->y = args[1];
  ch->plane = args[2];
  update_objects(ch);
  act("\nYou jump out of $v and look around.\n", ch, obj, TO_ACTOR + TYPE_ACT_CHAR + TYPE_VIC_OBJ, SENSE_SIGHT);
  act("$a jumps out of $v.", ch, obj, TO_NOTACTVIC + TYPE_ACT_CHAR + TYPE_VIC_OBJ, SENSE_SIGHT);
  do_look(ch, "");

  if (ch->level == LEVEL_MORTAL && (args[2] == PLANE_NEXUS || args[2] == PLANE_HNEXUS))
  {
    send_to_char(ch, "#R+#r------------------------------------------------------------------------#R+\n");
    send_to_char(ch, "#r| #RWARNING:#Y This plane is meant for characters of greater power than you. #r|\n");
    send_to_char(ch, "#r| #YWe recommend that you return to the newbie plane by typing #CENTER BLUE#Y. #r|\n");
    send_to_char(ch, "#R+#r------------------------------------------------------------------------#R+\n");
  }
  return 1;
}
