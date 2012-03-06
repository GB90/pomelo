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
    �ļ���      ��  c-qd-timer-init.c
    ����        ��  ���ļ�����ƽ̨��timer_init�����Ĳ���
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
	int ret1 = -1, ret2 = -1, ret3 = -1;
	u8 id, mode;
	id = 3;

	//������ʼ��
	inittest();

	//��������1
	mode = TIMER_MODE_HEART;
	ret1 = timer_init(id, mode);
	assert(ret1 == 0,"timer_init 1 error");
	timer_close(id);

	//��������2
	mode = TIMER_MODE_MEASURE;
	ret2 = timer_init(id, mode);
	assert(ret2 == 0,"timer_init 2 error");
	timer_close(id);

	//��������3
	mode = TIMER_MODE_PWM;
	ret3 = timer_init(id, mode);
	assert(ret3 == 0,"timer_init 3 error");
	timer_close(id);

	//��������4
	id = 0;
	mode = TIMER_MODE_HEART;
	ret1 = timer_init(id, mode);
	assert(ret1 == 0,"timer_init 4 error");
	timer_close(id);

	//��������5
	id = 0;
	mode = TIMER_MODE_MEASURE;
	ret1 = timer_init(id, mode);
	assert(ret1 == 0,"timer_init 5 error");
	timer_close(id);

	//��������6
	id = 0;
	mode = TIMER_MODE_PWM;
	ret1 = timer_init(id, mode);
	assert(ret1 == 0,"timer_init 6 error");
	timer_close(id);

	//��������7
	id = MAX_TIMER - 1;
	mode = TIMER_MODE_HEART;
	ret1 = timer_init(id, mode);
	assert(ret1 == 0,"timer_init 7 error");
	timer_close(id);

	//��������8
	id = MAX_TIMER - 1;
	mode = TIMER_MODE_MEASURE;
	ret1 = timer_init(id, mode);
	assert(ret1 == 0,"timer_init 8 error");
	timer_close(id);

	//��������9
	id = MAX_TIMER - 1;
	mode = TIMER_MODE_PWM;
	ret1 = timer_init(id, mode);
	assert(ret1 == 0,"timer_init 8 error");
	timer_close(id);

	//��������10
	id = 3;
	mode = TIMER_MODE_MEASURE;
	ret1 = timer_init(id, mode);
	assert(ret1 == -ERR_NOFILE,"timer_init 10 error");

	//��������11
	id = 3;
	mode = TIMER_MODE_MEASURE;
	ret1 = timer_init(id, mode);
	ret1 = timer_init(id, mode);
	assert(ret1 == -ERR_BUSY,"timer_init 11 error");
	timer_close(id);

	//��������12
	id = 3;
	mode = 4;
	ret1 = timer_init(id, mode);
	assert(ret1 == -ERR_INVAL,"timer_init 12 error");
	printf("ret1 = %d\r\n",ret1);
	timer_close(id);

	finaltest();
	return 0;
}
