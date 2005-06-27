#include "undertow.h"

typedef struct change_data CHANGE_DATA;

struct change_data
{
  CHANGE_DATA *next;
  int num;
  int age;
  char text[256];
  char date[32];
  char owner[32];
};

CHANGE_DATA *changehead = NULL;

void save_changes()
{
  FILE *fp;

  CHANGE_DATA *change;

  fp = fopen("data/changes.txt", "w");
  if (fp == NULL)
  {
    error("Error saving changes.");
    return;
  }

  for (change = changehead; change; change = change->next)
    fprintf(fp, "%d\n%s\n%s\n%s\n", change->age, change->text, change->date, change->owner);

  fprintf(fp, "EOF~\n");
  fflush(fp);
  fclose(fp);
  syslog("CHANGES", "Changes successfully saved.");
}

void load_changes()
{
  FILE *fp;
  CHANGE_DATA *change = NULL, *oldchange = NULL;
  char buf[256];
  char text[256];
  char date[32];
  char owner[32];
  int i;
  int num = 0;
  int age;

  fp = fopen("data/changes.txt", "r");
  if (fp == NULL)
  {
    syslog("WARNING", "Unable to read changes file. Assuming no changes.");
    changehead = NULL;
    return;
  }

  while (!feof(fp))
  {
    fgets(buf, 256, fp);

    for (i = 0; i < 10; ++i)
      if (buf[i] == '\n')
      {
        buf[i] = 0;
        break;
      }

    if (!strcmp(buf, "EOF~"))
      break;
    ++num;
    age = atoi(buf);

    fgets(text, 256, fp);
    for (i = 0; i < 256; ++i)
      if (text[i] == '\n')
      {
        text[i] = 0;
        break;
      }

    fgets(date, 32, fp);
    for (i = 0; i < 32; ++i)
      if (date[i] == '\n')
      {
        date[i] = 0;
        break;
      }

    fgets(owner, 32, fp);
    for (i = 0; i < 32; ++i)
      if (owner[i] == '\n')
      {
        owner[i] = 0;
        break;
      }

    oldchange = change;
    change = (CHANGE_DATA *) malloc(sizeof(CHANGE_DATA));

    change->next = NULL;
    if (oldchange == NULL)
      changehead = change;
    else
      oldchange->next = change;

    change->age = age;
    change->num = num;
    strlcpy(change->owner, owner, 32);
    strlcpy(change->date, date, 32);
    strlcpy(change->text, text, 256);
  }
  fclose(fp);
}

void free_changes()
{
  CHANGE_DATA *change, *temp;

  change = changehead;

  while (change)
  {
    temp = change->next;
    free(change);
    change = temp;
  }

  changehead = NULL;
}

int new_changes(ACCOUNT_DATA *acc)
{
  int i = 0;
  CHANGE_DATA *change = changehead;

  while (change != NULL)
  {
    if (change->age > acc->changes)
      ++i;
    change = change->next;
  }
  return i;
}

void show_changes(ACCOUNT_DATA *acc, int type)
{
  CHANGE_DATA *change;
  char buf[MAX_STRING_LENGTH];
  char buf2[12];
  char buf3[MAX_STRING_LENGTH];
  int num = 0;
  int new = 0;
  int total = 0;
  int latestchange = 0;
  int i, j = 0, o = 0;
  int count = 0;

  if (changehead == NULL)
  {
    send_to_descriptor(acc->d, "There are currently no documented changes.\n");
    return;
  }

  for (change = changehead; change; change = change->next)
    ++count;

  if (type == -1 && new_changes(acc) == 0)
  {
    send_to_descriptor(acc->d, "There are currently no new changes.\n");
    return;
  }
  else if (type != -2 && type * 20 >= count)
  {
    if (count <= 20)
    {
      strlcpy(buf, "Page out of range. The only valid page is 0.\n", MAX_STRING_LENGTH);
    }
    else
    {
      snprintf(buf, MAX_STRING_LENGTH, "Page out of range. The only valid pages are [0, %d].\n", (count-1) / 20);
    }

    send_to_descriptor(acc->d, buf);
    return;
  }

  send_to_descriptor(acc->d, "#R+#r-----------------------------------------------------------------------------#R+\n");
  send_to_descriptor(acc->d, "#r| #1[#2Num#1] #3Date       #RCoder#1: #3Description                                         #r|\n");
  send_to_descriptor(acc->d, "#R+#r-----------------------------------------------------------------------------#R+\n");
  for (change = changehead; change; change = change->next)
  {
    if ((type >= 0 && total < ((type+1) * 20) && total >= type * 20) || (type == -2 && count - total <= 20) || (type == -1 && change->age > acc->changes && new < 20))
    {
      latestchange = change->age;
      snprintf(buf2, 12, "%3d", change->num);
      if (change->num < 100)
        buf2[0] = '0';
      if (change->num < 10)
        buf2[1] = '0';

      if (strlen(change->text) > 51)
      {
        strlcpy(buf3, change->text, MAX_STRING_LENGTH);
        wordwrap(buf3, 51);
        snprintf(buf, MAX_STRING_LENGTH, "#r|%s#1[#2%s#1] #3%s #R%9s#1: #3", change->age > acc->changes ? "#R*#n" : " ", buf2, change->date, change->owner);
        send_to_descriptor(acc->d, buf);
        j = 0;
        o = 0;
        for (i = 0; i <= strlen(buf3); ++i)
        {
          if (buf3[i] == '\n' || buf3[i] == '\0')
          {
            for (; j < 52; ++j)
              buf[o++] = ' ';
            buf[o++] = '#';
            buf[o++] = 'r';
            buf[o++] = '|';
            buf[o++] = '\n';
            if (buf3[i] == '\0' || buf3[i+1] == '\0')
              break;
            buf[o++] = '|';
            buf[o++] = '#';
            buf[o++] = 'n';
            for (j = 25; j > 0; --j)
              buf[o++] = ' ';
          }
          else
          {
            ++j;
            buf[o++] = buf3[i];
          }
        }

        buf[o] = '\0';
        send_to_descriptor(acc->d, buf);
      }
      else
      {
        snprintf(buf, MAX_STRING_LENGTH, "#r|%s#1[#2%s#1] #3%s #R%9s#1: #3%-51s #r|\n", change->age > acc->changes ? "#R*#n" : " ", buf2, change->date, change->owner, change->text);
        send_to_descriptor(acc->d, buf);
      }

      ++num;
      if (change->age > acc->changes)
        ++new;
    }
    ++total;
  }

  new = 0;
  for (change = changehead; change; change = change->next)
    if (change->age > acc->changes)
      ++new;

  send_to_descriptor(acc->d, "#R+#r-----------------------------------------------------------------------------#R+\n");
  if (new == 1)
    strlcpy(buf, "#r| #2There has been #31#2 new change.                                                #r|\n", MAX_STRING_LENGTH);
  else
  {
    snprintf(buf3, MAX_STRING_LENGTH, "#3%d#2 new changes.", new);
    snprintf(buf, MAX_STRING_LENGTH, "#r| #2There have been %-63s #r|\n", buf3);
  }
  send_to_descriptor(acc->d, buf);
  snprintf(buf3, MAX_STRING_LENGTH, "#3%d#2 out of #3%d#2 changes shown.", num, total);
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2A total of %-72s #r|#n\n", buf3);
  send_to_descriptor(acc->d, buf);

  if (type == -1 && new >= 20)
    send_to_descriptor(acc->d, "#r| #2Too many new changes to display at once, sorry! Type changes new again.     #r|#n\n");

  if (acc->changes < latestchange)
  {
    acc->changes = latestchange;
    save_account(acc);
  }

  send_to_descriptor(acc->d, "#R+#r-----------------------------------------------------------------------------#R+\n");
  save_account(acc);
}

