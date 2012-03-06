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
    �ļ���      ��  c-jl-dbs-count.c
    ����        ��  ���ļ�����ƽ̨��dbs_count�����Ĳ���
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
	int ret,ret_t;
	int key;
	int data;
	dbs_set_t set;
	u32 num;
	//�����ĳ�ʼ��
	inittest();
	ret = dbs_init();

	//��������6-���󣬲���
	ret = dbs_count(0, 4, &key, &num);
	assert(ret == -ERR_NOFILE, "dbs_count Use Case 6 error");


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

	key = 100;
	data = 4;
	set.whence =  DBS_SEEK_SET;
	set.offset = 2;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret_t);

	key = 100;
	data = 5;
	set.whence =  DBS_SEEK_SET;
	set.offset = 3;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret_t);

	//��������1
	ret = dbs_count(0, 4, &key, &num);
	assert((ret==0)&&(num == 5), "dbs_count Use Case 1 error");

	//��������2-�߽�
	ret = dbs_count(16, 4, &key, &num);
	assert(ret == -ERR_INVAL, "dbs_count Use Case 2 error");

	//��������3-�߽�
	ret = dbs_count(0, 8+1, &key, &num);
	assert(ret == -ERR_INVAL, "dbs_count Use Case 3 error");

	//��������7-�����޴˹ؼ���
	key = 300;
	ret = dbs_count(0, 4, &key, &num);
	assert(ret == -ERR_NORECORD, "dbs_count Use Case 7 error");

	ret = dbs_close(0);		//�رղ��Ƴ����ݿ��ļ�
	p_err(ret);
	ret = dbs_remove(0);
	p_err(ret);
	finaltest();

	exit(0);
}

