#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <ctype.h>

/* all of this is socket stuff */
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>

#define MUD_NAME                 "Undertow"
#define COL_NAME                 "#CU#cndertow#n"
#define OWNER                    "Sartak"
#define PORT                     (9009)
#define MAX_CHARS_PER_ACCOUNT    (9)
#define TICKS_PER_SECOND         (8)
#define IDLE_OUT                 (TICKS_PER_SECOND * 60 * 60)

#define MAX_INPUT_LENGTH         (8192)
#define MAX_STRING_LENGTH        (8192)

#define A                        (1)
#define B                        (2)
#define C                        (2<<1)
#define D                        (2<<2)
#define E                        (2<<3)
#define F                        (2<<4)
#define G                        (2<<5)
#define H                        (2<<6)
#define I                        (2<<7)
#define J                        (2<<8)
#define K                        (2<<9)
#define L                        (2<<10)
#define M                        (2<<11)
#define N                        (2<<12)
#define O                        (2<<13)
#define P                        (2<<14)
#define Q                        (2<<15)
#define R                        (2<<16)
#define S                        (2<<17)
#define T                        (2<<18)
#define U                        (2<<19)
#define V                        (2<<20)
#define W                        (2<<21)
#define X                        (2<<22)
#define Y                        (2<<23)
#define Z                        (2<<24)

#define LEVEL_MORTAL             (1)
#define LEVEL_AVATAR             (2)
#define LEVEL_IMMORTAL           (5)
#define LEVEL_TRUSTEE            (6)
#define LEVEL_DEMIGOD            (8)
#define LEVEL_SUPREME            (12)

/* pwipe: fix so CFLAG_NPC is (A) and CFLAG_OLDPLAYER becomes CFLAG_NEWPLAYER */
#define CFLAG_OLDPLAYER          (A)
#define CFLAG_HARDCORE           (B)
#define CFLAG_DELETED            (C)
#define CFLAG_LOGGED             (D)
#define CFLAG_NPC                (E)

#define CTFLAG_AFK               (A)

#define AFLAG_ANSI               (A)
#define AFLAG_PUBLIC_EMAIL       (B)
#define AFLAG_ANSI_SPARSE        (C)
#define AFLAG_LOGGED             (D)
#define AFLAG_REPLYLOCK          (E)

#define OFLAG_NOTAKE             (A)

#define OTFLAG_MARKED            (A)

#define CLASS_ONE               (-2) /* CLASS_ONE should match all but NONE */
#define CLASS_ANY               (-1) /* CLASS_ANY should match *any* */
#define CLASS_NONE               (0)
#define CLASS_PLANARIST          (1)
#define CLASS_ELEMENTALIST       (2)
#define CLASS_AURAMANCER         (3)
#define CLASS_WARRIOR            (4)

#define STATE_PLAYING            (0)
#define STATE_VOID               (1)
#define STATE_LINKDEAD           (2)
#define STATE_GET_NAME           (3)
#define STATE_CONFIRM_NAME       (4)
#define STATE_NEW_PASS           (5)
#define STATE_CONFIRM_PASS       (6)
#define STATE_GET_ANSI           (7)
#define STATE_GET_PASS           (8)
#define STATE_ACCT_MENU          (9)
#define STATE_NEW_CHAR           (10)
#define STATE_CONFIRM_CHAR       (11)
#define STATE_DELETE_CHAR        (12)
#define STATE_CONFIRM_DEL        (13)
#define STATE_CONFIG_MENU        (14)
#define STATE_SET_EMAIL          (15)
#define STATE_CHANGE_PASS1       (16)
#define STATE_CHANGE_PASS2       (17)
#define STATE_CHANGE_PASS3       (18)
#define STATE_GET_HARDCORE       (19)
#define STATE_MAIL_MENU          (20)
#define STATE_MAIL_READ          (21)
#define STATE_MAIL_COMPOSE_REC   (22)
#define STATE_MAIL_COMPOSE_SUB   (23)
#define STATE_MAIL_COMPOSE_MSG   (24)
#define STATE_MAIL_COMPOSE_CFRM  (25)
#define STATE_MAIL_DELETE        (26)
#define STATE_MAIL_DELETE_CFRM   (27)

#define CHSTATUS_NORMAL          (0)
#define CHSTATUS_HARDCORE        (1)
#define CHSTATUS_DELETED         (2)
#define CHSTATUS_IMMORTAL        (3)

