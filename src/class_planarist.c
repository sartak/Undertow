#include "undertow.h"

/*
 * Close Portal - ability to close portals
 *              - as level increases the portal closes faster
 *              - also can close newer portals quicker
 * Persistent Portal - ability to create gettable portals
 *                   - one shot only
 *                   - as level increases item lifespan does too
 * Portal Control - can recall to coordinates other than [0,0]
 *                - success depends on level and distance
 * Quick Portal    - summoning portals takes less time
 * Quick Traversal - traversing portals takes less time
 * Quick Escape - can create an instant portal to a random coordinate
 *              - closes instantly (to prevent pursuers)
 *              - as level increases so does distance
 *              - as level increases tendency to go towards origin increases
 * Planemeld - can enter the plane for a limited time to evade attackers
 *           - cannot exit manually until fully learned
 *           - as level increases the planemeld time increases
 * Banish - creates a portal that engulfs an elemental
 *        - the planarist gets some experience (% based on level), no plutos
 *        - doesn't work on players
 */



void planarist_learn(CHAR_DATA *ch, char *arguments)
{
  send_to_char(ch, "You have already learned everything worth knowing.\n");
}

void planarist_powers(CHAR_DATA *ch, char *arguments)
{
  char buf[MAX_STRING_LENGTH];

  snprintf(buf, MAX_STRING_LENGTH, "#R+#r----------------------------[Planarist Powers]#r----------------------------#R+\n");
  send_to_char(ch, buf);
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Close Portal      #1[#R****#2******#1]       #2Persistent Portal #1[#2**********#1]      |\n");
  send_to_char(ch, buf);
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Portal Control    #1[#2**********#1]       #2Quick Portal      #1[#2**********#1]      |\n");
  send_to_char(ch, buf);
  snprintf(buf, MAX_STRING_LENGTH, "#r| #YQuick Traversal   #1[#R**********#1]       #2Quick Escape      #1[#2**********#1]      |\n");
  send_to_char(ch, buf);
  snprintf(buf, MAX_STRING_LENGTH, "#r| #2Banish            #1[#2**********#1]       #2Planemeld         #1[#2**********#1]      |\n");
  send_to_char(ch, buf);
  snprintf(buf, MAX_STRING_LENGTH, "#R+#r----------------------------------------------------------------------------#R+\n");
  send_to_char(ch, buf);
}
