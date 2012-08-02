#if defined(__WIN32__) && defined(DLL_EXPORT)
#define BUILD_QUICKMAIL_DLL
#endif
#include "quickmail.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>

#define LIBQUICKMAIL_VERSION_MAJOR 0
#define LIBQUICKMAIL_VERSION_MINOR 1
#define LIBQUICKMAIL_VERSION_MICRO 0

#define VERSION_STRINGIZE_(major, minor, micro) #major"."#minor"."#micro
#define VERSION_STRINGIZE(major, minor, micro) VERSION_STRINGIZE_(major, minor, micro)

#define LIBQUICKMAIL_VERSION VERSION_STRINGIZE(LIBQUICKMAIL_VERSION_MAJOR,LIBQUICKMAIL_VERSION_MINOR,LIBQUICKMAIL_VERSION_MICRO)

#define NEWLINE "\r\n"
#define NEWLINELENGTH 2

#define MIME_LINE_WIDTH 72

//defines used for indicating the position status
#define MAILPART_INITIALIZE 0
#define MAILPART_HEADER     1
#define MAILPART_BODY       2
#define MAILPART_END        3
#define MAILPART_DONE       4

struct email_info_struct {
  int current;  //must be zet to 0
  time_t timestamp;
  char* from;
  struct email_info_string_list_struct* to;
  struct email_info_string_list_struct* cc;
  struct email_info_string_list_struct* bcc;
  char* subject;
  char* body;
  struct email_info_string_list_struct* attachmentlist;
  char* buf;
  int buflen;
  char* mime_boundary;
  struct email_info_string_list_struct* current_attachment;
  FILE* attachment_handle;
  FILE* debuglog;
  char dtable[64];
};

////////////////////////////////////////////////////////////////////////

struct email_info_string_list_struct {
  char* data;
  struct email_info_string_list_struct* next;
};

void email_info_string_list_add (struct email_info_string_list_struct** list, const char* data)
{
  struct email_info_string_list_struct** p = list;
  while (*p)
    p = &(*p)->next;
  *p = (struct email_info_string_list_struct*)malloc(sizeof(struct email_info_string_list_struct));
  (*p)->data = strdup(data);
  (*p)->next = NULL;
}

void email_info_string_list_free (struct email_info_string_list_struct** list)
{
  struct email_info_string_list_struct* p = *list;
  struct email_info_string_list_struct* current;
  while (p) {
    current = p;
    p = current->next;
    free(current->data);
    free(current);
  }
  *list = NULL;
}

////////////////////////////////////////////////////////////////////////

char* randomize_zeros (char* data)
{
  //replace all 0s with random digits
  char* p = data;
  while (*p) {
    if (*p == '0')
      *p = '0' + rand() % 10;
    p++;
  }
  return data;
}

char* str_append (char** data, const char* newdata)
{
  //append a string to the end of an existing string
  int len = (*data ? strlen(*data) : 0);
  *data = (char*)realloc(*data, len + strlen(newdata) + 1);
  strcpy(*data + len, newdata);
  return *data;
}

////////////////////////////////////////////////////////////////////////

DLL_EXPORT_LIBQUICKMAIL quickmail quickmail_create (const char* from, const char* subject)
{
  int i;
  struct email_info_struct* mailobj = (struct email_info_struct*)malloc(sizeof(struct email_info_struct));
  mailobj->current = 0;
  mailobj->timestamp = time(NULL);
  mailobj->from = (from ? strdup(from) : NULL);
  mailobj->to = NULL;
  mailobj->cc = NULL;
  mailobj->bcc = NULL;
  mailobj->subject = (subject ? strdup(subject) : NULL);
  mailobj->body = NULL;
  mailobj->attachmentlist = NULL;
  mailobj->buf = NULL;
  mailobj->buflen = 0;
  mailobj->mime_boundary = NULL;
  mailobj->current_attachment = NULL;
  mailobj->attachment_handle = NULL;
  mailobj->debuglog = NULL;
  for (i = 0; i < 26; i++) {
    mailobj->dtable[i] = (char)('A' + i);
    mailobj->dtable[26 + i] = (char)('a' + i);
  }
  for (i = 0; i < 10; i++) {
    mailobj->dtable[52 + i] = (char)('0' + i);
  }
  mailobj->dtable[62] = '+';
  mailobj->dtable[63] = '/';
  srand(time(NULL));
  return mailobj;
}