#define SOCK_FLOODING            (-2)
#define SOCK_WOULDWAIT           (-1)
#define SOCK_SUCCESS             (0)
#define SOCK_CUTCONNECTION       (1)
#define SOCK_NOINPUT             (2)

#define PLANE_NONE               (0)
#define PLANE_NEWBIE             (1)
#define PLANE_NEXUS              (2)
#define PLANE_HNEWBIE            (3)
#define PLANE_HNEXUS             (4)
#define PLANE_HEAVEN             (10)
#define PLANE_HELL               (11)
#define PLANE_JAIL               (12)
#define PLANE_INTERPLANE         (20)
#define PLANE_INTRAPLANE         (21)

#define SENSE_NONE               (0)
#define SENSE_SIGHT              (1)
#define SENSE_HEARING            (2)
#define SENSE_SMELL              (3)
#define SENSE_TASTE              (4)
#define SENSE_TOUCH              (5)
#define SENSE_INSIGHT            (6)

#define POS_STANDING             (0)
#define POS_SITTING              (1)
#define POS_SLEEPING             (2)
#define POS_FIGHTING             (3)
#define POS_ANY                  (4)

#define LOG_NORMAL               (0)
#define LOG_ALWAYS               (1)
#define LOG_NEVER                (2)

#define EVENT_ALL                (-1)
#define EVENT_TICK               (1)
#define EVENT_SAVE               (2)
#define EVENT_LINKDEAD           (3)
#define EVENT_HEAL               (4)
#define EVENT_IDLE               (5)
#define EVENT_PORTAL             (6)
#define EVENT_CPORTAL            (7)
#define EVENT_DESTROY            (8)
#define EVENT_UNMARK             (9)
#define EVENT_LOGINIDLE         (10)
#define EVENT_CHANGEPOS         (11)
#define EVENT_GARBAGECOLLECT    (12)
#define EVENT_MESSAGE           (13)
#define EVENT_DELAY             (14)
#define EVENT_EXITPORTAL        (15)

#define OTYPE_NONE               (0)
#define OTYPE_PORTAL             (1)
#define OTYPE_WEAPON             (2)
#define OTYPE_ARMOR              (3)

#define EQUIP_RHAND              (0)
#define EQUIP_LHAND              (1)
#define EQUIP_LFINGER            (2)
#define EQUIP_RFINGER            (3)
#define EQUIP_ARMS               (4)
#define EQUIP_FEET               (5)
#define EQUIP_LEGS               (6)
#define EQUIP_TORSO              (7)
#define EQUIP_NECK               (8)
#define EQUIP_HEAD               (9)
#define EQUIP_MAX               (10)

#define TO_ACTOR       (A)
#define TO_VICTIM      (B)
#define TO_NOTACTOR    (C)
#define TO_NOTVICTIM   (D)
#define TO_NOTACTVIC   (E)

#define TYPE_ACT_CHAR  (F)
#define TYPE_ACT_OBJ   (G)
#define TYPE_ACT_NULL  (H)
#define TYPE_VIC_CHAR  (I)
#define TYPE_VIC_OBJ   (J)
#define TYPE_VIC_NULL  (K)

#define TYPE_CHAR (1)
#define TYPE_OBJ  (2)
#define TYPE_NULL (3)

#define SET_BIT(field, bit)      (field |= bit)
#define REMOVE_BIT(field, bit)   (field &= ~(bit))
#define TOGGLE_BIT(field, bit)   (field ^= bit)
#define IS_SET(field, bit)       (field & bit)
#define LOWER(c)                 ((c) >= 'A' && (c) <= 'Z' ? (c) + 'a' - 'A' : (c))
#define UPPER(c)                 ((c) >= 'a' && (c) <= 'z' ? (c) + 'A' - 'a' : (c))
#define MAX(a, b)                ((a) > (b) ? (a) : (b))
#define MIN(a, b)                ((a) > (b) ? (b) : (a))
#define IS_NPC(ch)               (IS_SET(ch->cflags, CFLAG_NPC))

typedef struct descriptor_data   DESCRIPTOR_DATA;
typedef struct char_data         CHAR_DATA;
typedef struct account_data      ACCOUNT_DATA;
typedef struct memory_data       MEMORY_DATA;
typedef struct event_data        EVENT_DATA;
typedef struct object_data       OBJECT_DATA;
typedef struct alias_data        ALIAS_DATA;
typedef struct mail_data         MAIL_DATA;

