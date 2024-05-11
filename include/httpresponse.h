#pragma once
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<string>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include<cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include<unistd.h>
#include<sys/sendfile.h>
#include "log.h"
class httpresponse
{
    public:
    void http_request(const char* request, int cfd);
    void send_respond_head(int cfd, int no, const char* desp, const char* type, long len);
    void send_file(int cfd, const char* filename);
    void send_dir(int cfd, const char* dirname);
    void send_error(int cfd, int status, char *title, char *text);
    void encode_str(char* to, int tosize, const char* from);
    void decode_str(char *to, char *from);
    int get_line(int sock, char *buf, int size);
    int hexit(char c);
    const char *get_file_type(const char *name);
};