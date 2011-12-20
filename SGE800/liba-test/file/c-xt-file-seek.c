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
    �ļ���         ��  c-tx-file-seek.c
    ����	       ��  ���ļ�����ƽ̨��file_seek�����Ĳ���
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

int main()
{
	int ret,ref;
	//�����ĳ�ʼ��
	inittest();

	system("rm /var/fi*");
	char *name = "file1";
	u8 pos,mode;
	int fd;
	//��������1����Ҫ�����file1�ļ�(�ɶ�д)������Ϊ��hello
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	fd = file_open(name,pos,mode);
	if(fd<=0){
		printf("file open error!\n");
		goto error;
	}
	char *buf_write = "hello";
	ret = file_write(fd, buf_write, 5);
	if(ret<=0){
		printf("write error!\n");
		goto error;
	}

	/*********��������1**************/
	ret = file_seek(fd, 1, FILE_SEEK_SET);
	if (ret<0){
		assert(ret==1,"test1:file seek error!");
	}else{
		char buf[10];
		memset(buf,0,10);
		ref = file_read(fd, buf, 10);
		if(ref<=0){
			assert(ret>0,"test1:file seek point error!");
			goto error;
		}
		assert((ret==1) && (strcmp(buf,"ello")==0),"test1:file seek error!");
	}

	/*********��������2**************/
	ret = file_seek(fd, 1, 5);
	assert(ret==-ERR_INVAL,"test2:file seek error!");

	/*********��������3**************/
	ret = file_seek(-1, 1, FILE_SEEK_SET);
	assert(ret==-ERR_INVAL,"test3:file seek error!");

	finaltest();
error:
	exit(0);
}
