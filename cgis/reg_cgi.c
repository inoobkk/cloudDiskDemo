#include <mysql/mysql.h>
#include "fcgi_config.h"
#include "fcgi_stdio.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int user_register(char* buf);
void returnCode(const char*);
int  main(){
	while(FCGI_Accept() >= 0){
		char* contentLength = getenv("CONTENT_LENGTH");

		int len;
		// 设置响应消息内容格式
		printf("Content-type: text/html\r\n\r\n");
		if(NULL == contentLength){
			returnCode("004");
			continue;
		}
		//printf("ok");
		//return 0;
		len = atoi(contentLength);
		if(len <= 0){
			returnCode("004");
			continue;
		}
		char buf[4096] = {0};
		int ret = 0;
		// 从标准输入读取用户信息
		// 为什么cgi是从标准输入读取数据？
		// 如果是在注册的同时服务器还在处理登录请求呢，登录请求的信息会不会也是标准输入的一部分，还是说注册的时候是将信息提交到reg模块，登录时提交的信息与注册时的信息不影响？
		ret = fread(buf, 1, len, stdin);
		//printf("ok"); return 0;
		if(0 == ret){
			returnCode("004");
			continue;
		}
		ret = user_register(buf);
		// printf("ok"); return 0;

		if(0 == ret) returnCode("002");
		if(-2 == ret) returnCode("003");
		if(-1 == ret) returnCode("004");
	}
	return 0;
}

// 注册用户
// 1.解析json字符串
// 2.查询用户是否存在
// 3.使用md5对密码进行加密
// 4.插入数据库
// 5.返回状态码
int user_register(char* buf){
	// 解析json字符串
	cJSON* root = cJSON_Parse(buf);
	if(NULL == root) return -1;
	cJSON* username = cJSON_GetObjectItem(root,"username");
	if(NULL == username) return -1;
	cJSON* password = cJSON_GetObjectItem(root,"password");
	if(NULL == password) return -1;
	cJSON* email = cJSON_GetObjectItem(root, "email");
	if(NULL == email) return -1;

	char username_[128];
	char password_[128];
	char email_[128];
	strcpy(username_, username->valuestring);
	strcpy(password_, password->valuestring);
	strcpy(email_, email->valuestring);
	// mysql查询与插入
	MYSQL* con = NULL;
	con = mysql_init(NULL);
	if(mysql_real_connect(con, NULL, "root", "null", "cloud_disk", 0, NULL, 0) == NULL){
		printf("mysql error");
		//mysql_close(con);
		return -1;
	}
	mysql_query(con, "set names utf8");	//设置中文编码
	char sql[256] = {0};
	sprintf(sql, "select * from user1 where username = '%s'", username_);
	if(mysql_query(con, sql) != 0){
		mysql_close(con);
		return -1;
	}
	MYSQL_RES* res_ret = NULL;
	res_ret = mysql_store_result(con);
	if(NULL == res_ret){
		mysql_close(con);
		return -1;
	}
	// 获取有多少条记录
	MYSQL_ROW row;
	unsigned int line = 0;
	line = mysql_num_rows(res_ret);
	if(0 == line) {
		// 对密码进行md5加密, 注：客户端完成
		sprintf(sql, "insert into user1 values('%s', '%s', '%s')", username_, password_, email_);
		if(mysql_query(con, sql) != 0){
			mysql_close(con);
			return -1;
		}
		mysql_close(con);
		return 0;
	}
	else{	// 用户已经存在，不允许注册
		
		mysql_close(con);
		return -2;
	}
}

void returnCode(const char* buf){
	char* out = NULL;
	cJSON* root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "code", buf);
	out = cJSON_Print(root);  // json->char*
	printf(out);
	cJSON_Delete(root);
	free(out);
}
