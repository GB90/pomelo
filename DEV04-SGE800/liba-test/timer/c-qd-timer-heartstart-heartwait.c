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
    �ļ���      ��  c-qd-timer-heartstart-heartwait.c
    ����        ��  ���ļ�����ƽ̨��timer_heart_start,timer_heart_wait�����Ĳ���
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
	int ret1 = -1, ret2 = -1;
	u8 id, mode;
	u32 set_interval = 20;


	//������ʼ��
	inittest();

	//��������1
	id = 2;
	mode = TIMER_MODE_HEART;
	timer_init(id, mode);
	timer_heart_setconfig(id, set_interval);
	ret1 = timer_heart_start(id);
	ret2 = timer_heart_wait(id);
	assert(((ret1 == 0) && (ret2 == 0))\
			,"time_heart_start,time_heart_wait 1 error");
	timer_close(id);

	//��������2
	id = 0;
	mode = TIMER_MODE_HEART;
	timer_init(id, mode);
	timer_heart_setconfig(id, set_interval);
	ret1 = timer_heart_start(id);
	ret2 = timer_heart_wait(id);
	assert(((ret1 == 0) && (ret2 == 0))\
				,"time_heart_start,time_heart_wait 2 error");
	timer_close(id);

	//��������3
	id = MAX_TIMER - 1;
	mode = TIMER_MODE_HEART;
	timer_init(id, mode);
	timer_heart_setconfig(id, set_interval);
	ret1 = timer_heart_start(id);
	ret2 = timer_heart_wait(id);
	assert(((ret1 == 0) && (ret2 == 0))\
				,"time_heart_start,time_heart_wait 3 error");
	timer_close(id);

	//��������4
	id = 7;
	mode = TIMER_MODE_HEART;
	timer_init(id, mode);
	timer_heart_setconfig(id, set_interval);
	ret1 = timer_heart_start(id);
	assert(ret1 == -ERR_INVAL, "time_heart_start,time_heart_wait 4 error");
	timer_close(id);

	//��������5
	id = 2;
	mode = TIMER_MODE_HEART;
	timer_heart_setconfig(id, set_interval);
	ret1 = timer_heart_start(id);
	assert(ret1 == -ERR_NOINIT, "time_heart_start,time_heart_wait 5 error");
	timer_close(id);

	//��������6
	id = 2;
	mode = TIMER_MODE_PWM;
	timer_init(id, mode);
	timer_heart_setconfig(id, set_interval);
	ret1 = timer_heart_start(id);
	assert(ret1 == -ERR_NOFUN, "time_heart_start,time_heart_wait 6 error");
	timer_close(id);

	finaltest();
	return 0;
}
