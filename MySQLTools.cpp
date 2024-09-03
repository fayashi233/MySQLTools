#include "MySQLTools.h"
using namespace MYSQLTOOLS;

// make_unique
template<typename T, class ...Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


// 默认构造函数
Connection::Connection() : conn_(nullptr) {}

// 带参数的构造函数
Connection::Connection(const std::string &host, const std::string &user, const std::string &password, 
            const std::string &dbname, unsigned int port,const std::string &charset) 
            : conn_(nullptr)
{
    connect(host, user, password, dbname, port,charset);
    
}

// 析构函数
Connection::~Connection() {
    disconnect();
}

// 连接到数据库
bool Connection::connect(const std::string &host, const std::string &user, const std::string &password, 
                const std::string &dbname, unsigned int port,const std::string &charset)
{
    conn_ = mysql_init(nullptr);
    if (!conn_) {
        return false;  // MySQL 初始化失败
    }

    if (!mysql_real_connect(conn_, host.c_str(), user.c_str(), password.c_str(), 
                            dbname.c_str(), port, nullptr, 0)) {
        mysql_close(conn_);
        conn_ = nullptr;
        return false;  // MySQL 连接失败
    }
    set_charset(charset);
    return true;
}

// 断开数据库连接
void Connection::disconnect() {
    if (conn_) {
        mysql_close(conn_);
        conn_ = nullptr;
    }
}

// 检查是否已连接
bool Connection::is_connected() const {
    return conn_ != nullptr;
}

// 获取原始连接指针
MYSQL* Connection::get_connection() const {
    return conn_;
}

// 设置自动提交模式
bool Connection::set_autocommit(bool mode) {
    if (mysql_autocommit(get_connection(), mode ? 1 : 0)) {
        return false;  // 设置自动提交模式失败
    }
    return true;
}

// 提交事务
bool Connection::commit() {
    if (mysql_commit(get_connection())) {
        return false;  // 提交事务失败
    }
    return true;
}

// 回滚事务
bool Connection::rollback() {
    if (mysql_rollback(get_connection())) {
        return false;  // 回滚事务失败
    }
    return true;
}

unsigned int Connection::mysql_errno() const
{
    return ::mysql_errno(conn_);
}

const char* Connection::mysql_error() const
{
    return ::mysql_error(conn_);
}

void Connection::set_charset(const std::string &charset)
{
    mysql_set_character_set(conn_,charset.c_str());
}

// 去除字符串两端的空白字符
void trimInPlace(std::string& str) {
    auto start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        str.clear();  // 字符串全为空白字符
        return;
    }
    auto end = str.find_last_not_of(" \t\n\r\f\v");
    str.erase(0, start);
    str.erase(end - start + 1, std::string::npos);
}

// 去除字符串中间多余的空格，只保留单个空格
void removeExtraSpacesInPlace(std::string& str) {
    bool inSpace = false;
    std::string temp;
    temp.reserve(str.size());  // 预留空间以减少重新分配

    for (char ch : str) {
        if (std::isspace(ch)) {  // 判断是否为空白字符
            if (!inSpace) {  // 如果当前不是空格，则添加一个空格
                temp += ' ';
                inSpace = true;
            }
        } else {
            temp += ch;
            inSpace = false;
        }
    }

    str = temp;  // 将临时字符串赋值给原字符串
    trimInPlace(str);  // 去除两端的空格
}


// 构造函数
SqlStatement::SqlStatement(Connection *conn) : stmt_(nullptr), is_prepared_(false),rc_(0) {
    stmt_ = mysql_stmt_init(conn->get_connection());
    if (!stmt_) {
        printf("Failed to initialize statement\n");
    }
}

// 析构函数
SqlStatement::~SqlStatement() {
    if (stmt_) {
        mysql_stmt_close(stmt_);
        stmt_ = nullptr;
    }
}

