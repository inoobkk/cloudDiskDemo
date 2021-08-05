

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
// code: 001 �ɹ�
// code: 002 ʧ��
int main() {

	char filename[128] = { 0 };
	char user[128] = { 0 };
	char md5[128] = { 0 };
	long size;	// �ļ���С
	char fileid[128] = { 0 };	// �ļ��ϴ���fastDFS����ļ�id
	char fdfs_file_url = { 0 };	// storage��host_name
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

// ���ϴ����ļ���Ϊ��ʱ�ļ������ڵ�ǰ��Ŀ¼
// -1 ���� 0 �ɹ�
// ˼���������ϴ��ļ����õĴӱ�׼��������ض�λ���Ƿ����֧�ִ��ļ����ϴ���
int recv_file(long len, char* user, char* filename, char* md5, long* size ) {
	int ret = 0;
	char boundary[128] = { 0 };	// ����߽�

	char* file_buf = (char*)malloc(len);
	if (NULL == file_buf) {
		return -1;
	}
	ret = fread(file_buf, 1, len, stdin);	// �ӱ�׼�����ȡ����
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
	char* begin = file_buf;	// ���
	char* p = begin;
	//===========��ȡ�߽���===========
	p = strstr(begin, "\r\n");	// �ҵ���һ���߽���
	if (NULL == p) {
		return -1;
	}
	strncpy(boundary, begin, p - begin);	// ����߽���
	boundary[p - begin] = '\0';
	
	//===========��ȡ�û���===========
	p += 2;	// pָ����һ��
	len -= (p - begin);
	char* q = strstr(begin, "user=");
	q += strlen("user=");
	q++;	// ��ʱqָ��username�ĵ�һ���ַ�
	char* k = strchr(q, '"');	// �û�����βλ��
	strncpy(user, q, k - q);	// �����û���
	user[k - q] = '\0';

	//===========��ȡ�ļ���===========
	begin = k;
	q = strstr(begin, "filename=");
	q += strlen("filename=");
	q++;
	k = strchr(q, '"');
	strncpy(filename, q, k - q);
	filename[k - q] = '\0';

	//===========��ȡmd5===========
	begin = k;
	q = begin;
	q = strstr(begin, "md5=");
	q += strlen("md5=");
	q++;
	k = strchr(q, '"');
	strncpy(md5, q, k - q);	// ����MD5
	md5[k - q] = '\0';

	//===========��ȡ�ļ���С===========
	begin = k;
	q = begin;
	q = strstr(begin, "size=");
	q += strlen("size=");
	k = strstr(q, "\r\n");	// size��������û��˫����
	char tmp[256] = { 0 };	// �ȱ������ַ���
	strncpy(tmp, q, k - q);
	tmp[k - q] = '\0';
	*size = strtol(tmp, NULL, 10);	// �ַ���תlong
	k += 2;
	begin = k;
	p = strstr(begin, "\r\n");
	p += 4;	// Խ��\r\n\r\n

	//===========��ȡ�ļ�����===========
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

// ����ʱ�ļ��ϴ���storage�����fileid
// -1 ʧ�� 0 �ɹ�

int upload_file_to_storage(char* filename, char* fileid) {
	int ret = 0;
	pid_t pid;
	int fd[2];
	// �����ܵ������ڽ���ͨ��
	if (pipe(fd) < 0) {
		return -1;
	}
	// �����ӽ���
	pid = fork();
	if (pid < 0) {
		return -1;
	}
	if (0 == pid) {		// �ӽ���
		// �رն���
		close(fd[0]);
		// ����׼����ض���дͨ��, Ϊʲô��ô������Ϊfdfs_upload_file�����һ�δ����ļ���storage�е�λ�õ��ַ���
		dup2(fd[1], STDOUT_FILENO);

		// ��·��ֱ��д�����ﲢ���ã����·�������仯����Ҫ�ҵ���������޸ģ����ܵ�ʱ����������������õ�·����
		char fdfs_cli_conf_path[256] = { 0 };
		sprintf(fdfs_cli_conf_path, "/etc/fdfs/client.conf");
		// ͨ��execlpִ��fdfs_upload_file
		execlp("fdfs_upload_file", "fdfs_upload_file", fdfs_cli_conf_path, filename, NULL);
		// ���ִ��ʧ��
		close(fd[1]);
	}
	else {	// ������
		close(fd[1]);
		read(fd[0], fileid, 128);
		// ȥ���ַ������ߵĿհ��ַ�
		trim_space(fileid);
		if (strlen(fileid) == 0) {
			printf("failed to get fileid\n");
			return -1;
		}
		wait(NULL);		// �ȴ��ӽ��̽�����������Դ
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
	// ���ݿ����ӵĺ���Ӧ�÷�װһ�£��ܶ�ط������õ�
	con = mysql_init(NULL);
	if (mysql_real_connect(con, NULL, "root", "null", "cloud_disk", 0, NULL, 0) == NULL) {
		printf("mysql connecting error");
		return -1;
	}
	mysql_query(con, "set names utf8");
	char type[128] = { 0 };
	char type_cpy[128] = { 0 };
	
	getFileType(filename, type);
	// �������������һ��type�Ļ���֪��Ϊʲô�ڵڶ���sql�����type�����ݻ�ı�
	strcpy(type_cpy, type);
	// ���ļ���Ϣ����file_info1����
	
	sprintf(sql, "insert into file_info1 values('%s', '%d', '%s', 'null', '%s', '%ld')", md5, 1, type, fileid, size);

	if (mysql_query(con, sql) != 0) {
		mysql_close(con);
		printf("failed to insert into file_info1\n");
		printf("sql: %s", sql);
		return -1;
	}
	// ���ļ������û����ļ��б�user_file_info

	// ������bug��Ӧ���Ƚ�sql���
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