typedef void CMD_FUN             (CHAR_DATA *ch, char *arguments);
typedef int  EVENT_FUN           (EVENT_DATA *event);

struct descriptor_data
{
  DESCRIPTOR_DATA *next;
  ACCOUNT_DATA    *acc;
  EVENT_DATA      *event;
  int              socket;
  int              state;
  int              linkdeath;
  char             ip[32];
  char             host[128];
  char             output[MAX_STRING_LENGTH];
  char             input[MAX_INPUT_LENGTH];
};

struct char_data
{
  CHAR_DATA       *next;
  CHAR_DATA       *fighting;
  DESCRIPTOR_DATA *d;
  ACCOUNT_DATA    *acc;
  EVENT_DATA      *event;
  OBJECT_DATA     *obj;
  ALIAS_DATA      *alias;
  OBJECT_DATA     *equip[EQUIP_MAX];
  char             accname[32];
  char             name[32];
  char             prompt[256];
  char             reply[32];
  int              cflags;
  int              tflags;
  int              time;
  int              level;
  int              invis;
  int              x;
  int              y;
  int              plane;
  int              position;
  int              plutos;
  int              class;
  int              sightradius;
  int              hp, maxhp;
  int              mana, maxmana;
  int              move, maxmove;
  int              resistance;
  int              primal;
  int              idle;
  int              logintime;
};

struct account_data
{
  ACCOUNT_DATA    *next;
  DESCRIPTOR_DATA *d;
  CHAR_DATA       *ch;
  MAIL_DATA       *mail;
  char             name[32];
  char             password[32];
  char             email[128];
  char             temp[128]; /* for menu traversal purposes only! */
  char             lasthost[128];
  char             firsttime[32];
  char             lasttime[32];
  char             character[MAX_CHARS_PER_ACCOUNT][32];
  char             chfirsttime[MAX_CHARS_PER_ACCOUNT][32];
  int              chtime[MAX_CHARS_PER_ACCOUNT];
  int              status[MAX_CHARS_PER_ACCOUNT];
  int              chlogins[MAX_CHARS_PER_ACCOUNT];
  int              class[MAX_CHARS_PER_ACCOUNT];
  int              totalchars;
  int              time;
  int              failed;
  int              charcount;
  int              cur_char;
  int              logins;
  int              aflags;
  int              changes;
};

struct object_data
{
  OBJECT_DATA     *next;
  OBJECT_DATA     *next_local;
  CHAR_DATA       *holder;
  OBJECT_DATA     *container;
  EVENT_DATA      *event;
  char             keywords[128];
  char             shortdesc[128];
  char             longdesc[128];
  char             owner[32];
  int              level;
  int              oflags;
  int              tflags;
  int              x;
  int              y;
  int              plane;
  int              type;
  int              v1, v2, v3, v4, v5, v6; /* values specific to object's type */
};

struct event_data
{
  EVENT_DATA      *next;
  EVENT_DATA      *next_local;

  union
  {
    CHAR_DATA *ch;
    OBJECT_DATA *obj;
    DESCRIPTOR_DATA *d;
  } owner;

  union
  {
    CHAR_DATA *ch;
    OBJECT_DATA *obj;
  } victim;

  EVENT_FUN       *fun;
  char             argument[512];
  int              args[3];
  int              ticks_left;
  int              type;
  int              touched;
};

struct mail_data
{
  int time;
  int read;
  char sender[32];
  char recipient[32];
  char subject[128];
  char message[1024];
};

struct memory_data
{
  unsigned long    bytes_allocated;
  unsigned long    d_allocated, d_recycled, d_free;
  unsigned long    ch_allocated, ch_recycled, ch_free;
  unsigned long    acc_allocated, acc_recycled, acc_free;
  unsigned long    event_allocated, event_recycled, event_free;
  unsigned long    obj_allocated, obj_recycled, obj_free;
  unsigned long    alias_allocated, alias_recycled, alias_free;
};

struct command_data
{
  const char  *command;
  CMD_FUN     *fun;
  int          position;
  int          minlevel;
  int          log;
  int          appear;
  int          class;
  int          disabled;
};

struct alias_data
{
  ALIAS_DATA *next;
  ALIAS_DATA *next_local;
  char in[16];
  char out[128];
};

