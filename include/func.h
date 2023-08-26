#include<sys/types.h>
#include<sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include<arpa/inet.h>
#include <fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>
#include<string.h>
#include<event2/bufferevent.h>
#include<event2/listener.h>
#include<event2/event.h>
#include<event2/buffer.h>
#include<signal.h>
#include<dirent.h>
#define PORT 9999
#define _HTTP_CLOSE_ "Connection: close\r\n"
int hexit(char c);
void strencode(char* to, size_t tosize, const char* from);
void strdecode(char *to, char *from);
int send_error(bufferevent *bev);
int send_dir(bufferevent *bev,const char *dirname);
int send_header(bufferevent *bev,int no,const char* desp,const char* type,long len);
char* get_file_type( char* name);
int send_file(bufferevent *bev,const char *filename);
int response_http(bufferevent *bev,const char* method,char* path);
void read_cb(bufferevent *bev,void *arg);
void write_cb(bufferevent *bev,void *arg);
void event_cb(bufferevent *bev,short events,void *arg);
void cb_listener(evconnlistener *listener,evutil_socket_t fd,sockaddr *addr,int len,void * userdata);
void signal_cb(evutil_socket_t fd,short event,void * arg);