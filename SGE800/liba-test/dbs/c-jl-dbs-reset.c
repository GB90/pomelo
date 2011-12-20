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
    �ļ���      ��  c-jl-dbs-reset.c
    ����        ��  ���ļ�����ƽ̨��dbs_reset�����Ĳ���
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
#include "../include/xj_assert.h"
#include "../include/error.h"

int main()
{
	int ret;
	int key;
	int data;
	dbs_set_t set;

	//�����ĳ�ʼ��
	inittest();
	ret = dbs_init();
	p_err(ret);

	//��������3
	ret = dbs_reset(0);
	assert(ret == -ERR_NOFILE, "dbs_reset Use Case 3 error");

	//�����ĳ�ʼ��
	ret = dbs_open(0, DBS_POS_RAM, DBS_MODE_RW);
	p_err(ret);

	key = 100;
	data = 1;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_FORWARD, 4, &data);
	p_err(ret);

	key = 100;
	data = 2;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	key = 100;
	data = 3;
	set.whence =  DBS_SEEK_SET;
	set.offset = 1;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	//��������1
	ret = dbs_reset(0);
	assert(ret == 0, "dbs_reset Use Case 1 error");

	//��������2-�߽�
	ret = dbs_reset(16);
	assert(ret == -ERR_INVAL, "dbs_reset Use Case 2 error");

	ret = dbs_close(0);
	p_err(ret);
	ret = dbs_remove(0);
	p_err(ret);

	//����ͳ��
	finaltest();
	exit(0);
}

