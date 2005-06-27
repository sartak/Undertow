#include "undertow.h"

MEMORY_DATA mem;

DESCRIPTOR_DATA *dhead = NULL;
DESCRIPTOR_DATA *dfree = NULL;
CHAR_DATA *chhead = NULL;
CHAR_DATA *chfree = NULL;
ACCOUNT_DATA *acchead = NULL;
ACCOUNT_DATA *accfree = NULL;
EVENT_DATA *eventhead = NULL;
EVENT_DATA *eventfree = NULL;
OBJECT_DATA *objhead = NULL;
OBJECT_DATA *objfree = NULL;
ALIAS_DATA *aliashead = NULL;
ALIAS_DATA *aliasfree = NULL;

void free_all()
{
  DESCRIPTOR_DATA *dtemp;
  CHAR_DATA       *chtemp;
  ACCOUNT_DATA    *acctemp;
  EVENT_DATA      *eventtemp;
  OBJECT_DATA     *objtemp;
  ALIAS_DATA      *aliastemp;

  while (dhead)
  {
    mem.bytes_allocated -= sizeof(DESCRIPTOR_DATA);
    mem.d_allocated--;
    dtemp = dhead->next;
    free(dhead);
    dhead = dtemp;
  }
  while (dfree)
  {
    mem.bytes_allocated -= sizeof(DESCRIPTOR_DATA);
    mem.d_allocated--;
    dtemp = dfree->next;
    free(dfree);
    dfree = dtemp;
  }

  while (chhead)
  {
    mem.bytes_allocated -= sizeof(CHAR_DATA);
    mem.ch_allocated--;
    chtemp = chhead->next;
    free(chhead);
    chhead = chtemp;
  }
  while (chfree)
  {
    mem.bytes_allocated -= sizeof(CHAR_DATA);
    mem.ch_allocated--;
    chtemp = chfree->next;
    free(chfree);
    chfree = chtemp;
  }

  while (acchead)
  {
    mem.bytes_allocated -= sizeof(ACCOUNT_DATA);
    mem.acc_allocated--;
    acctemp = acchead->next;
    free(acchead);
    acchead = acctemp;
  }
  while (accfree)
  {
    mem.bytes_allocated -= sizeof(ACCOUNT_DATA);
    mem.acc_allocated--;
    acctemp = accfree->next;
    free(accfree);
    accfree = acctemp;
  }

  while (eventhead)
  {
    mem.bytes_allocated -= sizeof(EVENT_DATA);
    mem.event_allocated--;
    eventtemp = eventhead->next;
    free(eventhead);
    eventhead = eventtemp;
  }
  while (eventfree)
  {
    mem.bytes_allocated -= sizeof(EVENT_DATA);
    mem.event_allocated--;
    eventtemp = eventfree->next;
    free(eventfree);
    eventfree = eventtemp;
  }

  while (objhead)
  {
    mem.bytes_allocated -= sizeof(OBJECT_DATA);
    mem.obj_allocated--;
    objtemp = objhead->next;
    free(objhead);
    objhead = objtemp;
  }
  while (objfree)
  {
    mem.bytes_allocated -= sizeof(OBJECT_DATA);
    mem.obj_allocated--;
    objtemp = objfree->next;
    free(objfree);
    objfree = objtemp;
  }

  while (aliashead)
  {
    mem.bytes_allocated -= sizeof(ALIAS_DATA);
    mem.alias_allocated--;
    aliastemp = aliashead->next;
    free(aliashead);
    aliashead = aliastemp;
  }
  while (aliasfree)
  {
    mem.bytes_allocated -= sizeof(ALIAS_DATA);
    mem.alias_allocated--;
    aliastemp = aliasfree->next;
    free(aliasfree);
    aliasfree = aliastemp;
  }

  free_changes();
  free_all_helps();

  if (mem.bytes_allocated > 0)
    syslog("WARNING", "%d bytes left allocated. Memory leak!", mem.bytes_allocated);
  if (mem.d_allocated > 0)
    syslog("WARNING", "%d descriptor_data instances (%d bytes) remain allocated.", mem.d_allocated, mem.d_allocated * sizeof(DESCRIPTOR_DATA));
  if (mem.ch_allocated > 0)
    syslog("WARNING", "%d char_data instances (%d bytes) remain allocated.", mem.ch_allocated, mem.ch_allocated * sizeof(CHAR_DATA));
  if (mem.acc_allocated > 0)
    syslog("WARNING", "%d account_data instances (%d bytes) remain allocated.", mem.acc_allocated, mem.acc_allocated * sizeof(ACCOUNT_DATA));
  if (mem.event_allocated > 0)
    syslog("WARNING", "%d event_data instances (%d bytes) remain allocated.", mem.event_allocated, mem.event_allocated * sizeof(EVENT_DATA));
  if (mem.obj_allocated > 0)
    syslog("WARNING", "%d object_data instances (%d bytes) remain allocated.", mem.obj_allocated, mem.obj_allocated * sizeof(OBJECT_DATA));
  if (mem.alias_allocated > 0)
    syslog("WARNING", "%d alias_data instances (%d bytes) remain allocated.", mem.alias_allocated, mem.alias_allocated * sizeof(ALIAS_DATA));
}

