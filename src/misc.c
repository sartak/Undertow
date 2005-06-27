#include "undertow.h"

void info(CHAR_DATA *ch, char *info, int newline)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];

  strlcpy(buf, "#1(#2Info#1) #R", MAX_STRING_LENGTH);
  strlcat(buf, info, MAX_STRING_LENGTH);

  for (d = dhead; d; d = d->next)
    if (d->state == STATE_PLAYING || d->state == STATE_LINKDEAD)
    {
      if ((d->acc->ch && d->acc->ch != ch) || newline)
        send_to_descriptor(d, "\n");
      send_to_descriptor(d, buf);
    }
}

void login_msg(CHAR_DATA *ch)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];

  snprintf(buf, MAX_STRING_LENGTH, "\n\a#1(#2Enter#1) #R%s#2 enters #RU#rndertow#2.#n\n", ch->name);

  for (d = dhead; d; d = d->next)
    if (d->state == STATE_PLAYING || d->state == STATE_LINKDEAD)
      send_to_descriptor(d, buf);
}

void login_msg_new(CHAR_DATA *ch)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];

  snprintf(buf, MAX_STRING_LENGTH, "\n\a#1(#2Enter#1) #R%s#2, a new player, enters #RU#rndertow#2.#n\n", ch->name);

  for (d = dhead; d; d = d->next)
    if (d->state == STATE_PLAYING || d->state == STATE_LINKDEAD)
      send_to_descriptor(d, buf);
}

void logout_msg(CHAR_DATA *ch)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];

  snprintf(buf, MAX_STRING_LENGTH, "\n#1(#2Quit#1) #R%s#2 escapes from #RU#rndertow#2.#n\n", ch->name);

  for (d = dhead; d; d = d->next)
    if (d->state == STATE_PLAYING || d->state == STATE_LINKDEAD)
      send_to_descriptor(d, buf);
}

CHAR_DATA *find_char_world(CHAR_DATA *ch, char *name)
{
  CHAR_DATA *vic = NULL, *best = NULL;
  int x = 0;
  int matchcount = 0;

  if (!stricmp(name, "self") || !stricmp(name, "me"))
    return ch;

  for (vic = chhead; vic; vic = vic->next)
  {
    x = beginning(vic->name, name);
    if (x == strlen(name) && x > matchcount)
    {
      best = vic;
      matchcount = x;
    }
  }
  return best;
}
