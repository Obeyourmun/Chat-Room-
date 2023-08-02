#include<iostream>
#include"myDB.h"
using namespace std;


int main()
{
    MyDB db; //连接数据库
    db.initDB("localhost","root","123456","test");//登录

    //将用户信息添加到数据库
//    db.exeSQL("INSERT Lin1 values(4, '风某', 123, '男');");
    string arr = "888888";
    string brr = "1234568";
    db.exeSQL("INSERT number (account, password)values("+arr+","+brr+");");

    //将所有用户信息读出，并输出。
 //   db.exeSQL("SELECT * from Lin1;");
 //   int idx = 1;
    db.exeSQL("select * from number");

  //  string query = "select * from test.login where username='" + username + "' and password='" + password + "'";

    return 0;
}

// void checkx(int clnt){
//   char arr[300] = {};
//   char brr[300] = {};
//   int fan = recv(clnt, arr, 256, 0);
//         if(fan<0){
//            for(int a=0; a<total; a++)
//            if(clnt == clnt_num[a].clnt_sock){
//             clnt_num[a].alive=false;
//             close(clnt_num[a].clnt_sock);
//             }
//         }
//   arrx = arr;
//   fan = recv(clnt, brr, 256, 0);
//        if(fan<0){
//            for(int a=0; a<total; a++)
//            if(clnt == clnt_num[a].clnt_sock){
//             clnt_num[a].alive=false;
//             close(clnt_num[a].clnt_sock);
//             }
//         }
//   brrx = brr;

//   return ;
// }