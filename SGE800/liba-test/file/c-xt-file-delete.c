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
    �ļ���         ��  c-tx-file-delete.c
    ����	       ��  ���ļ�����ƽ̨��file_delete�����Ĳ���
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
	//���Ի����������ݿ�----���ҽ��������ļ�����һ��x-xt-file-open
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	fd = file_open(name, pos, mode);
	if(fd<=0){
		printf("file open error!\n");
		goto error;
	}
	ret = file_close(fd);
	if (ret){
		printf("file close error!\n");
		goto error;
	}
	/*********��������1**************/
	pos = FILE_POS_RAM;
	ret = file_delete(name, pos);
	assert(ret==0,"test1:file delete error!");

	/*********��������2**************/
	pos = FILE_POS_RAM;
	ret = file_delete(name, -1);
	assert(ret==-ERR_INVAL,"test2:file delete error!");

	/*********��������3**************/
	pos = FILE_POS_RAM;
	ret = file_delete(name, 5);
	assert(ret==-ERR_INVAL,"test3:file delete error!");

	/*********��������4**************/
	pos = FILE_POS_RAM;
	ret = file_delete("file2", pos);
	assert(ret==-ERR_NOFILE,"test4:file delete error!");

	/*********��������5**************/
	pos = FILE_POS_RAM;
	ret = file_delete("file.c", pos);
	assert(ret==-ERR_INVAL,"test5:file delete error!");

	/*********��������6**************/
	char file_name[10]="";
	memset(file_name, 0, 10);
	strcat (file_name, "file");
	pos = FILE_POS_FLASH_CODE;
	ret = file_delete(file_name, pos);
	assert(ret==0,"test6:file delete error!");
	p_err(ret);

	/*********��������7**************/
	pos = FILE_POS_FLASH_DATA;
	ret = file_delete(file_name, pos);
	assert(ret==0,"test7:file delete error!");
	p_err(ret);

	/*********��������8**************/
	pos = FILE_POS_SD;  //��SD��
	ret = file_delete(file_name, pos);
	assert(ret==0,"test8:file delete error!");
	p_err(ret);

	/*********��������9**************/
	pos = FILE_POS_U;   //ֻ��
	ret = file_delete(file_name, pos);
	assert(ret==0,"test9:file delete error!");
	p_err(ret);
	/*********��������10**************/
	pos = FILE_POS_RAM;
	ret = file_delete("fi/s", pos);
	assert(ret==-ERR_INVAL,"test10:file delete error!");

	/*********��������11**************/
	char *file_long = "123filejfd334567jfkdhkdjf34567jjkd";
	pos = FILE_POS_RAM;
	ret = file_delete(file_long, pos);
	assert(ret==-ERR_INVAL,"test11:file delete error!");

	/*********��������12**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FILE");
	pos = FILE_POS_RAM;
	ret = file_delete(file_name, pos);
	assert(ret==0,"test12:file delete error!");
	p_err(ret);

	/*********��������13**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FilE");
	pos = FILE_POS_RAM;
	ret = file_delete(file_name, pos);
	assert(ret==0,"test13:file delete error!");
	p_err(ret);

	/*********��������15**************/
	memset(file_name, 0, 10);
	strcat (file_name, "Fi_lE678");
	pos = FILE_POS_RAM;
	ret = file_delete(file_name, pos);
	assert(ret==0,"test15:file delete error!");
	p_err(ret);

	/*********��������16**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FilE123");
	pos = FILE_POS_RAM;
	ret = file_delete(file_name, pos);
	assert(ret==0,"test16:file delete error!");
	p_err(ret);


	/*********��������17**************/
	pos = FILE_POS_RAM;
	ret = file_delete("fi$%", pos);
	assert(ret==-ERR_INVAL,"test17:file delete error!");

	/*********��������18**************/
	pos = FILE_POS_RAM;
	ret = file_delete("fi;ss", pos);
	assert(ret==-ERR_INVAL,"test18:file delete error!");

	/*********��������19**************/
	pos = FILE_POS_RAM;
	ret = file_delete("fi[ss]", pos);
	assert(ret==-ERR_INVAL,"test19:file delete error!");

	/*********��������20**************/
	pos = FILE_POS_RAM;
	ret = file_delete("fi}ss", pos);
	assert(ret==-ERR_INVAL,"test20:file delete error!");


	finaltest();
error:
	exit(0);
}
