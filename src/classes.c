#include "undertow.h"

void do_learn(CHAR_DATA *ch, char *arguments)
{
  switch (ch->class)
  {
    default:
      send_to_char(ch, "You first need to pick a class.\n");
    break;
    case CLASS_PLANARIST:
      planarist_learn(ch, arguments);
    break;
    case CLASS_ELEMENTALIST:
      elementalist_learn(ch, arguments);
    break;
    case CLASS_AURAMANCER:
      auramancer_learn(ch, arguments);
    break;
    case CLASS_WARRIOR:
      warrior_learn(ch, arguments);
    break;
  }
}

void do_powers(CHAR_DATA *ch, char *arguments)
{
  switch (ch->class)
  {
    default:
      send_to_char(ch, "You first need to pick a class.\n");
    break;
    case CLASS_PLANARIST:
      planarist_powers(ch, arguments);
    break;
    case CLASS_ELEMENTALIST:
      elementalist_powers(ch, arguments);
    break;
    case CLASS_AURAMANCER:
      auramancer_powers(ch, arguments);
    break;
    case CLASS_WARRIOR:
      warrior_powers(ch, arguments);
    break;
  }
}

void do_class(CHAR_DATA *ch, char *arguments)
{
  char arg[MAX_STRING_LENGTH];

  if (ch->class != CLASS_NONE)
  {
    send_to_char(ch, "Your path has already been chosen.\n");
    return;
  }

  one_arg(arguments, arg);

  if (!stricmp("warrior", arg))
  {
    ch->class = CLASS_WARRIOR;
    ch->acc->class[ch->acc->cur_char] = CLASS_WARRIOR;
    send_to_char(ch, "You are now a #Rwarrior#n.\n");
  }
  else if (!stricmp("planarist", arg))
  {
    ch->class = CLASS_PLANARIST;
    ch->acc->class[ch->acc->cur_char] = CLASS_PLANARIST;
    send_to_char(ch, "You are now a #Rplanarist#n.\n");
  }
  else if (!stricmp("auramancer", arg))
  {
    ch->class = CLASS_AURAMANCER;
    ch->acc->class[ch->acc->cur_char] = CLASS_AURAMANCER;
    send_to_char(ch, "You are now an #Rauramancer#n.\n");
  }
  else if (!stricmp("elementalist", arg))
  {
    ch->class = CLASS_ELEMENTALIST;
    ch->acc->class[ch->acc->cur_char] = CLASS_ELEMENTALIST;
    send_to_char(ch, "You are now an #Relementalist#n.\n");
  }
  else
  {
    send_to_char(ch, "The following classes are available:\n  Auramancer\n  Elementalist\n  Planarist\n  Warrior\n");
    return;
  }

  save_account(ch->acc);
}