DESCRIPTOR_DATA *new_descriptor()
{
  DESCRIPTOR_DATA *d;

  if (dfree == NULL)
  {
    d = (DESCRIPTOR_DATA *) malloc(sizeof(DESCRIPTOR_DATA));
    mem.bytes_allocated += sizeof(DESCRIPTOR_DATA);
    mem.d_allocated++;
  }
  else
  {
    d = dfree;
    dfree = dfree->next;
    mem.d_recycled++;
    mem.d_free--;
  }

  memset(d, 0, sizeof(DESCRIPTOR_DATA));
  return d;
}

void free_descriptor(DESCRIPTOR_DATA *d)
{
  DESCRIPTOR_DATA *temp;
  if (d == NULL)
  {
    syslog("WARNING", "Tried to free a NULL descriptor.");
    return;
  }

  for (temp = dfree; temp; temp = temp->next)
    if (temp == d)
    {
      syslog("WARNING", "Tried to free a previously-recycled descriptor.");
      return;
    }

  memset(d, 0, sizeof(DESCRIPTOR_DATA));
  d->next = dfree;
  dfree = d;
  mem.d_free++;
}

void link_descriptor(DESCRIPTOR_DATA *d)
{
  if (d == NULL)
  {
    syslog("WARNING", "Tried to link a NULL descriptor.");
    return;
  }
  d->next = dhead;
  dhead = d;
}

void unlink_descriptor(DESCRIPTOR_DATA *d)
{
  DESCRIPTOR_DATA *dtemp;
  if (d == NULL)
  {
    syslog("WARNING", "Tried to unlink a NULL descriptor.");
    return;
  }

  while (d == dhead)
    dhead = dhead->next;

  dtemp = dhead;
  while (dtemp)
  {
    while (dtemp->next == d)
      dtemp->next = d->next;
    dtemp = dtemp->next;
  }
}

CHAR_DATA *new_character()
{
  CHAR_DATA *ch;

  if (chfree == NULL)
  {
    ch = (CHAR_DATA *) malloc(sizeof(CHAR_DATA));
    mem.bytes_allocated += sizeof(CHAR_DATA);
    mem.ch_allocated++;
  }
  else
  {
    ch = chfree;
    chfree = chfree->next;
    mem.ch_recycled++;
    mem.ch_free--;
  }

  memset(ch, 0, sizeof(CHAR_DATA));
  return ch;
}

