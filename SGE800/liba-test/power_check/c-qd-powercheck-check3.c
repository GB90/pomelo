
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
    �ļ���      ��  c-qd-powercheck-check.c
    ����        ��  ���ļ�����ƽ̨��powercheck_check�����Ĳ�������3
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
#include "../include/powercheck.h"
#include "../include/gpio.h"

int main()
{
	/*power_check��ر���*/
	int ret = -1;
	u8 mode;
	u16 set_timeout = 1000;

	/*I/O����ر���*/
	u8 io_mode, io_od, io_pu;
	u8 io_clearval = 0;
	u8 io_setval = 1;						//I/O�������
	u8 io_trigger = 35;						//pb3�������紦��


	//������ʼ��
	inittest();

	//��������1
	io_mode = GPIO_OUT;
	io_od = GPIO_ODD;
	io_pu = GPIO_PUD;

	ret = gpio_init();											//��ʼ��I/O��
	if(ret != 0){
		printf("gpio_init error\r\n");
	}

	ret = gpio_set(io_trigger, io_mode, io_od, io_pu);			//����pb0Ϊ�����
	if(ret != 0){
		printf("gpio_set error\r\n");
	}
	/*������ʱ��ʼ��Ϊ�ߣ��ϵ���ʱ��ʼ��Ϊ��*/
//	gpio_output_set(io_trigger, io_clearval);					//PB3������ͣ�ģ��Ϊ����
	gpio_output_set(io_trigger, io_setval);						//PB3������ͣ�ģ��Ϊ�ϵ�



	//��������3
	/*�����⣬��������ֵΪ0*/
	mode = POWERCHECK_MODE_BLOCK_UPDOWN;
	powercheck_init(mode);
	powercheck_setwaittime(set_timeout);
	ret = powercheck_check();
	assert(ret == 0,"powercheck_check 3 error");
	powercheck_close();

	/*�ϵ��⣬��������ֵΪ1*/
/*
	mode = POWERCHECK_MODE_BLOCK_UPDOWN;
	powercheck_init(mode);
	powercheck_setwaittime(set_timeout);
	ret = powercheck_check();
	assert(ret == 1,"powercheck_check 3 error");
	printf("ret = %d\r\n",ret3);
	powercheck_close();
*/
	finaltest();
	return 0;
}


