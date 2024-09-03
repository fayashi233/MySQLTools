#include "../MySQLTools.h"
#include <iostream>
#include <string>


using Connection = MYSQLTOOLS::Connection;
using SqlStatement = MYSQLTOOLS::SqlStatement;
int main()
{
    Connection conn;
    if (!conn.connect("127.0.0.1", "root", "password", "test")) {
        printf("连接数据库失败\n");
        return -1;
    }

     // 使用智能指针管理 SqlStatement 对象
    SqlStatement *stmt  =  new SqlStatement(&conn);
    //std::string sql = "delete from userinfo where id=1006";

    std::string sql = "   update userinfo set    password='xxsea'   where username='fayashi'    ";

    if(!stmt->query(sql))
    {
        printf("执行失败\n");
    }else
    {
        printf("执行成功\n");
        printf("影响条数为：%d\n",stmt->rc());
    }

}