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
    �ļ���      ��  c-xt-syn-semwait.c
    ����        ��  ���ļ�����ƽ̨��syn_sem_wait������ȫ����������
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

	//������ʼ��
	inittest();
	ret = syn_init();
	p_err(ret);
	ret = syn_sem_init(2,3);
	p_err(ret);
	//��������1
	ret = syn_sem_wait(2,10);
	assert(ret == 0,"syn_sem_wait Use Case 1 error");

	//��������2-�߽�
	ret = syn_sem_wait(64,10);
	assert(ret == -ERR_INVAL,"syn_sem_wait Use Case 2 error");

	//��������3-����
	ret = syn_sem_wait(4,10);
	assert(ret == -ERR_NOINIT,"syn_sem_wait Use Case 3 error");

	//��������5
	ret = syn_sem_wait(2,SYN_RECV_BLOCK);
	assert(ret == 0,"syn_sem_wait Use Case 5 error");

	//��������6
	ret = syn_sem_wait(2,SYN_RECV_NONBLOCK);
	assert(ret == 0,"syn_sem_wait Use Case 6 error");

	//��������4
	ret = syn_sem_wait(2,2);
	assert(ret == -ERR_TIMEOUT,"syn_sem_wait Use Case 4 error");

	//��������7
	ret = syn_sem_wait(2,SYN_RECV_NONBLOCK);
	assert(ret == -ERR_BUSY,"syn_sem_wait Use Case 7 error");

	finaltest();
	exit(0);
}

