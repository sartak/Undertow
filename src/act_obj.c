#include "undertow.h"

#define DESTROY_TIME (TICKS_PER_SECOND * 60 * 5)

void take_obj(CHAR_DATA *ch, OBJECT_DATA *obj)
{
  char buf[MAX_STRING_LENGTH];

  obj->holder = ch;
  obj->next_local = ch->obj;
  ch->obj = obj;
  destroy_obj_events(obj, EVENT_DESTROY);

  strlcpy(buf, "You take ", MAX_STRING_LENGTH);
  strlcat(buf, obj->shortdesc, MAX_STRING_LENGTH);
  strlcat(buf, ".\n", MAX_STRING_LENGTH);
  send_to_char(ch, buf);

  strlcpy(buf, ch->name, MAX_STRING_LENGTH);
  strlcat(buf, " takes ", MAX_STRING_LENGTH);
  strlcat(buf, obj->shortdesc, MAX_STRING_LENGTH);
  strlcat(buf, ".", MAX_STRING_LENGTH);
  put_sense_except(ch, ch->x, ch->y, ch->plane, SENSE_SIGHT, buf);
}

void do_take(CHAR_DATA *ch, char *arguments)
{
  OBJECT_DATA *obj;
  char buf[MAX_STRING_LENGTH];
  int num = 0;

  if (!stricmp(arguments, "all"))
  {
    for (obj = objhead; obj; obj = obj->next)
    {
      if (obj->plane != ch->plane || obj->x != ch->x || obj->y != ch->y)
        continue;
      if (obj->holder != NULL || obj->container != NULL)
        continue;
      if (IS_SET(obj->oflags, OFLAG_NOTAKE))
        continue;
      take_obj(ch, obj);
      ++num;
    }

    if (num == 0)
      send_to_char(ch, "There's nothing here eligible for you to pick up.\n");
    else
      put_sense_except(ch, ch->x, ch->y, ch->plane, SENSE_SIGHT, ""); /* get the trailing \n */
    return;
  }

  obj = find_obj(ch, arguments, "f0");
  if (obj == NULL)
  {
    obj = find_obj(ch, arguments, "f*");
    if (obj == NULL)
      send_to_char(ch, "Take what, exactly?\n");
    else
      send_to_char(ch, "You must be at the same coordinates of an object to take it.\n");
    return;
  }
  if (obj->type == OTYPE_PORTAL || IS_SET(obj->oflags, OFLAG_NOTAKE))
  {
    send_to_char(ch, "You can't pick that up.\n");
    return;
  }

  take_obj(ch, obj);
  put_sense_except(ch, ch->x, ch->y, ch->plane, SENSE_SIGHT, ""); /* get the trailing \n */
}

void drop_obj(CHAR_DATA *ch, OBJECT_DATA *obj)
{
  char buf[MAX_STRING_LENGTH];
  int t = DESTROY_TIME;
  EVENT_DATA *event = find_event_obj(obj, EVENT_UNMARK);
  if (event)
    t += event->ticks_left;
  obj->next_local = NULL;
  obj->holder = NULL;
  obj->x = ch->x;
  obj->y = ch->y;
  obj->plane = ch->plane;
  setup_event_obj(obj, NULL, event_destroy, "", t, EVENT_DESTROY);

  strlcpy(buf, "You drop ", MAX_STRING_LENGTH);
  strlcat(buf, obj->shortdesc, MAX_STRING_LENGTH);
  strlcat(buf, ".\n", MAX_STRING_LENGTH);
  send_to_char(ch, buf);

  strlcpy(buf, ch->name, MAX_STRING_LENGTH);
  strlcat(buf, " drops ", MAX_STRING_LENGTH);
  strlcat(buf, obj->shortdesc, MAX_STRING_LENGTH);
  strlcat(buf, ".", MAX_STRING_LENGTH);
  put_sense_except(ch, ch->x, ch->y, ch->plane, SENSE_SIGHT, buf);
}

