#include "quickmail.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void show_help()
{
  printf(
    "Usage:  quickmail -h server [-p port] -f email [-t email] [-c email] [-b email] [-s subject] [-d body] [-a file] [-v]\n" \
    "Parameters:\n" \
    "  -h server      \thostname or IP address of SMTP server\n" \
    "  -p port        \tTCP port to use for SMTP connection (default is 25)\n" \
    "  -f email       \tFrom e-mail address\n" \
    "  -t email       \tTo e-mail address (multiple can be specified)\n" \
    "  -c email       \tCc e-mail address (multiple can be specified)\n" \
    "  -b email       \tBcc e-mail address (multiple can be specified)\n" \
    "  -s subject     \tSubject\n" \
    "  -d body        \tBody" /*", if not specified will be read from standard input"*/ "\n" \
    "  -a file        \tfile to attach (multiple can be specified)\n" \
    "  -v             \tverbose mode\n" \
    "  -?             \tshow help\n" \
    "\n"
  );
}

int main (int argc, char *argv[])
{
  //show version
  printf("quickmail %s\n", quickmail_get_version());
  //initialize mail object
  quickmail_initialize();
  quickmail mailobj = quickmail_create(NULL, NULL);

  //process command line parameters
  const char* smtp_server = NULL;
  int smtp_port = 25;
  {
    int i = 0;
    const char* param;
    int paramerror = 0;
    while (!paramerror && ++i < argc) {
      if (!argv[i][0] || (argv[i][0] != '/' && argv[i][0] != '-')) {
        paramerror++;
        break;
      } else {
        param = NULL;
        switch (tolower(argv[i][1])) {
          case '?' :
            show_help();
            return 0;
          case 'h' :
            if (argv[i][2])
              param = argv[i] + 2;
            else if (i + 1 < argc && argv[i + 1])
              param = argv[++i];
            if (!param)
              paramerror++;
            else
              smtp_server = param;
            break;
          case 'p' :
            if (argv[i][2])
              param = argv[i] + 2;
            else if (i + 1 < argc && argv[i + 1])
              param = argv[++i];
            if (!param)
              paramerror++;
            else
              smtp_port = atoi(param);
            break;
          case 'f' :
            if (argv[i][2])
              param = argv[i] + 2;
            else if (i + 1 < argc && argv[i + 1])
              param = argv[++i];
            if (!param)
              paramerror++;
            else
              quickmail_set_from(mailobj, param);
            break;
          case 't' :
            if (argv[i][2])
              param = argv[i] + 2;
            else if (i + 1 < argc && argv[i + 1])
              param = argv[++i];
            if (!param)
              paramerror++;
            else
              quickmail_add_to(mailobj, param);
            break;
          case 'c' :
            if (argv[i][2])
              param = argv[i] + 2;
            else if (i + 1 < argc && argv[i + 1])
              param = argv[++i];
            if (!param)
              paramerror++;
            else
              quickmail_add_cc(mailobj, param);
            break;
          case 'b' :
            if (argv[i][2])
              param = argv[i] + 2;
            else if (i + 1 < argc && argv[i + 1])
              param = argv[++i];
            if (!param)
              paramerror++;
            else
              quickmail_add_bcc(mailobj, param);
            break;
          case 's' :
            if (argv[i][2])
              param = argv[i] + 2;
            else if (i + 1 < argc && argv[i + 1])
              param = argv[++i];
            if (!param)
              paramerror++;
            else
              quickmail_set_subject(mailobj, param);
            break;
          case 'd' :
            if (argv[i][2])
              param = argv[i] + 2;
            else if (i + 1 < argc && argv[i + 1])
              param = argv[++i];
            if (!param)
              paramerror++;
            else
              quickmail_set_body(mailobj, param);
            break;
          case 'a' :
            if (argv[i][2])
              param = argv[i] + 2;
            else if (i + 1 < argc && argv[i + 1])
              param = argv[++i];
            if (!param)
              paramerror++;
            else
              quickmail_add_attachment_file(mailobj, param);
            break;
          case 'v' :
            quickmail_set_debug_log(mailobj, stdout);
            break;
          default :
            paramerror++;
            break;
        }
      }
    }
    if (paramerror || !smtp_server || !quickmail_get_from(mailobj)) {
      fprintf(stderr, "Invalid command line parameters\n");
      show_help();
      return 1;
    }
  }
/*
  //read body from standard input if not given
  if (!quickmail_get_body(mailobj)) {
    FILE* f;
    char* body = NULL;
    if ((f = fopen(stdin, "r")) != NULL) {
      //TO DO: read file
      fclose(f);
      quickmail_set_body(mailobj, body);
    }
  }
*/
  //send e-mail
  const char* errmsg;
  if ((errmsg = quickmail_send(mailobj, smtp_server, smtp_port, NULL, NULL)) != NULL)
    fprintf(stderr, "Error sending e-mail: %s\n", errmsg);
  //clean up
  quickmail_destroy(mailobj);
  return 0;
}
