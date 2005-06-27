#include "undertow.h"

typedef struct help_index HELP_INDEX;
typedef struct help_data HELP_DATA;

struct help_index
{
  HELP_INDEX *next;
  HELP_DATA *help;
  char keyword[128];
  char file[128];
  char see_also[128];
  int level;
};

struct help_data
{
  HELP_INDEX *hi;
  char text[MAX_STRING_LENGTH];
};

HELP_INDEX *hihead = NULL;

void free_all_helps()
{
  HELP_INDEX *hi, *ht;

  hi = hihead;
  while (hi)
  {
    ht = hi->next;
    if (hi->help)
      free(hi->help);
    free(hi);
    hi = ht;
  }
  hihead = NULL;
}

void load_all_helps()
{
  FILE *fp;
  HELP_INDEX *hi = NULL;
  char buf[MAX_STRING_LENGTH];

  fp = fopen("help/index.txt", "r");
  if (fp == NULL)
  {
    syslog("WARNING", "No /help/index.txt found.");
    return;
  }

  while (!feof(fp))
  {
    fread_line(fp, buf);
    if (!strcmp(buf, "EOF~"))
      break;
    hi = (HELP_INDEX *) malloc(sizeof(HELP_INDEX));
    hi->next = hihead;
    hihead = hi;
    hi->help = NULL;
    strlcpy(hi->keyword, buf, 128);
    fread_line(fp, buf);
    strlcpy(hi->see_also, buf, 128);
    fread_line(fp, buf);
    hi->level = atoi(buf);
    fread_line(fp, buf);
    strlcpy(hi->file, "help/", 128);
    strlcat(hi->file, buf, 128);
  }
  fclose(fp);
}

void send_hlist_to_char(CHAR_DATA *ch)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  HELP_INDEX *hi;

  strlcpy(buf, "\nThe complete help file list:\n", MAX_STRING_LENGTH);
  for (hi = hihead; hi; hi = hi->next)
  {
    if (ch->level < hi->level)
      continue;
    snprintf(buf2, MAX_STRING_LENGTH, "%2d: %s\n", hi->level, hi->keyword);
    if (hi->level < 10)
      buf2[0] = '0';
    strlcat(buf, buf2, MAX_STRING_LENGTH);
  }
  send_to_char(ch, buf);
}

HELP_DATA *load_help(HELP_INDEX *hi)
{
  FILE *fp;
  HELP_DATA *help;
  char buf[MAX_STRING_LENGTH];
  fp = fopen(hi->file, "r");
  if (fp == NULL)
    return NULL;

  help = (HELP_DATA *) malloc(sizeof(HELP_DATA));
  fread_eof(fp, buf);
  strlcpy(help->text, buf, MAX_STRING_LENGTH);
  fclose(fp);
  help->hi = hi;
  return help;
}

HELP_DATA *find_help(char *keyword)
{
  HELP_INDEX *hi;
  char all_kw[MAX_STRING_LENGTH];
  char kw[MAX_STRING_LENGTH];

  for (hi = hihead; hi; hi = hi->next)
  {
    strlcpy(all_kw, hi->keyword, MAX_STRING_LENGTH);
    one_arg(all_kw, kw);
    while (kw[0] != 0)
    {
      if (!stricmp(keyword, kw))
      {
        if (hi->help == NULL)
          hi->help = load_help(hi);
        return hi->help;
      }
      one_arg(all_kw, kw);
    }
  }
  return NULL;
}

void do_help(CHAR_DATA *ch, char *arguments)
{
  HELP_DATA *help;
  char arg[MAX_STRING_LENGTH];
  char args[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int i = 0;
  int a = 0, b = 0;

  if (arguments[0] == 0)
    strlcpy(args, "help", MAX_STRING_LENGTH);
  else
    strlcpy(args, arguments, MAX_STRING_LENGTH);

  while (i == 0)
  {
    one_arg(args, arg);
    if (arg[0] == 0)
      break;
    help = find_help(arg);
    if (help == NULL)
      continue;
    if (help->hi->level > ch->level)
      continue;
    send_to_char(ch, "\n#R+#r-----------------------------------#1[#2Help#1]#r-----------------------------------#R+#n\n");
    if (strcmp(help->hi->file, "help/help.help"))
    {
      for (a = 0; help->hi->keyword[a]; ++a,++b)
      {
        if (help->hi->keyword[a] == ' ')
          args[b++] = ',';
        args[b] = UPPER(help->hi->keyword[a]);
      }
      args[b] = 0;

      snprintf(buf, MAX_STRING_LENGTH, "#r| #2Keywords: #3%-64s #r|\n", args);
      send_to_char(ch, buf);
      if (help->hi->see_also[0] && help->hi->see_also[0] != '*')
      {
        snprintf(buf, MAX_STRING_LENGTH, "#r| #2See also: #C%-64s #r|\n", help->hi->see_also);
        send_to_char(ch, buf);
      }
      send_to_char(ch, "#R+#r----------------------------------------------------------------------------#R+#n\n");
    }
    send_to_char(ch, help->text);
    send_to_char(ch, "#R+#r----------------------------------------------------------------------------#R+#n\n");

    i = 1;
    break;
  }

  if (i == 0)
  {
    syslog("HELP", "%s tried to find help on [%s].", ch->name, arguments);
    send_to_char(ch, "Unable to find that help file.\n");
  }
}
