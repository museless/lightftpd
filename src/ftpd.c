/*
 * =============================
 *
 *  Filename: ftpd.c
 *  Created:  2018-05-11 23:51:17
 *  Author:   Muse
 *
 * ============================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <unistd.h>
#include <netdb.h>
#include <errno.h>


int main(int argc, char *argv[])
{
    printf("Pid: %d\n", getpid());

    int32_t sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        perror("socket");
        return  -1;
    }

    struct sockaddr_in  addr;
    socklen_t           len = sizeof(struct sockaddr_in);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(21);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sock, (const struct sockaddr *)&addr, len) == -1) {
        perror("bind");
        return  -1;
    }

    if (listen(sock, 8) == -1) {
        perror("listen");
        return  -1;
    }

    struct sockaddr_in  useraddr;
    int32_t             usock;
    socklen_t           addrlen;

    if ((usock = accept(sock, (struct sockaddr *)&useraddr, &addrlen)) == -1) {
        perror("accept");
        return  -1;
    }

    char    message[] = "220 welcome to use lightftpd\r\n";

    write(usock, (const void *)&message, strlen(message));

    sleep(120);

    return  -1;
}

