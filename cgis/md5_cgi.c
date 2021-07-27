#include "fcgi_config.h"
#include "fcgi_stdio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "cJSON.h"
void returnCode(const char* code){
	char* out = NULL;
	cJSON* root = cJSON_CreateObject();
	cJSON_AddStringToObject(root,"code",code);
	out = cJSON_Print(root);
	printf(out);
	cJSON_Delete(root);
	free(out);
}
// 获取md5值，保存在output中
// 返回值：
// -1：获取失败
// 0：获取成功
int getFileInfo(char* buf, char* md5_, char* username_, char* filename_){
	cJSON* root = cJSON_Parse(buf);
	if(NULL == root){
		// root为NULL，不用delete
		return -1;	
	}
	cJSON* md5 = cJSON_GetObjectItem(root, "md5");
	if(NULL == md5){
		// 删除root指向的cJSON对象
		cJSON_Delete(root);
		root = NULL;
		return -1;
	}
	strcpy(md5_, md5->valuestring);
	cJSON* username = cJSON_GetObjectItem(root, "username");
	if(NULL == username){
		cJSON_Delete(root);
		root = NULL;
		return -1;
	}
	strcpy(username_, username->valuestring);
	cJSON* filename = cJSON_GetObjectItem(root, "filename");
	if(NULL == filename){
		cJSON_Delete(root);
		root = NULL;
		return -1;
	}
	strcpy(filename_, filename->valuestring);
	cJSON_Delete(root);
	root = NULL;
	return 0;
}
// 返回值
// -1：数据库中不存在相应的md5记录
// -2：操作错误
// 0：查询到md5值
int md5_query(char* md5, char* username, char* filename){
	// 连接数据库
	MYSQL* con = NULL;
	con = mysql_init(NULL);	// 初始化一个MYSQL对象
	if(NULL == con){
		return -2;
	}
	// 如果连接失败
	if(mysql_real_connect(con, NULL, "root", "null", "cloud_disk", 0, NULL, 0) == NULL){
		mysql_close(con);
		return -2;
	}
	// 设置utf-8编码
	mysql_query(con, "set names utf8");
	char sql[128];
	sprintf(sql, "select * from file_info1 where md5 = '%s'", md5);
	if(mysql_query(con, sql) != 0){
		mysql_close(con);
		return -2;
	}
	MYSQL_RES* res_set = mysql_store_result(con);	// 结果集
	if(NULL == res_set){
		mysql_close(con);
		return -2;
	}
	unsigned int line = 0;
	line = mysql_num_rows(res_set);
	if(0 == line){
		mysql_close(con);
		return -1;
	}
	// 文件已经存在，将file_info中的count + 1
	sprintf(sql, "update file_info1 set count=count+1 where md5 = '%s'", md5);
	if(mysql_query(con, sql) != 0){
		mysql_close(con);
		return -2;
	}
	// 这里存在一个问题：先执行将count+1的sql语句还是先执行表user_files的插入语句。如果先执行前者，但是表的插入失败，此时应该执行将count - 1的语句，但是count - 1的语句也失败。如果先执行后者，插入执行成功，但是count+1的sql语句执行失败，此时又该执行表的删除操作，但是如果删除也失败了。。。
	// 取出一行数据
	MYSQL_ROW row;
	row = mysql_fetch_row(res_set);
	if(NULL == row){
		mysql_close(con);
		return -2;
	}
	// 表file_info1结构
	// row0:md5 row1:count row2:type, row3:url	
	char type[40], url[128];
	strcpy(type, row[2]);
	strcpy(url, row[3]);
	sprintf(sql,"insert into user_files value('%s', '%s', '%s', '%s', '%s')", username, filename, url, md5, type);
	if(mysql_query(con, sql) != 0){
		mysql_close(con);
		return -2;
	}

	mysql_close(con);
	return 0;

}

// 001: 秒传成功
// 002: 服务器上没有该文件，需要进行真正的上传
// 003：其他错误
int main(){
	while(FCGI_Accept() >= 0){
		printf("Content-type: text/html\r\n\r\n");
		char* contentLength = getenv("CONTENT_LENGTH");
		if(NULL == contentLength){
			returnCode("003");
			continue;	
		}
		int len = 0; 
		len = atoi(contentLength);
		if(len <= 0){
			returnCode("003");
			continue;
		}
		char buf[4096];
		int res = fread(buf, 1, len, stdin);
		if(0 == res){
			returnCode("003");
			continue;
		}
		char md5[256] = {0};
		char username[128] = {0};
		char filename[128] = {0};
		res = getFileInfo(buf, md5, username, filename);
		if(-1 == res){
			returnCode("003");
			continue;
		}
		// 从数据库中查找md5值，判断文件是否已经在服务器上
		res = md5_query(md5, username, filename);
		if(-1 == res){
			returnCode("002");
			continue;
		}
		else if(-2 == res){
			returnCode("003");
			continue;
		}
		else{
			returnCode("001");
			continue;
		}


	}
	return 0;
}
