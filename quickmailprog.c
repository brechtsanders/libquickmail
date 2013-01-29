#include "quickmail.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void show_help()
{
  printf(
    "Usage:  quickmail -h server [-p port] -f email [-t email] [-c email] [-b email] [-s subject] [-d body] [-a file] [-v]\n" \
    "Parameters:\n" \
    "  -h server      \thostname or IP address of SMTP server\n" \
    "  -p port        \tTCP port to use for SMTP connection (default is 25)\n" \
    "  -f email       \tFrom e-mail address\n" \
    "  -t email       \tTo e-mail address (multiple -t can be specified)\n" \
    "  -c email       \tCc e-mail address (multiple -c can be specified)\n" \
    "  -b email       \tBcc e-mail address (multiple -b can be specified)\n" \
    "  -s subject     \tSubject\n" \
    "  -m mimetype    \tMIME used for the body (must be specified before -d)\n" \
    //"  -d body        \tBody, if not specified will be read from standard input\n" \
    "  -d body        \tBody\n" \
    "  -a file        \tfile to attach (multiple -a can be specified)\n" \
    "  -v             \tverbose mode\n" \
    "  -?             \tshow help\n" \
    "\n"
  );
}

size_t email_info_attachment_read_stdin (void* handle, void* buf, size_t len)
{
  return fread(buf, 1, len, stdin);
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
  const char* mime_type = NULL;
  char* body = NULL;
  int smtp_port = 25;
  {
    int i = 0;
    char* param;
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
          case 'm' :
            if (argv[i][2])
              param = argv[i] + 2;
            else if (i + 1 < argc && argv[i + 1])
              param = argv[++i];
            if (!param)
              paramerror++;
            else
              mime_type = param;
            break;
          case 'd' :
            if (argv[i][2])
              param = argv[i] + 2;
            else if (i + 1 < argc && argv[i + 1])
              param = argv[++i];
            if (!param)
              paramerror++;
            else if (strcmp(param, "-") != 0)
              body = param;
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
/*/
  //read body from standard input if not given
  if (body) {
    quickmail_add_body_memory(mailobj, mime_type, body, strlen(body), 0);
  } else {
    printf("No Body\n");/////
    quickmail_add_body_custom (mailobj, mime_type, NULL, NULL, email_info_attachment_read_stdin, NULL, NULL);
  }
  mime_type = NULL;
/**/
  //send e-mail
  int status = 0;
  const char* errmsg;
  if ((errmsg = quickmail_send(mailobj, smtp_server, smtp_port, NULL, NULL)) != NULL) {
    status = 1;
    fprintf(stderr, "Error sending e-mail: %s\n", errmsg);
  }
  //clean up
  quickmail_destroy(mailobj);
  return status;
}
