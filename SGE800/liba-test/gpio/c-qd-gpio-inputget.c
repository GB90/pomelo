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
    ��Ŀ����   	 ��  SGE800���������ն�ƽ̨
    �ļ���     	 ��  c-qd-gpio-inputget.c
    ����       		 ��  ���ļ�����ƽ̨��gpio_inputget�����Ĳ��ԣ���Ӧ����������2����Ĳ�������
    �汾      		  ��  0.1
    ����        	��  ��ΰ��
    ��������   	 ��  2010.03
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
#include "../include/gpio.h"
#define		MAX_PIN 	32*3

int main()
{
	int ret1,ret2;
	u8 io = 2,get_val;
	ret1 = -1;
	ret2 = -1;

	u8 mode,od,pu;
	mode = GPIO_IN;
	od = GPIO_ODE;
	pu = GPIO_PUE;

	//������ʼ��
	inittest();

	gpio_init();
	gpio_set(io, mode, od, pu);

	//��������1
	ret1 = gpio_input_get(io, &get_val);
	assert((ret1 == 0) && (get_val == 0),"gpio_input_get 1 error");

	//��������2

	//��������3
	io = 0;
	gpio_set(io, mode, od, pu);
	ret1 = gpio_input_get(io, &get_val);
	assert((ret1 == 0) && (get_val == 1),"gpio_input_get 3 error");

	//��������4

/*	io = MAX_PIN - 1;
	gpio_set(io, mode, od, pu);
	ret1 = gpio_input_get(io, &get_val);
	assert((ret1 == 0) && (get_val == 0),"gpio_input_get 4 error");
*/
	//��������5
	io = MAX_PIN;
	ret1 = gpio_input_get(io, &get_val);
	assert(ret1 == -ERR_NODEV,"gpio_input_get 5 error");

	//��������6
	gpio_close();
	gpio_init();
	io = 2;
	ret1 = gpio_input_get(io, &get_val);
	assert(ret1 == -ERR_NOCFG,"gpio_input_get 6 error");

	//��������7
	gpio_close();
	io = 2;
	ret1 = gpio_input_get(io, &get_val);
	assert(ret1 == -ERR_NOINIT,"gpio_input_get 7 error");

	finaltest();
	return 0;
}






