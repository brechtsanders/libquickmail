#ifndef __INCLUDED_SMTPSOCKET_H
#define __INCLUDED_SMTPSOCKET_H

#ifdef __WIN32__
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

#define READ_BUFFER_CHUNK_SIZE 128
#define WRITE_BUFFER_CHUNK_SIZE 128

#ifdef __cplusplus
extern "C" {
#endif

//!connect network socket
/*!
  \param  smtpserver      hostname or IP address of server
  \param  smtpport        TCP port to connect to
  \param  errmsg          optional pointer to where error message will be stored (must not be freed by caller)
  \return open network socket or INVALID_SOCKET on error
*/
SOCKET socket_open (const char* smtpserver, unsigned int smtpport, char** errmsg);

//!disconnect network socket
/*!
  \param  sock        open network socket
*/
void socket_close (SOCKET sock);

//!send data to a network socket
/*!
  \param  sock        open network socket
  \param  buf         buffer containing data
  \param  len         size of buffer in bytes
  \return number of bytes sent
*/
int socket_send (SOCKET sock, const char* buf, int len);

//!check if data is waiting to be read from network socket
/*!
  \param  sock            open network socket
  \param  timeoutseconds  number of seconds to wait (0 to return immediately)
  \return nonzero if data is waiting
*/
int socket_data_waiting (SOCKET sock, int timeoutseconds);

//!read SMTP response from network socket
/*!
  \param  sock            open network socket
  \return null-terminated string containing received data (must be freed by caller), or NULL
*/
char* socket_receive_stmp (SOCKET sock);

//!read SMTP response from network socket
/*!
  \param  sock            open network socket
  \param  errmsg          optional pointer to where error message will be stored (must be freed by caller)
  \return SMTP status code
*/
int socket_get_smtp_code (SOCKET sock, char** errmsg);

#ifdef __cplusplus
}
#endif

#endif //__INCLUDED_SMTPSOCKET_H
