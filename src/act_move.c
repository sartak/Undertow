#include "undertow.h"

#define MAPX 79
#define MAPY 15
int map[MAPX][MAPY];

void color(int base, int x, int y)
{
  if (x < 0 || x >= MAPX || y < 0 || y >= MAPY || base < map[x][y])
    return;
  map[x][y] = base;

  color(base-number_range(1,2), x,   y-1);
  color(base-number_range(1,2), x,   y+1);
  color(base-number_range(1,2), x-1, y);
  color(base-number_range(1,2), x-1, y-1);
  color(base-number_range(1,2), x-1, y+1);
  color(base-number_range(1,2), x+1, y);
  color(base-number_range(1,2), x+1, y-1);
  color(base-number_range(1,2), x+1, y+1);
}

void chaosmap(int points)
{
  int x, y;
  int i;

  for (x = 0; x < MAPX; ++x)
    for (y = 0; y < MAPY; ++y)
      map[x][y] = 0;

  for (i = 0; i < points; ++i)
  {
    do
    {
      x = number_range(0, MAPX - 1);
      y = number_range(0, MAPY - 1);
    } while (map[x][y] == 7);
    color(7, x, y);
  }
}
void do_look(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  char coord[32];
  CHAR_DATA *gch;
  OBJECT_DATA *obj;

  if (ch->position == POS_SLEEPING)
  {
    send_to_char(ch, "You should probably wake up first.\n");
    return;
  }

  if (ch->plane == PLANE_INTERPLANE || ch->plane == PLANE_INTRAPLANE)
  {
    int x, y, c, o = 0;
    chaosmap(15);
    c = (ch->plane == PLANE_INTERPLANE);
    buf[o++] = '\n';
    for (y = 0; y < MAPY; ++y)
    {
      for (x = 0; x < MAPX; ++x)
      {
        switch (map[x][y])
        {
          case 0:
            buf[o++] = ' ';
            break;
          case 1:
          case 2:
            buf[o++] = '#';
            if (c)
              buf[o++] = 'b';
            else
              buf[o++] = 'r';
            buf[o++] = '.';
            break;
          case 3:
          case 4:
            buf[o++] = '#';
            if (c)
              buf[o++] = 'B';
            else
              buf[o++] = 'R';
            buf[o++] = '.';
            break;
          case 5:
          case 6:
            buf[o++] = '#';
            if (c)
              buf[o++] = 'c';
            else
              buf[o++] = 'y';
            buf[o++] = '.';
            break;
          case 7:
            buf[o++] = '#';
            if (c)
              buf[o++] = 'C';
            else
              buf[o++] = 'Y';
            buf[o++] = '*';
            break;
        }
      }
      buf[o++] = '\n';
    }
    buf[o] = 0;
    send_to_char(ch, buf);
    return;
  }

  for (gch = chhead; gch; gch = gch->next)
    if (gch != ch && gch->plane == ch->plane && (gch->x - ch->x) * (gch->x - ch->x) + (gch->y - ch->y) * (gch->y - ch->y) <= ch->sightradius * ch->sightradius)
      show_char_to_char(ch, gch);

  for (obj = objhead; obj; obj = obj->next)
    if (obj->holder == NULL && obj->container == NULL && obj->plane == ch->plane && (obj->x - ch->x) * (obj->x - ch->x) + (obj->y - ch->y) * (obj->y - ch->y) <= ch->sightradius * ch->sightradius)
      show_obj_to_char(ch, obj);

  if (ch->x == 0 && ch->y == 0)
    snprintf(coord, 32, "#Gthe origin#n");
  else
    snprintf(coord, 32, "#1[#R%d#1, #R%d#1]#n", ch->x, ch->y);
  snprintf(buf, MAX_STRING_LENGTH, "You are at %s ", coord);
  switch (ch->plane)
  {
    default:
    case PLANE_NONE:
      strlcat(buf, "in limbo!\n", MAX_STRING_LENGTH);
      break;
    case PLANE_NEWBIE:
      strlcat(buf, "on the Newbie plane.\n", MAX_STRING_LENGTH);
      break;
    case PLANE_NEXUS:
      strlcat(buf, "on the Nexus.\n", MAX_STRING_LENGTH);
      break;
    case PLANE_HNEWBIE:
      strlcat(buf, "on the #RHardcore#n Newbie plane.\n", MAX_STRING_LENGTH);
      break;
    case PLANE_HNEXUS:
      strlcat(buf, "on the #RHardcore#n Nexus.\n", MAX_STRING_LENGTH);
      break;
    case PLANE_HEAVEN:
      strlcat(buf, "in the skies.\n", MAX_STRING_LENGTH);
      break;
    case PLANE_HELL:
      strlcat(buf, "in the underworld.\n", MAX_STRING_LENGTH);
      break;
    case PLANE_JAIL:
      strlcat(buf, "in jail.\n", MAX_STRING_LENGTH);
      break;
    case PLANE_INTERPLANE:
      strlcat(buf, "in the interplanar void.\n", MAX_STRING_LENGTH);
      break;
    case PLANE_INTRAPLANE:
      strlcat(buf, "in the intraplanar void.\n", MAX_STRING_LENGTH);
      break;
  }
  send_to_char(ch, buf);
}

