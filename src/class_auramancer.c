#include "undertow.h"

void auramancer_learn(CHAR_DATA *ch, char *arguments)
{
  send_to_char(ch, "You have already learned everything worth knowing.\n");
}

void auramancer_powers(CHAR_DATA *ch, char *arguments)
{
  send_to_char(ch, "You have learned nothing.\n");
}