// 准备SQL语句
bool SqlStatement::prepare(const std::string &query) {
    sql_ = query;
    removeExtraSpacesInPlace(sql_);
    if (!stmt_ || mysql_stmt_prepare(stmt_, sql_.c_str(), sql_.length())) {
        return false;  // 准备语句失败
    }

    // 初始化参数绑定
    unsigned long param_count = mysql_stmt_param_count(stmt_);
    bind_params_.resize(param_count);
    memset(bind_params_.data(), 0, sizeof(MYSQL_BIND) * param_count);

    // 初始化结果绑定
    MYSQL_RES *result_metadata = mysql_stmt_result_metadata(stmt_);
    if (result_metadata) {
        unsigned long field_count = mysql_num_fields(result_metadata);
        bind_results_.resize(field_count);
        result_buffer_lengths_.resize(field_count);
        
        memset(result_buffer_lengths_.data(), 0, sizeof(unsigned long) * field_count);
        memset(bind_results_.data(), 0, sizeof(MYSQL_BIND) * field_count);
        mysql_free_result(result_metadata);
    }
    is_prepared_ = true;

    //printf("%s",sql_.c_str());

    return true;
}

// 绑定输入参数（整型）
bool SqlStatement::bind_param(unsigned int index, int &value) {
    if (!check_prepared() || !check_param_index(index)) return false;

    MYSQL_BIND &bind = bind_params_[index];
    bind.buffer_type = MYSQL_TYPE_LONG;
    bind.buffer = &value;
    bind.is_null = 0;
    bind.length = 0;
    return true;
}


bool MYSQLTOOLS::SqlStatement::bind_param(unsigned int index, float &value)
{
    if (!check_prepared() || !check_param_index(index)) return false;

    MYSQL_BIND &bind = bind_params_[index];
    bind.buffer_type = MYSQL_TYPE_FLOAT;
    bind.buffer = &value;
    bind.is_null = 0;
    bind.length = 0;
    return true;
}

// 绑定输入参数（浮点型）
bool SqlStatement::bind_param(unsigned int index, double &value) {
    if (!check_prepared() || !check_param_index(index)) return false;

    MYSQL_BIND &bind = bind_params_[index];
    bind.buffer_type = MYSQL_TYPE_DOUBLE;
    bind.buffer = &value;
    bind.is_null = 0;
    bind.length = 0;
    return true;
}

bool SqlStatement::bind_param(unsigned int index, const char *value)
{
    if (!check_prepared() || !check_param_index(index)) return false;

    MYSQL_BIND &bind = bind_params_[index];
    bind.buffer_type = MYSQL_TYPE_STRING;
    bind.buffer = const_cast<char*>(value);
    bind.buffer_length = strlen(value);
    bind.is_null = 0;
    bind.length = 0;
    return true;
}

// 绑定输入参数（字符串）
bool SqlStatement::bind_param(unsigned int index, const std::string &value) {
    if (!check_prepared() || !check_param_index(index)) return false;

    MYSQL_BIND &bind = bind_params_[index];
    bind.buffer_type = MYSQL_TYPE_STRING;
    bind.buffer = const_cast<char*>(value.c_str());
    bind.buffer_length = value.length();
    bind.is_null = 0;
    bind.length = 0;
    return true;
}

// 绑定输出参数（整型）
bool SqlStatement::bind_result(unsigned int index, int &value) {
    if (!check_prepared() || !check_result_index(index)) return false;

    MYSQL_BIND &bind = bind_results_[index];
    bind.buffer_type = MYSQL_TYPE_LONG;
    bind.buffer = &value;
    bind.is_null = 0;
    bind.length = 0;
    return true;
}

bool MYSQLTOOLS::SqlStatement::bind_result(unsigned int index, float &value)
{
    if (!check_prepared() || !check_result_index(index)) return false;

    MYSQL_BIND &bind = bind_results_[index];
    bind.buffer_type = MYSQL_TYPE_FLOAT;
    bind.buffer = &value;
    bind.is_null = 0;
    bind.length = 0;
    return true;
}

// 绑定输出参数（浮点型）
bool SqlStatement::bind_result(unsigned int index, double &value) {
    if (!check_prepared() || !check_result_index(index)) return false;

    MYSQL_BIND &bind = bind_results_[index];
    bind.buffer_type = MYSQL_TYPE_DOUBLE;
    bind.buffer = &value;
    bind.is_null = 0;
    bind.length = 0;
    return true;
}

// 绑定输出参数（字符串）
bool SqlStatement::bind_result(unsigned int index, char *buffer, unsigned long buffer_length) {
    if (!check_prepared() || !check_result_index(index)) return false;

    // 存储 buffer_length 到类成员中
    result_buffer_lengths_[index] = buffer_length;

    MYSQL_BIND &bind = bind_results_[index];
    bind.buffer_type = MYSQL_TYPE_STRING;
    bind.buffer = buffer;
    bind.buffer_length = buffer_length;
    bind.is_null = 0;
    //bind.length = &buffer_length;
    bind.length = &result_buffer_lengths_[index]; // 绑定到成员变量
    return true;
}

