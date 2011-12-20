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
    �ļ���     	 ��  c-qd-gpio-outputset-outputget.c
    ����       		 ��  ���ļ�����ƽ̨��gpio_outputset,gpio_outputget�����Ĳ���
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
#include "../include/timer.h"

#define 	MAX_PIN		32*3

int main()
{
	int ret, ret1,ret2;
	u8 io,set_val,get_val;
	ret1 = -1;
	ret2 = -1;
	u8 mode,od,pu;

	//������ʼ��
	inittest();
	gpio_init();

	//��������1
	io = 2;

	mode = GPIO_OUT;
	od = GPIO_ODD;
	pu = GPIO_PUD;

	ret = gpio_set(io, mode, od, pu);
	p_err(ret);
	set_val = 0;	//����I/O������͵�ƽ
	ret1 = gpio_output_set(io, set_val);
	p_err(ret1);
	ret2 = gpio_output_get(io, &get_val);
	p_err(ret2);
	assert((ret1 == 0) && (ret2 == 0) && (get_val == 0),"gpio_output_set,gpio_output_get 1 error");

	//��������2
	set_val = 1;	//����I/O������ߵ�ƽ
	ret1 = gpio_output_set(io, set_val);
	p_err(ret1);
	ret2 = gpio_output_get(io, &get_val);
	p_err(ret2);
	assert((ret1 == 0) && (ret2 == 0) && (get_val == 1),"gpio_output_set,gpio_output_get 2 error");

	//��������3
	io = 0;
	ret = gpio_set(io, mode, od, pu);
	p_err(ret);
	set_val = 0;	//����I/O������͵�ƽ
	ret1 = gpio_output_set(io, set_val);
	p_err(ret1);
	ret2 = gpio_output_get(io, &get_val);
	p_err(ret2);
	assert((ret1 == 0) && (ret2 == 0) && (get_val == 0),"gpio_output_set,gpio_output_get 3 error");


	//��������5
	io = 2;
	set_val = 2;
	ret1 = gpio_output_set(io, set_val);
	assert(ret1 == -ERR_INVAL,"gpio_output_set 5 error");

	//��������8
	gpio_close();
	set_val = 0;
	io = 2;
	gpio_set(io, mode, od, pu);
	ret1 = gpio_output_set(io, set_val);
	assert(ret1 == -ERR_NOINIT,"gpio_output_set 8 error");

	finaltest();
	return 0;

}






