#include "undertow.h"

char password_on[] =  {(signed char)IAC, (signed char)WILL, (signed char)TELOPT_ECHO, '\0'};
char password_off[] = {(signed char)IAC, (signed char)WONT, (signed char)TELOPT_ECHO, '\0'};

void finish_creation(DESCRIPTOR_DATA *d, int hardcore);

/*********************************************************************************************
 * int check_string(char *buf, int minlength, int maxlength, int alphanumeric);              *
 *                                                                                           *
 * This function checks whether the string 'buf' fits the following criteria:                *
 * strlen(buf) >= minlength                                                                  *
 * strlen(buf) <= maxlength (maxlength = -1 if no max length                                 *
 * if alphanumeric = 0 then we want only letters                                             *
 * if alphanumeric = 1 then we want only letters or numbers                                  *
 * if alphanumeric = 2 then we don't care about the specific characters                      *
 *                                                                                           *
 * This function returns 0 if all criteria are met.                                          *
 * This function returns 1 if the string is not long enough.                                 *
 * This function returns 2 if the string is too long.                                        *
 * This function returns 3 if alphanumeric = 0 and a number is in the string.                *
 * This function returns 4 if alphanumeric = 0 or = 1 and a nonalphanumeric                  *
 *   character is in the string.                                                             *
 *                                                                                           *
 *********************************************************************************************/

int check_string(char *buf, int minlength, int maxlength, int alphanumeric)
{
  int i;
  if (strlen(buf) < minlength)
    return 1;

  if (maxlength > -1 && strlen(buf) > maxlength)
    return 2;

  if (alphanumeric == 2)
    return 0;

  for (i = 0; i < strlen(buf); i++)
  {
    if (alphanumeric == 0)
      if (buf[i] < 65)
        return 3;

    if (buf[i] > 122 || buf[i] < 48)
      return 4;

    if (buf[i] > 90 && buf[i] < 97)
      return 4;

    if (buf[i] > 57 && buf[i] < 65)
      return 4;
  }

  return 0;
}

void show_account_menu(DESCRIPTOR_DATA *d, int askchoice)
{
  ACCOUNT_DATA *acc = d->acc;
  char buf[MAX_STRING_LENGTH];
  char buf2[128];
  char colorbuf[10];
  int i;

  send_to_descriptor(d, "\n#R+#r---------------------#1[#2Menu#1]#r---------------------#R+\n");
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Account: #C%-12s                          #r|\n", acc->name);
  send_to_descriptor(d, buf);

  serial_time(acc->time, buf);
  snprintf(buf2, 128, "#c(%s)", buf);
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Time On: %-39s #r|\n", buf2);
  send_to_descriptor(d, buf);
  snprintf(buf2, 128, "#c(%d)", acc->logins);
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Logins:  %-28s            #r|\n", buf2);
  send_to_descriptor(d, buf);
  send_to_descriptor(d, "#R+#r------------------------------------------------#R+\n");
  send_to_descriptor(d, "#r| #2Characters:                                    #r|\n");
  if (acc->charcount == 0)
    send_to_descriptor(d, "#r|#n None!                                          #r|\n");
  else
  {
    for (i = 0; i < acc->charcount; i++)
    {
      serial_time(acc->chtime[i], buf);
      snprintf(buf2, 128, "%s #c(%s) (%d)", acc->character[i], buf, acc->chlogins[i]);

      switch (acc->status[i])
      {
        default:
        case CHSTATUS_NORMAL:
            strlcpy(colorbuf, "#C", 10);
          break;
        case CHSTATUS_HARDCORE:
            strlcpy(colorbuf, "#R", 10);
          break;
        case CHSTATUS_DELETED:
            strlcpy(colorbuf, "#1", 10);
          break;
        case CHSTATUS_IMMORTAL:
           strlcpy(colorbuf, "#Y", 10);
          break;
      }

      snprintf(buf, MAX_STRING_LENGTH, "#r| #1[#3%d#1] %s%-42s   #r|\n", i+1, colorbuf, buf2);
      send_to_descriptor(d, buf);
    }
  }

  send_to_descriptor(d, "#R+#r------------------------------------------------#R+\n");
  send_to_descriptor(d, "#r| #1[#3N#1]#2ew Character                                #r|\n");
  send_to_descriptor(d, "#r| #1[#3D#1]#2elete Character                             #r|\n");
  send_to_descriptor(d, "#r| #1[#3C#1]#2hange Settings                              #r|\n");
  send_to_descriptor(d, "#r| #1[#3S#1]#2ee Players Online                           #r|\n");

  snprintf(buf2, 128, "[#3%d#1]", new_changes(d->acc));
  snprintf(buf, MAX_STRING_LENGTH, "#r| #1[#3R#1]#2ecent Change List #1%-10s                    #r|\n", buf2);
  send_to_descriptor(d, buf);

  send_to_descriptor(d, "#r| #1[#3Q#1]#2uit                                         #r|\n");
  send_to_descriptor(d, "#R+#r------------------------------------------------#R+#n\n");
  if (askchoice)
    send_to_descriptor(d, "What is your choice? ");
}