void do_drop(CHAR_DATA *ch, char *arguments)
{
  OBJECT_DATA *obj, *tempobj;
  int i;

  if (ch->obj == NULL)
  {
    send_to_char(ch, "You have nothing to drop.\n");
    return;
  }

  if (!stricmp(arguments, "all"))
  {
    for (i = 0; i < EQUIP_MAX; ++i)
      if (ch->equip[i])
        ch->equip[i] = NULL;

    obj = ch->obj;
    while (obj)
    {
      tempobj = obj->next_local;
      drop_obj(ch, obj);
      obj = tempobj;
    }

    ch->obj = NULL;

    put_sense_except(ch, ch->x, ch->y, ch->plane, SENSE_SIGHT, ""); /* get the trailing \n */
    return;
  }

  obj = find_obj(ch, arguments, "iwa");
  if (obj == NULL)
  {
    send_to_char(ch, "You're not carrying that item.\n");
    return;
  }

  for (i = 0; i < EQUIP_MAX; ++i)
    if (ch->equip[i] == obj)
      ch->equip[i] = NULL;

  tempobj = ch->obj;
  if (tempobj == obj)
    ch->obj = obj->next_local;
  else
  {
    while (tempobj->next_local != obj)
      tempobj = tempobj->next_local;
    tempobj->next_local = tempobj->next_local->next_local;
  }

  drop_obj(ch, obj);
  put_sense_except(ch, ch->x, ch->y, ch->plane, SENSE_SIGHT, ""); /* get the trailing \n */
}

void do_mark(CHAR_DATA *ch, char *arguments)
{
  OBJECT_DATA *obj;
  char buf[MAX_STRING_LENGTH];

  obj = find_obj(ch, arguments, "iwa");

  if (obj == NULL)
  {
    send_to_char(ch, "Mark what?\n");
    return;
  }

  if (IS_SET(obj->tflags, OTFLAG_MARKED))
  {
    destroy_obj_events(obj, EVENT_UNMARK);
    snprintf(buf, MAX_STRING_LENGTH, "You unmark %s.\n", obj->shortdesc);
  }
  else
  {
    setup_event_obj(obj, NULL, event_unmark, "", DESTROY_TIME * 3, EVENT_UNMARK);
    snprintf(buf, MAX_STRING_LENGTH, "You mark %s, prolonging its existence.\n", obj->shortdesc);
  }

  send_to_char(ch, buf);
  TOGGLE_BIT(obj->tflags, OTFLAG_MARKED);
}

void do_identify(CHAR_DATA *ch, char *arguments)
{
  OBJECT_DATA *obj;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  EVENT_DATA *event;
  obj = find_obj(ch, arguments, "iwaf*");

  if (obj == NULL)
  {
    send_to_char(ch, "Identify what?\n");
    return;
  }

  send_to_char(ch, "\n#R+#r--------------------------#1[#2Identification#1]#r--------------------------#R+\n");

  decolorize(obj->shortdesc, buf);
  snprintf(buf2, MAX_STRING_LENGTH, "#1[#R%s#1]", buf);
  snprintf(buf,  MAX_STRING_LENGTH, "#r| #nObject: %-64s #r|\n", buf2);
  send_to_char(ch, buf);
  if (obj->owner[0])
  {
    snprintf(buf2, MAX_STRING_LENGTH, "#1[#R%s#1]", obj->owner);
    snprintf(buf,  MAX_STRING_LENGTH, "#r| #nOwner:  %-64s #r|\n", buf2);
  }
  else
    strlcpy(buf, "#r| #nOwner:  #1[#RNobody!#1]                                                  #r|\n", MAX_STRING_LENGTH);
  send_to_char(ch, buf);

  snprintf(buf2, MAX_STRING_LENGTH, "#1[#R%d#1]", obj->level);
  snprintf(buf,  MAX_STRING_LENGTH, "#r| #nLevel:  %-64s #r|\n", buf2);
  send_to_char(ch, buf);
  switch(obj->type)
  {
    case OTYPE_NONE:
      break;
    default:
      strlcpy(buf, "#r| #nType:   #1[#RUnknown#1]                                                  #r|\n", MAX_STRING_LENGTH);
      send_to_char(ch, buf);
      break;
    case OTYPE_PORTAL:
      strlcpy(buf, "#r| #nType:   #1[#RPortal#1]                                                   #r|\n", MAX_STRING_LENGTH);
      send_to_char(ch, buf);
      break;
    case OTYPE_WEAPON:
      strlcpy(buf, "#r| #nType:   #1[#RWeapon#1]                                                   #r|\n", MAX_STRING_LENGTH);
      send_to_char(ch, buf);
      break;
    case OTYPE_ARMOR:
      strlcpy(buf, "#r| #nType:   #1[#RArmor#1]                                                    #r|\n", MAX_STRING_LENGTH);
      send_to_char(ch, buf);
      break;
  }
  event = find_event_obj(obj, EVENT_UNMARK);
  if (event)
  {
    time_words(event->ticks_left / TICKS_PER_SECOND, buf);
    snprintf(buf2, MAX_STRING_LENGTH, "#1[#R%s#1]", buf);
    snprintf(buf,  MAX_STRING_LENGTH, "#r| #nMark:   %-64s #r|\n", buf2);
    send_to_char(ch, buf);
  }
  send_to_char(ch, "#R+#r--------------------------------------------------------------------#R+#n\n");
}

