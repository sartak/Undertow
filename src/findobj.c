#include "undertow.h"

/* begin Atreides' hack */
static unsigned int which = 0; /* to loop which over multiple find_in_* calls */
static unsigned int cur = 0; /* to remember where I really am */
/*
  Given a string such as below, return a pointer to the keyword(s)
  and whichever one they want in the which variable.
  --------------
  |3.iron sword|
  --------------
  ^  ^
  |  \ finish
  \- start
*/
static char *find_which(char *args, unsigned int *which, char *where)
{
  char *p;
  char *q;
  int progress = 0; /* bitfield! */
  q = p = args;
  *which = *where = 0;

start:
  if ((*q|32) >= 'a' && (*q|32) <= 'z')
      goto letters;
  else if (*q >= '0' && *q <= '9')
      goto numbers;
  else if (*q == 0 || *q == '.')
      goto fwend;
  /*else*/
      goto fwerr;
letters:
    if (progress&1)
    {
        progress |= 2; /* second and final time */
        /* this SHOULD be the object */
        return q;
    }
    else
    {
        progress |= 1; /* hit letters for the first time */
        /* chew through letters */
        *where = *q;
        p = q; /* record starting place just incase it's just the obj */
        while ((*q|32) >= 'a' && (*q|32) <= 'z')
            q++;
    }
    goto start;
numbers:
    if (progress&4) /* hit numbers already once */
        goto fwerr;
    progress |= 4;
    while (*q >= '0' && *q <= '9')
    {
        *which *= 10;
        *which += *q - '0';
        q++;
    }
    goto start;
fwend:
  /* eat through them periods */
  while (*q == '.')
      q++;
  if ((progress&4) == 0)
      *which = 1;
  if (*q == 0 && (progress&1))
  {
    if (progress&4) /* found letters and numbers */
    {
      /*if (p == args)*/
        return q;
    }
    *where = 0;
    return p;
  }
  return q;
fwerr:
  return 0;
}

/* next word */
/* used within a nested loop and works fine... */
static void next_word(char *str, char *dst, char **last)
{
  if (str == 0 && (str = *last) == 0)
    return;
  /* loop through spaces */
  while (*str && *str == ' ')
    str++;
  /* copy string */
  while (*str && *str != ' ')
    *dst++ = *str++;
  /* set context pointer */
  *last = str;
  /* terminate string */
  *dst = 0;
}

/* determine if obj matches the keywords */
/* 2 = exact, 1 = inexact, 0 = no match */
static int find_match(OBJECT_DATA *obj, char *kw, int *perc)
{
  int x = 0;
  int akwlen;
  int mtotal;
  int arglen;
  char arg_kw[128]; /* OBJECT_DATA->keywords is currently 128 bytes */
  char obj_kw[128];
  char *c1, *c2;

  if (!stricmp(obj->keywords, kw))
    return 2;
  /*
    Not exact match, so find how many words in obj->keywords match
    against words in args, get the percentage and "return" it
  */
  /*
    What I want to do:
    - keep track of number of keywords in args that matches (must be 100%)
    - get the percentage of how many characters matched compared to
      the length of obj->keywords (yes I'm incorrectly including spaces)
  */
  next_word(kw, arg_kw, &c1);
  arglen = mtotal = 0;
  while (arg_kw[0] != 0)
  {
    akwlen = strlen(arg_kw);
    arglen += akwlen;
    next_word(obj->keywords, obj_kw, &c2);
    while (obj_kw[0] != 0)
    {
      x = beginning(obj_kw, arg_kw);
      if (x == akwlen) /* match */
      {
        mtotal += x;
        break; /* can't have it matching multiple keywords */
      }
      next_word(0, obj_kw, &c2);
    }
    next_word(0, arg_kw, &c1);
  }
  if (arglen == mtotal) /* matched everything */
  {
    /* it's *1000 so I can get a tenth of a percent */
    *perc = mtotal * 1000 / strlen(obj->keywords);
    return 1;
  }
  return 0;
}