void show_delete_menu(DESCRIPTOR_DATA *d)
{
  ACCOUNT_DATA *acc = d->acc;
  char buf[MAX_STRING_LENGTH];
  char buf2[128];
  char colorbuf[10];
  int i;

  send_to_descriptor(d, "\n#R+#r--------------------#1[#2Delete#1]#r--------------------#R+\n");
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Account: #C%-12s                          #r|\n", acc->name);
  send_to_descriptor(d, buf);

  serial_time(acc->time, buf);
  snprintf(buf2, 128, "#c(%s)", buf);
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Time On: %-39s #r|\n", buf2);
  send_to_descriptor(d, buf);
  snprintf(buf2, 128, "#c(%d)", acc->logins);
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Logins:  %-28s            #r|\n", buf2);
  send_to_descriptor(d, buf);
  send_to_descriptor(d, "#R+#r------------------------------------------------#R+\n");
  send_to_descriptor(d, "#r| #2Characters:                                    #r|\n");
  if (acc->charcount == 0)
    send_to_descriptor(d, "#r|#n None!                                          #r|\n");
  else
  {
    for (i = 0; i < acc->charcount; i++)
    {
      serial_time(acc->chtime[i], buf);
      snprintf(buf2, 128, "%s #c(%s) (%d)", acc->character[i], buf, acc->chlogins[i]);

      switch (acc->status[i])
      {
        default:
        case CHSTATUS_NORMAL:
            strlcpy(colorbuf, "#C", 10);
          break;
        case CHSTATUS_HARDCORE:
            strlcpy(colorbuf, "#R", 10);
          break;
        case CHSTATUS_DELETED:
            strlcpy(colorbuf, "#1", 10);
          break;
        case CHSTATUS_IMMORTAL:
           strlcpy(colorbuf, "#Y", 10);
          break;
      }

      snprintf(buf, MAX_STRING_LENGTH, "#r| #1[#3%d#1] %s%-42s   #r|\n", i+1, colorbuf, buf2);
      send_to_descriptor(d, buf);
    }
  }

  send_to_descriptor(d, "#R+#r------------------------------------------------#R+\n");
  send_to_descriptor(d, "#r| #1[#3R#1]#2eturn to Main Menu                          #r|\n");
  send_to_descriptor(d, "#R+#r------------------------------------------------#R+#n\n");
  send_to_descriptor(d, "What is your choice? ");
}

