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
    �ļ���      ��  c-qd-timer-pwmsetconfi-pwmgetconfig.c
    ����        ��  ���ļ�����ƽ̨��timer_pwm_setconfig,timer_pwm_getconfig�����Ĳ���
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
	u8 set_fz, set_fm;
	u8 get_fz, get_fm;
	u16 set_freq, get_freq;
	u16 freq1;


	//������ʼ��
	inittest();
	mode = TIMER_MODE_PWM;

	//��������1
	id = 2;
	freq1 = 10;
	set_freq = freq1;
	set_fz = 3;
	set_fm = 4;
	timer_init(id, mode);
	ret1 = timer_pwm_setconfig(id, set_freq, set_fz, set_fm);
	ret2 = timer_pwm_getconfig(id, &get_freq, &get_fz, &get_fm);

	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_freq == set_freq) \
			&& (get_fz == set_fz) \
			&& (get_fm == set_fm)) \
			,"timer_pwm_setconfig,time_pwm_getconfig 1 error");
	timer_close(id);

	//��������2
	id = 0;
	freq1 = 10;
	set_freq = freq1;
	set_fz = 3;
	set_fm = 4;
	timer_init(id, mode);
	ret1 = timer_pwm_setconfig(id, set_freq, set_fz, set_fm);
	ret2 = timer_pwm_getconfig(id, &get_freq, &get_fz, &get_fm);

	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_freq == set_freq) \
			&& (get_fz == set_fz) \
			&& (get_fm == set_fm)) \
			,"timer_pwm_setconfig,time_pwm_getconfig 2 error");
	timer_close(id);

	//��������3
	id = MAX_TIMER - 1;
	freq1 = 10;
	set_freq = freq1;
	set_fz = 3;
	set_fm = 4;
	timer_init(id, mode);
	ret1 = timer_pwm_setconfig(id, set_freq, set_fz, set_fm);
	ret2 = timer_pwm_getconfig(id, &get_freq, &get_fz, &get_fm);

	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_freq == set_freq) \
			&& (get_fz == set_fz) \
			&& (get_fm == set_fm)) \
			,"timer_pwm_setconfig,time_pwm_getconfig 3 error");
	timer_close(id);

	//��������4
	id = MAX_TIMER - 1;
	freq1 = 10;
	set_freq = freq1;
	set_fz = 3;
	set_fm = 4;
	timer_init(id, mode);
	ret1 = timer_pwm_setconfig(id, set_freq, set_fz, set_fm);
	ret2 = timer_pwm_getconfig(id, &get_freq, &get_fz, &get_fm);

	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_freq == set_freq) \
			&& (get_fz == set_fz) \
			&& (get_fm == set_fm)) \
			,"timer_pwm_setconfig,time_pwm_getconfig 4 error");
	timer_close(id);

	//��������5
	id = MAX_TIMER;
	freq1 = 10;
	set_freq = freq1;
	set_fz = 3;
	set_fm = 4;
	timer_init(id, mode);
	ret1 = timer_pwm_setconfig(id, set_freq, set_fz, set_fm);
	assert(ret1 == -ERR_INVAL,"timer_pwm_setconfig,time_pwm_getconfig 5 error");
	timer_close(id);

	//��������6
	id = 2;
	freq1 = 10;
	set_freq = freq1;
	set_fz = 0;
	set_fm = 4;
	timer_init(id, mode);
	ret1 = timer_pwm_setconfig(id, set_freq, set_fz, set_fm);
	assert(ret1 == -ERR_INVAL,"timer_pwm_setconfig,time_pwm_getconfig 6 error");
	timer_close(id);

	//��������7
	id = 2;
	set_freq = 0;
	set_fz = 3;
	set_fm = 4;
	timer_init(id, mode);
	ret1 = timer_pwm_setconfig(id, set_freq, set_fz, set_fm);
	assert(ret1 == -ERR_INVAL,"timer_pwm_setconfig,time_pwm_getconfig 7 error");
	printf("ret1 = %d\r\n",ret1);
	timer_close(id);

	//��������8
	id = 2;
	freq1 = 10;
	set_freq = freq1;
	set_fz = 3;
	set_fm = 4;
	ret1 = timer_pwm_setconfig(id, set_freq, set_fz, set_fm);
	assert(ret1 == -ERR_NOINIT,"timer_pwm_setconfig,time_pwm_getconfig 8 error");


	//��������9
	mode = TIMER_MODE_HEART;
	id = 2;
	freq1 = 10;
	set_freq = freq1;
	set_fz = 3;
	set_fm = 4;
	timer_init(id, mode);
	ret1 = timer_pwm_setconfig(id, set_freq, set_fz, set_fm);
	assert(ret1 == -ERR_NOFUN,"timer_pwm_setconfig,time_pwm_getconfig 9 error");

	finaltest();
	return 0;
}
