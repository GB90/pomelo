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
    �ļ���     	 ��  c-qd-adc-setwaittime.c
    ����       		 ��  ���ļ�����ƽ̨��adc_setwaittime��adc_getwaittime�����Ĳ���
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
#include "../include/adc.h"

int main()
{
	int ret1,ret2;
	u16 set_timeout;
	u16 get_timeout;

	//������ʼ��
	inittest();
	adc_init();

	//��������1
	set_timeout = 12;
	ret1 = adc_setwaittime(set_timeout);
	ret2 = adc_getwaittime(&get_timeout);
	assert(((ret1 == 0) && (ret1 == 0) && (set_timeout == get_timeout)),\
			"adc_setwaittime,adc_getwaittime 1 error");

	//��������2
	set_timeout = 0xffff;
	ret1 = adc_setwaittime(set_timeout);
	ret2 = adc_getwaittime(&get_timeout);
	assert(((ret1 == 0) && (ret1 == 0) && (set_timeout == get_timeout)),\
			"adc_setwaittime,adc_getwaittime 2 error");

	//��������3
	set_timeout = 0;
	ret1 = adc_setwaittime(set_timeout);
	assert(ret1 == -ERR_INVAL,\
			"adc_setwaittime,adc_getwaittime 3 error");

	//��������4
	adc_close();
	set_timeout = 12;
	ret1 = adc_setwaittime(set_timeout);
	assert(ret1 == -ERR_NOINIT,\
			"adc_setwaittime,adc_getwaittime 4 error");
	printf("ret1 = %d\r\n",ret1);



	finaltest();
	return 0;
}