void show_account_settings(DESCRIPTOR_DATA *d)
{
  ACCOUNT_DATA *acc = d->acc;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];

  send_to_descriptor(d, "\n#R+#r-------------------#1[#2Settings#1]#r-------------------#R+\n");
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Account: #C%-12s                          #r|\n", acc->name);
  send_to_descriptor(d, buf);
  send_to_descriptor(d, "#R+#r------------------------------------------------#R+\n");

  if (IS_SET(acc->aflags, AFLAG_ANSI))
    send_to_descriptor(d, "#r| #1[#3A#1]#2NSI Color:                             #1[#GOn#1] #r|\n");
  else
    send_to_descriptor(d, "#r| #1[#3A#1]#2NSI Color:                            #1[#ROff#1] #r|\n");

  if (IS_SET(acc->aflags, AFLAG_ANSI_SPARSE))
    send_to_descriptor(d, "#r| #1[#3F#1]#2ull ANSI Color:                       #1[#ROff#1] #r|\n");
  else
    send_to_descriptor(d, "#r| #1[#3F#1]#2ull ANSI Color:                        #1[#GOn#1] #r|\n");

  if (IS_SET(acc->aflags, AFLAG_REPLYLOCK))
    send_to_descriptor(d, "#r| #2Reply #1[#3L#1]#2ock:                             #1[#GOn#1] #r|\n");
  else
    send_to_descriptor(d, "#r| #2Reply #1[#3L#1]#2ock:                            #1[#ROff#1] #r|\n");

  if (!strcmp(acc->email, "Unset"))
    send_to_descriptor(d, "#r| #1[#3E#1]#2mail Address:                       #1[#RUnset#1] #r|\n");
  else
  {
    strlcpy(buf2, "#1[#GSet#1]", MAX_STRING_LENGTH);
    snprintf(buf, MAX_STRING_LENGTH, "#r| #1[#3E#1]#2mail Address:                %20s #r|\n", buf2);
    send_to_descriptor(d, buf);
    snprintf(buf2, MAX_STRING_LENGTH, "#1[#G%s#1]", acc->email);
    snprintf(buf, MAX_STRING_LENGTH, "#r| %52s #r|\n", buf2);
    send_to_descriptor(d, buf);
  }
  if (IS_SET(acc->aflags, AFLAG_PUBLIC_EMAIL))
    send_to_descriptor(d, "#r| #1[#3P#1]#2ublic Email:                           #1[#GOn#1] #r|\n");
  else
    send_to_descriptor(d, "#r| #1[#3P#1]#2ublic Email:                          #1[#ROff#1] #r|\n");

  send_to_descriptor(d, "#R+#r------------------------------------------------#R+\n");
  send_to_descriptor(d, "#r| #1[#3D#1]#2efault Settings                             #r|\n");
  send_to_descriptor(d, "#r| #1[#3C#1]#2hange Password                              #r|\n");
  send_to_descriptor(d, "#r| #1[#3R#1]#2eturn to Main Menu                          #r|\n");
  send_to_descriptor(d, "#R+#r------------------------------------------------#R+\n");
  send_to_descriptor(d, "#nWhat is your choice? ");
}


