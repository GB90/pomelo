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
    �ļ���      ��  c-jl-dbs-write.c
    ����        ��  ���ļ�����ƽ̨��dbs_write�����Ĳ���
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
	int data,data_r;
	u16 data_size;
	dbs_set_t set;

	//�����ĳ�ʼ��
	inittest();
	ret = dbs_init();
	p_err(ret);

	//��������7- ���󣬲������ݿ�
	key = 100;
	data = 255;
	set.whence = DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_write( 0, 4, &key, &set, 4, &data);
	assert(ret == -ERR_NOFILE, "dbs_write Use Case 7 error");

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
	key = 100;
	data = 103;
	set.whence =  DBS_SEEK_SET;
	set.offset = 2;
	ret = dbs_write(0, 4, &key, &set, 4, &data);
	ret_t = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert((ret==0)&&(data_r == data), "dbs_write Use Case 1 error");

	//��������2
	key = 100;
	data = 255;
	set.whence =  DBS_SEEK_END;
	set.offset = 1;
	ret = dbs_write(0, 4, &key, &set, 4, &data);
	ret_t = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert((ret==0)&&(data_r == data), "dbs_write Use Case 2 error");

	//��������3- �߽�
	key = 100;
	data = 255;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_write( 16, 4, &key, &set, 4, &data);
	assert(ret == -ERR_INVAL, "dbs_write Use Case 3 error");

	//��������4- �߽�
	key = 100;
	data = 255;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_write( 0, 8+1, &key, &set, 4, &data);
	assert(ret == -ERR_INVAL, "dbs_write Use Case 4 error");

	//��������5- �߽�
	key = 100;
	data = 255;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_write( 0, 4, &key, &set, 128+1, &data);
	assert(ret == -ERR_NODISK, "dbs_write Use Case 5 error");

	//��������6- �߽�
	key = 100;
	data = 255;
	set.whence = 2;
	set.offset = 0;
	ret = dbs_write( 0, 4, &key, &set, 4, &data);
	assert(ret == -ERR_INVAL, "dbs_write Use Case 6 error");

	//��������14- ����
	key = 200;
	data = 255;
	set.whence = DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_write( 0, 4, &key, &set, 4, &data);
	assert(ret == 0, "dbs_write Use Case 14 error");

	//��������15- ����
	key = 100;
	data = 101;
	set.whence = DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_write( 0, 4, &key, &set, 4, &data);
	ret_t = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert((ret==0)&&(data_r == data), "dbs_write Use Case 15 error");

	//��������16- ����
	key = 100;
	data = 102;
	set.whence = DBS_SEEK_SET;
	set.offset = 1;
	ret = dbs_write( 0, 4, &key, &set, 4, &data);
	ret_t = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert((ret==0)&&(data_r == data), "dbs_write Use Case 16 error");

	//��������17- ����
	key = 100;
	data = 205;
	set.whence = DBS_SEEK_END;
	set.offset = 0;
	ret = dbs_write( 0, 4, &key, &set, 4, &data);
	ret_t = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert((ret==0)&&(data_r == data), "dbs_write Use Case 17 error");

	//��������18- ����
	key = 100;
	data = 202;
	set.whence = DBS_SEEK_END;
	set.offset = 3;
	ret = dbs_write( 0, 4, &key, &set, 4, &data);
	ret_t = dbs_read(0, 4, &key, &set, 8, &data_size, &data_r);
	assert((ret==0)&&(data_r == data), "dbs_write Use Case 18 error");

	//��������8-����ֻ�������ݿ�
	ret = dbs_close(0);
	p_err(ret);
	ret = dbs_open(0, DBS_POS_RAM, DBS_MODE_RD);
	p_err(ret);

	key = 100;
	data = 255;
	set.whence = DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_write( 0, 4, &key, &set, 4, &data);
	assert(ret == -ERR_NOFUN, "dbs_write Use Case 8 error");


	ret = dbs_close(0);		//�رղ��Ƴ����ݿ��ļ�
	p_err(ret);
	ret = dbs_remove(0);
	p_err(ret);

	finaltest();

	exit(0);
}

