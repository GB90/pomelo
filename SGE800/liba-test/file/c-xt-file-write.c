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
    �ļ���         ��  c-tx-file-write.c
    ����	       ��  ���ļ�����ƽ̨��file_write�����Ĳ���
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
	char *buf = "how";
//	char buf_rev[10];
//	memset(buf_rev,0,10);
	file_seek(fd, 0, FILE_SEEK_END);
	ret = file_write(fd, buf, 3);
	assert(ret==3,"thread1:file write error!");


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
	char *buf = "are";
	char buf_rev[10];
	memset(buf_rev,0,10);
	file_seek(fd, 0, FILE_SEEK_END);
	ret = file_write(fd, buf, 3);
	assert(ret==3,"thread1:file write error!");

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
	char *buf = "you";
	char buf_rev[10];
	memset(buf_rev,0,10);

	file_seek(fd, 0, FILE_SEEK_END);
	ret = file_write(fd, buf, 3);
	assert(ret==3,"thread1:file write error!");

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
	/*********��������1**************/
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	fd = file_open(name,pos,mode);
	if(fd<=0){
		printf("test1:file opern error!\n");
		goto error;
	}
	char *buf = "hello";
	char buf_rev[10];
	memset(buf_rev,0,10);

	ret = file_write(fd, buf, 5);
	file_seek(fd, 0, FILE_SEEK_SET);
	file_read(fd, buf_rev, 10);
	assert((ret>0) && (strcmp(buf_rev,"hello")==0),"test1:file write error!");

	/*********��������2**************/
	ret = file_write(fd, buf, 4097);                   //count=CFG_FILE_DATA_MAX+1
	assert(ret==-ERR_INVAL, "test2:board test fail");

	/*********��������3**************/
	ret = file_write(-2, buf, 5);
	assert(ret==-ERR_INVAL, "test3:board test fail");

	/*********��������4**************/
	char *buf_err=NULL;
	ret = file_write(fd, buf_err, 5);
	assert(ret==-ERR_INVAL, "test4:board test fail");

	/*********��������5**************/
	ret = file_write(fd, buf, 0);   //�ӿڴ���
	assert(ret==-ERR_INVAL, "test5:board test fail");

	//���ܲ���--���߳�ͬʱд�ļ�

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
