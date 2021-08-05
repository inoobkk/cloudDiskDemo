

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "fcgi_stdio.h"
#include "cJSON.h"
#include <mysql/mysql.h>
#include <ctype.h>
int recv_file(long len, char* user, char* filename, char* md5, long* size);
void returnCode(const char* status);
int trim_space(char *inbuf);
void getFileType(char* filename, char* type);
int storage_fileinfo_to_mysql(char* user, char* filename, char* md5, long size, char* fileid);
int upload_file_to_storage(char* filename, char* fileid);
// code: 001 成功
// code: 002 失败
int main() {

	char filename[128] = { 0 };
	char user[128] = { 0 };
	char md5[128] = { 0 };
	long size;	// 文件大小
	char fileid[128] = { 0 };	// 文件上传到fastDFS后的文件id
	char fdfs_file_url = { 0 };	// storage的host_name
	while (FCGI_Accept() >= 0) {
		char *contentLength = getenv("CONTENT_LENGTH");
		long len;
		int ret = 0;
		printf("Content-type: text/html\r\n\r\n");
		if (contentLength != NULL) {
			len = strtol(contentLength, NULL, 10);
		}
		else {
			len = 0;
		}
		if (len <= 0) {
			printf("no data is posted\r\n");
			returnCode("002");
		}
		else {
			if (recv_file(len, user, filename, md5, &size) == -1) {
				returnCode("002");

			}
			else {
				if (upload_file_to_storage(filename, fileid) == -1) {
					printf("failed to upload file to storage\n");
					returnCode("002");
				}
				else {
					unlink(filename);
					ret = storage_fileinfo_to_mysql(user, filename, md5, size, fileid);
					if (-1 == ret) {
						returnCode("002");
					}
					else {
						returnCode("001");
					}
				}
			}
		}
	}
	
	return 0;
}

// 将上传的文件作为临时文件保存在当前的目录
// -1 出错 0 成功
// 思考，这里上传文件是用的从标准输入输出重定位，是否可以支持大文件的上传？
int recv_file(long len, char* user, char* filename, char* md5, long* size ) {
	int ret = 0;
	char boundary[128] = { 0 };	// 保存边界

	char* file_buf = (char*)malloc(len);
	if (NULL == file_buf) {
		return -1;
	}
	ret = fread(file_buf, 1, len, stdin);	// 从标准输入读取数据
	if (0 == ret) {
		return -1;
	}
	/*
	upload data:  "------WebKitFormBoundaryN1Rt@yMU\\?qm<6DS\r\n
	Content-Disposition: form-data; user=\"1\"; filename=\"1.txt\"; 
	md5=\"1f09ecbd362fa0dfff88d4788e6f5df0\"; size=19\r\n
	Content-Type: application/octet-stream\r\n\r\n
	this is a text file\r\n
	------WebKitFormBoundaryN1Rt@yMU\\?qm<6DS" 
	*/
	char* begin = file_buf;	// 起点
	char* p = begin;
	//===========获取边界线===========
	p = strstr(begin, "\r\n");	// 找到第一个边界线
	if (NULL == p) {
		return -1;
	}
	strncpy(boundary, begin, p - begin);	// 保存边界线
	boundary[p - begin] = '\0';
	
	//===========获取用户名===========
	p += 2;	// p指向下一行
	len -= (p - begin);
	char* q = strstr(begin, "user=");
	q += strlen("user=");
	q++;	// 此时q指向username的第一个字符
	char* k = strchr(q, '"');	// 用户名结尾位置
	strncpy(user, q, k - q);	// 拷贝用户名
	user[k - q] = '\0';

	//===========获取文件名===========
	begin = k;
	q = strstr(begin, "filename=");
	q += strlen("filename=");
	q++;
	k = strchr(q, '"');
	strncpy(filename, q, k - q);
	filename[k - q] = '\0';

	//===========获取md5===========
	begin = k;
	q = begin;
	q = strstr(begin, "md5=");
	q += strlen("md5=");
	q++;
	k = strchr(q, '"');
	strncpy(md5, q, k - q);	// 拷贝MD5
	md5[k - q] = '\0';

	//===========获取文件大小===========
	begin = k;
	q = begin;
	q = strstr(begin, "size=");
	q += strlen("size=");
	k = strstr(q, "\r\n");	// size数字两遍没有双引号
	char tmp[256] = { 0 };	// 先保存问字符串
	strncpy(tmp, q, k - q);
	tmp[k - q] = '\0';
	*size = strtol(tmp, NULL, 10);	// 字符串转long
	k += 2;
	begin = k;
	p = strstr(begin, "\r\n");
	p += 4;	// 越过\r\n\r\n

	//===========获取文件内容===========
	begin = p;
	p = strstr(p, boundary);
	p -= 2;
	if (NULL == p) return -1;
	int fd = 0;
	fd = open(filename, O_CREAT | O_WRONLY, 0644);
	if (fd < 0) {
		return -1;
	}
	ftruncate(fd, (p - begin));
	write(fd, begin, (p - begin));
	close(fd);
	free(file_buf);
	return 0;
}

