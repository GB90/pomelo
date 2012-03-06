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
    �ļ���         ��  c-tx-comport-close.c
    ����	       ��  ���ļ�����ƽ̨��comport_close�����Ĳ���
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
	inittest();

	//���Ի����ĳ�ʼ��
	u8 port1 = 5;
	u8 mode1 = COMPORT_MODE_NORMAL;
	ret = comport_init(port1, mode1);    //��ͨ����
	if(ret < 0){
		printf("port1 init error!\n");
		goto error;
	}

	u8 port2 = 1;
	u8 mode2 = COMPORT_MODE_485;
	ret = comport_init(port2, mode2);    //485����
	if(ret < 0){
		printf("port2 init error!\n");
		goto error;
	}

	port1 = 0;
	ret = comport_init(port1, mode1);    //��ͨ����
	if(ret < 0){
		printf("port1 init error!\n");
		goto error;
	}

	port1 = 6;
	ret = comport_init(port1, mode1);    //��ͨ����
	if(ret < 0){
		printf("port1 init error!\n");
		goto error;
	}


	/***************��������1���������ܣ�*****************/
	port1 = 5;
	ret = comport_close(port1);
	assert(ret==0,"test1:comport close error");
	p_err(ret);

	/***************��������2���������ܣ�*****************/
	ret = comport_close(port2);
	assert(ret==0,"test2:comport close error");
	p_err(ret);

	/***************��������3���߽磩*****************/
	port1 = 0;
	ret = comport_close(port1);
	assert(ret==0,"test3:comport close error");

	/***************��������4���߽磩*****************/
	port1 = 6;
	ret = comport_close(port1);
	assert(ret==0,"test4:comport close error");

	/***************��������5������*****************/
	port1 = 10;
	ret = comport_close(port1);
	assert(ret==-ERR_INVAL,"test5:comport close error");

	/***************��������6������*****************/
	port1 = 2;
	ret = comport_close(port1);
	assert(ret==-ERR_NOINIT,"test6:comport close error");


	finaltest();
error:
	exit(0);
}
