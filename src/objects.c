#include "undertow.h"

void update_objects(CHAR_DATA *ch)
{
  OBJECT_DATA *obj;

  for (obj = ch->obj; obj; obj = obj->next_local)
  {
    obj->x = ch->x;
    obj->y = ch->y;
    obj->plane = ch->plane;
  }
}

void create_portal(int type, CHAR_DATA *ch, int x1, int y1, int p1, int x2, int y2, int p2)
{
  OBJECT_DATA *obj, *obj2;
  char buf[MAX_STRING_LENGTH];

  obj = new_object();
  obj2 = new_object();
  link_object(obj);
  link_object(obj2);

  buf[0] = 0;
  if (type == 0)
  {
    strlcpy(obj->keywords, "red portal", 128);
    strlcpy(obj->shortdesc, "a #Rred#n portal", 128);
    strlcpy(obj->longdesc, "A #Rred#n portal spins", 128);
    strlcpy(buf, "A #Rred#n portal opens up.\n", MAX_STRING_LENGTH);
    setup_event_obj(obj, NULL, event_cportal, "", TICKS_PER_SECOND * 30, EVENT_CPORTAL);
    setup_event_obj(obj2, NULL, event_cportal, "", TICKS_PER_SECOND * 30, EVENT_CPORTAL);
    if (ch)
    {
      strlcpy(obj->owner, ch->name, 32);
      strlcpy(obj2->owner, ch->name, 32);
    }
  }
  else if (type == 1)
  {
    strlcpy(obj->keywords, "blue portal", 128);
    strlcpy(obj->shortdesc, "a #Cblue#n portal", 128);
    strlcpy(obj->longdesc, "A #Cblue#n portal ominously spins", 128);
  }
  else
  {
    strlcpy(obj->keywords, "green portal", 128);
    strlcpy(obj->shortdesc, "a #Ggreen#n portal", 128);
    strlcpy(obj->longdesc, "A #Ggreen#n portal ominously spins", 128);
  }

  strlcpy(obj2->keywords,  obj->keywords, 128);
  strlcpy(obj2->shortdesc, obj->shortdesc, 128);
  strlcpy(obj2->longdesc,  obj->longdesc, 128);


  obj->x = x1;
  obj->y = y1;
  obj->plane = p1;
  obj2->v1 = x1;
  obj2->v2 = y1;
  obj2->v3 = p1;
  SET_BIT(obj->oflags,  OFLAG_NOTAKE);
  obj->type = OTYPE_PORTAL;

  obj2->x = x2;
  obj2->y = y2;
  obj2->plane = p2;
  obj->v1 = x2;
  obj->v2 = y2;
  obj->v3 = p2;
  SET_BIT(obj2->oflags, OFLAG_NOTAKE);
  obj2->type = OTYPE_PORTAL;

  if (!buf[0])
    return;

  put_sense_area(x1, y1, p1, SENSE_SIGHT, buf);
  put_sense_area(x2, y2, p2, SENSE_SIGHT, buf);
}
