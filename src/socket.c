#include "undertow.h"

int listensocket;

void sckcreate(int port)
{
  struct sockaddr_in my_addr;
  static int so_reuseaddr = 1;

  listensocket = socket(AF_INET, SOCK_STREAM, 0);

  fcntl(listensocket, F_SETFL, O_NONBLOCK);

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);
  my_addr.sin_addr.s_addr = INADDR_ANY;

  setsockopt(listensocket, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof so_reuseaddr);
  bind(listensocket, (struct sockaddr *) &my_addr, sizeof(struct sockaddr));
  listen(listensocket, 5);
}

int sckaccept(char *ip, char *host)
{
  struct sockaddr_in  address;
  unsigned int        addr_size;
  int                 ret;
  struct hostent     *h;

  addr_size = sizeof(struct sockaddr_in);
  ret = accept(listensocket, (struct sockaddr *) &address, &addr_size);

  if (ret == -1)
    return SOCK_WOULDWAIT;

  fcntl(ret, F_SETFL, O_NONBLOCK);

  strlcpy(ip, inet_ntoa(address.sin_addr), 32);

  if (!strcmp(ip, "127.0.0.1"))
    strlcpy(host, "localhost", 128);
  else
  {
    getpeername(ret, (struct sockaddr *) &address, &addr_size);
    h = gethostbyaddr((char *) &address.sin_addr, sizeof address.sin_addr, AF_INET);
    if (h == NULL)
      strlcpy(host, ip, 128);
    else
      strlcpy(host, h->h_name, 128);
  }
  return ret;
}

int sckinput(int sck, char *buf)
{
  char buf2[MAX_INPUT_LENGTH];
  int ret = 0;
  int len = 0;
  int max = MAX_INPUT_LENGTH / 2;
  int i = 0, o = 0;

  errno = 0;
  while (42)
  {
    ret = recv(sck, buf2 + len, max, 0);
    if (errno == EWOULDBLOCK || len == max || ret <= 0)
      break;
    len += ret;
    if (len >= max)
      return SOCK_FLOODING;
  }

  buf2[len+1] = 0;

  while (i < len && buf2[i] > 0)
    buf[o++] = buf2[i++];
  buf[o] = 0;
  if (len <= 0 && errno != EWOULDBLOCK)
    return SOCK_CUTCONNECTION;
  else if (len <= 0)
    return SOCK_WOULDWAIT;

  buf[len] = 0;

  if (buf[0] == 0)
    return SOCK_NOINPUT;

  return SOCK_SUCCESS;
}

void sckoutput(int sck, char *buf)
{
  int totalsent, len, x;
  char buf2[MAX_STRING_LENGTH];
  if (buf[0] == 0 || sck <= 0)
    return;

  replace(buf, buf2, "\n", "\r\n", MAX_STRING_LENGTH);

  len = strlen(buf2);
  totalsent = 0;
  while (totalsent < len)
  {
    x = send(sck, totalsent + buf2, len - totalsent, 0);
    totalsent += x;
    if (x <= 0)
    {
      buf2[0] = 0;
      break;
    }
  }
}

void sckclose()
{
  close(listensocket);
}
