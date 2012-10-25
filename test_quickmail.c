#include "quickmail.h"
#include <stdio.h>

#define FROM        "me@domain.com"
#define TO          "user@domain.com"
//#define CC          "otheruser@domain.com"
//#define BCC         "otheruser@domain.com"
#define SMTPSERVER  "mail.server.com"
#define SMTPPORT    25
#define SMTPUSER    NULL
#define SMTPPASS    NULL

#if defined(NOCURL) && defined(__WIN32__)
#include <winsock2.h>
#endif

int main ()
{
#if defined(NOCURL) && defined(__WIN32__)
  static WSADATA wsaData;
  int wsaerr = WSAStartup(MAKEWORD(1, 0), &wsaData);
  if (wsaerr)
    exit(1);
  atexit((void(*)())WSACleanup);
#endif
  quickmail mailobj = quickmail_create(FROM, "libquickmail test e-mail");
#ifdef TO
  quickmail_add_to(mailobj, TO);
#endif
#ifdef CC
  quickmail_add_cc(mailobj, CC);
#endif
#ifdef BCC
  quickmail_add_bcc(mailobj, BCC);
#endif
  quickmail_set_body(mailobj, "This is a test e-mail.\nThis mail was sent using libquickmail.");
  quickmail_add_attachment_file(mailobj, "test_quickmail.c");
  quickmail_add_attachment_file(mailobj, "test_quickmail.cbp");
/*/
  quickmail_fsave(mailobj, stdout);
  quickmail_destroy(mailobj);
  return 0;
/**/

  const char* errmsg;
  quickmail_set_debug_log(mailobj, stderr);
  if ((errmsg = quickmail_send(mailobj, SMTPSERVER, SMTPPORT, SMTPUSER, SMTPPASS)) != NULL)
    fprintf(stderr, "Error sending e-mail: %s\n", errmsg);
  quickmail_destroy(mailobj);
  return 0;
}
