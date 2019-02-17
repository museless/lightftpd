/*
 * =============================
 *
 *  Filename: ftpd.h
 *  Created:  2018-05-12 23:40:17
 *  Author:   Muse
 *
 * ============================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <fcntl.h>

#include <unistd.h>
#include <netdb.h>
#include <errno.h>


#define EPOLL_ARR_SIZE      1024

#define USER_INITIALIZE     0
#define USER_WELCOMED       1
#define USER_LOGIN          2
#define USER_PASSWORD       3

#define ANONYMOUS_USER      "anonymous"
#define ANONYMOUS_USER_LEN  sizeof(ANONYMOUS_USER)

#define ANONYMOUS_PWD       "anonymous@"
#define ANONYMOUS_PWD_LEN   sizeof(ANONYMOUS_PWD)

typedef struct sockaddr     saddr_t;
typedef struct sockaddr_in  sockaddr_t;
typedef struct epoll_event  epollev_t;

typedef struct ftpsock      ftps_t;
typedef struct ftpservsock  ftpservs_t;
typedef struct ftpusersock  ftpusers_t;


struct ftpsock {
    int32_t fd;
};


struct ftpservsock {
    int32_t fd;
};


struct ftpusersock {
    int32_t fd;
    int32_t stage;

    char    user[32];
};

