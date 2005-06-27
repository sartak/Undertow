#include "undertow.h"

long fread_number(FILE *fp)
{
  char c;
  char buf[32];
  int pos = 1;
  int ret;

  if (feof(fp))
    return 0;

  buf[0] = ' ';

  do
  {
    c = fgetc(fp);
  } while (c == ' ' || c == '\n' || c == '\r');

  while (!feof(fp))
  {
    if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' ||
        c == '5' || c == '5' || c == '6' || c == '7' || c == '8' ||
        c == '9')
          buf[pos++] = c;
    else if (c == '-')
      buf[0] = '-';
    else
      break;

    c = fgetc(fp);
  }

  buf[pos] = 0;
  ret = atoi(buf);
  return ret;
}

void fread_word(FILE *fp, char *buf)
{
  char c;
  int pos = 0;

  if (feof(fp))
  {
    buf[0] = 0;
    return;
  }

  do
  {
    c = fgetc(fp);
  } while (c == ' ');

  while (!feof(fp))
  {
    if (c == ' ' || c == '\n' || c == '\r')
      break;
    buf[pos++] = c;
    if (c == '~')
      break;
    c = fgetc(fp);
  }

  buf[pos] = 0;
}

void fread_line(FILE *fp, char *buf)
{
  char c;
  int pos = 0;

  if (feof(fp))
  {
    buf[0] = 0;
    return;
  }

  do
  {
    c = fgetc(fp);
  } while (c == ' ');

  while (!feof(fp))
  {
    if (c == '\n' || c == '\r')
      break;
    buf[pos++] = c;
    if (c == '~')
      break;
    c = fgetc(fp);
  }

  buf[pos] = 0;
}

void fread_eof(FILE *fp, char *buf)
{
  char c;
  int pos = 0;

  if (feof(fp))
  {
    buf[0] = 0;
    return;
  }

  while (!feof(fp) && c)
  {
    c = fgetc(fp);
    buf[pos++] = c;
  }

  buf[pos] = 0;
}

/* pwipe: add account versioning! */

int char_exists(char *input)
{
  char file[64];
  FILE *fp;
  int x = 1;
  snprintf(file, 64, "account/%s.chr", input);
  to_lower(file);
  fp = fopen(file, "r");

  if (fp == NULL)
    x = 0;
  else
   fclose(fp);

  return x;
}

OBJECT_DATA *load_obj(FILE *fp, CHAR_DATA *ch)
{
  char buf[MAX_STRING_LENGTH];
  OBJECT_DATA *obj = new_object();
  link_object(obj);

  while (!feof(fp))
  {
    fread_word(fp, buf);
    if (!strcmp(buf, "EOO"))
      break;

    if (!strcmp(buf, "--Keywords"))
    {
      fread_line(fp, buf);
      strlcpy(obj->keywords, buf, 128);
      continue;
    }

    if (!strcmp(buf, "--Shortdesc"))
    {
      fread_line(fp, buf);
      strlcpy(obj->shortdesc, buf, 128);
      continue;
    }

    if (!strcmp(buf, "--Longdesc"))
    {
      fread_line(fp, buf);
      strlcpy(obj->longdesc, buf, 128);
      continue;
    }

    if (!strcmp(buf, "--Owner"))
    {
      fread_word(fp, buf);
      strlcpy(obj->owner, buf, 32);
      continue;
    }

    if (!strcmp(buf, "--Level"))
    {
      obj->level = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "--Flags"))
    {
      obj->oflags = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "--Type"))
    {
      obj->type = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "--Values"))
    {
      obj->v1 = fread_number(fp);
      obj->v2 = fread_number(fp);
      obj->v3 = fread_number(fp);
      obj->v4 = fread_number(fp);
      obj->v5 = fread_number(fp);
      obj->v6 = fread_number(fp);
      continue;
    }

    errorf("Unknown object field: [%s] in %s's playerfile.", buf, ch->name);
  }

  return obj;
}

