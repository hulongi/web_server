#pragma once
#include"epoll_server.h"
#include"thread_pool.hpp"
#include"httpresponse.h"
#include "log.h"
class server
{
private:
    epoll_server epoll;
    thread_pool pool;
    httpresponse response;
    bool is_running;
    //mutex mtx;
    //condition_variable condition;
    int port;
    int max;
public:
    server(int min_num,int max_num,int queue_size,int port);
    int start();
    //mutex& get_mutex();
    //condition_variable& get_condition();
    void do_accept(int lfd);
    void do_read(int cfd);
    void disconnect(int cfd);
    ~server();
};



