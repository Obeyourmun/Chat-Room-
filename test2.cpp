_Pragma("once")
#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<iostream>
#include<pthread.h>
#include<string>
#include<stdio.h>
#include<stdlib.h>
#include<codecvt>
#include<string>
#include<codecvt>
#include<string>
#include<cstdlib>
#include<memory>
#include<string>
#include<sys/epoll.h>
#include<iostream>
#include<mutex>
#include "Threadpool.h"
#include "myDB.h"
#define MAX_NUM 200
using namespace std;

MyDB db; //连接数据库
struct epoll_event ev, events[MAX_NUM];
int epollfd; int total=0;
int serv_sock;//
struct sockaddr_in serv_addr;
pthread_t pthread[MAX_NUM];
int pthread_num;
int arg = 0; std::mutex mutex1;
int chuanx = 0;
string arrx, brrx;
void error_handling(char* msgg);

struct clnt{
    int clnt_sock;
    struct sockaddr_in clnt_addr;
    bool alive = true;
    int fx = 0;
}clnt_num[MAX_NUM];

//设置非阻塞
int setnonblocking(int clnt){
    int old_option = fcntl(clnt, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(clnt, F_SETFL, new_option);
    return old_option;
}

//断开连接
void handle_client_disconnection(int clnt) {
    for(int a=0; a<total; a++)
        if(clnt == clnt_num[a].clnt_sock){
            clnt_num[a].alive = false;
            close(clnt_num[a].clnt_sock);
        }
}

//从客户端接收信息
string receive_data(int clnt) {
    char buffer[300] = {};
    int recv_status = recv(clnt, buffer, 256, 0);
    if(recv_status < 0){
        handle_client_disconnection(clnt);
    }
    return buffer;
}

//线程主函数
void main2(){
    int clnt = arg;
    string received_msg = receive_data(clnt);
    if(clnt_num[chuanx].fx==1)arrx = received_msg;
    if(clnt_num[chuanx].fx==2){
        brrx = received_msg;
    }
    if(clnt_num[chuanx].fx > 2){
        cout<<received_msg<<endl;
        for(int a=0; a<total; a++){
            if(clnt!=clnt_num[a].clnt_sock && clnt_num[a].alive==true){
                int send_status = send(clnt_num[a].clnt_sock, received_msg.c_str(), 256, 0);
                if(send_status < 0) perror("send_pthread error");
                if(send_status == 0) perror("send_pthread = 0 error");
            }
        }
    }
    return ;  
}

//错误输出代码
void error_handling(char* msgg){
    fputs(msgg, stderr);
    fputc('\n', stderr);
    close(serv_sock);
    exit(1);
}

// 创建 socket, 设置为非阻塞模式并启动监听，还有一些 epoll 的设置
void setupSocketAndEpoll(int argc, char* argv[]){
    if(argc != 2){
        error_handling("argc error");
    } 

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1) error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    int serv_bind = bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(serv_bind == -1) error_handling("bind() error");

    int serv_lisn = listen(serv_sock, 5);
    if(serv_lisn == -1) error_handling("listen() error");

    // 创建 epoll 实例
    epollfd = epoll_create(10);
    if (epollfd == -1) error_handling("epoll error()");

    // 将监听的端口的 socket 对应的文件描述符添加到 epoll 事件列表中
    ev.events = EPOLLIN;
    ev.data.fd = serv_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serv_sock, &ev) == -1) error_handling("epoll_ctl: serv_sock");
}

// 处理客户端请求
void handleClientRequests(ThreadPool &pool){
    int nfds = epoll_wait(epollfd, events, MAX_NUM, -1);
    if (nfds == -1) error_handling("epoll_wait error()");

    for(int a=0; a<nfds; a++){
        if(events[a].data.fd == serv_sock){
            socklen_t clnt_addr_size = sizeof(clnt_num[total].clnt_addr);
            clnt_num[total].clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_num[total].clnt_addr, &clnt_addr_size);
            if(clnt_num[total].clnt_sock == -1) error_handling("clnt_num[num] error");

            setnonblocking(clnt_num[total].clnt_sock);//设置非阻塞

            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = clnt_num[total].clnt_sock;

            //设置参数
            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clnt_num[total].clnt_sock, &ev) == -1)
                error_handling("epoll_ctl: conn_sock");

            total++;//总计数 
 //              checkx(clnt_num[total].clnt_sock);//注册或登录账号密码
        // //       cout<<"four"<<endl;
        //        db.exeSQL("INSERT number (account, password)values("+arrx+","+brrx+");"); 
        //        cout<<arrx<<" "<<brrx;
        //        arrx = {}; brrx={};              
            //   setnonblocking(clnt_num[total].clnt_sock);//设置非阻塞
        }
        else{//有消息到来
            {
                unique_lock<mutex> argl(mutex1);
                arg = events[a].data.fd;
                //
                for(int ji=0; ji<total; ji++){
                    if(clnt_num[ji].clnt_sock == events[a].data.fd){
                        clnt_num[ji].fx++;
                        chuanx = ji;
                        if(clnt_num[ji].fx == 3){
                            // 将账号密码插入数据库
                            db.exeSQL("INSERT number (account, password)values("+arrx+","+brrx+");"); 
                            arrx = {}; brrx={};
                        }
                        break;
                    }
                }
            }
            // 将主函数加入线程池
            pool.enqueue(main2);
        }
    }
}


int main(int argc, char* argv[]){
    db.initDB("localhost","root","123456","test");//登录数据库 

    ThreadPool pool(30);//创建一个包含30个线程的线程池

    //进行一系列的socket设置和epoll的设置
    setupSocketAndEpoll(argc, argv);
  
    //主循环，接收并处理客户端信息
    while(1){
        handleClientRequests(pool);
    }

    close(serv_sock);  
    cout<<"结束\n";
    return 0;
}
