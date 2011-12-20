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
    �ļ���         ��  c-tx-comport-init.c
    ����	       ��  ���ļ�����ƽ̨��comport_init�����Ĳ���
    �汾              ��  0.1
    ����              ��  ����
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>			//exit
#include <unistd.h>			//sleep
#include <db.h>

//ƽ̨��ͷ�ļ�
#include "../include/comport.h"
#include "../include/xj_assert.h"
#include "../include/error.h"
int main()
{
	int ret;
	//�����ĳ�ʼ��
	inittest();

	/****��������1���������ԣ�232ģʽ��**************/
	u8 port1 = 5;
	u8 mode1 = COMPORT_MODE_NORMAL;
	ret = comport_init(port1, mode1);    //��ͨ����
	assert(ret==0,"test1:comport init error");

	/****��������2���������ԣ�485ģʽ��**************/
	u8 port2 = 1;
	u8 mode2 = COMPORT_MODE_485;
	ret = comport_init(port2, mode2);    //485����
	assert(ret==0,"test1:comport init error");

	/****��������3���߽���ԣ�**************/
	port1 = 0;
	mode1 = COMPORT_MODE_NORMAL;
	ret = comport_init(port1, mode1);    //��ͨ����
	assert(ret==0,"test3:comport init error");

	/****��������4���߽���ԣ�**************/
	port1 = 6;
	mode1 = COMPORT_MODE_NORMAL;
	ret = comport_init(port1, mode1);
	assert(ret==0,"test4:comport init error");

	/****��������5��������ԣ����ںţ�**************/
	port1 = 8;
	mode1 = COMPORT_MODE_NORMAL;
	ret = comport_init(port1, mode1);
	assert(ret==-ERR_INVAL,"test5:comport init error");

	/****��������6��������ԣ��򿪷�ʽ��**************/
	port1 = 2;
	mode1 = 22;
	ret = comport_init(port1, mode1);
	assert(ret==-ERR_INVAL,"test6:comport init error");
	p_err(ret);

	/****��������7��������ԣ��ö˿�û�д˹��ܣ�**************/
	port2 = 4;
	mode2 = COMPORT_MODE_485;
	ret = comport_init(port2, mode2);
	assert(ret==-ERR_NOFUN,"test7:comport init error");
	finaltest();

	exit(0);
}
