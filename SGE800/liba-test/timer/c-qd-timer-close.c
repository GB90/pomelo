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
    �ļ���      ��  c-qd-timer-close.c
    ����        ��  ���ļ�����ƽ̨��timer_close�����Ĳ���
    �汾        ��  0.1
    ����        ��  ��ΰ��
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

//���Գ���ͷ�ļ�
#include "../include/timer.h"

int main()
{
	int ret = -1;
	u8 id, mode;


	//������ʼ��
	inittest();

	//��������1
	id = 2;
	mode = TIMER_MODE_HEART;
	timer_init(id, mode);
	ret = timer_close(id);
	assert(ret == 0,"timer_close 1 error");

	//��������2
	id = 0;
	mode = TIMER_MODE_HEART;
	timer_init(id, mode);
	ret = timer_close(id);
	assert(ret == 0,"timer_close 2 error");

	//��������3
	id = 5;
	mode = TIMER_MODE_HEART;
	timer_init(id, mode);
	ret = timer_close(id);
	assert(ret == 0,"timer_close 3 error");

	//��������4
	id = 2;
	ret = timer_close(id);
	assert(ret == -ERR_NOINIT,"timer_close 4 error");

	finaltest();
	return 0;
}
