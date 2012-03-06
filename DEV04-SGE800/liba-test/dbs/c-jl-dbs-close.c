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
    �ļ���      ��  c-jl-dbs-close.c
    ����        ��  ���ļ�����ƽ̨��dbs_close�����Ĳ���
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
	//�����ĳ�ʼ��
	inittest();

	//��������3-����
	ret = dbs_close(0);
	assert(ret == -ERR_NOINIT,"dbs_close Use Case 3 error");

	//�����ĳ�ʼ��
	ret = dbs_init();

	//��������4-����
	ret = dbs_close(0);
	assert(ret == -ERR_NOFILE,"dbs_close Use Case 4 error");
	//�����ĳ�ʼ��
	ret = dbs_open(0, DBS_POS_RAM, DBS_MODE_RW);
	p_err(ret);

	//��������1
	ret = dbs_close(0);
	assert(ret == 0,"dbs_close Use Case 1 error");

	//��������2-�߽�
	ret = dbs_open(0, DBS_POS_RAM, DBS_MODE_RW);
	p_err(ret);
	ret = dbs_close(16);
	assert(ret == -ERR_INVAL,"dbs_close Use Case 2 error");

	ret = dbs_close(0);		//�رղ��Ƴ����ݿ��ļ�
	p_err(ret);
	ret = dbs_remove(0);
	p_err(ret);

	finaltest();

	exit(0);
}

