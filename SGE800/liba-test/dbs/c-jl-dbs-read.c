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
    �ļ���      ��  c-jl-dbs-read.c
    ����        ��  ���ļ�����ƽ̨��dbs_read�����Ĳ���
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
	int data,data_r;
	u16 data_size;
	dbs_set_t set;

	//�����ĳ�ʼ��
	inittest();
	ret = dbs_init();
	p_err(ret);

	//��������6-���󣬲������ݿ�
	key = 100;
	data = 255;
	set.whence = DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_read(0,4, &key,&set,8,&data_size,&data_r);
	assert(ret == -ERR_NOFILE, "dbs_read Use Case 6 error");

	//�����ĳ�ʼ��-��д��ʽ��ram�д�
	ret = dbs_open(0, DBS_POS_RAM, DBS_MODE_RW);
	p_err(ret);

	key = 100;
	data = 0;
	set.whence = DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_FORWARD, 4, &data);

	data = 1;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);

	data = 2;
	set.offset = 1;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);

	data = 3;
	set.offset = 2;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);

	//��������1-����
	key = 100;
	set.whence = DBS_SEEK_SET;
	set.offset = 2;
	ret = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert((ret==0)&&(data_r == 2), "dbs_read Use Case 1 error");

	//��������2-�߽�
	key = 100;
	set.whence = DBS_SEEK_SET;
	set.offset = 2;
	ret = dbs_read(16, 4, &key, &set, 8, &data_size, &data_r);
	assert(ret == -ERR_INVAL, "dbs_read Use Case 2 error");

	//��������3-�߽�
	key = 100;
	set.whence = DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_read(0, 9, &key, &set, 8, &data_size, &data_r);
	assert(ret == -ERR_INVAL, "dbs_read Use Case 3 error");

	//��������4-�߽�
	key = 100;
	set.whence = DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_read(0, 4, &key, &set, 129, &data_size, &data_r);
	assert(ret == -ERR_INVAL, "dbs_read Use Case 4 error");

	//��������5-�߽�
	key = 100;
	set.whence = 2;
	set.offset = 0;
	ret = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert(ret == -ERR_INVAL, "dbs_read Use Case 5 error");

	//��������7-����
	key = 100;
	set.whence = DBS_SEEK_END;
	set.offset = 6;
	ret = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert(ret == -ERR_NORECORD, "dbs_read Use Case 7 error");

	//��������12-����
	key = 200;
	set.whence = DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert(ret == -ERR_NORECORD, "dbs_read Use Case 12 error");

	//��������13-����
	key = 100;
	set.whence = DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert(ret == 0, "dbs_read Use Case 13 error");

	//��������14-����
	key = 100;
	set.whence = DBS_SEEK_SET;
	set.offset = 3;
	ret = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert(ret == 0, "dbs_read Use Case 14 error");

	//��������15-����
	key = 100;
	set.whence = DBS_SEEK_END;
	set.offset = 0;
	ret = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert(ret == 0, "dbs_read Use Case 15 error");

	//��������16-����
	key = 100;
	set.whence = DBS_SEEK_END;
	set.offset = 3;
	ret = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert(ret == 0, "dbs_read Use Case 16 error");

	ret = dbs_close(0);		//�رղ��Ƴ����ݿ��ļ�
	p_err(ret);
	ret = dbs_remove(0);
	p_err(ret);

	finaltest();

	exit(0);
}

