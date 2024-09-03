#include <mysql/mysql.h>
#include <iostream>
#include <cstring>

int main() {
    MYSQL *conn;
    MYSQL_STMT *stmt;
    MYSQL_BIND input_bind[1];  // 用于输入参数绑定
    MYSQL_BIND result_bind[2]; // 用于输出结果绑定
    MYSQL_RES *prepare_meta_result;
    int id = 5;                // 输入的ID参数
    char username[50];
    char password[50];
    unsigned long length[2];
    my_bool is_null[2];
    my_bool error[2];

    // 初始化MySQL连接句柄
    conn = mysql_init(NULL);
    if (conn == NULL) {
        std::cerr << "mysql_init() failed\n";
        return EXIT_FAILURE;
    }

    // 连接到数据库
    if (mysql_real_connect(conn, "localhost", "root", "password", "test", 0, NULL, 0) == NULL) {
        std::cerr << "mysql_real_connect() failed\n";
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // SQL语句，使用?作为占位符
    const char *sql = "SELECT username, password FROM userinfo WHERE id = ?";

    // 初始化预处理语句句柄
    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "mysql_stmt_init() failed\n";
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 准备SQL语句
    if (mysql_stmt_prepare(stmt, sql, strlen(sql)) != 0) {
        std::cerr << "mysql_stmt_prepare() failed\n";
        std::cerr << "Error: " << mysql_stmt_error(stmt) << "\n";
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 绑定输入参数
    memset(input_bind, 0, sizeof(input_bind));
    input_bind[0].buffer_type = MYSQL_TYPE_LONG;  // 对应C++中的int类型
    input_bind[0].buffer = &id;  // 传递指向整数变量的指针
    input_bind[0].is_null = 0;
    input_bind[0].length = 0;

    if (mysql_stmt_bind_param(stmt, input_bind) != 0) {
        std::cerr << "mysql_stmt_bind_param() failed\n";
        std::cerr << "Error: " << mysql_stmt_error(stmt) << "\n";
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 执行预处理语句
    if (mysql_stmt_execute(stmt) != 0) {
        std::cerr << "mysql_stmt_execute() failed\n";
        std::cerr << "Error: " << mysql_stmt_error(stmt) << "\n";
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 准备获取结果
    memset(result_bind, 0, sizeof(result_bind));

    // 绑定username列
    result_bind[0].buffer_type = MYSQL_TYPE_STRING;
    result_bind[0].buffer = username;
    result_bind[0].buffer_length = sizeof(username);
    result_bind[0].is_null = &is_null[0];
    result_bind[0].length = &length[0];
    result_bind[0].error = &error[0];

    // 绑定password列
    result_bind[1].buffer_type = MYSQL_TYPE_STRING;
    result_bind[1].buffer = password;
    result_bind[1].buffer_length = sizeof(password);
    result_bind[1].is_null = &is_null[1];
    result_bind[1].length = &length[1];
    result_bind[1].error = &error[1];

    // 绑定输出结果
    if (mysql_stmt_bind_result(stmt, result_bind)) {
        std::cerr << "mysql_stmt_bind_result() failed\n";
        std::cerr << "Error: " << mysql_stmt_error(stmt) << "\n";
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 获取查询结果
    while (mysql_stmt_fetch(stmt) == 0) {
        if (!is_null[0]) {
            std::cout << "Username: " << username << "\n";
        } else {
            std::cout << "Username is NULL\n";
        }

        if (!is_null[1]) {
            std::cout << "Password: " << password << "\n";
        } else {
            std::cout << "Password is NULL\n";
        }
    }

    // 清理并关闭句柄
    mysql_stmt_close(stmt);
    mysql_close(conn);

    return EXIT_SUCCESS;
}