DLL_EXPORT_LIBQUICKMAIL void quickmail_destroy (quickmail mailobj)
{
  free(mailobj->from);
  email_info_string_list_free(&mailobj->to);
  email_info_string_list_free(&mailobj->cc);
  email_info_string_list_free(&mailobj->bcc);
  free(mailobj->subject);
  free(mailobj->body);
  email_info_string_list_free(&mailobj->attachmentlist);
  free(mailobj->buf);
  free(mailobj->mime_boundary);
  if (mailobj->attachment_handle)
    fclose(mailobj->attachment_handle);
  free(mailobj);
}

DLL_EXPORT_LIBQUICKMAIL void quickmail_set_from (quickmail mailobj, const char* from)
{
  free(mailobj->from);
  mailobj->from = strdup(from);
}

DLL_EXPORT_LIBQUICKMAIL void quickmail_add_to (quickmail mailobj, const char* email)
{
  email_info_string_list_add(&mailobj->to, (email ? email : NULL));
}

DLL_EXPORT_LIBQUICKMAIL void quickmail_add_cc (quickmail mailobj, const char* email)
{
  email_info_string_list_add(&mailobj->cc, (email ? email : NULL));
}

DLL_EXPORT_LIBQUICKMAIL void quickmail_add_bcc (quickmail mailobj, const char* email)
{
  email_info_string_list_add(&mailobj->bcc, (email ? email : NULL));
}

DLL_EXPORT_LIBQUICKMAIL void quickmail_set_subject (quickmail mailobj, const char* subject)
{
  free(mailobj->subject);
  mailobj->from = (subject ? strdup(subject) : NULL);
}

DLL_EXPORT_LIBQUICKMAIL void quickmail_set_body (quickmail mailobj, const char* body)
{
  free(mailobj->body);
  mailobj->body = (body ? strdup(body) : NULL);
}

DLL_EXPORT_LIBQUICKMAIL void quickmail_add_attachment_file (quickmail mailobj, const char* path)
{
  email_info_string_list_add(&mailobj->attachmentlist, (path ? strdup(path) : NULL));
}

DLL_EXPORT_LIBQUICKMAIL void quickmail_set_debug_log (quickmail mailobj, FILE* filehandle)
{
  mailobj->debuglog = filehandle;
}

DLL_EXPORT_LIBQUICKMAIL void quickmail_fsave (quickmail mailobj, FILE* filehandle)
{
  int i;
  size_t n;
  char buf[80];
  while ((n = quickmail_get_data(buf, sizeof(buf), 1, mailobj)) > 0) {
    for (i = 0; i < n; i++)
      printf("%c", buf[i]);
  }
}