bool SqlStatement::bind_result(unsigned int index, std::string& buffer, unsigned long buffer_length) {
    if (!check_prepared() || !check_result_index(index)) return false;
    // 存储 buffer_length 到类成员中
    result_buffer_lengths_[index] = buffer_length;

    buffer.resize(buffer_length);
    MYSQL_BIND &bind = bind_results_[index];
    bind.buffer_type = MYSQL_TYPE_STRING;
    bind.buffer = (void*)buffer.data();
    bind.buffer_length = buffer_length;
    bind.is_null = 0;
    //bind.length = &buffer_length;
    bind.length = &result_buffer_lengths_[index]; // 绑定到成员变量
    return true;
}



// 对于 SELECT 语句，在执行后获取的行数应使用 mysql_stmt_num_rows()，
// 但这个操作必须在绑定结果集（即 mysql_stmt_bind_result()）之后、获取数据之前进行。

// 执行语句
bool SqlStatement::execute() {
    if (!check_prepared()) return false;

    if (!bind_params_.empty() && mysql_stmt_bind_param(stmt_, bind_params_.data())) {
        return false;  // 绑定参数失败
    }

    if (mysql_stmt_execute(stmt_)) {
        printf("Execute error: %s\n", mysql_stmt_error(stmt_));
        return false;  // 执行语句失败
    }

    if (!bind_results_.empty() && mysql_stmt_bind_result(stmt_, bind_results_.data())) {
        return false;  // 绑定结果失败
    }

    // 统计影响行数
    if(is_SELECTsql()) {
        // 调用 mysql_stmt_store_result 来将结果集缓存在客户端
        if (mysql_stmt_store_result(stmt_)) {
            printf("Store result error: %s\n", mysql_stmt_error(stmt_));
            return false;
        }
        rc_ = mysql_stmt_num_rows(stmt_);  // 现在设置行数
    } else {
        rc_ = (unsigned long long)mysql_stmt_affected_rows(stmt_);
    }

    return true;
}

bool SqlStatement::query(const std::string &query)
{
     if (mysql_stmt_prepare(stmt_, query.c_str(), query.length())) {
        printf("Prepare error: %s\n", mysql_stmt_error(stmt_));
        return false;
    }

    // 执行语句
    if (mysql_stmt_execute(stmt_)) {
        printf("Execute error: %s\n", mysql_stmt_error(stmt_));
        return false;
    }

    // 统计影响行数
    if(is_SELECTsql()) {
        if (mysql_stmt_store_result(stmt_)) {
            printf("Store result error: %s\n", mysql_stmt_error(stmt_));
            return false;
        }
        rc_ = mysql_stmt_num_rows(stmt_);
    } else {
        rc_ = (unsigned long long)mysql_stmt_affected_rows(stmt_);
    }

    return true;
}

// 获取结果
int SqlStatement::fetch() {
    if (!check_prepared()) return -1;

    int status = mysql_stmt_fetch(stmt_);  

    if (status == 0) {
        return 1;  // 成功获取一行
    } else if (status == MYSQL_NO_DATA) {
        return 0;  // 没有更多数据
    } else {
        return -1;  // 获取数据失败
    }
}

// 获取影响数据的行数，如果是insert、update和delete，保存影响记录的行数，如果是select，保存结果集的行数。
unsigned long long SqlStatement::rc()
{
    return rc_;
}

const char* SqlStatement::sql() const
{
    return sql_.c_str();
}

unsigned int SqlStatement::mysql_errno() const
{
    return mysql_stmt_errno(stmt_);
}

const char *SqlStatement::mysql_error() const
{
    return mysql_stmt_error(stmt_);
}

// 检查语句是否已准备
inline bool SqlStatement::check_prepared() const {
    return is_prepared_;
}

// 检查参数索引
inline bool SqlStatement::check_param_index(unsigned int index) const {
    return index < bind_params_.size();
}

// 检查结果索引
inline bool SqlStatement::check_result_index(unsigned int index) const {
    return index < bind_results_.size();
}

bool SqlStatement::is_SELECTsql()
{
    if(sql_.size()<6) return false; 
    std::string tmp = sql_.substr(0,6);
    for(char &c:tmp)
    {
        c = std::toupper(c);
    }

    if(tmp=="SELECT") return true;
    else return false;
}
