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
    �ļ���      ��  c-jl-dbs_open.c
    ����        ��  ���ļ�����ƽ̨��dbs_open�����Ĳ���
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep
#include <db.h>

//ƽ̨��ͷ�ļ�
#include "../include/dbs.h"
#include "../include/error.h"
#include "../include/xj_assert.h"

int main()
{
	int ret;
	//�����ĳ�ʼ��,ȷ��DBS_POS_RAM��ָ��Ŀ¼û��.db�ļ�
	inittest();

	//��������5- ����δ��ʼ������
	ret = dbs_open(0, DBS_POS_RAM, DBS_MODE_RW);
	assert(ret == -ERR_NOINIT,"dbs_open Use Case 5 error");

	//�����ĳ�ʼ��
	ret = dbs_init();
	p_err(ret);

	//��������1-����
	ret = dbs_open(0, DBS_POS_RAM, DBS_MODE_RW);
	assert(ret == 0,"dbs_open Use Case 1 error");
	p_err(ret);

	//��������6-�����ظ���
	ret = dbs_open(0, DBS_POS_RAM, DBS_MODE_RW);
	assert(ret == -ERR_BUSY,"dbs_open Use Case 6 error");

	ret = dbs_close(0);
	p_err(ret);

	//��������8-�����ظ������ļ�
	ret = dbs_open(0, DBS_POS_RAM, DBS_MODE_CREAT);
	assert(ret == -ERR_BUSY,"dbs_open Use Case 8 error");

	ret = dbs_remove(0);
	p_err(ret);

	//��������2-�߽�
	ret = dbs_open(16, DBS_POS_RAM, DBS_MODE_RW);
	assert(ret == -ERR_INVAL,"dbs_open Use Case 2 error");

	//��������3-�߽�
	ret = dbs_open(0, DBS_POS_RAM, 4);
	assert(ret == -ERR_INVAL,"dbs_open Use Case 3 error");


	//��������4-�߽�
	ret = dbs_open(0, 6, DBS_MODE_RW);
	assert(ret == -ERR_INVAL,"dbs_open Use Case 4 error");

	//��������7-����û�д����ļ�
	ret = dbs_open(1, DBS_POS_RAM, DBS_MODE_OPEN);
	assert(ret == -ERR_NOFILE,"dbs_open Use Case 7 error");

	//��������9-����
	ret = dbs_open(2, DBS_POS_FLASH_CODE, DBS_MODE_RW);
	assert(ret == 0,"dbs_open Use Case 9 error");
	p_err(ret);

	ret = dbs_close(2);
	p_err(ret);
	ret = dbs_remove(2);
	p_err(ret);

	//��������11-����
	ret = dbs_open(0, DBS_POS_SD, DBS_MODE_RW);
	assert(ret == 0,"dbs_open Use Case 11 error");
	p_err(ret);

	ret = dbs_close(0);
	p_err(ret);
	ret = dbs_remove(0);
	p_err(ret);

	//��������12-����
	ret = dbs_open(0, DBS_POS_RAM, DBS_MODE_CREAT);
	p_err(ret);
	ret = dbs_close(0);
	p_err(ret);
	ret = dbs_open(0, DBS_POS_FLASH_CODE, DBS_MODE_CREAT);
	assert(ret == -ERR_INVAL,"dbs_open Use Case 12 error");

	ret = dbs_remove(0);
	p_err(ret);

	//��������13-����
	ret = dbs_open(2, DBS_POS_RAM, DBS_MODE_RD);
	assert(ret == -ERR_NOFILE,"dbs_open Use Case 13 error");

	//��������14-����
	ret = dbs_open(10, DBS_POS_RAM, DBS_MODE_CREAT);
	assert(ret == 0 ,"dbs_open Use Case 14 error");


	ret = dbs_close(10);
	p_err(ret);
	ret = dbs_remove(10);
	p_err(ret);

	//����ͳ��
	finaltest();
	exit(0);
}

