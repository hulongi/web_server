#pragma once
#include"server.h"
//#include"server.cpp"
#define PORT 9666
const char* dir="/home/hulong/vscode/override_server/res";
int main()
{
    Logger* logger=Logger::get_instance();
    logger->init();
    server serv(100,1000,100,PORT);
    int ret=chdir(dir);
    if(ret==-1)
    {
        perror("chdir error");
        exit(1);
    }
    serv.start();
    return 0;
}