void free_character(CHAR_DATA *ch)
{
  CHAR_DATA *temp;
  if (ch == NULL)
  {
    syslog("WARNING", "Tried to free a NULL char.");
    return;
  }

  for (temp = chfree; temp; temp = temp->next)
    if (temp == ch)
    {
      syslog("WARNING", "Tried to free a previously-recycled character.");
      return;
    }

  memset(ch, 0, sizeof(CHAR_DATA));
  ch->next = chfree;
  chfree = ch;
  mem.ch_free++;
}

void link_character(CHAR_DATA *ch)
{
  if (ch == NULL)
  {
    syslog("WARNING", "Tried to link a NULL character.");
    return;
  }
  ch->next = chhead;
  chhead = ch;
}

void unlink_character(CHAR_DATA *ch)
{
  CHAR_DATA *chtemp;
  if (ch == NULL)
  {
    syslog("WARNING", "Tried to unlink a NULL character.");
    return;
  }

  while (ch == chhead)
    chhead = chhead->next;

  chtemp = chhead;

  while (chtemp)
  {
    while (chtemp->next == ch)
      chtemp->next = ch->next;
    chtemp = chtemp->next;
  }
}

ACCOUNT_DATA *new_account()
{
  ACCOUNT_DATA *acc;

  if (accfree == NULL)
  {
    acc = (ACCOUNT_DATA *) malloc(sizeof(ACCOUNT_DATA));
    mem.bytes_allocated += sizeof(ACCOUNT_DATA);
    mem.acc_allocated++;
  }
  else
  {
    acc = accfree;
    accfree = accfree->next;
    mem.acc_recycled++;
    mem.acc_free--;
  }

  memset(acc, 0, sizeof(ACCOUNT_DATA));
  return acc;
}

void free_account(ACCOUNT_DATA *acc)
{
  ACCOUNT_DATA *temp;
  if (acc == NULL)
  {
    syslog("WARNING", "Tried to free a NULL account.");
    return;
  }

  for (temp = accfree; temp; temp = temp->next)
    if (temp == acc)
    {
      syslog("WARNING", "Tried to free a previously-recycled account.");
      return;
    }

  memset(acc, 0, sizeof(ACCOUNT_DATA));
  acc->next = accfree;
  accfree = acc;
  mem.acc_free++;
}

void link_account(ACCOUNT_DATA *acc)
{
  if (acc == NULL)
  {
    syslog("WARNING", "Tried to link a NULL account.");
    return;
  }
  acc->next = acchead;
  acchead = acc;
}

void unlink_account(ACCOUNT_DATA *acc)
{
  ACCOUNT_DATA *acctemp;
  if (acc == NULL)
  {
    syslog("WARNING", "Tried to unlink a NULL account.");
    return;
  }

  while (acc == acchead)
    acchead = acchead->next;

  acctemp = acchead;
  while (acctemp)
  {
    while (acctemp->next == acc)
      acctemp->next = acc->next;
    acctemp = acctemp->next;
  }
}

EVENT_DATA *new_event()
{
  EVENT_DATA *event;

  if (eventfree == NULL)
  {
    event = (EVENT_DATA *) malloc(sizeof(EVENT_DATA));
    mem.bytes_allocated += sizeof(EVENT_DATA);
    mem.event_allocated++;
  }
  else
  {
    event = eventfree;
    eventfree = eventfree->next;
    mem.event_recycled++;
    mem.event_free--;
  }

  memset(event, 0, sizeof(EVENT_DATA));
  return event;
}

void free_event(EVENT_DATA *event)
{
  EVENT_DATA *temp;
  if (event == NULL)
  {
    syslog("WARNING", "Tried to free a NULL event.");
    return;
  }

  for (temp = eventfree; temp; temp = temp->next)
    if (temp == event)
    {
      syslog("WARNING", "Tried to free a previously-recycled event.");
      return;
    }

  memset(event, 0, sizeof(EVENT_DATA));
  event->next = eventfree;
  eventfree = event;
  mem.event_free++;
}

