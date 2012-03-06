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
    �ļ���      ��  c-qd-timer-heart-setconfig-getconfig.c
    ����        ��  ���ļ�����ƽ̨��timer-heart-setconfig,timer-heart-getconfig�����Ĳ���
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
	int ret = -1, ret1 = -1, ret2 = -1;
	u8 id, mode;
	id = 2;
	u32 set_interval, get_interval;
	mode = TIMER_MODE_HEART;

	//������ʼ��
	inittest();

	//��������1
	ret = timer_init(id, mode);
	if(ret != 0){
		printf("timer_init error\r\n");
	}
	set_interval = 40;
	ret1 = timer_heart_setconfig(id, set_interval);
	ret2 = timer_heart_getconfig(id, &get_interval);
	assert(((ret1 == 0) && (ret2 == 0) && (get_interval == 40))\
			,"time_heart_setconfig,time_heart_getconfig 1 error");
	timer_close(id);

	//��������2
	timer_init(id, mode);
	set_interval = 125;
	ret1 = timer_heart_setconfig(id, set_interval);
	ret2 = timer_heart_getconfig(id, &get_interval);
	assert(((ret1 == 0) && (ret2 == 0) && (get_interval == 125))\
			,"time_heart_setconfig,time_heart_getconfig 2 error");
	timer_close(id);

	//��������3
	id = 0;
	timer_init(id, mode);
	set_interval = 80;
	ret1 = timer_heart_setconfig(id, set_interval);
	ret2 = timer_heart_getconfig(id, &get_interval);
	assert(((ret1 == 0) && (ret2 == 0) && (get_interval == set_interval))\
			,"time_heart_setconfig,time_heart_getconfig 3 error");
	timer_close(id);

	//��������4
	id = MAX_TIMER - 1;
	timer_init(id, mode);
	set_interval = 250;
	ret1 = timer_heart_setconfig(id, set_interval);
	ret2 = timer_heart_getconfig(id, &get_interval);
	assert(((ret1 == 0) && (ret2 == 0) && (get_interval == set_interval))\
			,"time_heart_setconfig,time_heart_getconfig 4 error");
	timer_close(id);

	//��������5
	id = 2;
	timer_init(id, mode);
	set_interval = 20;
	ret1 = timer_heart_setconfig(id, set_interval);
	ret2 = timer_heart_getconfig(id, &get_interval);
	assert(((ret1 == 0) && (ret2 == 0) && (get_interval == set_interval))\
			,"time_heart_setconfig,time_heart_getconfig 5 error");
	timer_close(id);

	//��������6
	id = 2;
	timer_init(id, mode);
	set_interval = 1000;
	ret1 = timer_heart_setconfig(id, set_interval);
	perror("aaaaaa");
	ret2 = timer_heart_getconfig(id, &get_interval);
	assert(((ret1 == 0) && (ret2 == 0) && (get_interval == set_interval))\
			,"time_heart_setconfig,time_heart_getconfig 6 error");
	printf("ret1 = %d\r\n",ret1);
	printf("ret2 = %d\r\n",ret2);
	printf("get_interval = %d\r\n",get_interval);
	timer_close(id);

	//��������7
	id = 7;
	timer_init(id, mode);
	set_interval = 40;
	ret1 = timer_heart_setconfig(id, set_interval);
	ret2 = timer_heart_getconfig(id, &get_interval);
	assert(((ret1 == -ERR_INVAL) && (ret2 == -ERR_INVAL))\
			,"time_heart_setconfig,time_heart_getconfig 7 error");
	timer_close(id);

	//��������8
	id = 2;
	timer_init(id, mode);
	set_interval = 2;
	ret1 = timer_heart_setconfig(id, set_interval);
	assert(ret1 == -ERR_INVAL, "time_heart_setconfig,time_heart_getconfig 8 error");
	timer_close(id);

	//��������9
	id = 2;
	timer_init(id, mode);
	set_interval = 126;
	ret1 = timer_heart_setconfig(id, set_interval);
	assert(ret1 == -ERR_INVAL, "time_heart_setconfig,time_heart_getconfig 9 error");
	timer_close(id);

	//��������10
	id = 2;
	timer_init(id ,mode);
	set_interval = 1002;
	ret1 = timer_heart_setconfig(id, set_interval);
	assert(ret1 == -ERR_INVAL, "time_heart_setconfig,time_heart_getconfig 10 error");
	timer_close(id);

	//��������11
/*	id = 2;
	timer_init(id, mode);
	set_interval = 40;
	ret1 = timer_heart_setconfig(id, set_interval);
	assert(ret1 == -ERR_NOINIT, "time_heart_setconfig,time_heart_getconfig 11 error");
	timer_close(id);
*/
	//��������12
	timer_close(id);
	id = 2;
	mode = TIMER_MODE_PWM;
	ret = timer_init(id, mode);
	set_interval = 40;
	ret1 = timer_heart_setconfig(id, set_interval);
	assert(ret1 == -ERR_NOFUN, "time_heart_setconfig,time_heart_getconfig 12 error");

	finaltest();
	return 0;
}
