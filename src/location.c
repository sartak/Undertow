#include "undertow.h"

int is_local(int x1, int y1, int p1, int x2, int y2, int p2, int r)
{
  if (p1 == p2 && ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= r * r))
    return 1;
  return 0;
}

void show_char_to_char(CHAR_DATA *ch, CHAR_DATA *vic)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char pos[32];
  if (vic->position == POS_STANDING)
    strlcpy(pos, "stands", 32);
  if (vic->position == POS_SITTING)
    strlcpy(pos, "sits", 32);
  if (vic->position == POS_SLEEPING)
    strlcpy(pos, "rests", 32);

  if (IS_NPC(vic))
  {
    if (vic->name[0] == 'a' || vic->name[0] == 'e' || vic->name[0] == 'i' || vic->name[0] == 'o' || vic->name[0] == 'u')
      snprintf(buf, MAX_STRING_LENGTH, "#r(#RX#r)#n An %s %s ", vic->name, pos);
    else
      snprintf(buf, MAX_STRING_LENGTH, "#r(#RX#r)#n A %s %s ", vic->name, pos);
  }
  else
    snprintf(buf, MAX_STRING_LENGTH, "%s %s ", vic->name, pos);

  displacement_words(ch->x, ch->y, vic->x, vic->y, buf2);

  strlcat(buf, buf2, MAX_STRING_LENGTH);
  if (buf2[0] != 'h')
    strlcat(buf, " of you", MAX_STRING_LENGTH);

  if (!IS_NPC(vic) && vic->d->state == STATE_LINKDEAD)
    strlcat(buf, " #1[#yLinkdead#1]#n", MAX_STRING_LENGTH);
  strlcat(buf, ".\n", MAX_STRING_LENGTH);
  send_to_char(ch, buf);
}

void show_obj_to_char(CHAR_DATA *ch, OBJECT_DATA *obj)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  strlcpy(buf, obj->longdesc, MAX_STRING_LENGTH);
  buf[0] = UPPER(buf[0]);

  strlcat(buf, " ", MAX_STRING_LENGTH);

  displacement_words(ch->x, ch->y, obj->x, obj->y, buf2);

  strlcat(buf, buf2, MAX_STRING_LENGTH);
  if (buf2[0] != 'h')
    strlcat(buf, " of you.\n", MAX_STRING_LENGTH);
  else
    strlcat(buf, ".\n", MAX_STRING_LENGTH);
  send_to_char(ch, buf);
}

void put_sense(CHAR_DATA *ch, int sense, char *msg)
{
  switch (sense)
  {
    case SENSE_INSIGHT:
/*      if (ch->position == POS_SLEEPING)
        break;
      send_to_char(ch, msg);
      break;*/
    case SENSE_SIGHT:
/*      if (ch->position == POS_SLEEPING)
        break;
      send_to_char(ch, msg);
      break;*/
    case SENSE_HEARING:
/*      if (ch->position == POS_SLEEPING)
        break;
      send_to_char(ch, msg);
      break;*/
    case SENSE_SMELL:
/*      if (ch->position == POS_SLEEPING)
        break;
      send_to_char(ch, msg);
      break;*/
    case SENSE_TASTE:
/*      if (ch->position == POS_SLEEPING)
        break;
      send_to_char(ch, msg);
      break;*/
    case SENSE_TOUCH:
      if (ch->position == POS_SLEEPING)
        break;
      send_to_char(ch, msg);
      break;
  }
}

void put_sense_area(int x, int y, int plane, int sense, char *msg)
{
  put_sense_except(NULL, x, y, plane, sense, msg);
}

void put_sense_except(CHAR_DATA *gch, int x, int y, int plane, int sense, char *msg)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];

  buf[0] = '\n'; buf[1] = '\0';
  strlcat(buf, msg, MAX_STRING_LENGTH);

  for (d = dhead; d; d = d->next)
    if (d->acc == NULL || d->acc->ch == NULL)
      continue;
    else
      if (d->acc->ch && d->acc->ch != gch && d->acc->ch->plane == plane && (d->acc->ch->x - x) * (d->acc->ch->x - x) + (d->acc->ch->y - y) * (d->acc->ch->y - y) <= d->acc->ch->sightradius * d->acc->ch->sightradius)
        put_sense(d->acc->ch, sense, buf);
}

void put_sense_except2(CHAR_DATA *ach, CHAR_DATA *bch, int x, int y, int plane, int sense, char *msg)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];

  buf[0] = '\n'; buf[1] = '\0';
  strlcat(buf, msg, MAX_STRING_LENGTH);

  for (d = dhead; d; d = d->next)
    if (d->acc == NULL || d->acc->ch == NULL)
      continue;
    else
      if (d->acc->ch && d->acc->ch != ach && d->acc->ch != bch && d->acc->ch->plane == plane && (d->acc->ch->x - x) * (d->acc->ch->x - x) + (d->acc->ch->y - y) * (d->acc->ch->y - y) <= d->acc->ch->sightradius * d->acc->ch->sightradius)
        put_sense(d->acc->ch, sense, buf);
}
