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
    �ļ���      ��  c-xt-syn-semgetvalue.c
    ����        ��  ���ļ�����ƽ̨��syn_sem_getvalue������ȫ����������
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep

//ƽ̨��ͷ�ļ�
#include "../include/syn.h"
#include "../include/xj_assert.h"
#include "../include/error.h"

int main()
{
	int ret;
	int value;
	//������ʼ��
	inittest();
	ret = syn_init();
	p_err(ret);
	ret = syn_sem_init(2,1);
	p_err(ret);

	//��������1
	ret = syn_sem_getvalue(2,&value);
	assert((ret == 0)&&(value == 1),"syn_sem_getvalue Use Case 1 error");

	//��������2
	ret = syn_sem_getvalue(64,&value);
	assert(ret == -ERR_INVAL,"syn_sem_getvalue Use Case 2 error");

	//��������3
	ret = syn_sem_getvalue(4,&value);
	assert(ret == -ERR_NOINIT,"syn_sem_getvalue Use Case 3 error");

	finaltest();
	return 0;
}

