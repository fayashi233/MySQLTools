#include<mysql/mysql.h>
#include<iostream>
//连接mysql服务器
int main()
{
    MYSQL *ConnectPointer=mysql_init(NULL);
	ConnectPointer=mysql_real_connect(ConnectPointer,"127.0.0.1","root","password","test",0,NULL,0);
	if (ConnectPointer){
   
		printf("connect database successfully\n");
	}else{
   
		printf("failed to connect database\n");
	}
	mysql_close(ConnectPointer);
	printf("database connection closed successfully\n");
	printf("\n");
	return 0;
}

// MYSQL *mysql_real_connect(
//     MYSQL *mysql,           // mysql_init() 函数的返回值
//     const char *host,       // mysql服务器的主机地址, 写IP地址即可
//                             // localhost, null -> 代表本地连接
//     const char *user,       // 连接mysql服务器的用户名, 默认: root 
//     const char *passwd,     // 连接mysql服务器用户对应的密码, root用户的密码
//     const char *db,         // 要使用的数据库的名字
//     unsigned int port,      // 连接的mysql服务器监听的端口
//                             // 如果==0, 使用mysql的默认端口3306, !=0, 使用指定的这个端口
//     const char *unix_socket,// 本地套接字, 不使用指定为 NULL
//     unsigned long client_flag); // 通常指定为0