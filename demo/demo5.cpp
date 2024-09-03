#include "../MySQLTools.h"
#include "cstdio"
#include <memory>
#include <vector>

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

    std::string sql = "    select id,username,password from     userinfo where username=?    ";

    std::string i_username = "fayashi";

    int o_id;
    char o_username[100]; // 用来存储输出的用户名
    char o_password[100]; // 用来存储输出的密码

    if (!stmt->prepare(sql)) {
        printf("准备语句失败\n");
        return -1;
    }

    printf("准备语句：%s\n",stmt->sql());
    if(stmt->is_SELECTsql()) printf("select\n");
    else 
        printf("notselect\n");


    // 绑定输入参数
    if (!stmt->bind_param(0, i_username)) {
        printf("绑定输入参数失败\n");
        return -1;
    }

    // 绑定输出参数
    if (!stmt->bind_result(0, o_id)) {
        printf("绑定输出参数失败\n");
        return -1;
    }
    if (!stmt->bind_result(1, o_username, sizeof(o_username))) {
        printf("绑定输出参数失败\n");
        return -1;
    }
    if (!stmt->bind_result(2, o_password, sizeof(o_password))) {
        printf("绑定输出参数失败\n");
        return -1;
    }

    // 执行
    if (stmt->execute()) {
        printf("执行成功,影响行数: %llu\n", stmt->rc());
    } else {
        printf("执行失败\n");
        return -1;
    }

    // 获取结果
    while (stmt->fetch() > 0) {  // 检查 fetch 的返回值
        printf("%d %s %s\n", o_id, o_username, o_password);
    }


    return 0;
}
