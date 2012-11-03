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

//!get version quickmail library
/*!
  \return library version
*/
DLL_EXPORT_LIBQUICKMAIL const char* quickmail_get_version ();

//!initialize quickmail library
/*!
  \return zero on success
*/
DLL_EXPORT_LIBQUICKMAIL int quickmail_initialize ();

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

//!add an e-mail header to a quickmail object
/*!
  \param  mailobj     quickmail object
  \param  headerline  header line to add
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_header (quickmail mailobj, const char* headerline);

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

//!add an attachment from memory to a quickmail object
/*!
  \param  mailobj     quickmail object
  \param  filename    name of file to attach (must not include full path)
  \param  data        data content
  \param  datalen     size of data in bytes
  \param  mustfree    non-zero if data must be freed by quickmail_destroy
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_attachment_memory (quickmail mailobj, const char* filename, char* data, size_t datalen, int mustfree);

//!type of pointer to function for opening attachment data
/*!
  \param  filedata    custom data as passed to quickmail_add_attachment_custom
  \return data structure to be used in calls to quickmail_attachment_read_fn and quickmail_attachment_close_fn functions
  \sa     quickmail_add_attachment_custom()
*/
typedef void* (*quickmail_attachment_open_fn)(void* filedata);

//!type of pointer to function for reading attachment data
/*!
  \param  handle      data structure obtained via the corresponding quickmail_attachment_open_fn function
  \param  buf         buffer for receiving data
  \param  len         size in bytes of buffer for receiving data
  \return number of bytes read (zero on end of file)
  \sa     quickmail_add_attachment_custom()
*/
typedef size_t (*quickmail_attachment_read_fn)(void* handle, void* buf, size_t len);

//!type of pointer to function for closing attachment data
/*!
  \param  handle      data structure obtained via the corresponding quickmail_attachment_open_fn function
  \sa     quickmail_add_attachment_custom()
*/
typedef void (*quickmail_attachment_close_fn)(void* handle);

//!type of pointer to function for cleaning up custom data in quickmail_destroy
/*!
  \param  filedata    custom data as passed to quickmail_add_attachment_custom
  \sa     quickmail_add_attachment_custom()
*/
typedef void (*quickmail_attachment_free_filedata_fn)(void* filedata);

//!add an attachment with custom read functions to a quickmail object
/*!
  \param  mailobj                        quickmail object
  \param  filename                       name of file to attach (must not include full path)
  \param  data                           custom data passed to attachment_data_open and attachment_data_filedata_free functions
  \param  attachment_data_open           function for opening attachment data
  \param  attachment_data_read           function for reading attachment data
  \param  attachment_data_close          function for closing attachment data (optional, free() will be used if NULL)
  \param  attachment_data_filedata_free  function for cleaning up custom data in quickmail_destroy (optional, free() will be used if NULL)
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_attachment_custom (quickmail mailobj, const char* filename, char* data, quickmail_attachment_open_fn attachment_data_open, quickmail_attachment_read_fn attachment_data_read, quickmail_attachment_close_fn attachment_data_close, quickmail_attachment_free_filedata_fn attachment_data_filedata_free);

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
  \param  nmemb       number of records to copy to \p buffer
  \param  mailobj     quickmail object
  \return number of bytes copied to \p buffer or 0 if at end
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
