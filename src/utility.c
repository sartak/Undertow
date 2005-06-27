#include "undertow.h"

void log_time(char *buf)
{
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strlcpy(buf, asctime(timeinfo), 32);

  if (buf[8] == ' ')
    buf[ 0] = '0';
  else
    buf[ 0] = buf[8];

  buf[ 1] = buf[9];
  buf[ 2] = ' ';
  buf[ 3] = buf[4];
  buf[ 4] = buf[5];
  buf[ 5] = buf[6];
  buf[ 6] = ' ';
  buf[ 7] = buf[22];
  buf[ 8] = buf[23];
  buf[ 9] = ' ';
  buf[10] = buf[11];
  buf[11] = buf[12];
  buf[12] = buf[13];
  buf[13] = buf[14];
  buf[14] = buf[15];
  buf[15] = buf[16];
  buf[16] = buf[17];
  buf[17] = buf[18];
  buf[18] = '\0';
}

void log_timeofday(char *buf)
{
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strlcpy(buf, asctime(timeinfo), 32);

  buf[0] = buf[11];
  buf[1] = buf[12];
  buf[2] = buf[13];
  buf[3] = buf[14];
  buf[4] = buf[15];
  buf[5] = buf[16];
  buf[6] = buf[17];
  buf[7] = buf[18];
  buf[8] = '\0';
}

void syslog(char *type, char *arg, ...)
{
  CHAR_DATA *ch;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char timebuf[32];
  va_list args;
  va_start(args, arg);
  vsprintf(buf, arg, args);
  va_end(args);

  log_time(timebuf);
  if (!DEBUG)
    printf("\n");
  printf("[\33[1;31m%s\33[0m] :: %s\n", timebuf, buf);
  fflush (stdout);

  snprintf(buf2, MAX_STRING_LENGTH, "[%s] <%s> %s", timebuf, type, buf);
  fprintf(fsyslog, "%s\n", buf2);
  fflush (fsyslog);

  snprintf(buf2, MAX_STRING_LENGTH, "\n#Y[%s: %s#Y]#n\n", type, buf);
  for (ch = chhead; ch; ch = ch->next)
    if (ch->level >= LEVEL_IMMORTAL)
      send_to_char(ch, buf2);
}

void error(char *arg)
{
  char buf[32];
  log_time(buf);

  if (!DEBUG)
    fprintf(stderr, "\n");
  fprintf(stderr, "[\33[1;31m%s\33[0m] :: ERROR: %s\n", buf, arg);
  fprintf(fsyslog, "[%s] <ERROR> %s\n", buf, arg);
  fflush (fsyslog);
  shut_down();
  exit(1);
}

void errorf(char *arg, ...)
{
  char buf[MAX_STRING_LENGTH];
  va_list args;

  va_start(args, arg);
  vsprintf(buf, arg, args);
  va_end(args);

  error(buf);
}

void debug(char *arg)
{
  if (DEBUG)
  {
    char buf[32];
    log_time(buf);

    printf("[\33[1;31m%s\33[0m] :: Debug: %s\n", buf, arg);
    fflush (stdout);
  }
}

void debugf(char *arg, ...)
{
  if (DEBUG)
  {
    char buf[MAX_STRING_LENGTH];
    va_list args;

    va_start(args, arg);
    vsprintf(buf, arg, args);
    va_end(args);

    debug(buf);
  }
}

int number_range(int min, int max)
{
  if (min == max)
    return min;
  else if (min > max)
    return (rand() % (min - max + 1)) + max;
  else
    return (rand() % (max - min + 1)) + min;
}

int percent()
{
  return rand() % 101;
}

int roll(int numdice, int sides, int bonus)
{
  int i;
  int sum = 0;

  if (numdice <= 0 || sides <= 0)
    return 0;

  for (i = 1; i <= numdice; i++)
    sum += number_range(1, sides) + bonus;

  return sum;
}

void serial_time(unsigned long time, char *buf)
{
  int h = 0, m = 0, s = 0;
  char buf2[32];

  s = time;

  while (s >= 60)
  {
    s -= 60;
    m++;
  }

  while (m >= 60)
  {
    m -= 60;
    h++;
  }

  if (h < 10)
    snprintf(buf2, 10, "0%i:", h);
  else
    snprintf(buf2, 10, "%i:", h);
  strlcpy(buf, buf2, MAX_STRING_LENGTH);

  if (m < 10)
    snprintf(buf2, 10, "0%i:", m);
  else
    snprintf(buf2, 10, "%i:", m);
  strlcat(buf, buf2, MAX_STRING_LENGTH);

  if (s < 10)
    snprintf(buf2, 10, "0%i", s);
  else
    snprintf(buf2, 10, "%i", s);
  strlcat(buf, buf2, MAX_STRING_LENGTH);
}

