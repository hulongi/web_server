#include"epoll_server.h"
epoll_server::epoll_server(/* args */)
{
    epfd=epoll_create(MAX_SIZE);
    if(epfd==-1)
    {
        perror("epoll_create error");
        exit(1);
    }
}

epoll_server::~epoll_server()
{
    close(epfd);
}

int epoll_server::epoll_add(int fd,epoll_event *event)
{
    
    return epoll_ctl(epfd,EPOLL_CTL_ADD,fd,event);

}
int epoll_server::epoll_mod(int fd,epoll_event *event)
{
    return epoll_ctl(epfd,EPOLL_CTL_MOD,fd,event);
}

int epoll_server::epoll_creat_()
{
    epfd=epoll_create(MAX_SIZE);
    return epfd;
}

int epoll_server::epoll_wait_( int op)
{
    return epoll_wait(epfd,all,MAX_SIZE,op);
}

int epoll_server::init_listen_fd(int port)
{
    int lfd= socket(AF_INET,SOCK_STREAM,0);
    if(lfd==-1)
    {
        perror("socket error");
        exit(1);
    }

    sockaddr_in serv;
    memset(&serv,0,sizeof(serv));
    serv.sin_family=AF_INET;
    serv.sin_port=htons(port);
    serv.sin_addr.s_addr=htonl(INADDR_ANY);

    int flag=1;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(flag));

    int ret=bind(lfd,(sockaddr*)&serv,sizeof(serv));
    if(ret==-1)
    {
        perror("bind error");
        exit(1);
    }

    ret=listen(lfd,64);
    if(ret==-1)
    {
        perror("listen error");
        exit(1);
    }

    epoll_event ev;
    ev.events=EPOLLIN;
    ev.data.fd=lfd;

    ret=epoll_add(lfd,&ev);
    if(ret==-1)
    {
        perror("epoll_ctl add error");
        exit(1);
    }
    return lfd;
}

void epoll_server::epoll_run(int port)
{
    
}

epoll_event *epoll_server::get_event(int i)
{
    return &all[i];
}

void epoll_server::disconnect(int cfd)
{
    int ret=epoll_del(cfd,NULL);
}

void epoll_server::do_accept(int lfd)
{
    sockaddr_in client;
    socklen_t len=sizeof(client);
    int cfd=accept(lfd,(sockaddr*)&client,&len);
    if(cfd==-1)
    {
        perror("accept error");
        exit(1);
    }
     char ip[64] = {0};
    printf("New Client IP: %s, Port: %d, cfd = %d\n",
    inet_ntop(AF_INET, &client.sin_addr.s_addr, ip, sizeof(ip)),
    ntohs(client.sin_port), cfd);
    LOG("New Client IP: %s, Port: %d, cfd = %d\n",
    inet_ntop(AF_INET, &client.sin_addr.s_addr, ip, sizeof(ip)),
    ntohs(client.sin_port), cfd);

    int flag=fcntl(cfd,F_GETFL);
    flag|=O_NONBLOCK;
    fcntl(cfd,F_SETFL,flag);

    epoll_event ev;
    ev.data.fd=cfd;
    ev.events=EPOLLIN|EPOLLET;
    int ret=epoll_add(cfd,&ev);
    if(ret==-1)
    {
        perror("epoll_ctl add cfd error");
        exit(1);
    }
    
}

int epoll_server::epoll_del(int fd, epoll_event *event)
{
    return epoll_ctl(epfd,EPOLL_CTL_DEL,fd,event);
}
