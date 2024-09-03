#include<mysql/mysql.h>
#include<iostream>
#include<string.h>
//增删改
int main()
{
    MYSQL *ConnectPointer=mysql_init(NULL);
	ConnectPointer=mysql_real_connect(ConnectPointer,"127.0.0.1","root","password","test",0,NULL,0);
	if (ConnectPointer){
   
		printf("connect database successfully\n");
	}else{
   
		printf("failed to connect database\n");
	}

    // //查
    // char sqlstr[50];
    // strcpy(sqlstr,"select username,password from userinfo");
    // if(mysql_query(ConnectPointer, sqlstr)==0)
    //     printf("查询成功\n");
    // else    
    //     printf("查询失败\n");
    
    // MYSQL_RES* res =  mysql_store_result(ConnectPointer);
    // printf("共查询到%d条数据:\n",mysql_num_rows(res));
    // while(MYSQL_ROW row = mysql_fetch_row(res))
    // {
    //     printf("username=%s,password=%s\n",row[0],row[1]);
    // }

    char sqlstr[100];
    strcpy(sqlstr,"insert into userinfo (username,password,lastupdate) values('fayashi','123456',CURRENT_TIMESTAMP)");

    //设置事务为手动提交
    mysql_autocommit(ConnectPointer,0);

    // //增
    // if(mysql_query(ConnectPointer, sqlstr)==0)
    // {
    //     printf("插入成功\n");
        
        
    //     printf("插入行数为%d行\n",mysql_affected_rows(ConnectPointer));
    //     mysql_commit(ConnectPointer);
    // }
    // else    
    // {
    //     printf("插入失败\n");
    //     mysql_rollback(ConnectPointer);
    // }

    // //删
    // memset(sqlstr,0,sizeof(sqlstr));
    // strcpy(sqlstr,"delete from userinfo where id='2'");
    // if(mysql_query(ConnectPointer, sqlstr)==0)
    // {
    //     printf("删除成功\n");
        
        
    //     printf("删除行数为%d行\n",mysql_affected_rows(ConnectPointer));
    //     mysql_commit(ConnectPointer);
    // }
    // else    
    // {
    //     printf("删除失败\n");
    //     mysql_rollback(ConnectPointer);
    // }

    //改
    memset(sqlstr,0,sizeof(sqlstr));
    strcpy(sqlstr,"UPDATE userinfo SET password='qwert' WHERE username='fayashi'");
    if(mysql_query(ConnectPointer, sqlstr)==0)
    {
        printf("修改成功\n");
        
        
        printf("修改行数为%d行\n",mysql_affected_rows(ConnectPointer));
        mysql_commit(ConnectPointer);
    }
    else    
    {
        printf("修改失败\n");
        mysql_rollback(ConnectPointer);
    }




	
    mysql_close(ConnectPointer);
	printf("database connection closed successfully\n");
	printf("\n");
	return 0;
}

// int mysql_query(MYSQL *mysql, const char *query);
// 参数:
//     - mysql: mysql_real_connect() 的返回值
//     - query: 一个可以执行的sql语句, 结尾的位置不需要加 ;
// 返回值: 
//     - 如果查询成功，返回0。如果是查询, 结果集在mysql 对象中
//     - 如果出现错误，返回非0值。 

// 将结果集从 mysql(参数) 对象中取出
// MYSQL_RES 对应一块内存, 里边保存着这个查询之后得到的结果集
// 如何将行和列的数据从结果集中取出, 需要使用其他函数
// 返回值: 具有多个结果的MYSQL_RES结果集合。如果出现错误，返回NULL。 
//MYSQL_RES *mysql_store_result(MYSQL *mysql);

// 从结果集中列的个数
// 参数: 调用 mysql_store_result() 得到的返回值
// 返回值: 结果集中的列数
//unsigned int mysql_num_fields(MYSQL_RES *result)

// 获取表头 -> 字段名
// 参数: 调用 mysql_store_result() 得到的返回值
// 返回值: MYSQL_FIELD* 指向一个结构体
// 通过查询官方文档, 返回是一个结构体的数组
// 通过这个函数得到结果集中所有列的名字
//MYSQL_FIELD *mysql_fetch_fields(MYSQL_RES *result);

