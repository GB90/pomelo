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
    �ļ���      ��  c-jl-dbs-delete_bulk.c
    ����        ��  ���ļ�����ƽ̨��dbs_delete_bulk�����Ĳ���
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
	dbs_set_t set, from, to;

	//�����ĳ�ʼ��
	inittest();
	ret = dbs_init();
	p_err(ret);

	//��������6-���󣬲������ݿ�
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 2;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == -ERR_NOFILE, "dbs_deletebulk Use Case 6 error");

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
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 4;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == 0, "dbs_deletebulk Use Case 1 error");

	//��������2-�߽�
	key = 100;
	data = 1;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_FORWARD, 4, &data);
	p_err(ret);

	data = 2;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	data = 3;
	set.whence =  DBS_SEEK_SET;
	set.offset = 1;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 2;
	ret = dbs_delete_bulk(16, 4, &key, &from, &to);
	assert(ret == -ERR_INVAL, "dbs_deletebulk Use Case 2 error");

	//��������3-�߽�
	ret = dbs_delete_bulk(0, 9, &key, &from, &to);
	assert(ret == -ERR_INVAL, "dbs_deletebulk Use Case 3 error");

	//��������4-�߽�
	from.whence = 2;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 2;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == -ERR_INVAL, "dbs_deletebulk Use Case 4 error");

	//��������5-�߽�
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = 2;
	to.offset = 2;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == -ERR_INVAL, "dbs_deletebulk Use Case 5 error");

	//��������6-����
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 3;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == -ERR_NORECORD, "dbs_deletebulk Use Case 6 error");

	//��������11-����
	from.whence = DBS_SEEK_SET;
	from.offset = 2;
	to.whence = DBS_SEEK_SET;
	to.offset = 1;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == -ERR_INVAL, "dbs_deletebulk Use Case 11 error");

	//��������12-����
	key = 200;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_END;
	to.offset = 0;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == -ERR_NORECORD, "dbs_deletebulk Use Case 12 error");

	//��������13-����
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_END;
	to.offset = 0;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == 0, "dbs_deletebulk Use Case 13 error");

	//��������14-����
	key = 100;
	data = 1;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_FORWARD, 4, &data);
	p_err(ret);

	data = 2;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	data = 3;
	set.whence =  DBS_SEEK_SET;
	set.offset = 1;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	from.whence = DBS_SEEK_END;
	from.offset = 0;
	to.whence = DBS_SEEK_END;
	to.offset = 2;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == 0, "dbs_deletebulk Use Case 14 error");


	//��������15-����
	key = 100;
	data = 1;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_FORWARD, 4, &data);
	p_err(ret);

	data = 2;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	data = 3;
	set.whence =  DBS_SEEK_SET;
	set.offset = 1;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	from.whence = DBS_SEEK_END;
	from.offset = 2;
	to.whence = DBS_SEEK_END;
	to.offset = 0;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == -ERR_INVAL, "dbs_deletebulk Use Case 15 error");

	//��������16-����
	from.whence = DBS_SEEK_END;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 0;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == 0, "dbs_deletebulk Use Case 16 error");

	//��������17-����
	key = 100;
	data = 1;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_FORWARD, 4, &data);
	p_err(ret);

	data = 2;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	data = 3;
	set.whence =  DBS_SEEK_SET;
	set.offset = 1;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	from.whence = DBS_SEEK_END;
	from.offset = 2;
	to.whence = DBS_SEEK_SET;
	to.offset = 2;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == -ERR_INVAL, "dbs_deletebulk Use Case 17 error");

	//��������18-����
	from.whence = DBS_SEEK_SET;
	from.offset = 1;
	to.whence = DBS_SEEK_SET;
	to.offset = 2;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == 0, "dbs_deletebulk Use Case 18 error");

	//��������19-����
	key = 100;
	data = 1;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_FORWARD, 4, &data);
	p_err(ret);

	data = 2;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	from.whence = DBS_SEEK_END;
	from.offset = 0;
	to.whence = DBS_SEEK_END;
	to.offset = 0;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == 0, "dbs_deletebulk Use Case 19 error");

	//��������20-����
	key = 100;
	data = 2;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	from.whence = DBS_SEEK_SET;
	from.offset = 1;
	to.whence = DBS_SEEK_END;
	to.offset = 0;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == 0, "dbs_deletebulk Use Case 20 error");

	//��������21-����
	key = 100;
	data = 1;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_FORWARD, 4, &data);
	p_err(ret);

	data = 2;
	set.whence =  DBS_SEEK_SET;
	set.offset = 0;
	ret = dbs_insert(0, 4, &key, &set, DBS_SEEK_BACKWARD, 4, &data);
	p_err(ret);

	from.whence = DBS_SEEK_END;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 2;
	ret = dbs_delete_bulk(0, 4, &key, &from, &to);
	assert(ret == 0, "dbs_deletebulk Use Case 21 error");

	ret = dbs_close(0);		//�رղ��Ƴ����ݿ��ļ�
	p_err(ret);
	ret = dbs_remove(0);
	p_err(ret);
	finaltest();

	exit(0);
}