void do_inventory(CHAR_DATA *ch, char *arguments)
{
  OBJECT_DATA *obj;
  int i = 0;
  CHAR_DATA *vic;

  if (ch->level < LEVEL_IMMORTAL || arguments[0] == '\0')
    vic = ch;
  else
  {
    vic = find_char_world(ch, arguments);

    if (vic == NULL)
    {
      send_to_char(ch, "I'm afraid I cannot find that player.\n");
      return;
    }
  }

  if (ch != vic)
  {
    send_to_char(ch, vic->name);
    send_to_char(ch, " is carrying:");
  }
  else
    send_to_char(ch, "You are carrying:");

  for (obj = vic->obj; obj; obj = obj->next_local)
  {
    ++i;
    send_to_char(ch, "\n  ");
    send_to_char(ch, obj->shortdesc);
  }
  if (i == 0)
    send_to_char(ch, " Nothing!\n");
  else
    send_to_char(ch, "\n");
}

void do_equipment(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];

  snprintf(buf, MAX_STRING_LENGTH,
    "#r[#1Right Finger#r]#n %s\n"
    "#r[#1Left Finger #r]#n %s\n"
    "#r[#1Neck        #r]#n %s\n"
    "#r[#1Right Hand  #r]#n %s\n"
    "#r[#1Left Hand   #r]#n %s\n"
    "#r[#1Arms        #r]#n %s\n"
    "#r[#1Feet        #r]#n %s\n"
    "#r[#1Legs        #r]#n %s\n"
    "#r[#1Torso       #r]#n %s\n"
    "#r[#1Head        #r]#n %s\n",
    ch->equip[EQUIP_RFINGER] ? ch->equip[EQUIP_RFINGER]->shortdesc : "nothing",
    ch->equip[EQUIP_LFINGER] ? ch->equip[EQUIP_LFINGER]->shortdesc : "nothing",
    ch->equip[EQUIP_NECK]    ? ch->equip[EQUIP_NECK]->shortdesc    : "nothing",
    ch->equip[EQUIP_RHAND]   ? ch->equip[EQUIP_RHAND]->shortdesc   : "nothing",
    ch->equip[EQUIP_LHAND]   ? ch->equip[EQUIP_LHAND]->shortdesc   : "nothing",
    ch->equip[EQUIP_ARMS]    ? ch->equip[EQUIP_ARMS]->shortdesc    : "nothing",
    ch->equip[EQUIP_FEET]    ? ch->equip[EQUIP_FEET]->shortdesc    : "nothing",
    ch->equip[EQUIP_LEGS]    ? ch->equip[EQUIP_LEGS]->shortdesc    : "nothing",
    ch->equip[EQUIP_TORSO]   ? ch->equip[EQUIP_TORSO]->shortdesc   : "nothing",
    ch->equip[EQUIP_HEAD]    ? ch->equip[EQUIP_HEAD]->shortdesc    : "nothing");

  send_to_char(ch, buf);
}

void do_invwipe(CHAR_DATA *ch, char *arguments)
{
  OBJECT_DATA *obj, *temp;
  int i;
  CHAR_DATA *vic;
  if (arguments[0] == '\0')
  {
    vic = ch;
  }
  else
  {
    vic = find_char_world(ch, arguments);

    if (vic == NULL)
    {
      send_to_char(ch, "I'm afraid I cannot find that player.\n");
      return;
    }
    if (vic->level > ch->level)
    {
      send_to_char(ch, "I'm telling on you!\n");
      syslog("INVWIPE", "%s tried to invwipe his superior %s!", ch->name, vic->name);
      return;
    }
  }

  obj = vic->obj;
  while (obj)
  {
    temp = obj->next_local;
    unlink_object(obj);
    free_object(obj);
    obj = temp;
  }
  vic->obj = NULL;

  for (i = 0; i < EQUIP_MAX; ++i)
    if (vic->equip[i])
    {
      unlink_object(vic->equip[i]);
      free_object(vic->equip[i]);
      vic->equip[i] = NULL;
    }

  send_to_char(ch, "Done.\n");
  if (ch != vic)
  {
    send_to_char(vic, "\nYou feel as though a great burden were lifted from you.\n");
    syslog("INVWIPE", "%s invwiped %s.", ch->name, vic->name);
  }
  else
    syslog("INVWIPE", "%s invwiped himself.", ch->name);
}

