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



//!type of pointer to function for opening attachment data
/*!
  \param  filedata    custom data as passed to quickmail_add_body_custom/quickmail_add_attachment_custom
  \return data structure to be used in calls to quickmail_attachment_read_fn and quickmail_attachment_close_fn functions
  \sa     quickmail_add_body_custom()
  \sa     quickmail_add_attachment_custom()
*/
typedef void* (*quickmail_attachment_open_fn)(void* filedata);

//!type of pointer to function for reading attachment data
/*!
  \param  handle      data structure obtained via the corresponding quickmail_attachment_open_fn function
  \param  buf         buffer for receiving data
  \param  len         size in bytes of buffer for receiving data
  \return number of bytes read (zero on end of file)
  \sa     quickmail_add_body_custom()
  \sa     quickmail_add_attachment_custom()
*/
typedef size_t (*quickmail_attachment_read_fn)(void* handle, void* buf, size_t len);

//!type of pointer to function for closing attachment data
/*!
  \param  handle      data structure obtained via the corresponding quickmail_attachment_open_fn function
  \sa     quickmail_add_body_custom()
  \sa     quickmail_add_attachment_custom()
*/
typedef void (*quickmail_attachment_close_fn)(void* handle);

//!type of pointer to function for cleaning up custom data in quickmail_destroy
/*!
  \param  filedata    custom data as passed to quickmail_add_body_custom/quickmail_add_attachment_custom
  \sa     quickmail_add_body_custom()
  \sa     quickmail_add_attachment_custom()
*/
typedef void (*quickmail_attachment_free_filedata_fn)(void* filedata);

//!type of pointer to function for cleaning up custom data in quickmail_destroy
/*!
  \param  mailobj                        quickmail object
  \param  filename                       mime type for body or name of file for attachment
  \param  attachment_data_open           function for opening attachment data
  \param  attachment_data_read           function for reading attachment data
  \param  attachment_data_close          function for closing attachment data (optional, free() will be used if NULL)
  \param  callbackdata                   custom data passed to quickmail_list_attachments
  \sa     quickmail_list_bodies()
  \sa     quickmail_list_attachments()
*/
typedef void (*quickmail_list_attachment_callback_fn)(quickmail mailobj, const char* filename, quickmail_attachment_open_fn email_info_attachment_open, quickmail_attachment_read_fn email_info_attachment_read, quickmail_attachment_close_fn email_info_attachment_close, void* callbackdata);



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

//!get the sender (from) e-mail address of a quickmail object
/*!
  \param  mailobj     quickmail object
  \return sender e-mail address
*/
DLL_EXPORT_LIBQUICKMAIL const char* quickmail_get_from (quickmail mailobj);

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

//!set the subject of a quickmail object
/*!
  \param  mailobj     quickmail object
  \return e-mail subject
*/
DLL_EXPORT_LIBQUICKMAIL const char* quickmail_get_subject (quickmail mailobj);

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

//!set the body of a quickmail object
//!any existing bodies will be removed and a single plain text body will be added
/*!
  \param  mailobj     quickmail object
  \return e-mail body (caller must free result)
*/
DLL_EXPORT_LIBQUICKMAIL char* quickmail_get_body (quickmail mailobj);

//!add a body file to a quickmail object (deprecated)
/*!
  \param  mailobj     quickmail object
  \param  mimetype    mime type (text/plain will be used if set to NULL)
  \param  path        path of file with body data
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_body_file (quickmail mailobj, const char* mimetype, const char* path);

//!add a body from memory to a quickmail object
/*!
  \param  mailobj     quickmail object
  \param  mimetype    mime type (text/plain will be used if set to NULL)
  \param  data        body content
  \param  datalen     size of data in bytes
  \param  mustfree    non-zero if data must be freed by quickmail_destroy
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_body_memory (quickmail mailobj, const char* mimetype, char* data, size_t datalen, int mustfree);

//!add a body with custom read functions to a quickmail object
/*!
  \param  mailobj                        quickmail object
  \param  mimetype                       mime type (text/plain will be used if set to NULL)
  \param  data                           custom data passed to attachment_data_open and attachment_data_filedata_free functions
  \param  attachment_data_open           function for opening attachment data
  \param  attachment_data_read           function for reading attachment data
  \param  attachment_data_close          function for closing attachment data (optional, free() will be used if NULL)
  \param  attachment_data_filedata_free  function for cleaning up custom data in quickmail_destroy (optional, free() will be used if NULL)
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_body_custom (quickmail mailobj, const char* mimetype, char* data, quickmail_attachment_open_fn attachment_data_open, quickmail_attachment_read_fn attachment_data_read, quickmail_attachment_close_fn attachment_data_close, quickmail_attachment_free_filedata_fn attachment_data_filedata_free);

//!remove body from quickmail object
/*!
  \param  mailobj     quickmail object
  \param  mimetype    mime type (text/plain will be used if set to NULL)
  \return zero on success
*/
DLL_EXPORT_LIBQUICKMAIL int quickmail_remove_body (quickmail mailobj, const char* mimetype);

//!list bodies by calling a callback function for each body
/*!
  \param  mailobj                        quickmail object
  \param  callback                       function to call for each attachment
  \param  callbackdata                   custom data to pass to the callback function
  \sa     quickmail_list_attachment_callback_fn
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_list_bodies (quickmail mailobj, quickmail_list_attachment_callback_fn callback, void* callbackdata);

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

//!remove attachment from quickmail object
/*!
  \param  mailobj     quickmail object
  \param  filename    name of file to attach (must not include full path)
  \return zero on success
*/
DLL_EXPORT_LIBQUICKMAIL int quickmail_remove_attachment (quickmail mailobj, const char* filename);

//!list attachments by calling a callback function for each attachment
/*!
  \param  mailobj                        quickmail object
  \param  callback                       function to call for each attachment
  \param  callbackdata                   custom data to pass to the callback function
  \sa     quickmail_list_attachment_callback_fn
*/
DLL_EXPORT_LIBQUICKMAIL void quickmail_list_attachments (quickmail mailobj, quickmail_list_attachment_callback_fn callback, void* callbackdata);

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
