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

typedef struct email_info_struct* quickmail;

DLL_EXPORT_LIBQUICKMAIL quickmail quickmail_create (const char* from, const char* subject);
DLL_EXPORT_LIBQUICKMAIL void quickmail_destroy (quickmail mailobj);
DLL_EXPORT_LIBQUICKMAIL void quickmail_set_from (quickmail mailobj, const char* from);
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_to (quickmail mailobj, const char* email);
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_cc (quickmail mailobj, const char* email);
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_bcc (quickmail mailobj, const char* email);
DLL_EXPORT_LIBQUICKMAIL void quickmail_set_subject (quickmail mailobj, const char* subject);
DLL_EXPORT_LIBQUICKMAIL void quickmail_set_body (quickmail mailobj, const char* body);
DLL_EXPORT_LIBQUICKMAIL void quickmail_add_attachment_file (quickmail mailobj, const char* path);
DLL_EXPORT_LIBQUICKMAIL void quickmail_fsave (quickmail mailobj, FILE* filehandle);
DLL_EXPORT_LIBQUICKMAIL size_t quickmail_get_data (void* buffer, size_t size, size_t nmemb, void* mailobj);
DLL_EXPORT_LIBQUICKMAIL const char* quickmail_send (quickmail mailobj, const char* smtpserver, unsigned int smtpport);

#ifdef __cplusplus
}
#endif

#endif //__INCLUDED_QUICKMAIL_H
