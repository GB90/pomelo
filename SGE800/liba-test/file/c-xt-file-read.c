/*****************************************************************************/
/*��̵����ɷ����޹�˾                                     ��Ȩ��2008-2015   */
/*****************************************************************************/
/* ��Դ���뼰������ĵ�Ϊ���������̵����ɷ����޹�˾�������У�δ��������    */
/* �ɲ��������޸Ļ򷢲�������׷����صķ������Ρ�                          */
/*                                                                           */
/*                      ���������̹ɷ����޹�˾                             */
/*                      www.xjgc.com                                         */
/*                      (0374) 321 2924                                      */
/*                                                                           */
/*****************************************************************************/


/******************************************************************************
    ��Ŀ����    ��  SGE800���������ն�ƽ̨
    �ļ���         ��  c-tx-file-read.c
    ����	       ��  ���ļ�����ƽ̨��file_read�����Ĳ���
    �汾              ��  0.1
    ����              ��  ����
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>			//exit
#include <unistd.h>			//sleep
#include <db.h>
#include <string.h>

//ƽ̨��ͷ�ļ�
#include "../include/file.h"
#include "../include/xj_assert.h"
#include "../include/error.h"
#include "../include/thread.h"

void thread_test1()
{
	int ret;
	char *name = "file1";
	u8 pos,mode;
	int fd;

	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	fd = file_open(name,pos,mode);
	if(fd<=0){
		printf("file opern error!\n");
	}
	char buf[10];
	memset(buf,0,10);

	ret = file_read(fd, buf, 10);
	assert((ret>0) && (strcmp(buf,"hello")==0),"file read error!");
	while(1){

	}

}

void thread_test2()
{
	int ret;
	char *name = "file1";
	u8 pos,mode;
	int fd;

	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	fd = file_open(name,pos,mode);
	if(fd<=0){
		printf("file opern error!\n");
	}
	char buf[10];
	memset(buf,0,10);

	ret = file_read(fd, buf, 10);
	assert((ret>0) && (strcmp(buf,"hello")==0),"file read error!");

	while(1){

	}
}

void thread_test3()
{
	int ret;
	char *name = "file1";
	u8 pos,mode;
	int fd;

	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	fd = file_open(name,pos,mode);
	if(fd<=0){
		printf("file opern error!\n");
	}
	char buf[10];
	memset(buf,0,10);
    ret = file_read(fd, buf, 10);
	assert((ret>0) && (strcmp(buf,"hello")==0),"file read error!");

	while(1){

	}
}

int main()
{
	int ret;
	//�����ĳ�ʼ��
	inittest();
	system("rm /var/fi*");
	char *name = "file1";
	u8 pos,mode;
	int fd;

	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	fd = file_open(name,pos,mode);
	if(fd<=0){
		printf("file opern error!\n");
		goto error;
	}
	//���Ի���Ҫ�����file1�ļ�(�ɶ�д)������Ϊ��hello
	char *buf_write = "hello";
	ret = file_write(fd, buf_write, 5);
	if(ret<=0){
		printf("write error!\n");
		goto error;
	}
	ret = file_seek(fd, 0, FILE_SEEK_SET);
	if(ret<0){
		printf("seek error!\n");
		goto error;
	}
	/*********��������1**************/
	char buf[10];
	memset(buf,0,10);
	ret = file_read(fd, buf, 10);
	assert((ret>0) && (strcmp(buf,"hello")==0),"test1:file read error!");

	/*********��������2**************/
	ret = file_read(fd, buf, 4097);
	assert(ret==-ERR_INVAL, "test2:board test fail");

	/*********��������3**************/
	ret = file_read(-2, buf, 5);
	assert(ret==-ERR_INVAL, "test3:board test fail");

	/*********��������4**************/
	ret = file_read(fd, buf, 0);
	assert(ret==-ERR_INVAL, "test4:error test fail");

	//���ܲ���--���߳�ͬʱ��ȡ�ļ�
	ret = thread_init();                //�߳�ģ���ʼ��
	if (ret){
		printf("init thread fail!\n");
		goto error;
	}
	u8 id = 3;
	ret = thread_create(id, (void *)thread_test1, NULL, THREAD_MODE_NORMAL, 0);
	if (ret){
		printf("thread create fail!\n");
		goto error;
	}
	id = 5;
	ret = thread_create(id, (void *)thread_test2, NULL, THREAD_MODE_NORMAL, 0);
	if (ret){
		printf("thread create fail!\n");
		goto error;
	}
	id = 7;
	ret = thread_create(id, (void *)thread_test3, NULL, THREAD_MODE_NORMAL, 0);
	if (ret){
		printf("thread create fail!\n");
		goto error;
	}
	sleep(20);

	finaltest();
error:
	exit(0);
}