DLL_EXPORT_LIBQUICKMAIL size_t quickmail_get_data (void* ptr, size_t size, size_t nmemb, void* userp)
{
  struct email_info_struct* mailobj = (struct email_info_struct*)userp;

  //abort if no data is requested
  if (size * nmemb == 0)
    return 0;

  //initialize on first run
  if (mailobj->current == MAILPART_INITIALIZE) {
    free(mailobj->buf);
    mailobj->buf = NULL;
    mailobj->buflen = 0;
    free(mailobj->mime_boundary);
    mailobj->mime_boundary = (mailobj->attachmentlist ? randomize_zeros(strdup("=SEPARATOR=_0000_0000_0000_0000_0000_0000_=")) : NULL);
    mailobj->current_attachment = mailobj->attachmentlist;
    if (mailobj->attachment_handle)
      fclose(mailobj->attachment_handle);
    mailobj->attachment_handle = NULL;
    mailobj->current++;
  }

  //process current part of mail if no partial data is pending
  while (mailobj->buflen == 0) {
    if (mailobj->buflen == 0 && mailobj->current == MAILPART_HEADER) {
      //generate header part
      struct email_info_string_list_struct* listentry;
      char** p = &mailobj->buf;
      //mailobj->buf = NULL;
      str_append(p, "User-Agent: libquickmail v" LIBQUICKMAIL_VERSION);
      if (mailobj->timestamp != 0) {
        char timestamptext[26];
        if (strftime(timestamptext, sizeof(timestamptext), "%a, %d %b %Y %H:%M:%S", gmtime(&mailobj->timestamp))) {
          str_append(p, NEWLINE "Date: ");
          str_append(p, timestamptext);
        }
      }
      if (mailobj->from && *mailobj->from) {
        str_append(p, NEWLINE "From: <");
        str_append(p, mailobj->from);
        str_append(p, ">");
      }
      listentry = mailobj->to;
      while (listentry) {
        if (listentry->data && *listentry->data) {
          str_append(p, NEWLINE "To: <");
          str_append(p, listentry->data);
          str_append(p, ">");
        }
        listentry = listentry->next;
      }
      listentry = mailobj->cc;
      while (listentry) {
        if (listentry->data && *listentry->data) {
          str_append(p, NEWLINE "Cc: <");
          str_append(p, listentry->data);
          str_append(p, ">");
        }
        listentry = listentry->next;
      }
      if (mailobj->subject) {
        str_append(p, NEWLINE "Subject: ");
        str_append(p, mailobj->subject);
      }
      if (mailobj->attachmentlist) {
        str_append(p, NEWLINE "MIME-Version: 1.0" NEWLINE "Content-Type: multipart/mixed; boundary=\"");
        str_append(p, mailobj->mime_boundary);
        str_append(p, NEWLINE NEWLINE "--");
        str_append(p, mailobj->mime_boundary);
        str_append(p, NEWLINE "Content-Type: text/plain");
      }

      str_append(p, NEWLINE NEWLINE);
      mailobj->buflen = strlen(mailobj->buf);
      mailobj->current++;
    }
    if (mailobj->buflen == 0 && mailobj->current == MAILPART_BODY) {
      if (mailobj->body) {
        //generate body part
        mailobj->buf = mailobj->body;
        mailobj->buflen = strlen(mailobj->buf);
        mailobj->body = NULL;
      } else if (mailobj->current_attachment) {
        if (mailobj->attachment_handle == NULL) {
          //open file to attach
          while (mailobj->current_attachment && mailobj->current_attachment->data && (mailobj->attachment_handle = fopen(mailobj->current_attachment->data, "rb")) == NULL) {
            mailobj->current_attachment = mailobj->current_attachment->next;
          }
          //generate attachment header
          if (mailobj->attachment_handle != NULL) {
            //determine base filename
            char* basename = mailobj->current_attachment->data + strlen(mailobj->current_attachment->data);
            while (basename != mailobj->current_attachment->data) {
              basename--;
              if (*basename == '/'
#ifdef _WIN32
                  || *basename == '\\' || *basename == ':'
#endif
              ) {
                basename++;
                break;
              }
            }
            //generate attachment header
            mailobj->buf = NULL;
            mailobj->buf = str_append(&mailobj->buf, NEWLINE "--");
            mailobj->buf = str_append(&mailobj->buf, mailobj->mime_boundary);
            mailobj->buf = str_append(&mailobj->buf, NEWLINE "Content-Type: application/octet-stream; Name=\"");
            mailobj->buf = str_append(&mailobj->buf, basename);
            mailobj->buf = str_append(&mailobj->buf, "\"" NEWLINE "Content-Description: The fixed length records" NEWLINE "Content-Transfer-Encoding: base64" NEWLINE NEWLINE);
            mailobj->buflen = strlen(mailobj->buf);
          }
        } else {
          //generate next line of attachment data
          int n;
          int mimelinepos = 0;
          unsigned char igroup[3] = {0, 0, 0};
          unsigned char ogroup[4];
          mailobj->buf = (char*)malloc(MIME_LINE_WIDTH + NEWLINELENGTH + 1);
          mailobj->buflen = 0;
          while (mimelinepos < MIME_LINE_WIDTH && (n = fread(igroup, 1, 3, mailobj->attachment_handle)) > 0) {
            //code data
            ogroup[0] = mailobj->dtable[igroup[0] >> 2];
            ogroup[1] = mailobj->dtable[((igroup[0] & 3) << 4) | (igroup[1] >> 4)];
            ogroup[2] = mailobj->dtable[((igroup[1] & 0xF) << 2) | (igroup[2] >> 6)];
            ogroup[3] = mailobj->dtable[igroup[2] & 0x3F];
            //padd with "=" characters if less than 3 characters were read
            if (n < 3) {
              ogroup[3] = '=';
              if (n < 2)
                ogroup[2] = '=';
            }
            memcpy(mailobj->buf + mimelinepos, ogroup, 4);
            mailobj->buflen += 4;
            mimelinepos += 4;
          }
          if (mimelinepos > 0) {
            memcpy(mailobj->buf + mimelinepos, NEWLINE, NEWLINELENGTH);
            mailobj->buflen += NEWLINELENGTH;
          }
          if (n <= 0)
          {
            //end of file
            fclose(mailobj->attachment_handle);
            mailobj->attachment_handle = NULL;
            mailobj->current_attachment = mailobj->current_attachment->next;
          }
        }
      } else {
        mailobj->current++;
      }
    }
    if (mailobj->buflen == 0 && mailobj->current == MAILPART_END) {
      mailobj->buf = NULL;
      if (mailobj->attachmentlist) {
        mailobj->buf = str_append(&mailobj->buf, NEWLINE "--");
        mailobj->buf = str_append(&mailobj->buf, mailobj->mime_boundary);
        mailobj->buf = str_append(&mailobj->buf, "--" NEWLINE);
      }
      //mailobj->buf = str_append(&mailobj->buf, NEWLINE "." NEWLINE);
      mailobj->buflen = strlen(mailobj->buf);
      mailobj->current++;
    }
    if (mailobj->buflen == 0 && mailobj->current == MAILPART_DONE) {
      break;
    }
  }

  //flush pending data if any
  if (mailobj->buflen > 0) {
    int len = (mailobj->buflen > size * nmemb ? size * nmemb : mailobj->buflen);
    memcpy(ptr, mailobj->buf, len);
    if (len < mailobj->buflen) {
      mailobj->buf = memmove(mailobj->buf, mailobj->buf + len, mailobj->buflen - len);
      mailobj->buflen -= len;
    } else {
      free(mailobj->buf);
      mailobj->buf = NULL;
      mailobj->buflen = 0;
    }
    return len;
  }

  //if (mailobj->current != MAILPART_DONE)
  //  ;//this should never be reached
  mailobj->current = 0;
  return 0;
}

