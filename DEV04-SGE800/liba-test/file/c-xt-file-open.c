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
    �ļ���         ��  c-tx-file-open.c
    ����	       ��  ���ļ�����ƽ̨��file_open�����Ĳ���
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
	u8 pos,mode;
	/*********��������28**************/
	char *name = "file1";
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	ret = file_open(name, pos, mode);
	assert(ret>0,"test28:file1 open error!");
	p_err(ret);

	/*********��������29**************/
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RD;
	ret = file_open(name, pos, mode);
	assert(ret>0,"test29:file1 open error!");
	p_err(ret);

	while(1){

	}

}

void thread_test2()
{
	int ret;
	u8 pos,mode;
	/*********��������28**************/
	char *name = "file1";
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	ret = file_open(name, pos, mode);
	assert(ret>0,"test28:file2 open error!");
	p_err(ret);

	/*********��������29**************/
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RD;
	ret = file_open(name, pos, mode);
	assert(ret>0,"test29:file2 open error!");
	p_err(ret);

	while(1){

	}
}

void thread_test3()
{
	int ret;
	u8 pos,mode;

	/*********��������28**************/
	char *name = "file1";
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	ret = file_open(name, pos, mode);
	assert(ret>0,"test28:file3 open error!");
	file_close(ret);
	p_err(ret);

	/*********��������29**************/
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RD;
	ret = file_open(name, pos, mode);
	assert(ret>00,"test29:file3 open error!");
	p_err(ret);

	while(1){

	}

}

int main()
{
	int ret;
	//�����ĳ�ʼ��
	inittest();
	system("rm /var/f*");
	system("rm /var/F*");
	system("rm /mnt/local/fi*");
	char *name = "file1";
	u8 pos,mode;

	/*********��������1���������ԣ�**************/
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	ret = file_open(name, pos, mode);
	assert(ret>0,"test1:file open error!");
	file_close(ret);

	/*********��������2���߽���ԣ�**************/
	pos = -1;   //�Ƿ�
	mode = FILE_MODE_OPEN;
	ret = file_open(name, pos, mode);
	assert(ret==-ERR_INVAL,"test2:border test1 fail!");

	/*********��������3���߽���ԣ�**************/
	pos = 5;
	mode = FILE_MODE_OPEN;
	ret = file_open(name, pos, mode);
	assert(ret==-ERR_INVAL,"test2:border test2 fail!");

	/*********��������4���߽���ԣ�**************/
	pos = FILE_POS_RAM;
	mode = -1;
	ret = file_open(name, pos, mode);
	assert(ret==-ERR_INVAL,"test4:border test3 fail!");

	/*********��������5���߽���ԣ�**************/
	pos = FILE_POS_RAM;
	mode = 4;
	ret = file_open(name, pos, mode);
	assert(ret==-ERR_INVAL,"test5:border test4 fail!");

	/*********��������6��������ԣ�**************/
	char err_name[5]="";
	strcat (err_name, "file");
	pos = FILE_POS_RAM;
	mode = FILE_MODE_OPEN;
	ret = file_open(err_name, pos, mode);
	assert(ret==-ERR_NOFILE,"test6:error test fail!");

	/*********��������7**************/
	err_name[3]= '.';
	pos = FILE_POS_RAM;
	mode = FILE_MODE_OPEN;
	ret = file_open(err_name, pos, mode);
	assert(ret==-ERR_INVAL,"test7:error test fail!");

	/*********��������8**************/
	pos = 5;
	mode = FILE_MODE_OPEN;
	ret = file_open(name, pos, mode);
	assert(ret==-ERR_INVAL,"test8:error test fail!");

	/*********��������9**************/
	pos = FILE_POS_RAM;
	mode = -6;
	ret = file_open(name, pos, mode);
	assert(ret==-ERR_INVAL,"test9:error test fail!");

	/*********��������10**************/
	char *err_n;
	u8 name_err = 2;
	err_n = &name_err;    //����ĺ�������
	pos = FILE_POS_RAM;
	mode = FILE_MODE_OPEN;
	ret = file_open(err_n, pos, mode);
	assert(ret==-ERR_INVAL,"test10:error test fail!");

	/*********��������11**************/
	char file_name[10]="";
	memset(file_name, 0, 10);
	strcat (file_name, "file");
	pos = FILE_POS_RAM;
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret>0,"test11:file open error!");
	p_err(ret);

	/*********��������12**************/
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RW;
	ret = file_open(file_name, pos, mode);
	assert(ret>0,"test12:file open error!");
	p_err(ret);

	/*********��������13**************/
	pos = FILE_POS_RAM;
	mode = FILE_MODE_RD;
	ret = file_open(file_name, pos, mode);
	assert(ret>0,"test13:file open error!");

	/*********��������14**************/
	pos = FILE_POS_FLASH_CODE;
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret>0,"test14:file open error!");

	/*********��������15**************/
	pos = FILE_POS_FLASH_DATA;         //ֻ��
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret>0,"test15:file open error!");
	p_err(ret);

	/*********��������16**************/
	pos = FILE_POS_SD;  //��SD��
	mode = FILE_MODE_RW;
	ret = file_open(file_name, pos, mode);
	assert(ret>0,"test16:file open error!");
	p_err(ret);

	/*********��������17**************/
	pos = FILE_POS_U;   //ֻ��
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret>0,"test17:file open error!");
	p_err(ret);

	/*********��������18**************/
	char *file_long = "123filejfd334567jfkdhkdjf34567jjkd";
	pos = FILE_POS_RAM;
	mode = FILE_MODE_CREAT;
	ret = file_open(file_long, pos, mode);
	assert(ret==-ERR_INVAL,"test18:file open error!");
	p_err(ret);

	/*********��������19**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FILE");
	pos = FILE_POS_RAM;
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret>0,"test19:file open error!");
	p_err(ret);

	/*********��������20**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FilE");
	pos = FILE_POS_RAM;
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret>0,"test20:file open error!");
	p_err(ret);

	/*********��������21**************/
	memset(file_name, 0, 10);
	strcat (file_name, "Fi_lE678");
	pos = FILE_POS_RAM;
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret>0,"test21:file open error!");
	p_err(ret);

	/*********��������22**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FilE123");
	pos = FILE_POS_RAM;
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret>0,"test23:file open error!");
	p_err(ret);

	/*********��������23**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FilE/12");
	pos = FILE_POS_RAM;
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret==-ERR_INVAL,"test24:file open error!");

	/*********��������24**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FilE$%12");
	pos = FILE_POS_RAM;
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret==-ERR_INVAL,"test25:file open error!");

	/*********��������25**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FilE;12");
	pos = FILE_POS_RAM;
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret==-ERR_INVAL,"test26:file open error!");

	/*********��������26**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FilE[12]");
	pos = FILE_POS_RAM;
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret==-ERR_INVAL,"test27:file open error!");

	/*********��������27**************/
	memset(file_name, 0, 10);
	strcat (file_name, "FilE12}");
	pos = FILE_POS_RAM;
	mode = FILE_MODE_CREAT;
	ret = file_open(file_name, pos, mode);
	assert(ret==-ERR_INVAL,"test28:file open error!");

	//���ܲ���-���߳�ֻ�����ļ�
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

