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
    �ļ���      ��  c-jl-dbs-capmem.c
    ����        ��  ���ļ�����ƽ̨���ڴ�й¶�Ĳ���
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

#define MAXKEY 100
#define MAXDATA 100

int main()
{
	int ret;
	u32 i,j;
	u32 *key = &i;
	u32 *data = &j;
	u16 data_size;
	dbs_set_t set;
	u32 data_r;



	//�����ĳ�ʼ��
	inittest();
	ret = dbs_init();
	p_err(ret);

	//�����ĳ�ʼ��-��д��ʽ��ram�д�
	ret = dbs_open(0, DBS_POS_RAM, DBS_MODE_RW);
	p_err(ret);

	while(1){
		//дMAXKEY��MAXDATA����¼
		for(i=0; i < MAXKEY; i++){
			for(j=0; j< MAXDATA; j++){
				set.whence = DBS_SEEK_SET;
				set.offset = 0;
				ret = dbs_insert(0, 4, key, &set, DBS_SEEK_FORWARD, 4, data);
				assert(ret==0, "dbs_insert error");
			}
		}
		printf("insert %d record\n",i*j);
//		finaltest();

		//��ȡMAXKEY��MAXDATA����¼
		for(i=0; i < MAXKEY; i++){
			for(j=0; j< MAXDATA; j++){
				set.whence = DBS_SEEK_SET;
				set.offset = j;
				ret = dbs_read(0, 4, key, &set, 8, &data_size, &data_r);
				assert(ret==0, "dbs_read error");
				p_err(ret);
			}
		}
		printf("read %d record\n",i*j);
//		finaltest();

		//ɾ��MAXKEY��MAXDATA����¼
		for(i=0; i < MAXKEY; i++){
			for(j=0; j< MAXDATA; j++){
				set.whence = DBS_SEEK_SET;
				set.offset = 0;
				ret = dbs_delete(0, 4, key, &set);
				assert(ret==0, "dbs_delete error");
			}
		}
		printf("delete %d record\n",i*j);
//		finaltest();
	}

	ret = dbs_close(0);		//�رղ��Ƴ����ݿ��ļ�
	p_err(ret);
	ret = dbs_remove(0);
	p_err(ret);

	exit(0);
}