DLL_EXPORT_LIBQUICKMAIL const char* quickmail_send (quickmail mailobj, const char* smtpserver, unsigned int smtpport, const char* username, const char* password)
{

  CURL *curl;
  CURLcode result = CURLE_FAILED_INIT;
  //curl_global_init(CURL_GLOBAL_ALL);
  if ((curl = curl_easy_init()) != NULL) {
    struct curl_slist *recipients = NULL;
    struct email_info_string_list_struct* listentry;
    //set destination URL
    size_t l = strlen(smtpserver) + 14;
    char* url = (char*)malloc(l);
    snprintf(url, l, "smtp://%s:%u", smtpserver, smtpport);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    free(url);
    //try Transport Layer Security (TLS), but continue anyway if it fails
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_TRY);
    //don't fail if the TLS/SSL a certificate could not be verified
    //alternative: add the issuer certificate (or the host certificate if
    //the certificate is self-signed) to the set of certificates that are
    //known to libcurl using CURLOPT_CAINFO and/or CURLOPT_CAPATH
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    //set authentication credentials if provided
    if (username && *username)
      curl_easy_setopt(curl, CURLOPT_USERNAME, username);
    if (password)
      curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
    //set from value for envelope reverse-path
    if (mailobj->from && *mailobj->from)
      curl_easy_setopt(curl, CURLOPT_MAIL_FROM, mailobj->from);
    //set recipients
    listentry = mailobj->to;
    while (listentry) {
      if (listentry->data && *listentry->data)
        recipients = curl_slist_append(recipients, listentry->data);
      listentry = listentry->next;
    }
    listentry = mailobj->cc;
    while (listentry) {
      if (listentry->data && *listentry->data)
        recipients = curl_slist_append(recipients, listentry->data);
      listentry = listentry->next;
    }
    listentry = mailobj->bcc;
    while (listentry) {
      if (listentry->data && *listentry->data)
        recipients = curl_slist_append(recipients, listentry->data);
      listentry = listentry->next;
    }
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    //set callback function for getting message body
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, quickmail_get_data);
    curl_easy_setopt(curl, CURLOPT_READDATA, mailobj);
    //enable debugging if requested
    if (mailobj->debuglog) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
      curl_easy_setopt(curl, CURLOPT_STDERR, mailobj->debuglog);
    }
    //send the message
    result = curl_easy_perform(curl);
    //free the list of recipients and clean up
    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
  }
  return (result == CURLE_OK ? NULL : curl_easy_strerror(result));
}