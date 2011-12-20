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
    �ļ���     	 ��  c-qd-adc-read.c
    ����       		 ��  ���ļ�����ƽ̨��adc_read�����Ĳ���
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
	int ret;
	u16 set_timeout = 12;
	u8 id;
	u16 result;

	//������ʼ��
	inittest();
	ret = adc_init();
	if(ret != 0){
		printf("adc_init() error\r\n");
	}
	ret = adc_setwaittime(set_timeout);
	if(ret != 0){
		printf("adc_init() error\r\n");
	}
	//��������1
	id = 2;
	printf("executing adc_read()\r\n");
	ret = adc_read(id,&result);
	printf("result = 0x%x\r\n",result);
	assert(ret == 0,"adc_read 1 error");

	//��������2
/*	id = 0;
	ret = adc_read(id,&result);
	printf("result = 0x%x\r\n",result);
	assert(ret == 0,"adc_read 2 error");
*/
	//��������3
/*	id = 3;
	ret = adc_read(id,&result);
	printf("result = 0x%x\r\n",result);
	assert(ret == 0,"adc_read 3 error");
*/
	//��������4
/*	id = 10;
	ret = adc_read(id,&result);
	assert(ret == -ERR_NODEV,"adc_read 4 error");
*/
	//��������5
/*	id = 10;
	ret = adc_read(id,&result);
	assert(ret == 0,"adc_read 5 error");
*/
	//��������6
/*	id = 2;
	adc_close();
	ret = adc_read(id,&result);
	assert(ret == -ERR_NOINIT,"adc_read 6 error");
*/
	finaltest();
	return 0;
}


