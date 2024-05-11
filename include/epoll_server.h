#pragma once
#include<sys/epoll.h>
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include "log.h"
#define MAX_SIZE 2000
class epoll_server
{
private:
    int epfd;
    epoll_event all[MAX_SIZE];
public:
    epoll_server(/* args */);
    int epoll_add(int fd, struct epoll_event *event);
    int epoll_del(int fd, struct epoll_event *event);
    int epoll_mod(int fd, struct epoll_event *event);
    int epoll_creat_();
    int epoll_wait_(int op);
    int init_listen_fd(int port);
    void epoll_run(int port);
    epoll_event* get_event(int i);
    void disconnect(int cfd);
    void do_accept(int lfd);
    //void set_epfd(int epfd_);
    //int get_epfd();
    ~epoll_server();
};