void do_ocreate(CHAR_DATA *ch, char *arguments)
{
  int level, c;
  OBJECT_DATA *obj;
  char buf[MAX_STRING_LENGTH];

  one_arg(arguments, buf);
  level = atoi(buf);
  if (level <= 0)
  {
    send_to_char(ch, "The command is: ocreate [level] [name] where level is the radial distance of the object's generation, and name is optional.\n");
    return;
  }

  if (arguments[0] != '\0')
  {
    if (!stricmp(arguments, "all"))
    {
      send_to_char(ch, "What do you think this is, the goon show?\n");
      return;
    }
    for (c = 0; c < strlen(arguments); ++c)
      if ((strlen(arguments) > 2 && strlen(arguments) < 40) &&
          (arguments[c] == ' ' ||
          (arguments[c] >= 'a' && arguments[c] <= 'z') ||
          (arguments[c] >= 'A' && arguments[c] <= 'Z'))) {} else
      {
        send_to_char(ch, "Malformed object name.\n");
        return;
      }
  }
  obj = new_object();
  link_object(obj);
  obj->holder = ch;
  obj->next_local = ch->obj;
  ch->obj = obj;

  if (arguments[0] == '\0')
  {
    strlcpy(obj->keywords, "ball protoplasm", 128);
    strlcpy(obj->shortdesc, "a ball of protoplasm", 128);
    strlcpy(obj->longdesc, "A ball of protoplasm merrily floats", 128);
  }
  else
  {
    strlcpy(obj->keywords, arguments, 128);
    strlcpy(obj->shortdesc, arguments, 128);
    strlcpy(obj->longdesc, arguments, 128);
    strlcat(obj->longdesc, " flollops", 128);
  }
  strlcpy(obj->owner, ch->name, 32);
  obj->x = ch->x;
  obj->y = ch->y;
  obj->plane = ch->plane;
  obj->type = OTYPE_NONE;
  obj->level = level;
  snprintf(buf, MAX_STRING_LENGTH, "You summon %s.\n", obj->shortdesc);
  send_to_char(ch, buf);
  snprintf(buf, MAX_STRING_LENGTH, "%s summons %s.\n", ch->name, obj->shortdesc);
  put_sense_except(ch, ch->x, ch->y, ch->plane, SENSE_SIGHT, buf);
}

void do_recall(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  OBJECT_DATA *obj;

  if (ch->x == 0 && ch->y == 0)
  {
    send_to_char(ch, "You're already at the origin.\n");
    return;
  }
  if ((ch->x * ch->x) + (ch->y * ch->y) <= 100)
  {
    send_to_char(ch, "You're already pretty close to the origin.\n");
    return;
  }
  for (obj = objhead; obj; obj = obj->next)
    if (obj->type == OTYPE_PORTAL && !strcmp(obj->owner, ch->name))
    {
      send_to_char(ch, "You already have a recall portal open.\n");
      return;
    }

  snprintf(buf, MAX_STRING_LENGTH, "%d %d %d", ch->x, ch->y, ch->plane);
  setup_event(ch, NULL, event_portal, buf, TICKS_PER_SECOND * 4, EVENT_PORTAL);
  send_to_char(ch, "You begin summoning a portal.\n");
}

void do_enter(CHAR_DATA *ch, char *arguments)
{
  OBJECT_DATA *obj;
  EVENT_DATA *event;
  obj = find_obj(ch, arguments, "f0");

  if (find_event(ch, EVENT_CHANGEPOS))
  {
    send_to_char(ch, "You're too busy to enter anything.\n");
    return;
  }

  if (obj == NULL)
  {
    obj = find_obj(ch, arguments, "f*");
    if (obj == NULL)
      send_to_char(ch, "Enter what, exactly?\n");
    else
      send_to_char(ch, "You must be at the same coordinates of an object to enter it.\n");
    return;
  }
  if (obj->type != OTYPE_PORTAL)
  {
    send_to_char(ch, "It looks a bit cramped in there.\n");
    return;
  }

  act("You jump into $v.", ch, obj, TO_ACTOR + TYPE_ACT_CHAR + TYPE_VIC_OBJ, SENSE_SIGHT);
  act("$a jumps into $v.", ch, obj, TO_NOTACTVIC + TYPE_ACT_CHAR + TYPE_VIC_OBJ, SENSE_SIGHT);

  setup_event(ch, NULL, event_message, "\n#RChaos engulfs you...#n\n", TICKS_PER_SECOND * 3, EVENT_MESSAGE);

  event = setup_event(ch, NULL, event_exitportal, "", TICKS_PER_SECOND * 6, EVENT_EXITPORTAL);
  event->args[0] = obj->v1;
  event->args[1] = obj->v2;
  event->args[2] = obj->v3;
  event->victim.obj = obj;

  ch->x = 0;
  ch->y = 0;
  ch->plane = obj->v3 == ch->plane ? PLANE_INTRAPLANE : PLANE_INTERPLANE;
  update_objects(ch);
}
