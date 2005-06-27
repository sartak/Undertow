#include "undertow.h"

/*
 * This is the worst file of code I have *ever* written.
 * It's definitely going into the archives for when I'm
 * feeling shitty and need something to cheer me up.
 */

struct social_data
{
  const char  *command;
  /* for vicless actions */
  char  *to_act;
  char  *to_notact;

  /* for vicful actions */
  char  *to_act_wvic;
  char  *to_vic;
  char  *to_notactvic;

  char  *to_act_obj;
  char  *to_notactobj;
};

const struct  social_data  soc_table [] =
{
  {
    "bow",
    "You bow #Bdeeply#n.",
    "$a bows #Bdeeply#n.",
    "You bow #Bdeeply#n at $v.",
    "$a bows #Bdeeply#n at you.",
    "$a bows #Bdeeply#n at $v.",
    "You bow #Bdeeply#n at $v.",
    "$a bows #Bdeeply#n at $v.",
  },
  {
    "cwd",
    "You conjure a small #rdragon#n, made of #Ycream#n of #ywheat#n. It flies around the room spitting #mraisins#n.",
    "$a conjures a small #rdragon#n, made of #Ycream#n of #ywheat#n. It flies around the room spitting #mraisins#n.",
    "You conjure a small #rdragon#n, made of #Ycream#n of #ywheat#n. It flies around the room spitting #mraisins#n at $v.",
    "$a conjures a small #rdragon#n, made of #Ycream#n of #ywheat#n. It flies around the room spitting #mraisins#n at you.",
    "$a conjures a small #rdragon#n, made of #Ycream#n of #ywheat#n. It flies around the room spitting #mraisins#n at $v.",
    "You conjure a small #rdragon#n, made of #Ycream#n of #ywheat#n. It flies around the room spitting #mraisins#n at $v.",
    "$a conjures a small #rdragon#n, made of #Ycream#n of #ywheat#n. It flies around the room spitting #mraisins#n at $v.",
  },
  {
    "lag",
    "You wrestle with the deadliest creature of all.",
    "$a wrestles with Death himself: the lagbeast.",
    "",
    "",
    "",
    "",
    ""
  },
  {
    "frown",
    "You frown #Bdarkly#n.",
    "$a frowns #Bdarkly#n.",
    "You frown at $v.",
    "$a frowns at you.",
    "$a frowns at $v.",
    "You frown at $v.",
    "$a frowns at $v."
  },
  {
    "giggle",
    "You giggle #Ginsanely#n.",
    "$a giggles #Ginsanely#n.",
    "You giggle #Ginsanely#n at $v.",
    "$a giggles #Ginsanely#n at you.",
    "$a giggles #Ginsanely#n at $v.",
    "You giggle #Ginsanely#n $v.",
    "$a giggles #Ginsanely#n $v."
  },
  {
    "grin",
    "You grin #Revilly#n.",
    "$a grins #Revilly#n.",
    "You grin at $v with #Revil#n in your eyes.",
    "$a grins at you with #Revil#n in $c eyes.",
    "$a grins at $v with #Revil#n in $c eyes.",
    "You grin at $v.",
    "$a grins at $v."
  },
  {
    "kick",
    "",
    "",
    "You maliciously kick $v in the shin!",
    "$a maliciously kicks you in the shin!",
    "$a maliciously kicks $v in the shin!",
    "You kick $v and stub your toe.",
    "$a kicks $v, stubbing $c toe."
  },
  {
    "laugh",
    "You laugh heartily.",
    "$a laughs heartily.",
    "You laugh at $v.",
    "$a laughs at you!",
    "$a laughs at $v.",
    "You laugh at $v.",
    "$a laughs at $v."
  },
  {
    "listen",
    "You listen attentively.",
    "$a listens attentively.",
    "You listen carefully to $v.",
    "$a listens carefully to you.",
    "$a listens carefully to $v.",
    "You listen to $v's infinite wisdom.",
    "$a listens to $v's infinite wisdom."
  },
  {
    "nod",
    "You nod #Bsolemnly#n.",
    "$a nods #Bsolemnly#n.",
    "You #Cnod in agreement#n with $v.",
    "$a #Cnods in agreement#n with you.",
    "$a #Cnods in agreement#n with $v.",
    "You nod at $v.",
    "$a nods at $v."
  },
  {
    "point",
    "You point off in the distance.",
    "$a points off in the distance.",
    "You point at $v.",
    "$a points at you.",
    "$a points at $v.",
    "You point at $v.",
    "$a points at $v."
  },
  {
    "shake",
    "You shake your fist at the sky.",
    "$a shakes $c fist at the sky.",
    "You shake hands with $v.",
    "$a shakes hand with you.",
    "$a shakes hands with $v.",
    "You shake $v violently.",
    "$a shakes $v violently."
  },
  {
    "shakeh",
    "You shake your head #Bnegatively#n.",
    "$a shakes $c head #Bnegatively#n.",
    "You shake your head at $v.",
    "$a shakes $c head #Bnegatively#n at you.",
    "$a shakes $c head #Bnegatively#n at $v.",
    "You shake your head #Bnegatively#n at $v.",
    "$a shakes $c head #Bnegatively#n at $v."
  },
  {
    "shrug",
    "You shrug.",
    "$a shrugs helplessly.",
    "You shrug helplessly at $v.",
    "$a shrugs helplessly at you.",
    "$a shrugs helplessly at $v.",
    "You shrug at $v.",
    "$a shrugs at $v."
  },
  {
    "sigh",
    "You sigh #Bloudly#n.",
    "$a sighs #Bloudly#n.",
    "You sigh at $v.",
    "$a sigh at you.",
    "$a sighs at $v.",
    "You sigh at $v.",
    "$a sighs at $v."
  },
  {
    "slap",
    "",
    "",
    "You slap $v around a bit with a large #Ctrout#n.",
    "$a slaps you around a bit with a large #Ctrout#n.",
    "$a slaps $v around a bit with a large #Ctrout#n.",
    "You slap $v around a bit with a large #Ctrout#n.",
    "$a slaps $v around a bit with a large #Ctrout#n.",
  },
  {
    "smile",
    "You smile #Ghappily#n.",
    "$a smiles #Ghappily#n.",
    "You smile #Rwarmly#n at $v.",
    "$a smiles #Rwarmly#n at you.",
    "$a smiles #Rwarmly#n at $v.",
    "You smiles at $v.",
    "$a smiles at $v."
  },
  {
    "sneeze",
    "You sneeze obnoxiously.",
    "$a sneezes obnoxiously.",
    "You fire your nosejuice at $v.",
    "$a sneezes all over you. What a jerk!",
    "$a launches Snotrocket Gamma at $v.",
    "You sneeze all over $v.",
    "$a sneezes all over $v."
  },
  {
    "snicker",
    "You snicker softly.",
    "$a snickers softly.",
    "You snicker softly at $v.",
    "$a snickers softly at you.",
    "$a snickers softly at $v.",
    "You snicker softly at $v.",
    "$a snickers softly at $v."
  },
  {
    "thank",
    "",
    "",
    "You thank $v whole-heartedly.",
    "$a thanks you whole-heartedly.",
    "$a thanks $v whole-heartedly.",
    "You thank $v whole-heartedly.",
    "$a thanks $v whole-heartedly.",
  },
  {
    "wave",
    "You wave #Ghappily#n.",
    "$a waves #Ghappily#n.",
    "You wave at $v.",
    "$a waves at you.",
    "$a waves at $v.",
    "You wave $v around proudly.",
    "$a waves $v around proudly."
  },

  {
    "",
    "",
    "",
    "",
    "",
    ""
  },
};