int wordwrap(char *buf, int width)
{
  int ret = 0, start = 0, i, j;

  while (strlen(buf) > start+width)
  {
    i = width;

    while (i >= 0 && buf[i+start-1] != ' ')
      --i;

    if (i == -1)
    {
      /* if there was no space, we need to bump everything
       * over by one to make room for the linebreak */

      i = width;
      for (j = strlen(buf)+1; j > i+start; --j)
        buf[j] = buf[j-1];
      buf[j] = '\n';
    }
    else
    {
      /* if there was a space, we can simply
       * replace it with the linebreak */

      buf[i+start-1] = '\n';
      start--;
    }

    start += i+1;
    ret++;
  }

  return ret;
}

int replace(const char *in, char *out, const char *replace, const char *with, int maxlen)
{
  int inlength = strlen(in);
  int replength = strlen(replace);
  int o = 0, i = 0, j = 0;
  int replaced;
  int ret = 0;

  if (in == out)
  {
    error("[replace()] Argument in == argument out. Big potential errors there, chief.");
    return 0;
  }

  while (i < inlength)
  {
    replaced = 0;
    for (j = 0; j < replength; j++)
    {
      if (in[i + j] != replace[j])
        break;

      if (j == replength - 1)
      {
        ret++;

        for (replaced = 0; replaced < strlen(with); replaced++)
          if (o+1 == maxlen)
          {
            debug("[replace()] Output truncated due to length.");
            out[o] = '\0';
            return ret;
          }
          else
            out[o++] = with[replaced];
        replaced = 1;
      }
    }

    if (replaced)
      i += replength;
    else if (o+1 == maxlen)
    {
      debug("[replace()] Output truncated due to length.");
      return ret;
      out[o] = '\0';
    }
    else
      out[o++] = in[i++];
  }

  out[o] = 0;
  return ret;
}

int stricmp(const char *a, const char *b)
{
  int i;

  for (i = 0; a[i] != 0; ++i)
    if (LOWER(a[i]) < LOWER(b[i]))
      return -1;
    else if (LOWER(a[i]) > LOWER(b[i]))
      return -1;

  if (b[i] == 0)
    return 0;
  return 1;
}

void to_lower(char *buf)
{
  int i;
  for(i = 0; buf[i]; ++i)
    buf[i] = LOWER(buf[i]);
}

int beginning(const char *big, const char *little)
{
  int i;
  int len = strlen(big);

  for (i = 0; i < len; i++)
    if (LOWER(big[i]) != LOWER(little[i]))
      return i;

  return len;
}

void one_arg(char *arguments, char *first)
{
  char *buf;
  int i;
  buf = arguments;

  while (*buf == ' ')
    ++buf;

  for (i = 0; buf[i]; ++i)
    if (buf[i] == ' ')
    {
      buf[i] = 0;
      strlcpy(first, buf, MAX_STRING_LENGTH);
      buf += i+1;
      while (*buf == ' ')
        ++buf;
      memmove(arguments, buf, strlen(buf)+1);
      return;
    }

  strlcpy(first, buf, MAX_STRING_LENGTH);
  arguments[0] = 0;
}

int time_words(int time, char *ret)
{
  int temp = time;
  int words = 0;
  char buf[MAX_STRING_LENGTH];

  if (temp == 0)
  {
    strlcpy(ret, "0 seconds", MAX_STRING_LENGTH);
    return 1;
  }

  ret[0] = '\0';

  if (temp >= 52 * 7 * 24 * 60 * 60)
  {
    snprintf(buf, MAX_STRING_LENGTH, "%d year%s", temp / (52 * 7 * 24 * 60 * 60), temp >= 2 * (52 * 7 * 24 * 60 * 60) ? "s" : "");
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 52 * 7 * 24 * 60 * 60;
  }

  if (temp >= 7 * 24 * 60 * 60)
  {
    snprintf(buf, MAX_STRING_LENGTH, "%s%d week%s", words > 0 ? ", " : "", temp / (7 * 24 * 60 * 60), temp >= 2 * (7 * 24 * 60 * 60) ? "s" : "");
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 7 * 24 * 60 * 60;
  }

  if (temp >= 24 * 60 * 60)
  {
    snprintf(buf, MAX_STRING_LENGTH, "%s%d day%s", words > 0 ? ", " : "", temp / (24 * 60 * 60), temp >= 2 * (24 * 60 * 60) ? "s" : "");
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 24 * 60 * 60;
  }

  if (temp >= 60 * 60)
  {
    snprintf(buf, MAX_STRING_LENGTH, "%s%d hour%s", words > 0 ? ", " : "", temp / (60 * 60), temp >= 2 * (60 * 60) ? "s" : "");
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 60 * 60;
  }

  if (temp >= 60)
  {
    snprintf(buf, MAX_STRING_LENGTH, "%s%d minute%s", words > 0 ? ", " : "", temp / (60), temp >= 2 * (60) ? "s" : "");
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 60;
  }

  if (temp > 0)
  {
    snprintf(buf, MAX_STRING_LENGTH, "%s%d second%s", words > 0 ? ", " : "", temp, temp >= 2 ? "s" : "");
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
  }

  return words;
}

