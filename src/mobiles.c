#include "undertow.h"

void spawn_mobile(int plane, int x, int y)
{
  CHAR_DATA *mob = new_character();
  link_character(mob);
  SET_BIT(mob->cflags, CFLAG_NPC);
  mob->plane = plane;
  mob->x = x;
  mob->y = y;
  if (plane == PLANE_NEWBIE || plane == PLANE_HNEWBIE)
    mob->level = 5;
  else
    mob->level = 10;
  switch (number_range(1, 6))
  {
    case 1:
      strlcpy(mob->name, "fire elemental", 32);
      break;
    case 2:
      strlcpy(mob->name, "air elemental", 32);
      break;
    case 3:
      strlcpy(mob->name, "earth elemental", 32);
      break;
    case 4:
      strlcpy(mob->name, "water elemental", 32);
      break;
    case 5:
      strlcpy(mob->name, "shadow elemental", 32);
      break;
    case 6:
      strlcpy(mob->name, "poison elemental", 32);
      break;
  }

  mob->plutos = mob->level * number_range(3, 7) + number_range(1, mob->level);
  mob->sightradius = 50;
  mob->maxhp = number_range(1, 10) * mob->level;
  mob->hp = mob->maxhp;
  mob->maxmana = number_range(1, 10) * mob->level;
  mob->mana = mob->maxmana;
  mob->maxmove = number_range(1, 10) * mob->level;
  mob->move = mob->maxmove;
  mob->position = POS_STANDING;
  mob->resistance = 0;
}

void do_spawn(CHAR_DATA *ch, char *arguments)
{
  spawn_mobile(ch->plane, ch->x, ch->y);
  send_to_char(ch, "You beget life.\n");
  put_sense_except(ch, ch->x, ch->y, ch->plane, SENSE_HEARING, "The loud crack of new life fills the stagnant atmosphere for a fleeting moment.\n");
}