/* The reason I have do_changes pass this off to another function is so noncharacters can
   read the newest changes, like accounts not yet logged into a char. */
void do_changes(CHAR_DATA *ch, char *arguments)
{
  int i = 0;
  if (!strcmp(arguments, "new")) /* only new changes */
    i = -1;
  else if (arguments[0] == 0) /* twenty most recent changes */
    i = -2;
  else                        /* a certain page.. nonnumeric input will result in page 0 by nature of atoi */
    i = atoi(arguments);
  show_changes(ch->acc, i);
}

void do_changeadd(CHAR_DATA *ch, char *arguments)
{
  CHANGE_DATA *temp, *change;
  char buf[128];
  int num = 0;
  if (arguments[0] == 0)
  {
    send_to_char(ch, "What change do you wish to add?\n");
    return;
  }

  if (changehead == NULL)
  {
    change = (CHANGE_DATA *) malloc(sizeof(CHANGE_DATA));
    num = 1;
    changehead = change;
  }
  else
  {
    for (change = changehead; change; change = change->next)
    {
      num = change->num;
      temp = change;
    }
    ++num;
    change = (CHANGE_DATA *) malloc(sizeof(CHANGE_DATA));
    temp->next = change;
  }

  change->next = NULL;
  change->num = num;
  change->age = time(NULL);
  strlcpy(change->owner, ch->name, 32);
  log_time(buf);
  buf[6] = 0;
  strlcpy(change->date, buf, 32);
  strlcpy(change->text, arguments, 256);

  snprintf(buf, MAX_STRING_LENGTH, "#RA new change has been added by #2%s#R!", ch->name);
  info(ch, buf, 0);

  save_changes();
}

void do_changedel(CHAR_DATA *ch, char *arguments)
{
  CHANGE_DATA *change = NULL, *temp = NULL;
  int num = 0;
  int del;
  char buf[MAX_STRING_LENGTH];

  if (changehead == NULL)
  {
    send_to_char(ch, "There are no changes to delete!\n");
    return;
  }

  for (change = changehead; change; change = change->next)
    ++num;

  del = atoi(arguments);
  if (del <= 0 || del > num)
  {
    send_to_char(ch, "Which change would you like to delete?\n");
    return;
  }

  if (del == 1)
  {
    change = changehead;
    changehead = changehead->next;
    snprintf(buf, MAX_STRING_LENGTH, "([%s] from %s)", change->text, change->owner);
    free(change);
  }
  else
    for (change = changehead; change; temp = change, change = change->next)
      if (change->num == del)
      {
        temp->next = change->next;
        snprintf(buf, MAX_STRING_LENGTH, "([%s] from %s)", change->text, change->owner);
        free(change);
        break;
      }

  syslog("CHANGES", "Change %d %s deleted by %s.", del, buf, ch->name);
  num = 0;
  for (change = changehead; change; change = change->next)
    change->num = ++num;
  save_changes();
  send_to_char(ch, "A change was deleted.\n");
}
