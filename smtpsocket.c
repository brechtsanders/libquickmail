#include "smtpsocket.h"
#include <ctype.h>

SOCKET socket_open (const char* smtpserver, unsigned int smtpport, char** errmsg)
{
  struct in_addr ipv4addr;
  SOCKET sock;
  //determine IPv4 address of SMTP server
  ipv4addr.s_addr = inet_addr(smtpserver);
  if (ipv4addr.s_addr == INADDR_NONE) {
    struct hostent* addr;
    if ((addr = gethostbyname(smtpserver)) != NULL && (addr->h_addrtype == AF_INET && addr->h_length >= 1 && ((struct in_addr*)addr->h_addr)->s_addr != 0))
      memcpy(&ipv4addr, addr->h_addr, sizeof(ipv4addr));
  }
  if (ipv4addr.s_addr == INADDR_NONE) {
    if (errmsg)
      *errmsg = "Unable to resolve SMTP server host name";
    return INVALID_SOCKET;
  }
  //create socket
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == INVALID_SOCKET) {
    if (errmsg)
      *errmsg = "Error creating socket for SMTP connection";
    return INVALID_SOCKET;
  }
  //connect
  struct sockaddr_in remote_sock_addr;
  remote_sock_addr.sin_family = AF_INET;
  remote_sock_addr.sin_port = htons(smtpport);
  remote_sock_addr.sin_addr.s_addr = ipv4addr.s_addr;
  if (connect(sock, (struct sockaddr*)&remote_sock_addr, sizeof(remote_sock_addr)) != 0) {
    if (errmsg)
      *errmsg = "Error connecting to SMTP server";
    socket_close(sock);
    return INVALID_SOCKET;
  }
  //set linger option
  static const struct linger linger_option = {-1, 2};   //linger 2 seconds when disconnecting
  setsockopt(sock, SOL_SOCKET, SO_LINGER, (const char*)&linger_option, sizeof(linger_option));
  return sock;
}

void socket_close (SOCKET sock)
{
#ifndef __WIN32__
  shutdown(sock, 2);
#else
  closesocket(sock);
#endif
}

int socket_send (SOCKET sock, const char* buf, int len)
{
  if (sock == 0 || !buf)
    return 0;
  if (len < 0)
    len = strlen(buf);
  int total_sent = 0;
  int l = 0;
  while (len > 0 && (l = send(sock, buf, len, 0)) < len) {
    if (l == SOCKET_ERROR || l > len)
      return total_sent;
    total_sent += l;
    buf += l;
    len -= l;
  }
  return total_sent + l;
}

int socket_data_waiting (SOCKET sock, int timeoutseconds)
{
  if (sock == 0)
    return 0;
  //make a set with only this socket
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(sock, &rfds);
  //make a timeval with the supplied timeout
  struct timeval tv;
  tv.tv_sec = timeoutseconds;
  tv.tv_usec = 0;
  //check the socket
  return (select(1, &rfds, NULL, NULL, &tv) > 0);
}

char* socket_receive_stmp (SOCKET sock)
{
  char* buf = NULL;
  do {
    free(buf);
    int size = READ_BUFFER_CHUNK_SIZE;
    buf = (char*)malloc(size);
    char* p = buf;
    while (recv(sock, p, 1, 0) == 1 && *p != '\r' && *p != '\n') {
      p++;
      if (p - buf >= size) {
        int len = p - buf;
        char* newbuf = (char*)malloc(size + READ_BUFFER_CHUNK_SIZE);
        memcpy(newbuf, buf, len);
        free(buf);
        buf = newbuf;
        p = buf + len;
        size += READ_BUFFER_CHUNK_SIZE;
      }
    }
    while (socket_data_waiting(sock, 0) && recv(sock, p, 1, 0) == 1 && (*p == '\r' || *p == '\n'))
      ;
    *p = 0;
  } while (!isdigit(buf[0]) || !isdigit(buf[1]) || !isdigit(buf[2]) || buf[3] != ' ');
  return buf;
}

int socket_get_smtp_code (SOCKET sock, char** errmsg)
{
  char* buf = socket_receive_stmp(sock);
  if (buf[3] != ' ')
    return 0;
  //get code
  buf[3] = 0;
  int code = atoi(buf);
  //get error message (if needed)
  if (errmsg && code >= 400)
    *errmsg = strdup(buf + 4);
  //clean up and return
  free(buf);
  return code;
}