void do_socials(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[32];
  int i = 0;
  int x = 0;
  int bestmatch = -1;
  int matchcount = 0;

  while (soc_table[i].command[0] != 0)
  {
    x = beginning(soc_table[i].command, arguments);
    if (x == strlen(arguments) && x > matchcount)
    {
      bestmatch = i;
      matchcount = x;
    }

    i++;
  }

  if (bestmatch == -1)
  {
    x = 0;
    send_to_char(ch, "\n#R+#r---------------------------------#1[#2Social#1]#r---------------------------------#R+\n");
    strlcpy(buf, "#r| ", MAX_STRING_LENGTH);
    for (i = 0; soc_table[i].command[0]; ++i)
    {
      if (x == 6)
      {
        x = 0;
        strlcat(buf, " #r|\n#r| ", MAX_STRING_LENGTH);
      }
      snprintf(buf2, 32, "%s%-11s ", x % 2 == 0 ? "#1" : "#2", soc_table[i].command);
      strlcat(buf, buf2, MAX_STRING_LENGTH);
      x++;
    }

    for (; x != 6; x++)
    {
      snprintf(buf2, 32, "%11s ", " ");
      strlcat(buf, buf2, MAX_STRING_LENGTH);
    }
    strlcat(buf, " #r|\n", MAX_STRING_LENGTH);
    send_to_char(ch, buf);
    send_to_char(ch, "#R+#r--------------------------------------------------------------------------#R+#n\n");
    return;
  }
  else
  {
    i = bestmatch;
    snprintf(buf, MAX_STRING_LENGTH, "Action: %s\nNext: %s\n", soc_table[i].command, soc_table[i+1].command[0] ? soc_table[i+1].command : "(none)");
    send_to_char(ch, buf);
    if (soc_table[i].to_act[0])
    {
      snprintf(buf, MAX_STRING_LENGTH, "To actor: %s\nTo area: %s\n", soc_table[i].to_act, soc_table[i].to_notact);
      send_to_char(ch, buf);
    }
    if (soc_table[i].to_act_wvic[0])
    {
      snprintf(buf, MAX_STRING_LENGTH, "To actor: %s\nTo victim: %s\nTo area: %s\n", soc_table[i].to_act_wvic, soc_table[i].to_vic, soc_table[i].to_notactvic);
      send_to_char(ch, buf);
    }
    if (soc_table[i].to_act_obj[0])
    {
      snprintf(buf, MAX_STRING_LENGTH, "With object, to actor: %s\nWith object, to area: %s\n", soc_table[i].to_act_obj, soc_table[i].to_notactobj);
      send_to_char(ch, buf);
    }
    return;
  }
}

