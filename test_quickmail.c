#include "quickmail.h"
#include <stdio.h>

#define FROM    "brecht.sanders@alpro.com"
#define TO      "brecht.sanders@alpro.com"
//#define TO      "brecht@edustria.be"
//#define CC      "brecht.sanders@alpro.com"


int main ()
{
  quickmail mailobj = quickmail_create(FROM, "SMTP TLS example message");
  quickmail_add_to(mailobj, TO);
#ifdef CC
  quickmail_add_cc(mailobj, CC);
#endif
  quickmail_set_body(mailobj, "My test body.\n(Line 2)");
  quickmail_add_attachment_file(mailobj, "test_quickmail.c");
  quickmail_add_attachment_file(mailobj, "test_quickmail.cbp");
  //quickmail_add_attachment_file(mailobj, "D:\\Download\\npp.6.1.3.bin.zip");
/**/
  quickmail_fsave(mailobj, stdout);
  quickmail_fsave(mailobj, stdout);
  //quickmail_destroy(mailobj);
  return 0;
/**/

  quickmail_send(mailobj, "10.32.1.47", 25);
  quickmail_destroy(mailobj);
  return 0;
}
