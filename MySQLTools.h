#ifndef MYSQLTOOLS_H
#define MYSQLTOOLS_H

#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <iostream>
#include <cctype>
#include <ctime>

namespace MYSQLTOOLS
{

class Connection
{
private:
    MYSQL *conn_;
    // 禁止拷贝和赋值操作
    Connection(const Connection &) = delete;
    Connection &operator=(const Connection &) = delete;

public:
    // 默认构造函数
    Connection();
    // 带参数的构造函数
    Connection(const std::string &host, const std::string &user, const std::string &password,
                const std::string &dbname, unsigned int port = 3306,const std::string &charset="utf8");
    // 析构函数
    ~Connection();
    // 连接到数据库
    bool connect(const std::string &host, const std::string &user, const std::string &password,
                    const std::string &dbname, unsigned int port = 3306,const std::string &charset="utf8");
    // 断开数据库连接
    void disconnect();
    // 检查是否已连接
    bool is_connected() const;
    // 获取原始连接指针
    MYSQL *get_connection() const;
    // 设置自动提交模式
    bool set_autocommit(bool mode);
    // 提交事务
    bool commit();
    // 回滚事务
    bool rollback();
    // 返回错误编号
    unsigned int mysql_errno() const;
    // 返回错误信息
    const char* mysql_error() const;
    // 设置字符集
    void set_charset(const std::string &charset);
    
};

class SqlStatement
{
private:
    MYSQL_STMT *stmt_;
    std::vector<MYSQL_BIND> bind_params_;
    std::vector<MYSQL_BIND> bind_results_;
    std::vector<unsigned long> result_buffer_lengths_;
    bool is_prepared_;
    unsigned long long rc_;  // 上一次执行sql语句受影响的行数或返回结果集的数量
    std::string sql_;

    // 禁止拷贝和赋值操作
    SqlStatement(const SqlStatement &) = delete;
    SqlStatement &operator=(const SqlStatement &) = delete;

public:
    // 构造函数
    SqlStatement(Connection *conn);
    // 析构函数
    ~SqlStatement();
    // 准备SQL语句
    bool prepare(const std::string &query);
    // 绑定输入参数
    bool bind_param(unsigned int index, int &value);
    bool bind_param(unsigned int index, float &value);
    bool bind_param(unsigned int index, double &value);
    bool bind_param(unsigned int index, const char* value);
    bool bind_param(unsigned int index, const std::string &value);

    // 绑定输出参数
    bool bind_result(unsigned int index, int &value);
    bool bind_result(unsigned int index, float &value);
    bool bind_result(unsigned int index, double &value);
    bool bind_result(unsigned int index, char *buffer, unsigned long buffer_length = 2000);
    bool bind_result(unsigned int index, std::string &buffer, unsigned long buffer_length = 2000);
    // 执行语句
    bool execute();
    // 执行sql语句
    bool query(const std::string &query);
    // 获取结果
    int fetch();
    // 获取影响数据的行数，如果是insert、update和delete，保存影响记录的行数，如果是select，保存结果集的行数。
    unsigned long long rc();
    // 返回sql语句
    const char* sql() const;
    // 返回错误编号
    unsigned int mysql_errno() const;
    // 返回错误信息
    const char* mysql_error() const;

private:
    // 检查语句是否已准备
    inline bool check_prepared() const;
    // 检查参数索引
    inline bool check_param_index(unsigned int index) const;
    // 检查结果索引
    inline bool check_result_index(unsigned int index) const;
public:
    // 判断是否是查询语句，如果是，返回true
    bool is_SELECTsql();


};

} // namespace MYSQLTOOLS

#endif // MYSQLTOOLS_H
