#include "undertow.h"

void update()
{
  EVENT_DATA *event = eventhead;
  EVENT_DATA *eventnext;
  int changed = 0;

  while (event)
  {
    eventnext = event->next;
    if (event->touched)
    {
      event = eventnext;
      continue;
    }

    event->touched++;

    if (--(event->ticks_left) == 0)
      changed = event->fun(event);

    if (!changed)
      event = event->next;
    else
    {
      update();
      return;
    }
  }

  for (event = eventhead; event; event = event->next)
    event->touched = 0;
}

void setup_char_events(CHAR_DATA *ch)
{
  setup_event(ch, NULL, event_ticks, "", TICKS_PER_SECOND, EVENT_TICK);
  setup_event(ch, NULL, event_save, "", TICKS_PER_SECOND * 120, EVENT_SAVE);
  if (ch->hp != ch->maxhp)
    setup_event(ch, NULL, event_heal, "", TICKS_PER_SECOND * 3, EVENT_HEAL); /* it will stop itself if the player doesn't need healing */
  setup_event(ch, NULL, event_idle, "", IDLE_OUT, EVENT_IDLE);
}

EVENT_DATA *setup_event(CHAR_DATA *owner, CHAR_DATA *victim, EVENT_FUN *fun, char *argument, int ticks_left, int type)
{
  char buf[MAX_STRING_LENGTH];
  EVENT_DATA *event;

  event = new_event();
  link_event(event);

  event->next_local = owner->event;
  owner->event = event;

  event->owner.ch = owner;
  event->victim.ch = victim;
  event->fun = fun;
  strlcpy(event->argument, argument, 512);
  event->ticks_left = ticks_left;
  event->type = type;

  switch(type)
  {
    case EVENT_PORTAL:
      one_arg(argument, buf);
      event->args[0] = atoi(buf);
      one_arg(argument, buf);
      event->args[1] = atoi(buf);
      one_arg(argument, buf);
      event->args[2] = atoi(buf);
    default:
      break;
  }

  return event;
}

EVENT_DATA *setup_event_obj(OBJECT_DATA *owner, CHAR_DATA *victim, EVENT_FUN *fun, char *argument, int ticks_left, int type)
{
  EVENT_DATA *event;

  event = new_event();
  link_event(event);

  event->next_local = owner->event;
  owner->event = event;

  event->owner.obj = owner;
  event->victim.ch = victim;
  event->fun = fun;
  strlcpy(event->argument, argument, 256);
  event->ticks_left = ticks_left;
  event->type = type;

  return event;
}

EVENT_DATA *setup_event_d(DESCRIPTOR_DATA *owner, CHAR_DATA *victim, EVENT_FUN *fun, char *argument, int ticks_left, int type)
{
  EVENT_DATA *event;

  event = new_event();
  link_event(event);

  event->next_local = owner->event;
  owner->event = event;

  event->owner.d = owner;
  event->victim.ch = victim;
  event->fun = fun;
  strlcpy(event->argument, argument, 256);
  event->ticks_left = ticks_left;
  event->type = type;

  return event;
}

int num_events(CHAR_DATA *ch, int type)
{
  EVENT_DATA *event;
  int ret = 0;

  for (event = ch->event; event; event = event->next_local)
    if (event->type == type || type == EVENT_ALL)
      ++ret;

  return ret;
}

int num_events_obj(OBJECT_DATA *obj, int type)
{
  EVENT_DATA *event;
  int ret = 0;

  for (event = obj->event; event; event = event->next_local)
    if (event->type == type || type == EVENT_ALL)
      ++ret;

  return ret;
}

EVENT_DATA *find_event(CHAR_DATA *ch, int type)
{
  EVENT_DATA *event;

  if (type == EVENT_ALL)
    return ch->event;

  for (event = ch->event; event; event = event->next_local)
    if (event->type == type)
      return event;
  return NULL;
}

EVENT_DATA *find_event_obj(OBJECT_DATA *obj, int type)
{
  EVENT_DATA *event;

  for (event = obj->event; event; event = event->next_local)
    if (event->type == type)
      return event;
  return NULL;
}

EVENT_DATA *find_event_d(DESCRIPTOR_DATA *d, int type)
{
  EVENT_DATA *event;

  for (event = d->event; event; event = event->next_local)
    if (event->type == type)
      return event;
  return NULL;
}

void destroy_all_events(CHAR_DATA *ch, int type)
{
  EVENT_DATA *event;

  while ((event = find_event(ch, type)))
    destroy_one_event(ch, event);
}

void destroy_one_event(CHAR_DATA *ch, EVENT_DATA *event)
{
  EVENT_DATA *eventtemp;

  if (ch->event == NULL)
    return;

  if (ch->event == event)
  {
    ch->event = event->next_local;
    unlink_event(event);
    free_event(event);
    return;
  }

  for (eventtemp = ch->event; eventtemp; eventtemp = eventtemp->next_local)
    if (eventtemp->next_local == event)
    {
      eventtemp->next_local = event->next_local;
      break;
    }

  unlink_event(event);
  free_event(event);
}

void destroy_obj_events(OBJECT_DATA *obj, int type)
{
  EVENT_DATA *event, *eventtemp;

  if (obj->event == NULL)
    return;

  if (type == EVENT_ALL)
  {
    for (event = obj->event; event; event = event->next_local)
      unlink_event(event);
    event = obj->event;
    while (event)
    {
      eventtemp = event->next_local;
      free_event(event);
      event = eventtemp;
    }
    obj->event = NULL;
    return;
  }

  while (obj->event && type == obj->event->type)
  {
    eventtemp = obj->event;
    obj->event = obj->event->next_local;
    unlink_event(eventtemp);
    free_event(eventtemp);
  }

  event = obj->event;
  while (event && event->next_local)
    if (type == event->next_local->type)
    {
      eventtemp = event->next_local;
      event->next_local = eventtemp->next_local;
      unlink_event(eventtemp);
      free_event(eventtemp);
    }
    else
      event = event->next_local;
}

void destroy_d_events(DESCRIPTOR_DATA *d, int type)
{
  EVENT_DATA *event, *eventtemp;

  if (d->event == NULL)
    return;

  if (type == EVENT_ALL)
  {
    for (event = d->event; event; event = event->next_local)
      unlink_event(event);
    event = d->event;
    while (event)
    {
      eventtemp = event->next_local;
      free_event(event);
      event = eventtemp;
    }
    d->event = NULL;
    return;
  }

  while (d->event && type == d->event->type)
  {
    eventtemp = d->event;
    d->event = d->event->next_local;
    unlink_event(eventtemp);
    free_event(eventtemp);
  }

  event = d->event;
  while (event && event->next_local)
    if (type == event->next_local->type)
    {
      eventtemp = event->next_local;
      event->next_local = eventtemp->next_local;
      unlink_event(eventtemp);
      free_event(eventtemp);
    }
    else
      event = event->next_local;
}