/* return value: 1 = social found, 0 = social not found */
int parse_social(CHAR_DATA *ch, char *verb, char *arguments)
{
  int i = 0;
  int x = 0;
  int bestmatch = -1;
  int matchcount = 0;
  CHAR_DATA *vic;
  OBJECT_DATA *obj;

  while (soc_table[i].command[0] != 0)
  {
    x = beginning(soc_table[i].command, verb);
    if (x == strlen(verb) && x > matchcount)
    {
      bestmatch = i;
      matchcount = x;
    }

    i++;
  }

  if (bestmatch == -1)
    return 0;

  vic = find_char_world(ch, arguments);
  obj = find_obj(ch, arguments, "f*iaw");

  if (soc_table[bestmatch].to_act[0] == '\0' &&
     (soc_table[bestmatch].to_act_wvic[0] == '\0' || vic == NULL) &&
     (soc_table[bestmatch].to_act_obj[0] == '\0' || obj == NULL))
  {
    send_to_char(ch, "This action requires a target.\n");
    return 1;
  }

  if ((soc_table[bestmatch].to_act_wvic[0] == '\0' || vic == NULL) && (soc_table[bestmatch].to_act_obj[0] == '\0' || obj == NULL))
  {
    act(soc_table[bestmatch].to_act, ch, NULL, TO_ACTOR + TYPE_ACT_CHAR + TYPE_VIC_NULL, SENSE_SIGHT);
    act(soc_table[bestmatch].to_notact, ch, NULL, TO_NOTACTVIC + TYPE_ACT_CHAR + TYPE_VIC_NULL, SENSE_SIGHT);
    return 1;
  }

  if (vic == NULL || soc_table[bestmatch].to_act_wvic[0] == '\0')
  {
    act(soc_table[bestmatch].to_act_obj, ch, obj, TO_ACTOR + TYPE_ACT_CHAR + TYPE_VIC_OBJ, SENSE_SIGHT);
    act(soc_table[bestmatch].to_notactobj, ch, obj, TO_NOTACTVIC + TYPE_ACT_CHAR + TYPE_VIC_OBJ, SENSE_SIGHT);
    return 1;
  }

  act(soc_table[bestmatch].to_act_wvic, ch, vic, TO_ACTOR + TYPE_ACT_CHAR + TYPE_VIC_CHAR, SENSE_SIGHT);
  act(soc_table[bestmatch].to_vic, ch, vic, TO_VICTIM + TYPE_ACT_CHAR + TYPE_VIC_CHAR, SENSE_SIGHT);
  act(soc_table[bestmatch].to_notactvic, ch, vic, TO_NOTACTVIC + TYPE_ACT_CHAR + TYPE_VIC_CHAR, SENSE_SIGHT);
  return 1;
}

