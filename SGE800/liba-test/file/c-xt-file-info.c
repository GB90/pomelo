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
    �ļ���         ��  c-tx-file-info.c
    ����	       ��  ���ļ�����ƽ̨��file_info�����Ĳ���
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
	//���Ի�����ʼ��������c-xt-file-open
	char *name = "file1";
	u8 pos;
	file_time_t file_time;
	u32 size;

	/************��������1*****************/
	pos = FILE_POS_RAM;
	ret = file_info(name, pos, &size, &file_time);
	assert(ret == 0,"test1:file info get error");
	printf("file time:year=%d,month=%d,day=%d,hour=%d,min=%d,sec=%d\n",
			file_time.year,file_time.mon,file_time.day,file_time.hour,file_time.min,file_time.sec);
	printf("size = %d\n",size);
	system("ls -l /var/file1");

	/************��������2*****************/
	ret = file_info(name, -1, &size, &file_time);
	assert(ret == -ERR_INVAL,"test2:file info get error");

	/************��������3*****************/
	ret = file_info(name, 5, &size, &file_time);
	assert(ret == -ERR_INVAL,"test3:file info get error");

	/*********��������4**************/
	pos = FILE_POS_RAM;
	ret = file_info("file2", pos, &size, &file_time);
	assert(ret==-ERR_NOFILE,"test4:file info get error!");

	/*********��������5**************/
	pos = FILE_POS_RAM;
	ret = file_info("file.c", 5, &size, &file_time);
	ret = file_delete("file.c", pos);
	assert(ret==-ERR_INVAL,"test5:file info get error!");

	/*********��������6**************/
	pos = FILE_POS_RAM;
	u32 *size1;
	ret = file_info(name, 5, size1, &file_time);
	assert(ret==-ERR_INVAL,"test6:file info get error!");

	/*********��������7**************/
	pos = FILE_POS_RAM;
	file_time_t *file_time1;
	ret = file_info(name, 5, &size, file_time1);
	assert(ret==-ERR_INVAL,"test7:file info get error!");


	/*********��������8**************/
	char file_name[10]="";
	memset(file_name, 0, 10);
	strcat (file_name, "file");
	pos = FILE_POS_FLASH_CODE;
	ret = file_info(file_name, pos, &size, &file_time);
	assert(ret==0,"test8:file info get error!");
	p_err(ret);

	/*********��������9**************/
	pos = FILE_POS_FLASH_DATA;
	ret = file_info(file_name, pos, &size, &file_time);
	assert(ret==0,"test9:file info get error!");
	p_err(ret);

	/*********��������10**************/
	pos = FILE_POS_SD;  //��SD��
	ret = file_info(file_name, pos, &size, &file_time);
	assert(ret==0,"test10:file info get error!");
	p_err(ret);

	/*********��������11**************/
	pos = FILE_POS_U;   //ֻ��
	ret = file_info(file_name, pos, &size, &file_time);
	assert(ret==0,"test11:file info get error!");
	p_err(ret);
	/*********��������12**************/
	pos = FILE_POS_RAM;
	ret = file_info("fi/s", pos, &size, &file_time);
	assert(ret==-ERR_INVAL,"test12:file info get error!");

	/*********��������13**************/
	char *file_long = "123filejfd334567jfkdhkdjf34567jjkd";
	pos = FILE_POS_RAM;
	ret = file_info(file_long, pos, &size, &file_time);
	assert(ret==-ERR_INVAL,"test13:file info get error!");

	/*********��������14**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FILE");
	pos = FILE_POS_RAM;
	ret = file_info(file_name, pos, &size, &file_time);
	assert(ret==0,"test14:file info get error!");
	p_err(ret);

	/*********��������15**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FilE");
	pos = FILE_POS_RAM;
	ret = file_info(file_name, pos, &size, &file_time);
	assert(ret==0,"test15:file info get error!");
	p_err(ret);

	/*********��������17**************/
	memset(file_name, 0, 10);
	strcat (file_name, "Fi_lE678");
	pos = FILE_POS_RAM;
	ret = file_info(file_name, pos, &size, &file_time);
	assert(ret==0,"test17:file info get error!");
	p_err(ret);

	/*********��������18**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FilE123");
	pos = FILE_POS_RAM;
	ret = file_info(file_name, pos, &size, &file_time);
	assert(ret==0,"test18:file info get error!");
	p_err(ret);

	/*********��������19**************/
	pos = FILE_POS_RAM;
	ret = file_info("fi$%", pos, &size, &file_time);
	assert(ret==-ERR_INVAL,"test19:file info get error!");

	/*********��������20**************/
	pos = FILE_POS_RAM;
	ret = file_info("fi;ss", pos, &size, &file_time);
	assert(ret==-ERR_INVAL,"test20:file info get error!");

	/*********��������21**************/
	pos = FILE_POS_RAM;
	ret = file_info("fi[ss]", pos, &size, &file_time);
	assert(ret==-ERR_INVAL,"test21:file info get error!");

	/*********��������22**************/
	pos = FILE_POS_RAM;
	ret = file_info("fi}ss", pos, &size, &file_time);
	assert(ret==-ERR_INVAL,"test22:file info get error!");


	finaltest();

	exit(0);
}
