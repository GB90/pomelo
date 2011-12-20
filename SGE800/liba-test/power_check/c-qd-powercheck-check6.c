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
    ����        ��  ���ļ�����ƽ̨��powercheck_check�����Ĳ�������4�����ܲ���2
    �汾        ��  0.1
    ����        ��  ��ΰ��
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>		//printf
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
	u8 io_check = 32;						//pb0�����紦��ʱ��
	u8 io_clearval = 0;
	u8 io_setval = 1;						//I/O�������
	u8 io_trigger = 35;						//pb3�������紦��


	//������ʼ��
	inittest();

	//��������4
	io_mode = GPIO_OUT;
	io_od = GPIO_ODD;
	io_pu = GPIO_PUD;

	ret = gpio_init();										//��ʼ��I/O��
	if(ret != 0){
		printf("gpio_init error\r\n");
	}

	ret = gpio_set(io_check, io_mode, io_od, io_pu);		//����pb0Ϊ�����
	if(ret != 0){
		printf("gpio_set error\r\n");
	}
	ret = gpio_set(io_trigger, io_mode, io_od, io_pu);		//����pb0Ϊ�����
	if(ret != 0){
		printf("gpio_set error\r\n");
	}

	gpio_output_set(io_trigger, io_setval);					//PB3������ͣ�ģ��Ϊ����
	gpio_output_set(io_check, io_setval);					//PB0������ͣ������⺯��ִ����ɺ��ø�

	mode = POWERCHECK_MODE_BLOCK_DOWN;
	powercheck_init(mode);
	powercheck_setwaittime(0);					//���紦��ģ�鴦������״̬

	while(1)
	{
		gpio_output_set(io_check, io_setval);				//PB0�������
		ret = powercheck_check();
		gpio_output_set(io_check, io_clearval);				//PB0�������
		printf("ret = %d\r\n",ret);
		assert(ret == 0,"powercheck_check 1 error");
		sleep(1);
	}

	finaltest();
	return 0;
}


