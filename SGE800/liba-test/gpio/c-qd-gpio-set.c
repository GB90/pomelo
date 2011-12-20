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
    �ļ���     	 ��  c-qd-gpio-set.c
    ����       		 ��  ���ļ�����ƽ̨��gpio_set�����Ĳ���
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
	int ret;
	u8 io,mode,od,pu;
	io = 2;
	ret = -1;

	//������ʼ��
	inittest();

	gpio_init();

	//��������1
	mode = GPIO_OUT;
	od = GPIO_ODD;
	pu = GPIO_PUD;
	ret = gpio_set(io, mode, od, pu);
	assert(ret == 0,"gpio_set 1 error");

	//��������2
	mode = GPIO_IN;
	od = GPIO_ODE;
	pu = GPIO_PUE;
	ret = gpio_set(io, mode, od, pu);
	assert(ret == 0,"gpio_set 2 error");

	//��������3
	io = 0;
	mode = GPIO_IN;
	od = GPIO_ODE;
	pu = GPIO_PUE;
	ret = gpio_set(io, mode, od, pu);
	assert(ret == 0,"gpio_set 3 error");

	//��������4
/*	io = MAX_PIN - 1;
	mode = GPIO_IN;
	od = GPIO_ODE;
	pu = GPIO_PUE;
	ret = gpio_set(io, mode, od, pu);
	assert(ret == 0,"gpio_set 4 error");
*/

	//��������5
	io = MAX_PIN;
	mode = GPIO_OUT;
	od = GPIO_ODD;
	pu = GPIO_PUD;
	ret = gpio_set(io, mode, od, pu);
	assert(ret == -ERR_NODEV,"gpio_set 5 error");

	//��������6
	io = 2;
	mode = 6;
	od = GPIO_ODD;
	pu = GPIO_PUD;
	ret = gpio_set(io, mode, od, pu);
	assert(ret == -ERR_INVAL,"gpio_set 6 error");

	//��������7
	io = 2;
	mode = GPIO_OUT;
	od = 6;
	pu = GPIO_PUD;
	ret = gpio_set(io, mode, od, pu);
	assert(ret == -ERR_INVAL,"gpio_set 7 error");

	//��������8
	io = 2;
	mode = GPIO_OUT;
	od = GPIO_ODD;
	pu = 6;
	ret = gpio_set(io, mode, od, pu);
	assert(ret == -ERR_INVAL,"gpio_set 8 error");

	//��������9
	gpio_close();
	io = 2;
	mode = GPIO_OUT;
	od = GPIO_ODD;
	pu = GPIO_PUD;
	ret = gpio_set(io, mode, od, pu);
	assert(ret == -ERR_NOINIT,"gpio_set 9 error");
	//printf("ret = %d\r\n",ret);

	finaltest();
	return 0;
}




