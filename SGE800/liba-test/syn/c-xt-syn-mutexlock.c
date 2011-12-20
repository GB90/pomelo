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
    �ļ���      ��  c-xt-syn-mutexlock.c
    ����        ��  ���ļ�����ƽ̨��syn_mutex_lock������ȫ����������
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep

//ƽ̨��ͷ�ļ�
#include "../include/thread.h"
#include "../include/syn.h"
#include "../include/xj_assert.h"
#include "../include/error.h"

void thread_uc4(void)
{
	int ret;
	ret = syn_mutex_lock(2,2);
	assert(ret == -ERR_TIMEOUT, "syn_mutex_lock Use Case 4 error");

}

void thread_uc7(void)
{
	int ret;
	ret = syn_mutex_lock(2,SYN_RECV_NONBLOCK);
	assert(ret == -ERR_BUSY, "syn_mutex_lock Use Case 7 error");

}

int main()
{
	int ret;
	u8 id4,id7;
	//������ʼ��
	inittest();
	ret = syn_init();
	p_err(ret);
	ret = syn_mutex_init(2);
	p_err(ret);
	ret = thread_init();
	p_err(ret);

	//��������1
	ret = syn_mutex_lock(2,2);
	assert(ret == 0, "syn_mutex_lock Use Case 1 error");

	//��������2-�߽�
	ret = syn_mutex_lock(64,10);
	assert(ret == -ERR_INVAL, "syn_mutex_lock Use Case 2 error");

	//��������3-����δ��ʼ��
	ret = syn_mutex_lock(4,10);
	assert(ret == -ERR_NOINIT, "syn_mutex_lock Use Case 3 error");

	//��������4-���ǣ���ռ����
	id4=1;
	ret = thread_create(id4, (void *)thread_uc4, NULL, THREAD_MODE_NORMAL, 0);
	p_err(ret);

	sleep(2);

	//��������5-���ǣ������
	syn_mutex_unlock(2);
	p_err(ret);
	ret = syn_mutex_lock(2,SYN_RECV_BLOCK);
	assert(ret == 0, "syn_mutex_lock Use Case 5 error");

	//��������6-���ǣ������
	syn_mutex_unlock(2);
	p_err(ret);
	ret = syn_mutex_lock(2,SYN_RECV_NONBLOCK);
	assert(ret == 0, "syn_mutex_lock Use Case 6 error");

	//��������7-���ǣ���ռ����
	id7=2;
	ret = thread_create(id7, (void *)thread_uc7, NULL, THREAD_MODE_NORMAL, 0);
	p_err(ret);

	sleep(2);
	finaltest();
	return 0;
}

