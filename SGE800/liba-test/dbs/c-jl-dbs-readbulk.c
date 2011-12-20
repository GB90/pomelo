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
    �ļ���      ��  c-jl-dbs-read_bulk.c
    ����        ��  ���ļ�����ƽ̨��dbs_read_bulk�����Ĳ���
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep
#include <db.h>
#include <string.h>
//ƽ̨��ͷ�ļ�
#include "../include/dbs.h"
#include "../include/xj_assert.h"
#include "../include/error.h"

int main()
{
	int ret,ret_t;
	int key;
	int data;
	char data_r[50];
#pragma pack(1)
	struct {
		u16 len;
		int data;
	}data_bulk[8];
#pragma pack()
	u16 data_size;
	dbs_set_t set, from, to;

	//�����ĳ�ʼ��
	inittest();
	ret = dbs_init();
	p_err(ret);

	//��������7-���󣬲������ݿ�
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 3;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 24, &data_size, data_r );
	assert(ret == -ERR_NOFILE, "dbs_readbulk Use Case 7 error");

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
	to.offset = 3;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 24, &data_size, data_r );
	memcpy(&data_bulk,data_r,24);
//	for(i=0; i<4; i++){
//		printf("%d;",data_bulk[i].len);
//		printf("%d    ",data_bulk[i].data);
//	}
	assert((ret==0)&&
			(data_bulk[0].data == 1)&&
			(data_bulk[1].data == 2)&&
			(data_bulk[2].data == 3)&&
			(data_bulk[3].data == 4)&&
			(data_size == 24), "dbs_readbulk Use Case 1 error");

	//��������2-�߽�
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 3;
	ret = dbs_read_bulk(16, 4, &key, &from, &to, 24, &data_size, data_r );
	assert(ret == -ERR_INVAL, "dbs_readbulk Use Case 2 error");

	//��������3-�߽�
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 3;
	ret = dbs_read_bulk(0, 9, &key, &from, &to, 24, &data_size, data_r );
	assert(ret == -ERR_INVAL, "dbs_readbulk Use Case 3 error");

	//��������4-�߽�
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 3;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 129, &data_size, data_r );
	assert(ret == -ERR_INVAL, "dbs_readbulk Use Case 4 error");

	//��������5-�߽�
	key = 100;
	from.whence = 2;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 3;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 24, &data_size, data_r );
	assert(ret == -ERR_INVAL, "dbs_readbulk Use Case 5 error");

	//��������6-�߽�
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = 2;
	to.offset = 3;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 24, &data_size, data_r );
	assert(ret == -ERR_INVAL, "dbs_readbulk Use Case 6 error");

	//��������8-����
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 7;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 48, &data_size, data_r );
	assert(ret == -ERR_NORECORD, "dbs_readbulk Use Case 8 error");

	//��������13-����
	key = 100;
	from.whence = DBS_SEEK_END;
	from.offset = 6;
	to.whence = DBS_SEEK_SET;
	to.offset = 2;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 30, &data_size, data_r );
	assert(ret == -ERR_NORECORD, "dbs_readbulk Use Case 13 error");

	//��������14-����
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 6;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 42, &data_size, data_r );
	assert(ret == -ERR_NORECORD, "dbs_readbulk Use Case 14 error");

	//��������16-���ǣ��޴˼�ֵ
	key = 200;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 3;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 24, &data_size, data_r );
	assert(ret == -ERR_NORECORD, "dbs_readbulk Use Case 16 error");

	//��������17-����
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 4;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 30, &data_size, data_r );
	memcpy(&data_bulk,data_r,30);
	assert((ret==0)&&
			(data_bulk[0].data == 1)&&
			(data_bulk[1].data == 2)&&
			(data_bulk[2].data == 3)&&
			(data_bulk[3].data == 4)&&
			(data_bulk[4].data == 5)&&
			(data_size == 30), "dbs_readbulk Use Case 17 error");

	//��������18-����
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_END;
	to.offset = 0;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 30, &data_size, data_r );
	memcpy(&data_bulk,data_r,30);
	assert((ret==0)&&
			(data_bulk[0].data == 1)&&
			(data_bulk[1].data == 2)&&
			(data_bulk[2].data == 3)&&
			(data_bulk[3].data == 4)&&
			(data_bulk[4].data == 5)&&
			(data_size == 30),  "dbs_readbulk Use Case 18 error");

	//��������19-����
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_END;
	to.offset = 6;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 42, &data_size, data_r );
	assert(ret == -ERR_NORECORD, "dbs_readbulk Use Case 19 error");

	//��������20-����
	key = 100;
	from.whence = DBS_SEEK_END;
	from.offset = 4;
	to.whence = DBS_SEEK_END;
	to.offset = 0;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 30, &data_size, data_r );
	assert(ret == -ERR_INVAL, "dbs_readbulk Use Case 20 error");

	//��������21-����-�����
	key = 100;
	from.whence = DBS_SEEK_END;
	from.offset = 0;
	to.whence = DBS_SEEK_END;
	to.offset = 4;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 30, &data_size, data_r );
	memset(&data_bulk[0],0,8*sizeof data_bulk[0]);
	memcpy(&data_bulk,data_r,30);
	assert((ret==0)&&
			(data_bulk[0].data == 5)&&
			(data_bulk[1].data == 4)&&
			(data_bulk[2].data == 3)&&
			(data_bulk[3].data == 2)&&
			(data_bulk[4].data == 1)&&
			(data_size == 30), "dbs_readbulk Use Case 21 error");

	//��������22-����
	key = 100;
	from.whence = DBS_SEEK_END;
	from.offset = 4;
	to.whence = DBS_SEEK_SET;
	to.offset = 0;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 6, &data_size, data_r );
	memset(&data_bulk[0],0,8*sizeof data_bulk[0]);
	memcpy(&data_bulk,data_r,6);
	assert((ret==0)&&
			(data_bulk[0].data == 1)&&
			(data_size == 6), "dbs_readbulk Use Case 22 error");

	//��������23-����
	key = 100;
	from.whence = DBS_SEEK_END;
	from.offset = 4;
	to.whence = DBS_SEEK_SET;
	to.offset = 4;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 30, &data_size, data_r );
	assert(ret == -ERR_INVAL, "dbs_readbulk Use Case 23 error");

	//��������24-����
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 0;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 6, &data_size, data_r );
	memset(&data_bulk[0],0,8*sizeof data_bulk[0]);
	memcpy(&data_bulk,data_r,6);
	assert((ret == 0)&&
			(data_bulk[0].data == 1)&&
			(data_size == 6), "dbs_readbulk Use Case 24 error");

	//��������25-����
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 1;
	to.whence = DBS_SEEK_SET;
	to.offset = 4;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 24, &data_size, data_r );
	memset(&data_bulk[0],0,8*sizeof data_bulk[0]);
	memcpy(&data_bulk,data_r,24);
	assert((ret==0)&&
			(data_bulk[0].data == 2)&&
			(data_bulk[1].data == 3)&&
			(data_bulk[2].data == 4)&&
			(data_bulk[3].data == 5)&&
			(data_size == 24), "dbs_readbulk Use Case 25 error");

	//��������26-����
	key = 100;
	from.whence = DBS_SEEK_END;
	from.offset = 2;
	to.whence = DBS_SEEK_END;
	to.offset = 2;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 6, &data_size, data_r );
	memset(&data_bulk[0],0,8*sizeof data_bulk[0]);
	memcpy(&data_bulk,data_r,24);
	assert((ret==0)&&
			(data_bulk[0].data == 3)&&
			(data_size == 6), "dbs_readbulk Use Case 26 error");

	//��������27-����
	key = 100;
	from.whence = DBS_SEEK_SET;
	from.offset = 1;
	to.whence = DBS_SEEK_END;
	to.offset = 0;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 24, &data_size, data_r );
	memset(&data_bulk[0],0,8*sizeof data_bulk[0]);
	memcpy(&data_bulk,data_r,24);
	assert((ret==0)&&
			(data_bulk[0].data == 2)&&
			(data_bulk[1].data == 3)&&
			(data_bulk[2].data == 4)&&
			(data_bulk[3].data == 5)&&
			(data_size == 24), "dbs_readbulk Use Case 27 error");

	//��������28-����
	key = 100;
	from.whence = DBS_SEEK_END;
	from.offset = 0;
	to.whence = DBS_SEEK_SET;
	to.offset = 0;
	ret = dbs_read_bulk(0, 4, &key, &from, &to, 30, &data_size, data_r );
	memset(&data_bulk[0],0,8*sizeof data_bulk[0]);
	memcpy(&data_bulk,data_r,30);
	assert((ret==0)&&
			(data_bulk[0].data == 5)&&
			(data_bulk[1].data == 4)&&
			(data_bulk[2].data == 3)&&
			(data_bulk[3].data == 2)&&
			(data_bulk[4].data == 1)&&
			(data_size == 30), "dbs_readbulk Use Case 28 error");


	ret = dbs_close(0);		//�رղ��Ƴ����ݿ��ļ�
	p_err(ret);
	ret = dbs_remove(0);
	p_err(ret);

	finaltest();

	exit(0);
}

