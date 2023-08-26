

#include"func.h"
int main()
{
    chdir("/home/hulong/vscode/server_libevent/res");
    //init server
    sockaddr_in server;
    memset(&server,0,sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr=htonl(INADDR_ANY);

    //创建server base
    event_base * base;
    base=event_base_new();
    
    //监听 绑定 接受连接请求
    evconnlistener * listener;
    listener=evconnlistener_new_bind(base,cb_listener,base,
    LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
    36,(sockaddr*)&server,sizeof(server));

    //创建信号事件，捕捉处理
    event *signal_event;
    signal_event=evsignal_new(base,SIGINT,signal_cb,(void*)base);


    //启动循环监听
    event_base_dispatch(base);
    //释放资源
    event_free(signal_event);
    evconnlistener_free(listener);
    event_base_free(base);
    return 0;
}

