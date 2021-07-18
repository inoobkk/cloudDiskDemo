#include <mysql/mysql.h>
#include "fcgi_config.h"
#include "fcgi_stdio.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 接收客户端发送的json数据
// 解析json格式数据，提取用户名和密码
// 查询数据库
// 返回状态码
void returnCode(const char* status);
int user_login(char* client_data);
int query_mysql(char* username, char* password);
void returnCode(const char* status);
int main(){
	while(FCGI_Accept() >= 0){
		char* contentLength = getenv("CONTENT_LENGTH");
		// 设置响应消息内容格式
		printf("Content-type: text/html\r\n\r\n");
		if(NULL == contentLength){
			returnCode("004");
			continue;
		}
		int len = 0;
		len = atoi(contentLength);	// char*->int
		if(len<=0){
			returnCode("004");
			continue;
		}
		// 读取用户信息
		char buf[2048] = {0};
		int ret = 0;
		ret = fread(buf, 1, len, stdin); //从标准输入读取客户端传给服务器的数据，应该涉及到输入重定向
		if(0==ret){
			returnCode("004");
			continue;
		}
		ret = user_login(buf);
		if(1 == ret){
			returnCode("001");
		}
		else if(2 == ret){
			returnCode("002");
		}
		else if(3 == ret){
			returnCode("003");
		}
		else{
			returnCode("004");
		}
	}

	return 0;
}
// 解析json格式的字符串，提取用户名和密码
// 查询数据库
int user_login(char* client_data){
	cJSON* root = cJSON_Parse(client_data);
	if(NULL == root) return 4;
	cJSON* username = cJSON_GetObjectItem(root,"username");
	if(NULL == username) return 4;
	cJSON* password = cJSON_GetObjectItem(root,"password");
	if(NULL == password) return 4;
	char username_[128];
	char password_[128];
	strcpy(username_,username->valuestring);
	strcpy(password_,password->valuestring);
	// 查询数据库
	int ret = 0;
	ret = query_mysql(username_, password_);
	return ret;
}
// 返回值：
// 1 查询到该用户，密码匹配
// 2 查询到该用户，密码不匹配
// 3 未查询到该用户
// 4 其他错误
int query_mysql(char* username, char* password){
	MYSQL* con = NULL;
	con = mysql_init(NULL);
	
	if(mysql_real_connect(con, NULL, "root", "null", "cloud_disk", 0, NULL, 0) == NULL){
		return 4;
	}	
	// 设置utf-8编码
	mysql_query(con, "set names utf8");
	char sql[256] = {0};
	sprintf(sql, "select * from user1 where username = '%s'", username);
	// 查询失败
	if(mysql_query(con, sql) != 0){
		mysql_close(con);
		return 4;
	}
	// 获取记录条数
	MYSQL_RES* res = NULL;
	res = mysql_store_result(con);
	if(NULL == res){
		mysql_close(con);
		return 4;
	}
	MYSQL_ROW row;
	unsigned int num;
	num = mysql_num_rows(res);
	if(0 == num){
		mysql_close(con);
		return 3;
	}
	sprintf(sql, "select * from user1 where username = '%s' and password = '%s'", username, password);
	if(mysql_query(con, sql) != 0){
		mysql_close(con);
		return 4;
	}
	res = mysql_store_result(con);
	if(NULL == res){
		mysql_close(con);
		return 4;
	}
	num = mysql_num_rows(res);
	if(0 == num){
		mysql_close(con);
		return 2;	// 密码不匹配
	}
	else{
		mysql_close(con);
		return 1;
	}
	return 0;
}

void returnCode(const char* status){
	char* out = NULL;
	cJSON* root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "code", status);
	out = cJSON_Print(root);	// json->char*
	printf(out);
	cJSON_Delete(root);
	free(out);
}
