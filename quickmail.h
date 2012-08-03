#ifndef __INCLUDED_QUICKMAIL_H
#define __INCLUDED_QUICKMAIL_H

#include <stdio.h>

#ifdef __WIN32__
#if defined(BUILD_QUICKMAIL_DLL)
#define DLL_EXPORT_LIBQUICKMAIL __declspec(dllexport)
#elif !defined(STATIC) && !defined(BUILD_QUICKMAIL_STATIC)
#define DLL_EXPORT_LIBQUICKMAIL __declspec(dllimport)
#else
#define DLL_EXPORT_LIBQUICKMAIL
#endif
#else
#define DLL_EXPORT_LIBQUICKMAIL
#endif

#ifdef __cplusplus
extern "C" {
#endif

//!quickmail object type
typedef struct email_info_struct* quickmail;

//!get version of this library
/*!
  \return library version
*/
DLL_EXPORT_LIBQUICKMAIL const char* quickmail_get_version ();

//!create a new quickmail object
/*!
  \param  from        sender e-mail address
  \param  subject     e-mail subject
  \return quickmail object
*/
DLL_EXPORT_LIBQUICKMAIL quickmail quickmail_create (const char* from, const char* subject);

//!clean up a quickmail object
/*!
  \param  mailobj     quickmail object
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_destroy (quickmail mailobj);

//!set the sender (from) e-mail address of a quickmail object
/*!
  \param  mailobj     quickmail object
  \param  from        sender e-mail address
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_set_from (quickmail mailobj, const char* from);

//!add a recipient (to) e-mail address to a quickmail object
/*!
  \param  mailobj     quickmail object
  \param  e-mail      recipient e-mail address
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_to (quickmail mailobj, const char* email);

//!add a carbon copy recipient (cc) e-mail address to a quickmail object
/*!
  \param  mailobj     quickmail object
  \param  e-mail      recipient e-mail address
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_cc (quickmail mailobj, const char* email);

//!add a blind carbon copy recipient (bcc) e-mail address to a quickmail object
/*!
  \param  mailobj     quickmail object
  \param  e-mail      recipient e-mail address
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_bcc (quickmail mailobj, const char* email);

//!set the subject of a quickmail object
/*!
  \param  mailobj     quickmail object
  \param  subject     e-mail subject
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_set_subject (quickmail mailobj, const char* subject);

//!set the body of a quickmail object
/*!
  \param  mailobj     quickmail object
  \param  body        e-mail body
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_set_body (quickmail mailobj, const char* body);

//!add a file attachment to a quickmail object
/*!
  \param  mailobj     quickmail object
  \param  path        path of file to attach
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_attachment_file (quickmail mailobj, const char* path);

//!set the debug logging destination of a quickmail object
/*!
  \param  mailobj     quickmail object
  \param  filehandle  file handle of logging destination (or NULL for no logging)
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_set_debug_log (quickmail mailobj, FILE* filehandle);

//!save the generated e-mail to a file
/*!
  \param  mailobj     quickmail object
  \param  filehandle  file handle to write the e-mail contents to
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_fsave (quickmail mailobj, FILE* filehandle);

//!read data the next data from the e-mail contents (can be used as CURLOPT_READFUNCTION callback function)
/*!
  \param  buffer      buffer to copy data to (bust be able to hold size * nmemb bytes)
  \param  size        record size
  \param  nmemb       number of records to copy to buffer
  \param  mailobj     quickmail object
  \return number of bytes copied to buffer or 0 if at end
*/
DLL_EXPORT_LIBQUICKMAIL size_t quickmail_get_data (void* buffer, size_t size, size_t nmemb, void* mailobj);

//!send the e-mail via SMTP
/*!
  \param  mailobj     quickmail object
  \param  smtpserver  IP address or hostname of SMTP server
  \param  smtpport    SMTP port number (normally this is 25)
  \param  username    username to use for authentication (or NULL if not needed)
  \param  password    password to use for authentication (or NULL if not needed)
  \return NULL on success or error message on error
*/
DLL_EXPORT_LIBQUICKMAIL const char* quickmail_send (quickmail mailobj, const char* smtpserver, unsigned int smtpport, const char* username, const char* password);

#ifdef __cplusplus
}
#endif

#endif //__INCLUDED_QUICKMAIL_H
