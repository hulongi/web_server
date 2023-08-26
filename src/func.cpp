#include"func.h"
int hexit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return 0;
}
//编码
void strencode(char* to, size_t tosize, const char* from)
{
    int tolen;

    for (tolen = 0; *from != '\0' && tolen + 4 < tosize; ++from)
    {
        if (isalnum(*from) || strchr("/_.-~", *from) != (char*)0)
        {
            *to = *from;
            ++to;
            ++tolen;
        }
        else
        {
            sprintf(to, "%%%02x", (int) *from & 0xff);
            to += 3;
            tolen += 3;
        }
    }
    *to = '\0';
}
//解码
void strdecode(char *to, char *from)
{
    for ( ; *from != '\0'; ++to, ++from)
    {
        if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2]))
        {
            // 依次判断from中 %20 三个字符
            *to = hexit(from[1])*16 + hexit(from[2]);
            // 移过已经处理的两个字符(%21指针指向1),表达式3的++from还会再向后移一个字符
            from += 2;
        }
        else
        {
            *to = *from;
        }
    }
    *to = '\0';
}
int send_error(bufferevent *bev)
{
    send_header(bev,404, "File Not Found", "text/html", -1);
    send_file(bev,"404.html");
    return 0;
}
int send_dir(bufferevent *bev,const char *dirname)
{
    char encoded_name[1024];
    char path[1024];
    char timestr[64];
    struct stat sb;
    struct dirent **dirinfo;
    int i;

    char buf[4096] = {0};
    sprintf(buf, "<html><head><meta charset=\"utf-8\"><title>%s</title></head>", dirname);
    sprintf(buf+strlen(buf), "<body><h1>当前目录：%s</h1><table>", dirname);
    //添加目录内容
    int num = scandir(dirname, &dirinfo, NULL, alphasort);
    for(i=0; i<num; ++i)
    {
        // 编码
        strencode(encoded_name, sizeof(encoded_name), dirinfo[i]->d_name);

        sprintf(path, "%s%s", dirname, dirinfo[i]->d_name);
        printf("############# path = %s\n", path);
        if (lstat(path, &sb) < 0)
        {
            sprintf(buf+strlen(buf), 
                    "<tr><td><a href=\"%s\">%s</a></td></tr>\n", 
                    encoded_name, dirinfo[i]->d_name);
        }
        else
        {
            strftime(timestr, sizeof(timestr), 
                     "  %d  %b   %Y  %H:%M", localtime(&sb.st_mtime));
            if(S_ISDIR(sb.st_mode))
            {
                sprintf(buf+strlen(buf), 
                        "<tr><td><a href=\"%s/\">%s/</a></td><td>%s</td><td>%ld</td></tr>\n",
                        encoded_name, dirinfo[i]->d_name, timestr, sb.st_size);
            }
            else
            {
                sprintf(buf+strlen(buf), 
                        "<tr><td><a href=\"%s\">%s</a></td><td>%s</td><td>%ld</td></tr>\n", 
                        encoded_name, dirinfo[i]->d_name, timestr, sb.st_size);
            }
        }
        bufferevent_write(bev, buf, strlen(buf));
        memset(buf, 0, sizeof(buf));
    }
    sprintf(buf+strlen(buf), "</table></body></html>");
    bufferevent_write(bev, buf, strlen(buf));
    printf("################# Dir Read OK !!!!!!!!!!!!!!\n");

    return 0;
}
int send_header(bufferevent *bev,int no,const char* desp,const char* type,long len)
{
    char buf[256]={0};

    sprintf(buf, "HTTP/1.1 %d %s\r\n", no, desp);
    //HTTP/1.1 200 OK\r\n
    bufferevent_write(bev, buf, strlen(buf));
    // 文件类型
    sprintf(buf, "Content-Type:%s\r\n", type);
    printf("send type:%s\n",type);
    bufferevent_write(bev, buf, strlen(buf));
    // 文件大小
    sprintf(buf, "Content-Length:%ld\r\n", len);
    bufferevent_write(bev, buf, strlen(buf));
    // Connection: close
    bufferevent_write(bev, _HTTP_CLOSE_, strlen(_HTTP_CLOSE_));
    //send \r\n
    bufferevent_write(bev, "\r\n", 2);

    return 0;
}
char* get_file_type( char* name)
{
     char* dot;
    printf("name:%s\n",name);
    dot = strrchr(name, '.');	//自右向左查找‘.’字符;如不存在返回NULL
    printf("dot:%s\n",dot);
    if (dot == (char*)0)
        return "text/plain; charset=utf-8";
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
        return "text/html; charset=utf-8";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcmp(dot, ".png") == 0)
        return "image/png";
    if (strcmp(dot, ".css") == 0)
        return "text/css";
    if (strcmp(dot, ".au") == 0)
        return "audio/basic";
    if (strcmp( dot, ".wav") == 0)
        return "audio/wav";
    if (strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
        return "video/quicktime";
    if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
        return "video/mpeg";
    if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
        return "model/vrml";
    if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
        return "audio/midi";
    if (strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(dot, ".ogg") == 0)
        return "application/ogg";
    if (strcmp(dot, ".pac") == 0)
        return "application/x-ns-proxy-autoconfig";

    return "text/plain; charset=utf-8";
}
int send_file(bufferevent *bev,const char *filename)
{
    int fd = open(filename, O_RDONLY);
    int ret = 0;
    char buf[4096] = {0};

    while((ret = read(fd, buf, sizeof(buf)) ) )
    {
        bufferevent_write(bev, buf, ret);
        memset(buf, 0, ret);
    }
    close(fd);
    return 0;
}
int response_http(bufferevent *bev,const char* method,char* path)
{
    if(strcasecmp(method,"GET")==0)
    {
        strdecode(path,path);
        char *pf=path+1;
        if(strcmp(path,"/")==0 ||strcmp(path,"./")==0)
        {
            pf="./";
        }
        printf("*** http request resouse path:%s pf:%s***\n",path,pf);
        struct stat sb;
        if(stat(pf,&sb)<0)
        {
            perror("stat error");
            send_error(bev);
            return -1;
        }

        //处理文件夹
        if(S_ISDIR(sb.st_mode))
        {
            send_header(bev,200,"OK",get_file_type(".html"),-1);
            send_dir(bev,pf);
        }
        else
        {
            send_header(bev,200,"OK",get_file_type(pf),-1);
            send_file(bev,pf);
        }

    }
    return 0;
    
}
void read_cb(bufferevent *bev,void *arg)
{
    printf("************begin call %s***********\n",__FUNCTION__);

    char buf[BUFSIZ]={0};
    char method[64]={0},path[4096]={0},protocol[64]={0};
    bufferevent_read(bev,buf,BUFSIZ);
    printf("buf[%s]\n",buf);
    sscanf(buf,"%[^ ] %[^ ] %[^ \r\n]",method,path,protocol);
    printf("method[%s],path[%s],protocol[%s]\n",method,path,protocol);
    if(strcasecmp(method,"GET")==0)
    {
        response_http(bev,method,path);
    }

    printf("************end call %s***********\n",__FUNCTION__);
    return ;
}
void write_cb(bufferevent *bev,void *arg)
{
    printf("send successful\n");
}
void event_cb(bufferevent *bev,short events,void *arg)
{
    if(events & BEV_EVENT_EOF)
    {
        printf("connect closed\n");
    }
    if(events & BEV_ERROR)
    {
        printf("error\n");
    }
    bufferevent_free(bev);
    printf("资源释放\n");
}
void cb_listener(evconnlistener *listener,evutil_socket_t fd,sockaddr *addr,int len,void * userdata)
{
    printf("************begin call %s***********\n",__FUNCTION__);
    printf("new client\n");
    event_base *base=(event_base*)userdata;
    //添加事件
    bufferevent *bev;
    bev=bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);
    if(!bev)
    {
        printf("bufferevent error\n");
        event_base_loopbreak(base);
        return;
    }

    //刷新缓冲区
    bufferevent_flush(bev,EV_READ | EV_WRITE,BEV_NORMAL);

    //设置回调
    bufferevent_setcb(bev,read_cb,write_cb,event_cb,NULL);

    //开启读缓冲
    bufferevent_enable(bev,EV_READ | EV_WRITE);
    printf("************end call %s***********\n",__FUNCTION__);
}
void signal_cb(evutil_socket_t fd,short event,void * arg)
{
     struct event_base *base = (event_base*)arg;
    struct timeval delay = { 1, 0 };

    printf("Caught an interrupt signal; exiting cleanly in one seconds.\n");
    event_base_loopexit(base, &delay);
}