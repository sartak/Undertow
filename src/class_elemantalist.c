#include "undertow.h"

void elementalist_learn(CHAR_DATA *ch, char *arguments)
{
  send_to_char(ch, "You have already learned everything worth knowing.\n");
}

void elementalist_powers(CHAR_DATA *ch, char *arguments)
{
  send_to_char(ch, "You have learned nothing.\n");
}
