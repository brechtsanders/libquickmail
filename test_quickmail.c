#include "quickmail.h"
#include <stdio.h>

#define FROM    "brecht.sanders@alpro.com"
#define TO      "brecht.sanders@alpro.com"
//#define TO      "brecht@edustria.be"
//#define CC      "brecht.sanders@alpro.com"


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
  if ((errmsg = quickmail_send(mailobj, "10.32.1.47", 25, NULL, NULL)) != NULL)
    fprintf(stderr, "Error sending e-mail: %s\n", errmsg);
  quickmail_destroy(mailobj);
  return 0;
}