// 将临时文件上传到storage，获得fileid
// -1 失败 0 成功

int upload_file_to_storage(char* filename, char* fileid) {
	int ret = 0;
	pid_t pid;
	int fd[2];
	// 创建管道，用于进程通信
	if (pipe(fd) < 0) {
		return -1;
	}
	// 创建子进程
	pid = fork();
	if (pid < 0) {
		return -1;
	}
	if (0 == pid) {		// 子进程
		// 关闭读端
		close(fd[0]);
		// 将标准输出重定向到写通道, 为什么这么做？因为fdfs_upload_file会输出一段代表文件在storage中的位置的字符串
		dup2(fd[1], STDOUT_FILENO);

		// 将路径直接写在这里并不好，如果路径发生变化，需要找到这里进行修改，可能到时候就忘了是在这里用的路径名
		char fdfs_cli_conf_path[256] = { 0 };
		sprintf(fdfs_cli_conf_path, "/etc/fdfs/client.conf");
		// 通过execlp执行fdfs_upload_file
		execlp("fdfs_upload_file", "fdfs_upload_file", fdfs_cli_conf_path, filename, NULL);
		// 如果执行失败
		close(fd[1]);
	}
	else {	// 父进程
		close(fd[1]);
		read(fd[0], fileid, 128);
		// 去掉字符串两边的空白字符
		trim_space(fileid);
		if (strlen(fileid) == 0) {
			printf("failed to get fileid\n");
			return -1;
		}
		wait(NULL);		// 等待子进程结束，回收资源
		close(fd[0]);
	}
	return 0;
}
void returnCode(const char* status) {
	char* out = NULL;
	cJSON* root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "code", status);
	out = cJSON_Print(root);
	printf(out);
	cJSON_Delete(root);
	free(out);
}
int storage_fileinfo_to_mysql(char* user, char* filename, char* md5, long size, char* fileid) {
	MYSQL* con = NULL;
	char sql[128] = { 0 };
	// 数据库连接的函数应该封装一下，很多地方都会用到
	con = mysql_init(NULL);
	if (mysql_real_connect(con, NULL, "root", "null", "cloud_disk", 0, NULL, 0) == NULL) {
		printf("mysql connecting error");
		return -1;
	}
	mysql_query(con, "set names utf8");
	char type[128] = { 0 };
	char type_cpy[128] = { 0 };
	
	getFileType(filename, type);
	// 这里如果不保存一份type的话不知道为什么在第二次sql语句中type的内容会改变
	strcpy(type_cpy, type);
	// 将文件信息插入file_info1表中
	
	sprintf(sql, "insert into file_info1 values('%s', '%d', '%s', 'null', '%s', '%ld')", md5, 1, type, fileid, size);

	if (mysql_query(con, sql) != 0) {
		mysql_close(con);
		printf("failed to insert into file_info1\n");
		printf("sql: %s", sql);
		return -1;
	}
	// 将文件插入用户的文件列表user_file_info

	// 这里有bug，应该先将sql清空
	char sql1[128] = { 0 };
	printf("type: %s", type_cpy);
	sprintf(sql1, "insert into user_files values('%s', '%s', 'www.baidu.com', '%s', '%s')", user, filename, md5, type_cpy);
	

	if (mysql_query(con, sql1) != 0) {
		mysql_close(con);
		printf("faled to insert into user_files\n");
		printf("sql: %s", sql1);
		
		return -1;
	}
	mysql_close(con);
	return 0;
}	
int trim_space(char *inbuf)
{
	int i = 0;
	int j = strlen(inbuf) - 1;

	char *str = inbuf;

	int count = 0;

	if (str == NULL)
	{
		//LOG(UTIL_LOG_MODULE, UTIL_LOG_PROC, "inbuf   == NULL\n");
		return -1;
	}


	while (isspace(str[i]) && str[i] != '\0')
	{
		i++;
	}

	while (isspace(str[j]) && j > i)
	{
		j--;
	}

	count = j - i + 1;

	strncpy(inbuf, str + i, count);

	inbuf[count] = '\0';

	return 0;
}
void getFileType(char* filename, char* type) {
	char* p = filename;
	for (; *p != '.'; ++p);
	int i = 0;
	++p;
	for (; *p != '\0'; ++p) {
		type[i++] = *p;
	}
	type[i] = '\0';
}