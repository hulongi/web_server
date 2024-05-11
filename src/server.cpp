#include"server.h"
server::server(int min_num,int max_num,int queue_size,int port):pool(min_num,max_num,queue_size)
{
    this->port=port;
    max=1024;
}

int server::start()
{
    is_running=true;
   int epfd=epoll.epoll_creat_();
    if(epfd==-1)
    {
        perror("epoll_create error");
        exit(1);
    }
    int lfd=epoll.init_listen_fd(port);

    while(1)
    {
        int ret=epoll.epoll_wait_(0);
        if(ret==-1)
        {
            perror("epoll_wait error");
            exit(1);
        }
        for(int i=0;i<ret;i++)
        {
            epoll_event *pev=epoll.get_event(i);
            if(!(pev->events&EPOLLIN))
            {
                continue;
            }
            if(pev->data.fd==lfd)
            {
                pool.enqueue(bind(&server::do_accept,this,lfd));
            }
            else
            {
                int fd=pev->data.fd;
                pool.enqueue(bind(&server::do_read,this,fd));
            }
            
        }
    }
    return 0;
}
void server::do_accept(int lfd)
{
    epoll.do_accept(lfd);
}
void server::do_read(int cfd)
{
    char line[1024]={0};
    int len=response.get_line(cfd,line,sizeof(line));
    if(len==0)
    {
        printf("客户端断开连接...\n");
        disconnect(cfd);
    }
    else
    {
        printf("============= 请求头 ============\n");   
        printf("请求行数据: %s", line);
        while(1)
        {
            char buf[1024]={0};
            len=response.get_line(cfd,buf,sizeof(buf));
            if(buf[0]=='\n')
            {
                break;
            }
            else if(len==-1)
            {
                break;
            }
        }
         printf("============= The End ============\n");
    }
    if(strncasecmp("get",line,3)==0)
    {
        response.http_request(line,cfd);
        disconnect(cfd);
    }
}

void server::disconnect(int cfd)
{
    epoll.disconnect(cfd);
}

server::~server()
{
    is_running=false;
    
}