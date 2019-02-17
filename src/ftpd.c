/*
 * =============================
 *
 *  Filename: ftpd.c
 *  Created:  2018-05-11 23:51:17
 *  Author:   Muse
 *
 * ============================
 */

#include "ftpd.h"


static bool parse_cmd(const char *data, char *cmd, char *args);


int main(int argc, char *argv[])
{
    printf("Pid: %d\n", getpid());

    int32_t ftpsock = socket(AF_INET, SOCK_STREAM, 0);

    if (ftpsock == -1) {
        perror("socket");
        return  -1;
    }

    sockaddr_t  addr;
    socklen_t   len = sizeof(sockaddr_t);
    int32_t     val = 1;
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(21);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    setsockopt(
        ftpsock, SOL_SOCKET, SO_REUSEADDR, (const void *)&val, sizeof(int32_t)
    );

    if (bind(ftpsock, (const saddr_t *)&addr, len) == -1) {
        perror("bind");
        return  -1;
    }

    if (listen(ftpsock, 8) == -1) {
        perror("listen");
        return  -1;
    }

    int32_t     epid = epoll_create(1024), evsize;
    epollev_t   event, eventarr[EPOLL_ARR_SIZE];

    char        message[] = "220 Welcome to use lightftpd ready\r\n";
    char        error[] = "500 Not implemented request\r\n";
    char        bye[] = "221 Good bye\r\n";
    char        login[] = "331 Please specify the password\r\n";
    char        loginsuccess[] = "230 Login successful\r\n";

    int32_t     msglen = strlen(message), errlen = strlen(error);
    int32_t     byelen = strlen(bye), loginlen = strlen(login);
    int32_t     successlen = strlen(loginsuccess);

    char        databuf[BUFSIZ];
    ftpservs_t  servsock = {ftpsock};

    event.events = EPOLLIN;
    event.data.ptr = &servsock;
    epoll_ctl(epid, EPOLL_CTL_ADD, ftpsock, &event);

    while (true) {
        evsize = epoll_wait(epid, eventarr, EPOLL_ARR_SIZE, -1);

        if (evsize == -1) {
            if (errno == EINTR) {
                continue;
            }

            perror("epoll_wait");
            return  -1;
        }

        for (int32_t idx = 0; idx < evsize; idx++) {
            epollev_t  *eventptr = (eventarr + idx);
            int32_t     fd = ((ftps_t *)eventptr->data.ptr)->fd;

            if (fd == ftpsock) {
                sockaddr_t  uaddr;
                socklen_t   alen;
                int32_t     usock = accept(ftpsock, (saddr_t *)&uaddr, &alen);

                fcntl(usock, F_SETFD, O_NONBLOCK);

                if (usock == -1) {
                    perror("accept");
                    return  -1;
                }

                ftpusers_t *user = malloc(sizeof(ftpusers_t));

                if (user == NULL) {
                    perror("malloc");
                    return  -1;
                }

                user->fd = usock;
                user->stage = USER_INITIALIZE;

                event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                event.data.ptr = user;
                epoll_ctl(epid, EPOLL_CTL_ADD, usock, &event);
                continue;
            }

            if (eventptr->events & EPOLLHUP) {
                epoll_ctl(epid, EPOLL_CTL_DEL, fd, NULL);
                free(eventptr->data.ptr);
                printf("user: %d closed connection\n", fd);
                close(fd);
                continue;
            }

            ftpusers_t *so = (ftpusers_t *)eventptr->data.ptr;

            if (eventptr->events & EPOLLIN) {
                int32_t res = read(fd, databuf, BUFSIZ);

                if (res == -1) {
                    if (errno != EAGAIN) {
                        perror("read");
                        return  -1;
                    }
                }

                if (res != 0) {
                    char    cmd[32] = {0};
                    char    args[32] = {0};

                    if (!parse_cmd(databuf, cmd, args)) {
                        write(fd, (const void *)&error, errlen);
                        continue;
                    }

                    if (strncmp(databuf, "QUIT", 4) == 0) {
                        write(fd, (const void *)&bye, byelen);

                    } else if (strncmp(databuf, "USER", 4) == 0) {
                        if (!strncmp(args, ANONYMOUS_USER, ANONYMOUS_USER_LEN)) {
                            so->stage = USER_LOGIN;
                            write(fd, (const void *)&login, loginlen);

                        } else {
                            write(fd, (const void *)&error, errlen);
                            continue;
                        }

                    } else if (strncmp(databuf, "PASS", 4) == 0) {
                        const char *passerr = "500 Password error\r\n";

                        if (!strncmp(args, ANONYMOUS_PWD, ANONYMOUS_PWD_LEN)) {
                            so->stage = USER_PASSWORD;
                            write(fd, (const void *)&loginsuccess, successlen);

                        } else {
                            write(fd, (const void *)&passerr, strlen(passerr));
                            continue;
                        }
                    }
                }
            }

            if (eventptr->events & EPOLLOUT) {
                if (so->stage == USER_INITIALIZE) {
                    write(fd, (const void *)&message, msglen);
                    so->stage = USER_WELCOMED;
                }
            }
        }
    }

    return  -1;
}


bool parse_cmd(const char *data, char *cmd, char *args)
{
    char   *pcrlf = strstr(data, "\r\n");

    if (!pcrlf) {
        return  false;
    }

    char   *penter = strchr(data, (int32_t)' ');

    if (penter) {
        int32_t cmdlen = penter - data;

        strncpy(cmd, data, cmdlen);
        strncpy(args, data + cmdlen + 1, pcrlf - penter - 1);

    } else {
        strncpy(cmd, data, pcrlf - data);
    }

    return  true;
}