void link_event(EVENT_DATA *event)
{
  if (event == NULL)
  {
    syslog("WARNING", "Tried to link a NULL event.");
    return;
  }
  event->next = eventhead;
  eventhead = event;
}

void unlink_event(EVENT_DATA *event)
{
  EVENT_DATA *eventtemp;
  if (event == NULL)
  {
    syslog("WARNING", "Tried to unlink a NULL event.");
    return;
  }

  while (event == eventhead)
    eventhead = eventhead->next;

  eventtemp = eventhead;
  while (eventtemp)
  {
    while (eventtemp->next == event)
      eventtemp->next = event->next;
    eventtemp = eventtemp->next;
  }
}

OBJECT_DATA *new_object()
{
  OBJECT_DATA *obj;

  if (objfree == NULL)
  {
    obj = (OBJECT_DATA *) malloc(sizeof(OBJECT_DATA));
    mem.bytes_allocated += sizeof(OBJECT_DATA);
    mem.obj_allocated++;
  }
  else
  {
    obj = objfree;
    objfree = objfree->next;
    mem.obj_recycled++;
    mem.obj_free--;
  }

  memset(obj, 0, sizeof(OBJECT_DATA));
  return obj;
}

void free_object(OBJECT_DATA *obj)
{
  OBJECT_DATA *temp;
  if (obj == NULL)
  {
    syslog("WARNING", "Tried to free a NULL object.");
    return;
  }

  for (temp = objfree; temp; temp = temp->next)
    if (temp == obj)
    {
      syslog("WARNING", "Tried to free a previously-recycled object.");
      return;
    }

  memset(obj, 0, sizeof(OBJECT_DATA));
  obj->next = objfree;
  objfree = obj;
  mem.obj_free++;
}

void link_object(OBJECT_DATA *obj)
{
  if (obj == NULL)
  {
    syslog("WARNING", "Tried to link a NULL object.");
    return;
  }

  obj->next = objhead;
  objhead = obj;
}

void unlink_object(OBJECT_DATA *obj)
{
  OBJECT_DATA *objtemp;
  if (obj == NULL)
  {
    syslog("WARNING", "Tried to unlink a NULL object.");
    return;
  }

  while (obj == objhead)
    objhead = objhead->next;

  objtemp = objhead;
  while (objtemp)
  {
    while (objtemp->next == obj)
      objtemp->next = obj->next;
    objtemp = objtemp->next;
  }
}

ALIAS_DATA *new_alias()
{
  ALIAS_DATA *alias;

  if (aliasfree == NULL)
  {
    alias = (ALIAS_DATA *) malloc(sizeof(ALIAS_DATA));
    mem.bytes_allocated += sizeof(ALIAS_DATA);
    mem.alias_allocated++;
  }
  else
  {
    alias = aliasfree;
    aliasfree = aliasfree->next;
    mem.alias_recycled++;
    mem.alias_free--;
  }

  memset(alias, 0, sizeof(ALIAS_DATA));
  return alias;
}

void free_alias(ALIAS_DATA *alias)
{
  ALIAS_DATA *temp;
  if (alias == NULL)
  {
    syslog("WARNING", "Tried to free a NULL alias.");
    return;
  }

  for (temp = aliasfree; temp; temp = temp->next)
    if (temp == alias)
    {
      syslog("WARNING", "Tried to free a previously-recycled alias.");
      return;
    }

  memset(alias, 0, sizeof(ALIAS_DATA));
  alias->next = aliasfree;
  aliasfree = alias;
  mem.alias_free++;
}

void link_alias(ALIAS_DATA *alias)
{
  if (alias == NULL)
  {
    syslog("WARNING", "Tried to link a NULL alias.");
    return;
  }

  alias->next = aliashead;
  aliashead = alias;
}

