#include "undertow.h"

void warrior_learn(CHAR_DATA *ch, char *arguments)
{
  send_to_char(ch, "You have already learned everything worth knowing.\n");
}

void warrior_powers(CHAR_DATA *ch, char *arguments)
{
  send_to_char(ch, "You have learned nothing.\n");
}
