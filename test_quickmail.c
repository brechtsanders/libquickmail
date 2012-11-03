#include "quickmail.h"
#include <stdio.h>

#define FROM        "my@domain.com"
#define TO          "someuser@domain.com"
//#define CC          "otheruser@domain.com"
//#define BCC         "otheruser@domain.com"
#define SMTPSERVER  "smtp.mydomain.com"
#define SMTPPORT    25
#define SMTPUSER    NULL
#define SMTPPASS    NULL

int main ()
{
  printf("libquickmail %s\n", quickmail_get_version());
  quickmail_initialize();
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
  quickmail_add_header(mailobj, "Importance: Low");
  quickmail_add_header(mailobj, "X-Priority: 5");
  quickmail_add_header(mailobj, "X-MSMail-Priority:: Low");
  quickmail_set_body(mailobj, "This is a test e-mail.\nThis mail was sent using libquickmail.");
  quickmail_add_attachment_file(mailobj, "test_quickmail.c");
  quickmail_add_attachment_file(mailobj, "test_quickmail.cbp");
  quickmail_add_attachment_memory(mailobj, "test.txt", "Test\n123", 8, 0);
/**/
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