static int find_in_inv(CHAR_DATA *ch, char *args, OBJECT_DATA **exact, OBJECT_DATA **other)
{
  /* try to cut down on number of stack variables */
  /* started at 13, down to 4 after spawning off a function */
  OBJECT_DATA *obj;
  int mperc = 0, m, perc = 0;
  int i, is_inv;
  *exact = *other = 0;

  /* yes, nutty way to keep track of inv */
  for (obj = ch->obj; obj != 0; obj = obj->next_local)
  {
    is_inv = 1;
    if (obj->container != NULL) /* we don't want contained items */
      continue;
    for (i = 0; i < EQUIP_MAX; ++i) /* we don't want weapons or armor */
      if (ch->equip[i] == obj)
      {
        is_inv = 0;
        break;
      }
    if (!is_inv)
      continue;
    m = find_match(obj, args, &perc);
    if (m == 1) /* inexact match */
    {
      if (which == 0) /* find best matching */
      {
        if (perc > mperc)
        {
          mperc = perc;
          *other = obj;
        }
      }
      else if (which == cur)
      {
        *other = obj;
        return 1;
      }
        cur++;
    }
    else if (m == 2) /* exact match */
    {
      if (which == 0 || which == cur)
      {
        *exact = obj;
        return 2;
      }
      cur++;
    }
  }
  return (other != 0) ? 1 : 0;
}
static int find_in_wpn(CHAR_DATA *ch, char *args, OBJECT_DATA **exact, OBJECT_DATA **other)
{
  /* try to cut down on number of stack variables */
  /* started at 13, down to 4 after spawning off a function */
  OBJECT_DATA *obj;
  int mperc = 0, m, perc = 0;
  *exact = *other = 0;

  /* yes, nutty way to keep track of inv */
  for (obj = ch->obj; obj != 0; obj = obj->next_local)
  {
    if (ch->equip[EQUIP_LHAND] != obj && ch->equip[EQUIP_RHAND] != obj) /* todo: check if shields */
      continue;
    m = find_match(obj, args, &perc);
    if (m == 1) /* inexact match */
    {
      if (which == 0) /* find best matching */
      {
        if (perc > mperc)
        {
          mperc = perc;
          *other = obj;
        }
      }
      else if (which == cur)
      {
        *other = obj;
        return 1;
      }
        cur++;
    }
    else if (m == 2) /* exact match */
    {
      if (which == 0 || which == cur)
      {
        *exact = obj;
        return 2;
      }
      cur++;
    }
  }
  return (other != 0) ? 1 : 0;
}
static int find_in_arm(CHAR_DATA *ch, char *args, OBJECT_DATA **exact, OBJECT_DATA **other)
{
  /* try to cut down on number of stack variables */
  /* started at 13, down to 4 after spawning off a function */
  OBJECT_DATA *obj;
  int mperc = 0, m, perc = 0;
  int i, is_eq;
  *exact = *other = 0;

  /* yes, nutty way to keep track of inv */
  for (obj = ch->obj; obj != 0; obj = obj->next_local)
  {
    is_eq = 0;
    for (i = 0; i < EQUIP_MAX; ++i)
    {
      if (i == EQUIP_LHAND || i == EQUIP_RHAND) /* todo: check if shields */
        continue;
      if (ch->equip[i] == obj)
      {
        is_eq = 1;
        break;
      }
    }
    if (!is_eq)
      continue;
    m = find_match(obj, args, &perc);
    if (m == 1) /* inexact match */
    {
      if (which == 0) /* find best matching */
      {
        if (perc > mperc)
        {
          mperc = perc;
          *other = obj;
        }
      }
      else if (which == cur)
      {
        *other = obj;
        return 1;
      }
        cur++;
    }
    else if (m == 2) /* exact match */
    {
      if (which == 0 || which == cur)
      {
        *exact = obj;
        return 2;
      }
      cur++;
    }
  }
  return (other != 0) ? 1 : 0;
}
static int find_in_flr(CHAR_DATA *ch, char *args, OBJECT_DATA **exact, OBJECT_DATA **other, long limit)
{
  OBJECT_DATA *obj;
  int mperc = 0, m, perc = 0;
  int plane = 0, mud = 0, dist = 0;

  *exact = *other = 0;

  for (obj = objhead; obj; obj = obj->next)
  {
    if (limit < 0)
      plane = 1;
    if (limit == -2)
      mud = 1;
    dist = (obj->x - ch->x) * (obj->x - ch->x) + (obj->y - ch->y) * (obj->y - ch->y) <= limit * limit;
    if (obj->plane != ch->plane && !mud)
      continue;
    if (!dist && !plane)
      continue;
    if (obj->holder != NULL || obj->container != NULL)
      continue;
    m = find_match(obj, args, &perc);
    if (m == 1) /* inexact match */
    {
      if (which == 0) /* find best matching */
      {
        if (perc > mperc)
        {
          mperc = perc;
          *other = obj;
        }
      }
      else if (which == cur)
      {
        *other = obj;
        return 1;
      }
        cur++;
    }
    else if (m == 2) /* exact match */
    {
      if (which == 0 || which == cur)
      {
        *exact = obj;
        return 2;
      }
      cur++;
    }
  }
  return (other != 0) ? 1 : 0;
}

/*
  syntax for arguments:
  [which.]object
*/
OBJECT_DATA *find_obj(CHAR_DATA *ch, char *arguments, char *order)
{
  OBJECT_DATA *match = 0;
  OBJECT_DATA *inexact = 0;
  OBJECT_DATA *other;
  char *p;
  char *w;
  char where;
  long limit = 0;
  p = 0;

  if (arguments[0] == 0)
    return NULL;

  arguments[127] = 0;
  w = find_which(arguments, &which, &where);
  if (w == 0)
    return 0;
  cur = 1;

  if (where)
  {
    for (p = order; *p; p++)
      if (*p == where)
      {
        p = &where;
        break;
      }
  }
  if (p == 0)
    p = order;

  for (; *p; p++)
  {
    /* NOTE: if you want to check inv 14 million times, it will */
    switch (*p)
    {
      case 'a': /* armor */
        if (find_in_arm(ch, w, &match, (inexact == 0) ? &inexact : &other) >= 2)
          return match;
        break;
      case 'i': /* inventory */
        if (find_in_inv(ch, w, &match, (inexact == 0) ? &inexact : &other) >= 2)
          return match;
        break;
      case 'f': /* floor */
        p++;
        /* find the limit */
        if (*p == '*')
          limit = ch->sightradius;
        else if (*p)
          limit = strtol(p, 0, 10);
        if (find_in_flr(ch, w, &match, (inexact == 0) ? &inexact : &other, limit) >= 2)
          return match;
        break;
      case 'w': /* weapon */
        if (find_in_wpn(ch, w, &match, (inexact == 0) ? &inexact : &other) >= 2)
          return match;
        break;
      default:
        if (*p < '0' && *p > '9')
          syslog("WARNING", "[find_obj] Unknown order parameter: %c, from [%s]", *p, order);
        break;
    }
  }

  return inexact;
}
