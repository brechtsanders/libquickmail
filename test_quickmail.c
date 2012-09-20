#include "quickmail.h"
#include <stdio.h>

#define FROM        "user@domain.com"
#define TO          "user@domain.com"
//#define CC          "otheruser@domain.com"
#define SMTPSERVER  "smtp.domain.com"
#define SMTPPORT    25


int main ()
{
  quickmail mailobj = quickmail_create(FROM, "libquickmail test e-mail");
  quickmail_add_to(mailobj, TO);
#ifdef CC
  quickmail_add_cc(mailobj, CC);
#endif
  quickmail_set_body(mailobj, "This is a test e-mail.\nThis mail was sent using libquickmail.");
  quickmail_add_attachment_file(mailobj, "test_quickmail.c");
  quickmail_add_attachment_file(mailobj, "test_quickmail.cbp");
  //quickmail_add_attachment_file(mailobj, "D:\\Download\\npp.6.1.3.bin.zip");
/**/
  quickmail_fsave(mailobj, stdout);
  quickmail_destroy(mailobj);
  return 0;
/**/

  const char* errmsg;
  quickmail_set_debug_log(mailobj, stderr);
  if ((errmsg = quickmail_send(mailobj, SMTPSERVER, SMTPPORT, NULL, NULL)) != NULL)
    fprintf(stderr, "Error sending e-mail: %s\n", errmsg);
  quickmail_destroy(mailobj);
  return 0;
}