void do_north(CHAR_DATA *ch, char *arguments)
{
  int max = 225000000, x, y;

  if (find_event(ch, EVENT_CHANGEPOS))
  {
    send_to_char(ch, "You're too busy to move.\n");
    return;
  }

  if (ch->plane == PLANE_NEWBIE || ch->plane == PLANE_HNEWBIE)
    max = 10000;
  else if (ch->plane == PLANE_JAIL || ch->plane == PLANE_NONE)
    max = 0;
  x = ch->x;
  y = ch->y + 1;
  if (x*x + y*y > max)
  {
    send_to_char(ch, "An invisible force prevents further progress.\n");
    return;
  }
  ch->y++;
  do_look(ch, "");
}

void do_south(CHAR_DATA *ch, char *arguments)
{
  int max = 225000000, x, y;

  if (find_event(ch, EVENT_CHANGEPOS))
  {
    send_to_char(ch, "You're too busy to move.\n");
    return;
  }

  if (ch->plane == PLANE_NEWBIE || ch->plane == PLANE_HNEWBIE)
    max = 10000;
  else if (ch->plane == PLANE_JAIL || ch->plane == PLANE_NONE)
    max = 0;
  x = ch->x;
  y = ch->y - 1;
  if (x*x + y*y > max)
  {
    send_to_char(ch, "An invisible force prevents further progress.\n");
    return;
  }
  ch->y--;
  do_look(ch, "");
}

void do_east(CHAR_DATA *ch, char *arguments)
{
  int max = 225000000, x, y;

  if (find_event(ch, EVENT_CHANGEPOS))
  {
    send_to_char(ch, "You're too busy to move.\n");
    return;
  }

  if (ch->plane == PLANE_NEWBIE || ch->plane == PLANE_HNEWBIE)
    max = 10000;
  else if (ch->plane == PLANE_JAIL || ch->plane == PLANE_NONE)
    max = 0;
  x = ch->x + 1;
  y = ch->y;
  if (x*x + y*y > max)
  {
    send_to_char(ch, "An invisible force prevents further progress.\n");
    return;
  }
  ch->x++;
  do_look(ch, "");
}

void do_west(CHAR_DATA *ch, char *arguments)
{
  int max = 225000000, x, y;

  if (find_event(ch, EVENT_CHANGEPOS))
  {
    send_to_char(ch, "You're too busy to move.\n");
    return;
  }

  if (ch->plane == PLANE_NEWBIE || ch->plane == PLANE_HNEWBIE)
    max = 10000;
  else if (ch->plane == PLANE_JAIL || ch->plane == PLANE_NONE)
    max = 0;
  x = ch->x - 1;
  y = ch->y;
  if (x*x + y*y > max)
  {
    send_to_char(ch, "An invisible force prevents further progress.\n");
    return;
  }
  ch->x--;
  do_look(ch, "");
}

void do_stand(CHAR_DATA *ch, char *arguments)
{
  EVENT_DATA *event;

  if ((event = find_event(ch, EVENT_CHANGEPOS)) != NULL)
  {
    if (event->args[1] == POS_STANDING)
      send_to_char(ch, "You're already on your way.\n");
    else
      send_to_char(ch, "You're too busy to stand.\n");
    return;
  }

  switch(ch->position)
  {
    case POS_STANDING:
    case POS_FIGHTING:
      send_to_char(ch, "You're already standing.\n");
      return;
    break;
    case POS_SITTING:
    case POS_SLEEPING:
      send_to_char(ch, "You begin standing.\n");
    break;
  }
  event = setup_event(ch, NULL, event_changepos, "", TICKS_PER_SECOND, EVENT_CHANGEPOS);
  event->args[0] = ch->position;
  event->args[1] = POS_STANDING;
}

void do_sit(CHAR_DATA *ch, char *arguments)
{
  EVENT_DATA *event;

  if ((event = find_event(ch, EVENT_CHANGEPOS)) != NULL)
  {
    if (event->args[1] == POS_SITTING)
      send_to_char(ch, "You're already on your way.\n");
    else
      send_to_char(ch, "You're too busy to sit.\n");
    return;
  }

  switch(ch->position)
  {
    case POS_SITTING:
      send_to_char(ch, "You're already sitting.\n");
      return;
    break;
    case POS_FIGHTING:
      send_to_char(ch, "You're too busy fighting to sit.\n");
      return;
    break;
    case POS_STANDING:
      send_to_char(ch, "You begin sitting down.\n");
    break;
    case POS_SLEEPING:
      send_to_char(ch, "You begin waking up.\n");
    break;
  }
  event = setup_event(ch, NULL, event_changepos, "", TICKS_PER_SECOND, EVENT_CHANGEPOS);
  event->args[0] = ch->position;
  event->args[1] = POS_SITTING;
}

void do_sleep(CHAR_DATA *ch, char *arguments)
{
  EVENT_DATA *event;

  if ((event = find_event(ch, EVENT_CHANGEPOS)) != NULL)
  {
    if (event->args[1] == POS_SLEEPING)
      send_to_char(ch, "You're already on your way.\n");
    else
      send_to_char(ch, "You're too busy to sleep.\n");
    return;
  }

  switch(ch->position)
  {
    case POS_SLEEPING:
      send_to_char(ch, "You're already sleeping.\n");
      return;
    break;
    case POS_FIGHTING:
      send_to_char(ch, "You're too busy fighting to take a nap.\n");
      return;
    break;
    case POS_STANDING:
    case POS_SITTING:
      send_to_char(ch, "You begin falling asleep.\n");
    break;
  }
  event = setup_event(ch, NULL, event_changepos, "", TICKS_PER_SECOND, EVENT_CHANGEPOS);
  event->args[0] = ch->position;
  event->args[1] = POS_SLEEPING;
}