int time_words_short(int time, char *ret)
{
  int temp = time;
  int words = 0;
  char buf[MAX_STRING_LENGTH];

  if (temp == 0)
  {
    strlcpy(ret, "0s", MAX_STRING_LENGTH);
    return 1;
  }

  ret[0] = '\0';

  if (temp >= 52 * 7 * 24 * 60 * 60)
  {
    snprintf(buf, MAX_STRING_LENGTH, "%dy", temp / (52 * 7 * 24 * 60 * 60));
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 52 * 7 * 24 * 60 * 60;
  }

  if (temp >= 7 * 24 * 60 * 60)
  {
    snprintf(buf, MAX_STRING_LENGTH, "%s%dw", words > 0 ? " " : "", temp / (7 * 24 * 60 * 60));
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 7 * 24 * 60 * 60;
  }

  if (temp >= 24 * 60 * 60)
  {
    snprintf(buf, MAX_STRING_LENGTH, "%s%dd", words > 0 ? " " : "", temp / (24 * 60 * 60));
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 24 * 60 * 60;
  }

  if (temp >= 60 * 60)
  {
    snprintf(buf, MAX_STRING_LENGTH, "%s%dh", words > 0 ? " " : "", temp / (60 * 60));
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 60 * 60;
  }

  if (temp >= 60)
  {
    snprintf(buf, MAX_STRING_LENGTH, "%s%dm", words > 0 ? " " : "", temp / (60));
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 60;
  }

  if (temp > 0)
  {
    snprintf(buf, MAX_STRING_LENGTH, "%s%ds", words > 0 ? " " : "", temp);
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
  }

  return words;
}

int time_words_long(int time, char *ret)
{
  int temp = time;
  int words = 0;
  char buf[MAX_STRING_LENGTH];
  char num[MAX_STRING_LENGTH];

  if (temp == 0)
  {
    strlcpy(ret, "zero seconds", MAX_STRING_LENGTH);
    return 1;
  }

  ret[0] = '\0';

  if (temp >= 52 * 7 * 24 * 60 * 60)
  {
    number_words(temp / (52 * 7 * 24 * 60 * 60), num);
    snprintf(buf, MAX_STRING_LENGTH, "%s year%s", num, temp >= 2 * (52 * 7 * 24 * 60 * 60) ? "s" : "");
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 52 * 7 * 24 * 60 * 60;
  }

  if (temp >= 7 * 24 * 60 * 60)
  {
    number_words(temp / (7 * 24 * 60 * 60), num);
    snprintf(buf, MAX_STRING_LENGTH, "%s%s week%s", words > 0 ? ", " : "", num, temp >= 2 * (7 * 24 * 60 * 60) ? "s" : "");
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 7 * 24 * 60 * 60;
  }

  if (temp >= 24 * 60 * 60)
  {
    number_words(temp / (24 * 60 * 60), num);
    snprintf(buf, MAX_STRING_LENGTH, "%s%s day%s", words > 0 ? ", " : "", num, temp >= 2 * (24 * 60 * 60) ? "s" : "");
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 24 * 60 * 60;
  }

  if (temp >= 60 * 60)
  {
    number_words(temp / (60 * 60), num);
    snprintf(buf, MAX_STRING_LENGTH, "%s%s hour%s", words > 0 ? ", " : "", num, temp >= 2 * (60 * 60) ? "s" : "");
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 60 * 60;
  }

  if (temp >= 60)
  {
    number_words(temp / 60, num);
    snprintf(buf, MAX_STRING_LENGTH, "%s%s minute%s", words > 0 ? ", " : "", num, temp >= 2 * (60) ? "s" : "");
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
    temp %= 60;
  }

  if (temp > 0)
  {
    number_words(temp, num);
    snprintf(buf, MAX_STRING_LENGTH, "%s%s second%s", words > 0 ? ", " : "", num, temp >= 2 ? "s" : "");
    strlcat(ret, buf, MAX_STRING_LENGTH);
    words++;
  }

  return words;
}

