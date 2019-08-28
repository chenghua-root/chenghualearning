#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

#define MAX_EVENT 20
#define READ_BUF_LEN 256

/*
 * 说明：本文EPOLL处理代码直接拷贝自https://www.jianshu.com/p/ee381d365a29,
 * 在此基础上改写为多epoll模型。
 *
 * 例子中EPOLL触发模型为边缘触发，生成环境一般为水平触发。
 * 其差别以及为什么生成环境使用水平触发可参考网上讨论。
 *
 */

/**
 * 设置 file describe 为非阻塞模式
 * @param fd 文件描述
 * @return 返回0成功，返回-1失败
 */
static int make_socket_non_blocking (int fd) {
    int flags, s;
    // 获取当前flag
    flags = fcntl(fd, F_GETFL, 0);
    if (-1 == flags) {
        perror("Get fd status");
        return -1;
    }

    flags |= O_NONBLOCK;

    // 设置flag
    s = fcntl(fd, F_SETFL, flags);
    if (-1 == s) {
        perror("Set fd status");
        return -1;
    }
    return 0;
}

#define EPOLL_INS_CNT 4
int epfds[EPOLL_INS_CNT] = {0};

void epoll_do_create(void *arg_) {
    int64_t idx = (int64_t)arg_;
    if (idx < 0 || idx >= EPOLL_INS_CNT) {
        printf("epoll idx invalid, idx=%d\n", idx);
    }
    printf("epoll create. idx=%d\n", idx);

    struct epoll_event ev, event[MAX_EVENT];
    int epfd = 0;
    epfd = epoll_create1(0);
    if (-1 == epfd) {
        perror("Create epoll instance");
        return;
    }
    epfds[idx] = epfd;

    for ( ; ; ) {
        int wait_count;
        // 等待事件
        wait_count = epoll_wait(epfd, event, MAX_EVENT, -1);

        int done = 0;
        int i = 0;
        for (i = 0 ; i < wait_count; i++) {
            ssize_t result_len = 0;
            char buf[READ_BUF_LEN] = { 0 };

            result_len = read(event[i].data.fd, buf, sizeof(buf)/sizeof(buf[0]));
            if (-1 == result_len) {
                if (EAGAIN != errno) {
                    perror ("Read data");
                    done = 1;
                }
                break;
            } else if (!result_len) {
                done = 1;
                break;
            }

            write(STDOUT_FILENO, buf, result_len);
        }
        if (done) {
            printf("Closed connection\n");
            close (event[i].data.fd);
        }
    }
}

void epoll_multi_create() {
    pthread_t threads[EPOLL_INS_CNT];
    int i = 0;
    for (i = 0; i < EPOLL_INS_CNT; ++i) {
        int j = i;
        pthread_create(&threads[i], NULL, epoll_do_create, j);
    }
    /*TODO:确保epoll实例已经创建完毕*/
}

int main() {
    // epoll 实例 file describe
    int epfd = 0;
    int listenfd = 0;
    int result = 0;
    struct epoll_event ev, event[MAX_EVENT];
    // 绑定的地址
    const char * const local_addr = "10.16.80.8";
    struct sockaddr_in server_addr = { 0 };

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listenfd) {
        perror("Open listen socket");
        return -1;
    }
    /* Enable address reuse */
    int on = 1;
    // 打开 socket 端口复用, 防止测试的时候出现 Address already in use
    result = setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
    if (-1 == result) {
        perror ("Set socket");
        return 0;
    }

    server_addr.sin_family = AF_INET;
    inet_aton (local_addr, &(server_addr.sin_addr));
    server_addr.sin_port = htons(8088);
    result = bind(listenfd, (const struct sockaddr *)&server_addr, sizeof (server_addr));
    if (-1 == result) {
        perror("Bind port");
        return 0;
    }
    result = make_socket_non_blocking(listenfd);
    if (-1 == result) {
        return 0;
    }

    result = listen(listenfd, 200);
    if (-1 == result) {
        perror("Start listen");
        return 0;
    }

    // 创建多个epoll实例
    epoll_multi_create();

    // 创建epoll实例
    epfd = epoll_create1(0);
    if (-1 == epfd) {
        perror("Create epoll instance");
        return 0;
    }

    ev.data.fd = listenfd;
    ev.events = EPOLLIN | EPOLLET /* 边缘触发选项。 */;

    // 设置epoll的事件
    result = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);
    if(-1 == result) {
        perror("Set epoll_ctl");
        return 0;
    }

    uint64_t accept_cnt = 0;
    for ( ; ; ) {
        int wait_count = 0;
        // 等待事件
        wait_count = epoll_wait(epfd, event, MAX_EVENT, -1);

        int i = 0;
        for (i = 0 ; i < wait_count; i++) {
            uint32_t events = event[i].events;
            // IP地址缓存
            char host_buf[NI_MAXHOST];
            // PORT缓存
            char port_buf[NI_MAXSERV];

            int __result;
            // 判断epoll是否发生错误
            if (events & EPOLLERR || events & EPOLLHUP || (! events & EPOLLIN)) {
                printf("Epoll has error\n");
                close (event[i].data.fd);
                continue;
            } else if (listenfd == event[i].data.fd) {
                // listen的 file describe 事件触发， accpet事件

                for ( ; ; ) { // 由于采用了边缘触发模式，这里需要使用循环
                    struct sockaddr in_addr = { 0 };
                    socklen_t in_addr_len = sizeof (in_addr);
                    int accp_fd = accept(listenfd, &in_addr, &in_addr_len);
                    if (-1 == accp_fd) {
                        perror("Accept");
                        break;
                    }
                    __result = getnameinfo(&in_addr, sizeof (in_addr),
                                           host_buf, sizeof (host_buf) / sizeof (host_buf[0]),
                                           port_buf, sizeof (port_buf) / sizeof (port_buf[0]),
                                           NI_NUMERICHOST | NI_NUMERICSERV);

                    if (! __result) {
                        printf("\n==========New connection: host = %s, port = %s============\n", host_buf, port_buf);
                    }

                    __result = make_socket_non_blocking(accp_fd);
                    if (-1 == __result) {
                        return 0;
                    }

                    ev.data.fd = accp_fd;
                    ev.events = EPOLLIN | EPOLLET;
                    /* 为新accept的 file describe 设置epoll事件
                     * epoll_ctl为线程安全
                     */
                    __result = epoll_ctl(epfds[accept_cnt++%EPOLL_INS_CNT], EPOLL_CTL_ADD, accp_fd, &ev);

                    if (-1 == __result) {
                        perror("epoll_ctl");
                        return 0;
                    }
                }
            }
        }

    }
    close (epfd);
    return 0;
}