/* act_comm.c */
  extern char lastchat[10][700];
  extern char lastimm[10][700];
  extern char lasttru[10][700];
  extern char lastdem[10][700];
  extern char lastsup[10][700];

/* act_info.c */
  void send_who(DESCRIPTOR_DATA *d);

/* changes.c */
  void save_changes();
  void load_changes();
  void free_changes();
  int new_changes(ACCOUNT_DATA *acc);
  void show_changes(ACCOUNT_DATA *acc, int type);

/* class_auramancer.c */
  void auramancer_learn(CHAR_DATA *ch, char *arguments);
  void auramancer_powers(CHAR_DATA *ch, char *arguments);

/* class_elementalist.c */
  void elementalist_learn(CHAR_DATA *ch, char *arguments);
  void elementalist_powers(CHAR_DATA *ch, char *arguments);

/* class_planarist.c */
  void planarist_learn(CHAR_DATA *ch, char *arguments);
  void planarist_powers(CHAR_DATA *ch, char *arguments);

/* class_warrior.c */
  void warrior_learn(CHAR_DATA *ch, char *arguments);
  void warrior_powers(CHAR_DATA *ch, char *arguments);

/* classes.c */

/* comm.c */
  extern int RUNNING;
  extern char str_boot_time[64];
  extern char str_reset_time[64];
  extern time_t current_time;
  extern time_t boot_time;
  extern time_t reset_time;
  extern int last_socket;

  void game_loop();
  void new_connection();
  void input();
  void output();
  void cut_connection(DESCRIPTOR_DATA *d);
  void send_to_descriptor(DESCRIPTOR_DATA *d, char *buf);
  void send_to_char(CHAR_DATA *ch, char *buf);
  void colorize(char *in, char *out, int size);
  void decolorize(char *in, char *out);

/* copyover.c */
  void initiate_copyover();
  void recover_copyover();

/* event.c */
  /* event_* functions are defined lower */

/* findobj.c */
  OBJECT_DATA *find_obj(CHAR_DATA *ch, char *arguments, char *order);

/* help.c */
  void free_all_helps();
  void load_all_helps();
  void send_hlist_to_char(CHAR_DATA *ch);

/* interp.c */
  void parse(DESCRIPTOR_DATA *d, char *input);
  void autodisable(int read, char *cmdlev, FILE *fp);

/* location.c */
  int is_local(int x1, int y1, int p1, int x2, int y2, int p2, int r);
  void show_char_to_char(CHAR_DATA *ch, CHAR_DATA *vic);
  void show_obj_to_char(CHAR_DATA *ch, OBJECT_DATA *obj);
  void put_sense(CHAR_DATA *ch, int sense, char *msg);
  void put_sense_area(int x, int y, int plane, int sense, char *msg);
  void put_sense_except(CHAR_DATA *ch, int x, int y, int plane, int sense, char *msg);
  void put_sense_except2(CHAR_DATA *ach, CHAR_DATA *bch, int x, int y, int plane, int sense, char *msg);

/* login.c */
  int check_string(char *buf, int minlength, int maxlength, int alphanumeric);
  void nanny(DESCRIPTOR_DATA *d, char *input);
  void show_account_menu(DESCRIPTOR_DATA *d, int askchoice);
  void show_delete_menu(DESCRIPTOR_DATA *d);
  void show_account_settings(DESCRIPTOR_DATA *d);
  void wipe_character(CHAR_DATA *ch);
  void wipe_descriptor(DESCRIPTOR_DATA *d);

/* mail.c */
  int new_mail(DESCRIPTOR_DATA *d);
  void show_mail_menu(DESCRIPTOR_DATA *d);
  void mail_nanny(DESCRIPTOR_DATA *d, char *input);

/* main.c */
  extern int port;
  extern int DEBUG;
  extern int copyover;
  extern FILE *fsyslog;
  int main(int argc, char *argv[]);
  void shut_down();

