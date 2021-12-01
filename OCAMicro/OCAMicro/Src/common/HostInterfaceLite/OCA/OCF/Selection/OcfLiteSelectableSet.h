/*
 *  By downloading or using this file, the user agrees to be bound by the terms of the license 
 *  agreement located in the LICENSE file in the root of this project
 *  as an original contracting party.
 *
 *  Description         : A set of selectable objects.
 *
 */

#ifndef OCFLITESELECTABLESET_H
#define OCFLITESELECTABLESET_H

// ---- Include system wide include files ----
#include <PlatformDataTypes.h>

// ---- Include local include files ----

// ---- Referenced classes and types ----

// ---- Helper types and constants ----
// ---- Class Definition ----

#ifdef _WIN32
#include <WinSock2.h>
#define OcfLiteSelectableSet fd_set
#elif __APPLE__
#include <sys/select.h>
#define OcfLiteSelectableSet fd_set
#else
// Forwarding to LwIP select, copy in the definition since including the header files seems to get lots of compiler errors
/* FD_SET used for lwip_select */
#ifndef FD_SET
  #undef  FD_SETSIZE
  /* Make FD_SETSIZE match NUM_SOCKETS in socket.c */
  #define FD_SETSIZE    10
  #define FD_SET(n, p)  ((p)->fd_bits[(n)/8] |=  (1 << ((n) & 7)))
  #define FD_CLR(n, p)  ((p)->fd_bits[(n)/8] &= ~(1 << ((n) & 7)))
  #define FD_ISSET(n,p) ((p)->fd_bits[(n)/8] &   (1 << ((n) & 7)))
  #define FD_ZERO(p)    memset((void*)(p),0,sizeof(*(p)))

  typedef struct fd_set {
          unsigned char fd_bits [(FD_SETSIZE+7)/8];
        } fd_set;

#endif /* FD_SET */

// ---- Helper functions ----
#define OcfLiteSelectableSet fd_set
#endif


#endif