void unlink_alias(ALIAS_DATA *alias)
{
  ALIAS_DATA *aliastemp;
  if (alias == NULL)
  {
    syslog("WARNING", "Tried to unlink a NULL alias.");
    return;
  }

  while (alias == aliashead)
    aliashead = aliashead->next;

  aliastemp = aliashead;
  while (aliastemp)
  {
    while (aliastemp->next == alias)
      aliastemp->next = alias->next;
    aliastemp = aliastemp->next;
  }
}

void garbage_collector_init()
{
  EVENT_DATA *event;

  event = new_event();
  link_event(event);

  event->type = EVENT_GARBAGECOLLECT;
  event->fun = event_garbagecollect;
  event->ticks_left = TICKS_PER_SECOND * 60 * 10;
}

/*
 * Sartak says, 'We're gonna be disappointed when it works!'
 * Atreides waits!
 * %%% Disconnected from server.
 */

int event_garbagecollect(EVENT_DATA *event)
{
  int num = 0, freed = 0, bytes = mem.bytes_allocated;
  DESCRIPTOR_DATA *dtemp;
  CHAR_DATA       *chtemp;
  ACCOUNT_DATA    *acctemp;
  EVENT_DATA      *eventtemp;
  OBJECT_DATA     *objtemp;
  ALIAS_DATA      *aliastemp;

  for (dtemp = dfree; dtemp; dtemp = dtemp->next)
    ++num;
  for (num *= .2f; num > 0; --num)
  {
    dtemp = dfree->next;
    free(dfree);
    dfree = dtemp;
    mem.bytes_allocated -= sizeof(DESCRIPTOR_DATA);
    mem.d_allocated--;
    mem.d_free--;
    freed++;
  }

  for (chtemp = chfree; chtemp; chtemp = chtemp->next)
    ++num;
  for (num *= .2f; num > 0; --num)
  {
    chtemp = chfree->next;
    free(chfree);
    chfree = chtemp;
    mem.bytes_allocated -= sizeof(CHAR_DATA);
    mem.ch_allocated--;
    mem.ch_free--;
    freed++;
  }

  for (acctemp = accfree; acctemp; acctemp = acctemp->next)
    ++num;
  for (num *= .2f; num > 0; --num)
  {
    acctemp = accfree->next;
    free(accfree);
    accfree = acctemp;
    mem.bytes_allocated -= sizeof(ACCOUNT_DATA);
    mem.acc_allocated--;
    mem.acc_free--;
    freed++;
  }

  for (eventtemp = eventfree; eventtemp; eventtemp = eventtemp->next)
    ++num;
  for (num *= .2f; num > 0; --num)
  {
    eventtemp = eventfree->next;
    free(eventfree);
    eventfree = eventtemp;
    mem.bytes_allocated -= sizeof(EVENT_DATA);
    mem.event_allocated--;
    mem.event_free--;
    freed++;
  }

  for (objtemp = objfree; objtemp; objtemp = objtemp->next)
    ++num;
  for (num *= .2f; num > 0; --num)
  {
    objtemp = objfree->next;
    free(objfree);
    objfree = objtemp;
    mem.bytes_allocated -= sizeof(OBJECT_DATA);
    mem.obj_allocated--;
    mem.obj_free--;
    freed++;
  }

  for (aliastemp = aliasfree; aliastemp; aliastemp = aliastemp->next)
    ++num;
  for (num *= .2f; num > 0; --num)
  {
    aliastemp = aliasfree->next;
    free(aliasfree);
    aliasfree = aliastemp;
    mem.bytes_allocated -= sizeof(ALIAS_DATA);
    mem.alias_allocated--;
    mem.alias_free--;
    freed++;
  }

  bytes -= mem.bytes_allocated;
  if (freed)
    syslog("GARBAGE", "Garbage collector: %d entit%s (%d bytes) freed this cycle.", freed, freed == 1 ? "y" : "ies", bytes);

  event->ticks_left = TICKS_PER_SECOND * 60 * 10;
  return 0;
}