/* mem.c */
  extern MEMORY_DATA mem;
  extern DESCRIPTOR_DATA *dhead;
  extern DESCRIPTOR_DATA *dfree;
  extern CHAR_DATA *chhead;
  extern CHAR_DATA *chfree;
  extern ACCOUNT_DATA *acchead;
  extern ACCOUNT_DATA *accfree;
  extern EVENT_DATA *eventhead;
  extern EVENT_DATA *eventfree;
  extern OBJECT_DATA *objhead;
  extern OBJECT_DATA *objfree;
  extern ALIAS_DATA *aliashead;
  extern ALIAS_DATA *aliasfree;

  void free_all();
  DESCRIPTOR_DATA *new_descriptor();
  void free_descriptor(DESCRIPTOR_DATA *d);
  void link_descriptor(DESCRIPTOR_DATA *d);
  void unlink_descriptor(DESCRIPTOR_DATA *d);
  CHAR_DATA *new_character();
  void free_character(CHAR_DATA *ch);
  void link_character(CHAR_DATA *ch);
  void unlink_character(CHAR_DATA *ch);
  ACCOUNT_DATA *new_account();
  void free_account(ACCOUNT_DATA *acc);
  void link_account(ACCOUNT_DATA *acc);
  void unlink_account(ACCOUNT_DATA *acc);
  EVENT_DATA *new_event();
  void free_event(EVENT_DATA *event);
  void link_event(EVENT_DATA *event);
  void unlink_event(EVENT_DATA *event);
  OBJECT_DATA *new_object();
  void free_object(OBJECT_DATA *obj);
  void link_object(OBJECT_DATA *obj);
  void unlink_object(OBJECT_DATA *obj);
  ALIAS_DATA *new_alias();
  void free_alias(ALIAS_DATA *alias);
  void link_alias(ALIAS_DATA *alias);
  void unlink_alias(ALIAS_DATA *alias);

  void garbage_collector_init();

/* misc.c */
  void info(CHAR_DATA *ch, char *info, int newline);
  void login_msg(CHAR_DATA *ch);
  void login_msg_new(CHAR_DATA *ch);
  void logout_msg(CHAR_DATA *ch);
  CHAR_DATA *find_char_world(CHAR_DATA *ch, char *name);

/* objects.c */
  void create_portal(int type, CHAR_DATA *ch, int x1, int y1, int p1, int x2, int y2, int p2);
  void update_objects(CHAR_DATA *ch);

/* save.c */
  long fread_number(FILE *fp);
  void fread_word(FILE *fp, char *buf);
  void fread_line(FILE *fp, char *buf);
  void fread_eof(FILE *fp, char *buf);
  int char_exists(char *input);
  CHAR_DATA *load_character(ACCOUNT_DATA *acc, char *name);
  void save_character(CHAR_DATA *ch);
  void load_account(char *name, ACCOUNT_DATA *acc);
  void save_account(ACCOUNT_DATA *acc);
  void save_address(DESCRIPTOR_DATA *d);
  void check_address(char *ip, char *buf);

/* socials.c */
  int parse_social(CHAR_DATA *ch, char *verb, char *arguments);
  void act(char *msg, void *actor, void *victim, int type, int sense);

/* socket.c*/
  extern int listensocket;

  void sckcreate(int port);
  int sckaccept(char *ip, char *host);
  int sckinput(int sck, char *buf);
  void sckoutput(int sck, char *buf);
  void sckclose();

/* strl.c */
#ifndef strlcpy
  size_t strlcpy(char *dst, const char *src, size_t siz);
  size_t strlcat(char *dst, const char *src, size_t siz);
#endif
#ifndef snprintf
  int snprintf(char * str, size_t size, const char * format, ...);
#endif
  size_t strlpre(char *dst, const char *src, size_t siz);

/* update.c */
  void update();
  void setup_char_events(CHAR_DATA *ch);
  EVENT_DATA *setup_event(CHAR_DATA *owner, CHAR_DATA *victim, EVENT_FUN *fun, char *argument, int ticks_left, int type);
  EVENT_DATA *setup_event_obj(OBJECT_DATA *owner, CHAR_DATA *victim, EVENT_FUN *fun, char *argument, int ticks_left, int type);
  EVENT_DATA *setup_event_d(DESCRIPTOR_DATA *owner, CHAR_DATA *victim, EVENT_FUN *fun, char *argument, int ticks_left, int type);
  int num_events(CHAR_DATA *ch, int type);
  int num_events_obj(OBJECT_DATA *obj, int type);
  EVENT_DATA *find_event(CHAR_DATA *ch, int type);
  EVENT_DATA *find_event_obj(OBJECT_DATA *obj, int type);
  EVENT_DATA *find_event_d(DESCRIPTOR_DATA *d, int type);
  void destroy_all_events(CHAR_DATA *ch, int type);
  void destroy_obj_events(OBJECT_DATA *obj, int type);
  void destroy_d_events(DESCRIPTOR_DATA *d, int type);
  void destroy_one_event(CHAR_DATA *ch, EVENT_DATA *event);

