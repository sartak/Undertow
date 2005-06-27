#include "undertow.h"

/* (recipient).mail contents:
   mail
   (time)
   (read)
   (sender)
   (subject)
   (message)
   eom
   mail
   (time)
   (read)
   (sender)
   (subject)
   (message)
   eom
   eof~
*/

int new_mail(DESCRIPTOR_DATA *d)
{
  return 0;
}

void do_mail(CHAR_DATA *ch, char *arguments)
{
  send_to_char(ch, "Unfortunately you currently have to be at the account menu to read or send mail.\n");
}

void show_mail_menu(DESCRIPTOR_DATA *d)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[64];
  int read = 0, unread = 0;
  FILE *fp;
  char file[64];
  int num = 0;
  MAIL_DATA *mail;

  num = 6; /* this is to disable the "warning: unused variable `num'" which is useful but not right now! */

  send_to_descriptor(d, "\n#R+#r---------------------#1[#2Mail#1]#r---------------------#R+\n");
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Account:     #C%-12s                      #r|\n", d->acc->name);
  send_to_descriptor(d, buf);
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Read Mail:   #C%-12d                      #r|\n", read);
  send_to_descriptor(d, buf);
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Unread Mail: #C%-12d                      #r|\n", unread);
  send_to_descriptor(d, buf);
  send_to_descriptor(d, "#R+#r------------------------------------------------#R+\n");

  snprintf(file, 64, "../account/mail/%s.mail", d->acc->name);
  to_lower(file);
  fp = fopen(file, "r");
  if (fp == NULL)
    send_to_descriptor(d, "#r| #nNo mail has been received!                     #r|\n");
  else
  {
    mail = (MAIL_DATA *) malloc(sizeof(MAIL_DATA));
    while (!feof(fp))
    {
      fread_word(fp, buf);
      if (!strcmp(buf, "EOF~"))
        break;

      if (!strcmp(buf, "Mail"))
      {
        memset(mail, 0, sizeof(MAIL_DATA));
        continue;
      }

      if (!strcmp(buf, "Time"))
      {
        mail->time = fread_number(fp);
        continue;
      }

      if (!strcmp(buf, "Read"))
      {
        mail->read = fread_number(fp);
        continue;
      }

      if (!strcmp(buf, "Sender"))
      {
        fread_word(fp, buf);
        strlcpy(mail->sender, buf, 32);
        continue;
      }

      if (!strcmp(buf, "Subject"))
      {
        fread_line(fp, buf);
        strlcpy(mail->subject, buf, 32);
        continue;
      }

      if (!strcmp(buf, "Message"))
      {
        while (!feof(fp))
        {
          fread_line(fp, buf);
          if (!strcmp(buf, "EOM"))
            break;
        }
      }

      if (!strcmp(buf, "EOM"))
      {
        snprintf(buf2, MAX_STRING_LENGTH, "%d", mail->time); /* note: this is so far from what should happen it hurts */
        snprintf(buf, MAX_STRING_LENGTH, "#1[%s%d#1] [#R%s#1] #R%s #n%-40.40s\n", mail->read == 0 ? "#R" : "#1", num, buf2, mail->sender, mail->subject);
        send_to_descriptor(d, buf);
        continue;
      }
    }

    free(mail);
  }

  send_to_descriptor(d, "#R+#r------------------------------------------------#R+\n");
  send_to_descriptor(d, "#r| #1[#3C#1]#2ompose New Mail                             #r|\n");
  send_to_descriptor(d, "#r| #1[#3D#1]#2elete a Letter                              #r|\n");
  send_to_descriptor(d, "#r| #1[#3R#1]#2eturn to Main Menu                          #r|\n");
  send_to_descriptor(d, "#R+#r------------------------------------------------#R+\n");
}

void mail_nanny(DESCRIPTOR_DATA *d, char *input)
{
  switch (d->state)
  {
    case STATE_MAIL_MENU:
      switch (input[0])
      {
        case '\0':
          show_mail_menu(d);
          send_to_descriptor(d, "#nWhat is your choice? ");
        break;
        case 'r':
        case 'R':
          d->state = STATE_ACCT_MENU;
          show_account_menu(d, 0);
          send_to_descriptor(d, "#nWhat is your choice? ");
        break;
        default:
          send_to_descriptor(d, "#nUnknown option.\nWhat is your choice? ");
      }
    break;
    case STATE_MAIL_READ:
    break;
    case STATE_MAIL_COMPOSE_REC:
    break;
    case STATE_MAIL_COMPOSE_SUB:
    break;
    case STATE_MAIL_COMPOSE_MSG:
    break;
    case STATE_MAIL_COMPOSE_CFRM:
    break;
    case STATE_MAIL_DELETE:
    break;
    case STATE_MAIL_DELETE_CFRM:
    break;
  }
}