void nanny(DESCRIPTOR_DATA *d, char *input)
{
  int i;
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *dt;

  switch(d->state)
  {
    case STATE_GET_NAME:
      if (input[0] == 0)
      {
        sckoutput(d->socket, "Fine, then.\n");
        wipe_descriptor(d);
        return;
      }
      i = check_string(input, 4, 12, 0);
      if (i == 1)
        send_to_descriptor(d, "Your name must have at least 4 letters.\n");
      else if (i == 2)
        send_to_descriptor(d, "Your name can contain at most 12 letters.\n");
      else if (i == 4 || i == 3)
        send_to_descriptor(d, "Your name can contain only letters.\n");
      else
      {
        input[0] = UPPER(input[0]);
        load_account(input, d->acc);
        d->acc->d = d;

        if (d->acc->name[0] == 0)
        {
          check_address(d->ip, d->acc->name);
          if (d->acc->name[0] != 0 && stricmp(input, d->acc->name))
            send_to_descriptor(d, "#RWARNING:#n Your IP is linked to an existing account. It is against the rules to have multiple accounts.\n\n");

          strlcpy(d->acc->name, input, 32);
          snprintf(buf, MAX_STRING_LENGTH, "Are you sure you want the name %s? ", input);
          send_to_descriptor(d, buf);
          d->state = STATE_CONFIRM_NAME;
          return;
        }

        send_to_descriptor(d, "Password? ");
        send_to_descriptor(d, password_on);
        d->state = STATE_GET_PASS;
        return;
      }
      send_to_descriptor(d, "What do you want your account name to be? ");
    break;


    case STATE_GET_PASS:
      send_to_descriptor(d, password_off);

      if (!strcmp(d->acc->password, input))
      {
        /* check to see if someone else is logged in under this account */
        for (dt = dhead; dt; dt = dt->next)
        {
          if (dt == d || dt->acc == NULL)
            continue;

          if (!stricmp(d->acc->name, dt->acc->name))
          {
            /* d = new connection, dt = old */
            if (dt->socket > 0 && dt->state != STATE_LINKDEAD)
            {
              sckoutput(dt->socket, "\n\nYou have been kicked off.\n");
              close(dt->socket);
            }

            dt->socket = d->socket;
            d->socket = -1; /* this is mandatory because wipe_descriptor closes d->socket */
            wipe_descriptor(d);
            d = dt;

            if (d->state != STATE_PLAYING && d->state != STATE_LINKDEAD)
              send_to_descriptor(d, "\nYou have kicked someone off while they were logging in. Sending you to the account menu.\n");
            else
            {
              destroy_all_events(d->acc->ch, EVENT_LINKDEAD);
              setup_char_events(d->acc->ch);
              d->acc->ch->idle = 0;
              syslog("RECONNECT", "%s (on %s@%s) has reconnected.", d->acc->ch->name, d->acc->name, d->ip);
              d->state = STATE_PLAYING;
              send_to_descriptor(d, "\n#RReconnected.#n\n");
              return;
            }
          }
        }

        syslog("LOGIN", "%s@%s has logged in.", d->acc->name, d->ip);
        d->state = STATE_ACCT_MENU;
        d->acc->logins++;
        save_account(d->acc);
        show_account_menu(d, 0);
        snprintf(buf, MAX_STRING_LENGTH, "#1[#2Last connection from #R%s#2 at #R%s#2.#1]\n", d->acc->lasthost, d->acc->lasttime);
        save_address(d);
        send_to_descriptor(d, buf);
        if (d->acc->failed == 1)
          snprintf(buf, MAX_STRING_LENGTH, "#1[#RWARNING:#2 1 failed login attempt since your last connection.#1]\n");
        else if (d->acc->failed > 1)
          snprintf(buf, MAX_STRING_LENGTH, "#1[#RWARNING:#2 %d failed login attempts since your last connection.#1]\n", d->acc->failed);
        if (d->acc->failed > 0)
        {
          send_to_descriptor(d, buf);
          d->acc->failed = 0;
          save_account(d->acc);
        }
        send_to_descriptor(d, "#nWhat is your choice? ");
        return;
      }

      sckoutput(d->socket, "Incorrect password. Logging you off...\n");
      syslog("PASSWORD", "%s@%s -- incorrect password! Logged off.", d->acc->name, d->ip);
      d->acc->failed++;
      save_account(d->acc);
      wipe_descriptor(d);
    break;

    case STATE_CONFIRM_NAME:
      if (input[0] == 'y' || input[0] == 'Y')
      {
        send_to_descriptor(d, "What do you want your password to be? ");
        syslog("NEW", "New account %s is being created.", d->acc->name);
        d->state = STATE_NEW_PASS;
        send_to_descriptor(d, password_on);
        return;
      }

      send_to_descriptor(d, "Okay. What do you want your name to be? ");
      d->state = STATE_GET_NAME;
    break;

    case STATE_NEW_PASS:
      send_to_descriptor(d, password_off);
      if (input[0] == 0)
      {
        send_to_descriptor(d, "\nWhat do you want your password to be? ");
        send_to_descriptor(d, password_on);
        return;
      }
      i = check_string(input, 4, 12, 1);
      if (i == 1)
        send_to_descriptor(d, "\nYour password must have at least 4 characters.\n");
      else if (i == 2)
        send_to_descriptor(d, "\nYour password can contain at most 12 characters.\n");
      else if (i == 4)
        send_to_descriptor(d, "\nYour password can contain only letters and numbers.\n");
      else
      {
        strlcpy(d->acc->password, input, 32);
        send_to_descriptor(d, "\nPlease retype your password for confirmation. ");
        send_to_descriptor(d, password_on);
        d->state = STATE_CONFIRM_PASS;
        return;
      }
      send_to_descriptor(d, "\nWhat do you want your password to be? ");
      send_to_descriptor(d, password_on);
    break;

    case STATE_CONFIRM_PASS:
      send_to_descriptor(d, password_off);
      if (strcmp(d->acc->password, input))
      {
        send_to_descriptor(d, "\nYour passwords must match exactly.\nWhat do you want your password to be? ");
        d->state = STATE_NEW_PASS;
        d->acc->password[0] = 0;
        send_to_descriptor(d, password_on);
        return;
      }

      send_to_descriptor(d, "\nDo you want ANSI color? ");
      d->state = STATE_GET_ANSI;
    break;

    case STATE_GET_ANSI:
      d->acc->aflags = 0;
      if (input[0] == 'y' || input[0] == 'Y')
        SET_BIT(d->acc->aflags, AFLAG_ANSI);
      else if (input[0] == 'n' || input[0] == 'N')
        ;
      else
      {
        send_to_descriptor(d, "Please type yes or no.\nDo you want ANSI color? ");
        return;
      }

      for (dt = dhead; dt; dt = dt->next)
      {
        if (dt == d)
          continue;

        if (!stricmp(d->acc->name, dt->acc->name) && (dt->state > STATE_GET_PASS || dt->state < STATE_CONFIRM_NAME))
        {
          sckoutput(d->socket, "You have been kicked off.\n");
          wipe_descriptor(d);
          return;
        }
      }

      d->acc->logins = 1;
      d->acc->changes = time(NULL);
      strlcpy(d->acc->email, "Unset", 128);
      d->state = STATE_ACCT_MENU;
      log_time(d->acc->firsttime);
      save_address(d);
      show_account_menu(d, 1);
      save_account(d->acc);
    break;

    case STATE_ACCT_MENU:
      switch (input[0])
      {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          i = atoi(input);
          if (i > d->acc->charcount)
          {
            send_to_descriptor(d, "Unknown option.\nWhat is your choice? ");
            return;
          }
          if (d->acc->status[i - 1] == CHSTATUS_DELETED)
          {
            send_to_descriptor(d, "You are unable to log onto that player.\nWhat is your choice? ");
            return;
          }

          {
            d->acc->ch = load_character(d->acc, d->acc->character[i - 1]);
            d->acc->ch->acc = d->acc;
            d->acc->ch->d = d;
            setup_char_events(d->acc->ch);
            d->state = STATE_PLAYING;
            d->acc->cur_char = i - 1;
            ++d->acc->chlogins[i - 1];
            destroy_d_events(d, EVENT_LOGINIDLE);

            if (d->acc->ch->level >= LEVEL_IMMORTAL)
              d->acc->status[i - 1] = CHSTATUS_IMMORTAL;

            if (!IS_SET(d->acc->ch->cflags, CFLAG_OLDPLAYER))
            {
              syslog("LOGIN", "%s (on %s@%s) is now playing for the first time!", d->acc->ch->name, d->acc->name, d->ip);
              login_msg_new(d->acc->ch);
              SET_BIT(d->acc->ch->cflags, CFLAG_OLDPLAYER);
              if (IS_SET(d->acc->ch->cflags, CFLAG_HARDCORE))
                d->acc->ch->plane = PLANE_HNEWBIE;
              else
                d->acc->ch->plane = PLANE_NEWBIE;
              d->acc->ch->maxhp = 1000;
              d->acc->ch->maxmove = 1000;
              d->acc->ch->maxmana = 1000;
              d->acc->ch->hp = d->acc->ch->maxhp;
              d->acc->ch->move = d->acc->ch->maxmove;
              d->acc->ch->mana = d->acc->ch->maxmana;
              d->acc->ch->level = LEVEL_MORTAL;
              d->acc->ch->sightradius = 5; /* pwipe: increase this to 50 */
              strlcpy(d->acc->ch->prompt, "#1[#2%pP#1] #1[#2%h#1/#2%HH %m#1/#2%MM %v#1/#2%VV#1]#n> ", 256);
              send_to_char(d->acc->ch, "\nWelcome to Undertow. This is a completely custom MUD. We recommend that you read #RHELP NEWBIE#n.\n");
            }
            else
            {
              syslog("LOGIN", "%s (on %s@%s) is now playing.", d->acc->ch->name, d->acc->name, d->ip);
              login_msg(d->acc->ch);
              do_look(d->acc->ch, "");
            }

            save_account(d->acc);
          }
        break;

        case 'r':
        case 'R':
          if (new_changes(d->acc) == 0)
          {
            send_to_descriptor(d, "There are no new changes.\nWhat is your choice? ");
            return;
          }
          show_changes(d->acc, -1);
          send_to_descriptor(d, "\n#nWhat is your choice? ");
        break;

        case 'c':
        case 'C':
          d->state = STATE_CONFIG_MENU;
          show_account_settings(d);
        break;

        case 's':
        case 'S':
          send_who(d);
          send_to_descriptor(d, "#nWhat is your choice? ");
        break;

        case 'n':
        case 'N':
          if (d->acc->charcount >= MAX_CHARS_PER_ACCOUNT)
          {
            send_to_descriptor(d, "You have met your maximum amount of characters for this account.\nWhat is your choice?");
            return;
          }

          d->state = STATE_NEW_CHAR;
          send_to_descriptor(d, "What do you want your name to be? ");
        break;

        case 'd':
        case 'D':
          if (d->acc->charcount == 0)
          {
            send_to_descriptor(d, "You have no characters to delete.\nWhat is your choice? ");
            return;
          }

          show_delete_menu(d);
          d->state = STATE_DELETE_CHAR;
        break;

        case 'q':
        case 'Q':
          save_account(d->acc);
          syslog("QUIT", "%s@%s quit at the login screen.", d->acc->name, d->ip);
          sckoutput(d->socket, "Goodbye.\n");

          wipe_descriptor(d);
        break;

        case '\0':
          show_account_menu(d, 1);
        break;

        default:
          send_to_descriptor(d, "Unknown option.\nWhat is your choice? ");
        break;
      }
    break;

    case STATE_CONFIG_MENU:
      switch (input[0])
      {
        case 'r':
        case 'R':
          d->state = STATE_ACCT_MENU;
          show_account_menu(d, 1);
        break;

        case 'a':
        case 'A':
          TOGGLE_BIT(d->acc->aflags, AFLAG_ANSI);
          if (IS_SET(d->acc->aflags, AFLAG_ANSI))
          {
            if (IS_SET(d->acc->aflags, AFLAG_ANSI_SPARSE))
              send_to_descriptor(d, "Your prompts will now be colored. If you want full color, turn Full ANSI Color on.\nWhat is your choice? ");
            else
              send_to_descriptor(d, "Welcome to the wonderful world of #Rc#Yo#Gl#Co#Mr#n!\nWhat is your choice? ");
          }
          else
            send_to_descriptor(d, "ANSI color turned off.\nWhat is your choice? ");
        break;

        case 'f':
        case 'F':
          TOGGLE_BIT(d->acc->aflags, AFLAG_ANSI_SPARSE);
          if (!IS_SET(d->acc->aflags, AFLAG_ANSI))
            send_to_descriptor(d, "You would probably notice a subsequent color level change if you had color on.\nWhat is your choice? ");
          else if (IS_SET(d->acc->aflags, AFLAG_ANSI_SPARSE))
            send_to_descriptor(d, "You will now only see colored prompts.\nWhat is your choice? ");
          else
            send_to_descriptor(d, "You will now see full color.\nWhat is your choice? ");
        break;

        case 'l':
        case 'L':
          TOGGLE_BIT(d->acc->aflags, AFLAG_REPLYLOCK);
          if (IS_SET(d->acc->aflags, AFLAG_REPLYLOCK))
            send_to_descriptor(d, "You will now reply only to the last person to whom you sent a tell.\nWhat is your choice? ");
          else
            send_to_descriptor(d, "You will now reply to the person who most recently sent you a tell.\nWhat is your choice? ");
        break;

        case 'p':
        case 'P':
          TOGGLE_BIT(d->acc->aflags, AFLAG_PUBLIC_EMAIL);
          if (IS_SET(d->acc->aflags, AFLAG_PUBLIC_EMAIL))
            send_to_descriptor(d, "Your email address is now public.\nWhat is your choice? ");
          else
            send_to_descriptor(d, "Your email address is now private.\nWhat is your choice? ");
        break;

        case 'c':
        case 'C':
          d->state = STATE_CHANGE_PASS1;
          send_to_descriptor(d, "What is your current password? ");
        send_to_descriptor(d, password_on);
        break;

        case 'e':
        case 'E':
          d->state = STATE_SET_EMAIL;
          send_to_descriptor(d, "What is your email address? ");
        break;

        case '\0':
          show_account_settings(d);
        break;

        case 'd':
        case 'D':
          SET_BIT(d->acc->aflags, AFLAG_ANSI);
          REMOVE_BIT(d->acc->aflags, AFLAG_ANSI_SPARSE);
          REMOVE_BIT(d->acc->aflags, AFLAG_PUBLIC_EMAIL);
          strlcpy(d->acc->email, "Unset", 128);
          send_to_descriptor(d, "Reverted to default settings.\nWhat is your choice?");
        break;

        default:
          send_to_descriptor(d, "Unknown option.\nWhat is your choice? ");
        break;
      }
    break;

    case STATE_CHANGE_PASS1:
      send_to_descriptor(d, password_off);
      if (input[0] == 0)
      {
        d->state = STATE_CONFIG_MENU;
        send_to_descriptor(d, "\nWhat is your choice? ");
        return;
      }

      if (strcmp(d->acc->password, input))
      {
        send_to_descriptor(d, "\nIncorrect password.\nWhat is your choice? ");
        d->state = STATE_CONFIG_MENU;
        return;
      }

      d->state = STATE_CHANGE_PASS2;
      d->acc->temp[0] = 0;
      send_to_descriptor(d, "\nWhat do you want your new password to be? ");
      send_to_descriptor(d, password_on);
    break;

    case STATE_CHANGE_PASS2:
      send_to_descriptor(d, password_off);
      if (input[0] == 0)
      {
        d->state = STATE_CONFIG_MENU;
        send_to_descriptor(d, "\nWhat is your choice? ");
        return;
      }

      i = check_string(input, 4, 12, 1);
      if (i == 1)
        send_to_descriptor(d, "\nYour password must have at least 4 characters.\n");
      else if (i == 2)
        send_to_descriptor(d, "\nYour password can contain at most 12 characters.\n");
      else if (i == 4)
        send_to_descriptor(d, "\nYour password can contain only letters and numbers.\n");
      else
      {
        strlcpy(d->acc->temp, input, 128);
        send_to_descriptor(d, "\nPlease retype your password for confirmation. ");
        d->state = STATE_CHANGE_PASS3;
        send_to_descriptor(d, password_on);
        return;
      }
      send_to_descriptor(d, "\nWhat do you want your password to be? ");
      send_to_descriptor(d, password_on);
    break;

    case STATE_CHANGE_PASS3:
      send_to_descriptor(d, password_off);
      if (input[0] == 0)
      {
        d->state = STATE_CONFIG_MENU;
        send_to_descriptor(d, "\nWhat is your choice? ");
        return;
      }

      if (strcmp(d->acc->temp, input))
      {
        send_to_descriptor(d, "\nPasswords must match exactly.\nWhat is your choice? ");
        d->state = STATE_CONFIG_MENU;
        return;
      }

      strlcpy(d->acc->password, input, 32);
      send_to_descriptor(d, "\nYour password is now changed.\nWhat is your choice? ");
      d->state = STATE_CONFIG_MENU;
      save_account(d->acc);
    break;

    case STATE_SET_EMAIL:
      i = check_string(input, -1, 47, 0);
      if (i == 2)
        send_to_descriptor(d, "Your email can contain at most 47 characters.\n");
      else if (strlen(input) == 0)
      {
        strlcpy(d->acc->email, "Unset", 128);
        d->state = STATE_CONFIG_MENU;
        send_to_descriptor(d, "Your email address is now unset.\nWhat is your choice? ");
      }
      else
      {
        replace(input, buf, "%", "", MAX_STRING_LENGTH);
        replace(buf, input, "~", "-", MAX_STRING_LENGTH);
        strlcpy(d->acc->email, input, 128);
        d->state = STATE_CONFIG_MENU;
        snprintf(buf, MAX_STRING_LENGTH, "Your email address is now %s.\nWhat is your choice? ", input);
        send_to_descriptor(d, buf);
        return;
      }
      send_to_descriptor(d, "What is your email address? ");
    break;

    case STATE_DELETE_CHAR:
      if (input[0] == 'r' || input[0] == 'R')
      {
        d->state = STATE_ACCT_MENU;
        show_account_menu(d, 1);
        return;
      }
      if (input[0] == 0)
      {
        show_delete_menu(d);
        return;
      }

      i = atoi(input);
      if (i < 1 || i > d->acc->charcount)
        send_to_descriptor(d, "Out of bounds.\nWhich character do you want to delete? ");
      else
      {
        d->acc->name[15] = --i;
        snprintf(buf, MAX_STRING_LENGTH, "Are you sure you want to delete %s?", d->acc->character[i]);
        send_to_descriptor(d, buf);
        d->state = STATE_CONFIRM_DEL;
      }
    break;

    case STATE_CONFIRM_DEL:
      if (input[0] == 'y' || input[0] == 'Y')
      {
        i = d->acc->name[15];
        d->acc->name[15] = 0;
        snprintf(buf, MAX_STRING_LENGTH, "%s has been deleted.\nWhat is your choice? ", d->acc->character[i]);
        send_to_descriptor(d, buf);
        syslog("DELETION", "%s (on %s@%s) has selfdeleted.", d->acc->character[i], d->acc->name, d->ip);
        d->state = STATE_ACCT_MENU;
        d->acc->chtime[i] = 0;
        snprintf(buf, MAX_STRING_LENGTH, "rm -f account/%s.chr", d->acc->character[i]);
        to_lower(buf);
        system(buf);

        d->acc->charcount--;
        while (i < d->acc->charcount)
        {
          strlcpy(d->acc->character[i], d->acc->character[i + 1], 32);
          i++;
        }
        save_account(d->acc);
        return;
      }
      d->state = STATE_ACCT_MENU;
      show_account_menu(d, 1);
    break;

    case STATE_NEW_CHAR:
      i = 0;
      if (input[0] == 0)
      {
        d->state = STATE_ACCT_MENU;
        show_account_menu(d, 1);
        return;
      }

      i = check_string(input, 4, 12, 0);
      if (i == 1)
        send_to_descriptor(d, "Your name must have atleast 4 letters.\n");
      else if (i == 2)
        send_to_descriptor(d, "Your name can contain at most 12 letters.\n");
      else if (i == 4 || i == 3)
        send_to_descriptor(d, "Your name can contain only letters.\n");
      else
      {
        input[0] = UPPER(input[0]);
        for (dt = dhead; dt; dt = dt->next)
          if ((d != dt) && !stricmp(input, dt->acc->character[dt->acc->charcount]))
            i = 1;
        if (i == 0 && char_exists(input))
          i = 1;
        if (i)
        {
          snprintf(buf, MAX_STRING_LENGTH, "The name %s is already taken.\nWhat do you want your name to be? ", input);
          send_to_descriptor(d, buf);
          return;
        }
        strlcpy(d->acc->character[d->acc->charcount], input, 32);
        snprintf(buf, MAX_STRING_LENGTH, "Are you sure you want the name %s? ", input);
        send_to_descriptor(d, buf);
        d->state = STATE_CONFIRM_CHAR;
        return;
      }
      send_to_descriptor(d, "What do you want your name to be? ");
    break;

    case STATE_CONFIRM_CHAR:
      if (input[0] == 'y' || input[0] == 'Y')
      {
        if (d->acc->logins == 1)
        {
          d->state = STATE_ACCT_MENU;
          finish_creation(d, 0);
          return;
        }
        d->state = STATE_GET_HARDCORE;
        send_to_descriptor(d, "Do you want this character to be hardcore (death = deletion)? ");
      }
      else
      {
        d->state = STATE_ACCT_MENU;
        show_account_menu(d, 1);
      }
    break;

    case STATE_GET_HARDCORE:
      finish_creation(d, input[0] == 'y' || input[0] == 'Y');
    break;
  }
}