CHAR_DATA *load_character(ACCOUNT_DATA *acc, char *name)
{
  char buf[MAX_STRING_LENGTH];
  FILE *fp;
  char file[64];
  CHAR_DATA *ch;
  ALIAS_DATA *alias, *tempalias;

  snprintf(file, 64, "account/%s.chr", name);
  to_lower(file);

  ch = new_character();
  ch->name[0] = 0;

  fp = fopen(file, "r");
  if (fp == NULL)
    return ch;

  link_character(ch);
  fread_line(fp, buf);
  while (!feof(fp))
  {
    fread_word(fp, buf);
    if (!strcmp(buf, "EOF~"))
      break;

    if (!strcmp(buf, "Name"))
    {
      fread_word(fp, buf);
      strlcpy(ch->name, buf, 32);
      ch->name[0] = UPPER(ch->name[0]); /* pwipe: remove this */
      continue;
    }

    if (!strcmp(buf, "Prompt"))
    {
      fread_line(fp, buf);
      strlcpy(ch->prompt, buf, 256);
      continue;
    }

    if (!strcmp(buf, "Account"))
    {
      fread_word(fp, buf);
      strlcpy(ch->accname, buf, 32);
      continue;
    }

    if (!strcmp(buf, "Flags"))
    {
      ch->cflags = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Time"))
    {
      ch->time = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Class"))
    {
      ch->class = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Level"))
    {
      ch->level = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Location"))
    {
      ch->x = fread_number(fp);
      ch->y = fread_number(fp);
      ch->plane = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Plutos"))
    {
      ch->plutos = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "SightRadius"))
    {
      ch->sightradius = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Stats"))
    {
      ch->hp = fread_number(fp);
      ch->maxhp = fread_number(fp);
      ch->mana = fread_number(fp);
      ch->maxmana = fread_number(fp);
      ch->move = fread_number(fp);
      ch->maxmove = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Resistance"))
    {
      ch->resistance = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Primal"))
    {
      ch->primal = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Alias"))
    {
      alias = new_alias();
      link_alias(alias);

      if (ch->alias == NULL)
        ch->alias = alias;
      else
      {
        for (tempalias = ch->alias; tempalias->next_local; tempalias = tempalias->next_local);
        tempalias->next_local = alias;
      }

      fread_word(fp, buf);
      strlcpy(alias->in, buf, 16);

      fread_line(fp, buf);
      strlcpy(alias->out, buf, 128);
      continue;
    }

    if (!strcmp(buf, "#OBJECT"))
    {
      OBJECT_DATA *obj;
      obj = load_obj(fp, ch);
      obj->x = ch->x;
      obj->y = ch->y;
      obj->plane = ch->plane;
      obj->holder = ch;

      obj->next_local = ch->obj;
      ch->obj = obj;
      continue;
    }

    errorf("Unknown field: [%s] from file: [%s]", buf, file);
  }

  fclose(fp);
  return ch;
}

void save_object(FILE *fp, OBJECT_DATA *obj)
{
  if (obj->next_local)
    save_object(fp, obj->next_local);

  fprintf(fp, "#OBJECT\n");
  fprintf(fp, "--Keywords   %s\n", obj->keywords);
  fprintf(fp, "--Shortdesc  %s\n", obj->shortdesc);
  fprintf(fp, "--Longdesc   %s\n", obj->longdesc);
  fprintf(fp, "--Owner      %s\n", obj->owner);
  fprintf(fp, "--Level      %d\n", obj->level);
  fprintf(fp, "--Flags      %d\n", obj->oflags);
  fprintf(fp, "--Type       %d\n", obj->type);
  fprintf(fp, "--Values     %d %d %d %d %d %d\n", obj->v1, obj->v2, obj->v3, obj->v4, obj->v5, obj->v6);
  fprintf(fp, "EOO\n");
}

void save_character(CHAR_DATA *ch)
{
  FILE *fp;
  char buf[64];
  ALIAS_DATA *alias;
  snprintf(buf, 64, "account/%s.chr", ch->name);
  to_lower(buf);
  fp = fopen(buf, "w");
  if (fp == NULL)
  {
    errorf("Error opening file [%s]. Unable to save character.", buf);
    return;
  }

  fprintf(fp, "#CHARACTER\n");
  fprintf(fp, "Name         %s\n", ch->name);
  fprintf(fp, "Account      %s\n", ch->accname);
  fprintf(fp, "Prompt       %s\n", ch->prompt);
  fprintf(fp, "Flags        %d\n", ch->cflags);
  fprintf(fp, "Time         %d\n", ch->time);
  fprintf(fp, "Class        %d\n", ch->class);
  fprintf(fp, "Level        %d\n", ch->level);
  fprintf(fp, "Location     %d %d %d\n", ch->x, ch->y, ch->plane);
  fprintf(fp, "Plutos       %d\n", ch->plutos);
  fprintf(fp, "SightRadius  %d\n", ch->sightradius);
  fprintf(fp, "Stats        %d %d %d %d %d %d\n", ch->hp, ch->maxhp, ch->mana, ch->maxmana, ch->move, ch->maxmove);
  fprintf(fp, "Resistance   %d\n", ch->resistance);
  fprintf(fp, "Primal       %d\n", ch->primal);

  for (alias = ch->alias; alias; alias = alias->next_local)
    fprintf(fp, "Alias        %s %s\n", alias->in, alias->out);

  if (ch->obj)
    save_object(fp, ch->obj);

  fprintf(fp, "EOF~\n");
  fflush(fp);
  fclose(fp);
  /* TODO: add chmod */
}

void load_acc_ch(FILE *fp, ACCOUNT_DATA *acc)
{
  char buf[MAX_STRING_LENGTH];
  int i = 0;
  while (!feof(fp))
  {
    fread_word(fp, buf);
    if (!strcmp(buf, "EOC"))
      break;
    /* pwipe: remove everything after || for each of these ifs */

    if (!strcmp(buf, "--Number") || !strcmp(buf, "Number"))
    {
      i = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "--Name") || !strcmp(buf, "Name"))
    {
      fread_line(fp, buf);
      strlcpy(acc->character[i], buf, 32);
      acc->character[i][0] = UPPER(acc->character[i][0]); /* remove this sometime */
      continue;
    }

    if (!strcmp(buf, "--Time") || !strcmp(buf, "Time"))
    {
      acc->chtime[i] = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "--Status") || !strcmp(buf, "Status"))
    {
      acc->status[i] = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "--Logins") || !strcmp(buf, "Logins"))
    {
      acc->chlogins[i] = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "--Class") || !strcmp(buf, "Class"))
    {
      acc->class[i] = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "--FirstTime"))
    {
      fread_line(fp, buf);
      strlcpy(acc->chfirsttime[i], buf, 32);
      continue;
    }

    if (!strcmp(buf, "#CHARACTER")) /* pwipe: nuke this! */
    {
      load_acc_ch(fp, acc);
      break;
    }
  }
}

void load_account(char *name, ACCOUNT_DATA *acc)
{
  char buf[MAX_STRING_LENGTH];
  FILE *fp;
  char file[64];

  acc->name[0] = 0;
  snprintf(file, 64, "account/%s.acc", name);
  to_lower(file);

  fp = fopen(file, "r");
  if (fp == NULL)
    return;

  fread_line(fp, buf);

  while (!feof(fp))
  {
    fread_word(fp, buf);
    if (!strcmp(buf, "EOF~"))
      break;

    if (!strcmp(buf, "Name"))
    {
      fread_word(fp, buf);
      strlcpy(acc->name, buf, 32);
      acc->name[0] = UPPER(acc->name[0]); /* pwipe: delete me */
      continue;
    }

    if (!strcmp(buf, "Password"))
    {
      fread_word(fp, buf);
      strlcpy(acc->password, buf, 32);
      continue;
    }

    if (!strcmp(buf, "Host"))
    {
      fread_line(fp, buf);
      strlcpy(acc->lasthost, buf, 128);
      continue;
    }

    if (!strcmp(buf, "LastTime"))
    {
      fread_line(fp, buf);
      strlcpy(acc->lasttime, buf, 32);
      continue;
    }

    if (!strcmp(buf, "FirstTime"))
    {
      fread_line(fp, buf);
      strlcpy(acc->firsttime, buf, 32);
      continue;
    }

    if (!strcmp(buf, "Email"))
    {
      fread_line(fp, buf);
      strlcpy(acc->email, buf, 128);
      continue;
    }

    if (!strcmp(buf, "Charcount"))
    {
      acc->charcount = fread_number(fp);
      if (acc->totalchars == 0)
        acc->totalchars = acc->charcount;
      continue;
    }

    if (!strcmp(buf, "Totalchars"))
    {
      acc->totalchars = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Time"))
    {
      acc->time = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Flags"))
    {
      acc->aflags = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Changes"))
    {
      acc->changes = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "FailedLogins"))
    {
      acc->failed = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "Logins"))
    {
      acc->logins = fread_number(fp);
      continue;
    }

    if (!strcmp(buf, "#CHARACTER"))
    {
      load_acc_ch(fp, acc);
      continue;
    }

    errorf("Unknown field: [%s] from file: [%s]", buf, file);
  }

  fclose(fp);
  return;
}

void save_account(ACCOUNT_DATA *acc)
{
  FILE *fp;
  char buf[64];
  int i;

  snprintf(buf, 64, "account/%s.acc", acc->name);
  to_lower(buf);

  fp = fopen(buf, "w");

  if (fp == NULL)
  {
    errorf("Error opening file [%s]. Unable to save account.", buf);
    return;
  }

  fprintf(fp, "#ACCOUNT\n");
  fprintf(fp, "Name          %s\n", acc->name);
  fprintf(fp, "Password      %s\n", acc->password);
  fprintf(fp, "Email         %s\n", acc->email);
  fprintf(fp, "Time          %d\n", acc->time);
  fprintf(fp, "Charcount     %d\n", acc->charcount);
  fprintf(fp, "Totalchars    %d\n", acc->totalchars);
  fprintf(fp, "Flags         %d\n", acc->aflags);
  fprintf(fp, "Changes       %d\n", acc->changes);
  log_time(buf);
  fprintf(fp, "LastTime      %s\n", buf);
  fprintf(fp, "FirstTime     %s\n", acc->firsttime);
  fprintf(fp, "FailedLogins  %d\n", acc->failed);
  fprintf(fp, "Logins        %d\n", acc->logins);
  if (acc->d)
    fprintf(fp, "Host          %s\n", acc->d->host);

  for (i = 0; i < acc->charcount; i++)
  {
    fprintf(fp, "#CHARACTER\n");
    fprintf(fp, "--Number      %d\n", i);
    fprintf(fp, "--Name        %s\n", acc->character[i]);
    fprintf(fp, "--Time        %d\n", acc->chtime[i]);
    fprintf(fp, "--Status      %d\n", acc->status[i]);
    fprintf(fp, "--Logins      %d\n", acc->chlogins[i]);
    fprintf(fp, "--Class       %d\n", acc->class[i]);
    fprintf(fp, "--FirstTime   %s\n", acc->chfirsttime[i]);
    fprintf(fp, "EOC\n");
  }

  fprintf(fp, "EOF~\n");
  fflush(fp);
  fclose(fp);
  /* TODO: add chmod */

  if (acc->ch != NULL && acc->ch->name[0] != 0)
    save_character(acc->ch);
}

void save_address(DESCRIPTOR_DATA *d)
{
  char buf[128];
  FILE *fp;

  snprintf(buf, 128, "data/addresses/%s.ip", d->ip);
  fp = fopen(buf, "w");
  if (fp == NULL)
  {
    errorf("Error when trying to save file [%s]", buf);
    return;
  }
  fprintf(fp, "%s\n", d->acc->name);
  fflush(fp);
  fclose(fp);
}

void check_address(char *ip, char *buf)
{
  char file[128];
  FILE *fp;

  snprintf(file, 128, "data/addresses/%s.ip", ip);
  fp = fopen(file, "r");
  buf[0] = 0;
  if (fp == NULL)
    return;
  fread_line(fp, buf);
  fclose(fp);
}