/* utility.c */
  void log_time(char *buf);
  void log_timeofday(char *buf);
  void syslog(char *type, char *arg, ...);
  void error(char *arg);
  void errorf(char *arg, ...);
  void debug(char *arg);
  void debugf(char *arg, ...);
  int number_range(int min, int max);
  int percent();
  int roll(int numdice, int sides, int bonus);
  void serial_time(unsigned long time, char *buf);
  int wordwrap(char *buf, int width);
  int replace(const char *in, char *out, const char *replace, const char *with, int maxlength);
  int stricmp(const char *a, const char *b);
  void to_lower(char *buf);
  int beginning(const char *big, const char *little);
  void one_arg(char *arguments, char *first);
  int time_words(int time, char *ret);
  int time_words_short(int time, char *ret);
  int time_words_long(int time, char *ret);
  void displacement_words(int x1, int y1, int x2, int y2, char *buf);
  void number_words(int n, char *buf);

CMD_FUN do_north;
CMD_FUN do_south;
CMD_FUN do_east;
CMD_FUN do_west;
CMD_FUN do_inventory;
CMD_FUN do_look;
CMD_FUN do_take;
CMD_FUN do_who;

CMD_FUN do_afk;
CMD_FUN do_alarm;
CMD_FUN do_alias;
CMD_FUN do_ansi;
CMD_FUN do_bug;
CMD_FUN do_bugs;
CMD_FUN do_changeadd;
CMD_FUN do_changedel;
CMD_FUN do_changes;
CMD_FUN do_chat;
CMD_FUN do_class;
CMD_FUN do_commands;
CMD_FUN do_config;
CMD_FUN do_delay;
CMD_FUN do_demtalk;
CMD_FUN do_disable;
CMD_FUN do_drop;
CMD_FUN do_email;
CMD_FUN do_emote;
CMD_FUN do_enter;
CMD_FUN do_equipment;
CMD_FUN do_events;
CMD_FUN do_finger;
CMD_FUN do_fingeracct;
CMD_FUN do_flush;
CMD_FUN do_fullansi;
CMD_FUN do_help;
CMD_FUN do_history;
CMD_FUN do_hlist;
CMD_FUN do_hosts;
CMD_FUN do_identify;
CMD_FUN do_immtalk;
CMD_FUN do_last;
CMD_FUN do_learn;
CMD_FUN do_mark;
CMD_FUN do_memory;
CMD_FUN do_ocreate;
CMD_FUN do_powers;
CMD_FUN do_prompt;
CMD_FUN do_recall;
CMD_FUN do_relevel;
CMD_FUN do_reply;
CMD_FUN do_replylock;
CMD_FUN do_save;
CMD_FUN do_say;
CMD_FUN do_sayto;
CMD_FUN do_score;
CMD_FUN do_sit;
CMD_FUN do_sleep;
CMD_FUN do_socials;
CMD_FUN do_spawn;
CMD_FUN do_stand;
CMD_FUN do_suggest;
CMD_FUN do_suggestions;
CMD_FUN do_suptalk;
CMD_FUN do_tell;
CMD_FUN do_time;
CMD_FUN do_timer;
CMD_FUN do_train;
CMD_FUN do_trutalk;
CMD_FUN do_typo;
CMD_FUN do_typos;
CMD_FUN do_users;
CMD_FUN do_whisper;
CMD_FUN do_worth;

CMD_FUN do_full;
CMD_FUN do_copyover;
CMD_FUN do_delete;
CMD_FUN do_invwipe;
CMD_FUN do_quit;
CMD_FUN do_shutdown;
CMD_FUN do_tout;
CMD_FUN do_undelete;

EVENT_FUN event_ticks;
EVENT_FUN event_save;
EVENT_FUN event_linkdeath;
EVENT_FUN event_heal;
EVENT_FUN event_idle;
EVENT_FUN event_portal;
EVENT_FUN event_cportal;
EVENT_FUN event_destroy;
EVENT_FUN event_unmark;
EVENT_FUN event_loginidle;
EVENT_FUN event_changepos;
EVENT_FUN event_garbagecollect;
EVENT_FUN event_message;
EVENT_FUN event_delay;
EVENT_FUN event_exitportal;