void finish_creation(DESCRIPTOR_DATA *d, int hardcore)
{
  char buf[MAX_STRING_LENGTH];
  d->state = STATE_ACCT_MENU;
  d->acc->charcount++;
  d->acc->totalchars++;
  d->acc->ch = new_character();
  strlcpy(d->acc->ch->name, d->acc->character[d->acc->charcount-1], 32);
  strlcpy(d->acc->ch->accname, d->acc->name, 32);
  snprintf(buf, MAX_STRING_LENGTH, "Character %s created.\n", d->acc->ch->name);
  send_to_descriptor(d, buf);
  buf[0] = 0;
  if (hardcore)
  {
    SET_BIT(d->acc->ch->cflags, CFLAG_HARDCORE);
    snprintf(buf, MAX_STRING_LENGTH, "hardcore ");
    d->acc->status[d->acc->charcount-1] = CHSTATUS_HARDCORE;
  }
  else
    d->acc->status[d->acc->charcount-1] = CHSTATUS_NORMAL;

  log_time(d->acc->chfirsttime[d->acc->charcount-1]);
  syslog("CREATION", "New %scharacter %s (on %s@%s) has been created.", buf, d->acc->ch->name, d->acc->name, d->ip);
  save_account(d->acc);
  free_character(d->acc->ch);
  d->acc->ch = NULL;
  show_account_menu(d, 1);
  if (d->acc->logins == 1)
    send_to_descriptor(d, "\n#r[#1HINT#r]#3 Press '#R1#3' to log into your new character.\n");
}

void wipe_character(CHAR_DATA *ch)
{
  OBJECT_DATA *obj, *tempobj;

  destroy_all_events(ch, EVENT_ALL);

  obj = ch->obj;
  while (obj)
  {
    tempobj = obj->next_local;
    destroy_obj_events(obj, EVENT_ALL);
    unlink_object(obj);
    free_object(obj);
    obj = tempobj;
  }

  ch->acc->ch = NULL;
  unlink_character(ch);
  free_character(ch);
}

void wipe_descriptor(DESCRIPTOR_DATA *d)
{
  close(d->socket);
  d->state = STATE_VOID;
  d->socket = -1;
  destroy_d_events(d, EVENT_ALL);
  unlink_account(d->acc);
  unlink_descriptor(d);
  free_account(d->acc);
  free_descriptor(d);
}