void displacement_words(int x1, int y1, int x2, int y2, char *buf)
{
  char nw[MAX_STRING_LENGTH];
  char nw2[MAX_STRING_LENGTH];
  if (x1 == x2 && y1 == y2)
  {
    strlcpy(buf, "here", MAX_STRING_LENGTH);
    return;
  }

  buf[0] = '\0';

  if (x1 == x2)
  {
    if (y1 < y2)
    {
      number_words(y2-y1, nw);
      snprintf(buf, MAX_STRING_LENGTH, "%s meter%s north", nw, y2-y1 == 1 ? "": "s");
    }
    if (y2 < y1)
    {
      number_words(y1-y2, nw);
      snprintf(buf, MAX_STRING_LENGTH, "%s meter%s south", nw, y1-y2 == 1 ? "": "s");
    }
  }
  if (y1 == y2)
  {
    if (x1 < x2)
    {
      number_words(x2-x1, nw);
      snprintf(buf, MAX_STRING_LENGTH, "%s meter%s east", nw, x2-x1 == 1 ? "": "s");
    }
    if (x2 < x1)
    {
      number_words(x1-x2, nw);
      snprintf(buf, MAX_STRING_LENGTH, "%s meter%s west", nw, x1-x2 == 1 ? "": "s");
    }
  }
  if (buf[0])
    return;

  number_words(abs(y1-y2), nw);
  number_words(abs(x1-x2), nw2);

  snprintf(buf, MAX_STRING_LENGTH, "%s meter%s %s, %s meter%s %s",
     nw,  abs(y1-y2) == 1 ? "" : "s", y1<y2 ? "north" : "south",
     nw2, abs(x1-x2) == 1 ? "" : "s", x1<x2 ? "east" : "west");
}

/* this is strictly a helper function for number_words */
void number_block(int n, char *buf)
{
  /* assertion: n >= 0 && n < 1000 */
  int a, b, c;
  char d[][10] =
  {
      "one",   "two", "three",
     "four",  "five",   "six",
    "seven", "eight",  "nine"
  };

  char m[][10] =
  {
            "ten",  "eleven", "twelve",
    "thirteen", "fourteen", "fifteen",   "sixteen",
          "seventeen", "eighteen", "nineteen"
  };

  char t[][10] =
  {
    "twenty", "thirty", "forty",
          "fifty", "sixty",
    "seventy", "eighty", "ninety"
  };

  buf[0] = '\0';

  a = n / 100;
  b = (n / 10) % 10;
  c = n % 10;

  if (a > 0)
  {
    strlcat(buf, d[a-1], MAX_STRING_LENGTH);
    strlcat(buf, " hundred ", MAX_STRING_LENGTH);
  }
  if (n % 100 < 20 && n % 100 >= 10)
    strlcat(buf, m[n%100 - 10], MAX_STRING_LENGTH);
  else
  {
    if (b > 0)
    {
      strlcat(buf, t[b-2], MAX_STRING_LENGTH);
      if (c == 0)
        strlcat(buf, " ", MAX_STRING_LENGTH);
    }
    if (c > 0)
    {
      if (b > 0)
        strlcat(buf, "-", MAX_STRING_LENGTH);
      strlcat(buf, d[c-1], MAX_STRING_LENGTH);
    }
  }
  if (a == b && b == c && c == 0)
    strlcat(buf, "zero", MAX_STRING_LENGTH);

  a = strlen(buf)-1;
  if (buf[a] == ' ')
    buf[a] = '\0';
}

void number_words(int n, char *buf)
{
  char buf2[MAX_STRING_LENGTH];
  int needcomma = 0;
  int magnitude = 1000000000;
  buf[0] = '\0';
  if (n < 0)
  {
    strlcat(buf, "negative ", MAX_STRING_LENGTH);
    n *= -1;
  }

  while (n > 0)
  {
    if (n >= magnitude)
    {
      if (needcomma)
        strlcat(buf, ", ", MAX_STRING_LENGTH);
      number_block(n/magnitude, buf2);
      strlcat(buf, buf2, MAX_STRING_LENGTH);
      switch (magnitude)
      {
        case 1000000000: strlcat(buf, " billion", MAX_STRING_LENGTH); break;
        case 1000000: strlcat(buf, " million", MAX_STRING_LENGTH); break;
        case 1000: strlcat(buf, " thousand", MAX_STRING_LENGTH); break;
      }
      needcomma++;
      n %= magnitude;
    }
    magnitude /= 1000;
  }
}