/* message variables
    $$ -> $

    $a -> actor name
    $b -> actor pronoun subjective (he/it)
    $c -> actor pronoun possessive (his/its)
    $d -> actor pronoun objective (him/it)

    $v -> victim name
    $e -> victim pronoun subjective (he/it)
    $f -> victim pronoun possessive (his/its)
    $g -> victim pronoun objective (him/it)
 */

void act(char *msg, void *actor, void *victim, int type, int sense)
{
  char buf[MAX_STRING_LENGTH];
  char *p;
  int o = 0;
  int a = TYPE_CHAR, v = TYPE_CHAR;
  CHAR_DATA *ac, *vc;
  OBJECT_DATA *ao, *vo;

  if (IS_SET(type, TYPE_ACT_CHAR))
  {
    a = TYPE_CHAR;
    ac = (CHAR_DATA *) actor;
  }
  if (IS_SET(type, TYPE_ACT_OBJ))
  {
    a = TYPE_OBJ;
    ao = (OBJECT_DATA *) actor;
  }
  if (IS_SET(type, TYPE_ACT_NULL))
    a = TYPE_NULL;

  if (IS_SET(type, TYPE_VIC_CHAR))
  {
    v = TYPE_CHAR;
    vc = (CHAR_DATA *) victim;
  }
  if (IS_SET(type, TYPE_VIC_OBJ))
  {
    v = TYPE_OBJ;
    vo = (OBJECT_DATA *) victim;
  }
  if (IS_SET(type, TYPE_VIC_NULL))
    v = TYPE_NULL;

  for (p = msg; *p != 0; ++p)
  {
    if (o + 100 > MAX_STRING_LENGTH)
    {
      syslog("WARNING", "act buf exceeds max string length");
      buf[o] = 0;
      return;
    }

    if (*p != '$')
      buf[o++] = *p;
    else
    {
      p++;
      switch (*p)
      {
        default:
          p--;
        case '$':
          buf[o++] = '$';
        break;
        case 'a':
          buf[o] = 0;
          if (a == TYPE_CHAR)
          {
            strlcat(buf, ac->name, MAX_STRING_LENGTH);
            o += strlen(ac->name);
          }
          else if (a == TYPE_OBJ)
          {
            strlcat(buf, ao->shortdesc, MAX_STRING_LENGTH);
            o += strlen(ao->shortdesc);
          }
        break;
        case 'v':
          buf[o] = 0;
          if (v == TYPE_CHAR)
          {
            if (vc == ac)
            {
              if (IS_SET(type, TO_ACTOR))
              {
                strlcat(buf, "yourself", MAX_STRING_LENGTH);
                o += 8;
              }
              else
              {
                strlcat(buf, "himself", MAX_STRING_LENGTH);
                o += 7;
              }
            }
            else
            {
              strlcat(buf, vc->name, MAX_STRING_LENGTH);
              o += strlen(vc->name);
            }
          }
          else if (v == TYPE_OBJ)
          {
            strlcat(buf, vo->shortdesc, MAX_STRING_LENGTH);
            o += strlen(vo->shortdesc);
          }
        break;
        case 'b':
          buf[o] = 0;
          if (a == TYPE_CHAR)
          {
            strlcat(buf, "he", MAX_STRING_LENGTH);
            o += 2;
          }
          else if (a == TYPE_OBJ)
          {
            strlcat(buf, "it", MAX_STRING_LENGTH);
            o += 2;
          }
        break;
        case 'c':
          buf[o] = 0;
          if (a == TYPE_CHAR)
          {
            strlcat(buf, "his", MAX_STRING_LENGTH);
            o += 3;
          }
          else if (a == TYPE_OBJ)
          {
            strlcat(buf, "its", MAX_STRING_LENGTH);
            o += 3;
          }
        break;
        case 'd':
          buf[o] = 0;
          if (a == TYPE_CHAR)
          {
            strlcat(buf, "him", MAX_STRING_LENGTH);
            o += 3;
          }
          else if (a == TYPE_OBJ)
          {
            strlcat(buf, "it", MAX_STRING_LENGTH);
            o += 2;
          }
        break;
        case 'e':
          buf[o] = 0;
          if (v == TYPE_CHAR)
          {
            strlcat(buf, "he", MAX_STRING_LENGTH);
            o += 2;
          }
          else if (v == TYPE_OBJ)
          {
            strlcat(buf, "it", MAX_STRING_LENGTH);
            o += 2;
          }
        break;
        case 'f':
          buf[o] = 0;
          if (v == TYPE_CHAR)
          {
            strlcat(buf, "his", MAX_STRING_LENGTH);
            o += 3;
          }
          else if (v == TYPE_OBJ)
          {
            strlcat(buf, "its", MAX_STRING_LENGTH);
            o += 3;
          }
        break;
        case 'g':
          buf[o] = 0;
          if (v == TYPE_CHAR)
          {
            strlcat(buf, "him", MAX_STRING_LENGTH);
            o += 3;
          }
          else if (v == TYPE_OBJ)
          {
            strlcat(buf, "it", MAX_STRING_LENGTH);
            o += 2;
          }
        break;
      }
    }
  }

  buf[o++] = '\n';
  buf[o] = 0;

  if (IS_SET(type, TO_ACTOR) && a == TYPE_CHAR)
    put_sense(ac, sense, buf);

  if (IS_SET(type, TO_VICTIM) && v == TYPE_CHAR && vc != ac)
  {
    for (++o; o > 0; --o)
      buf[o] = buf[o-1];
    buf[0] = '\n';
    put_sense(vc, sense, buf);
  }

  if (IS_SET(type, TO_NOTACTOR))
  {
    if (a == TYPE_CHAR)
      put_sense_except(ac, ac->x, ac->y, ac->plane, sense, buf);
    else
      put_sense_area(ao->x, ao->y, ao->plane, sense, buf);
  }

  if (IS_SET(type, TO_NOTVICTIM))
  {
    if (a == TYPE_CHAR && v == TYPE_CHAR)
      put_sense_except(vc, ac->x, ac->y, ac->plane, sense, buf);
    else if (a == TYPE_OBJ && v == TYPE_CHAR)
      put_sense_except(vc, ao->x, ao->y, ao->plane, sense, buf);
    else if (a == TYPE_CHAR && v == TYPE_OBJ)
      put_sense_area(ac->x, ac->y, ac->plane, sense, buf);
    else if (a == TYPE_OBJ && v == TYPE_OBJ)
      put_sense_area(ao->x, ao->y, ao->plane, sense, buf);
  }

  if (IS_SET(type, TO_NOTACTVIC))
  {
    if (a == TYPE_CHAR && v == TYPE_CHAR)
      put_sense_except2(ac, vc, ac->x, ac->y, ac->plane, sense, buf);
    else if (a == TYPE_CHAR)
      put_sense_except(ac, ac->x, ac->y, ac->plane, sense, buf);
    else if (v == TYPE_CHAR)
      put_sense_except(vc, ao->x, ao->y, ao->plane, sense, buf);
    else
      put_sense_area(ao->x, ao->y, ao->plane, sense, buf);
  }
}
