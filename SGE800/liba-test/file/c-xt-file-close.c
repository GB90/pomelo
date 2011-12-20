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
    �ļ���         ��  c-tx-file-close.c
    ����	       ��  ���ļ�����ƽ̨��file_close�����Ĳ���
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
	int ret;
	//�����ĳ�ʼ��
	inittest();

	char *name = "file1";
	u8 pos,mode;
	int fd;
	//���Ի����������ݿ�
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	fd = file_open(name, pos, mode);
	if(fd<=0){
		printf("file open error!\n");
		goto error;
	}
	/*********��������1**************/
	ret = file_close(fd);
	assert(ret==0,"test1:file close error!");

	/*********��������2**************/
	ret = file_close(-1);
	assert(ret==-ERR_INVAL,"test2:file close error!");

	/*********��������3**************/
	ret = file_close(100);
	assert(ret==-ERR_INVAL,"test3:file close error!");



	finaltest();
error:
	exit(0);
}
