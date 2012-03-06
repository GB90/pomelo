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
    �ļ���      ��  c-xt-syn-rwlockinit.c
    ����        ��  ���ļ�����ƽ̨��syn_rwlock_init������ȫ����������
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

	//��������1
	ret = syn_rwlock_init(2);
	assert(ret == 0,"syn_rwlock_init Use Case 1 error");

	//��������2
	ret = syn_rwlock_init(64);
	assert(ret == -ERR_INVAL,"syn_rwlock_init Use Case 2 error");

	//��������3
	ret = syn_rwlock_init(2);
	assert(ret == -ERR_BUSY,"syn_rwlock_init Use Case 3 error");

	finaltest();
	return 0;
}

