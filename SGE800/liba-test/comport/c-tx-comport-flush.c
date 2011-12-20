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
    �ļ���         ��  c-tx-comport-flush.c
    ����	       ��  ���ļ�����ƽ̨��comport_flush�����Ĳ���
    �汾              ��  0.1
    ����              ��  ����
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>			//exit
#include <unistd.h>			//sleep
#include <db.h>
#include <string.h>

//ƽ̨��ͷ�ļ�
#include "../include/comport.h"
#include "../include/xj_assert.h"
#include "../include/error.h"

int main()
{
	int ret,ref;
	inittest();

	//���Ի����ĳ�ʼ��
	u8 port1 = 2;
	u8 mode = COMPORT_MODE_NORMAL;
	ret = comport_init(port1, mode);    //��ͨ����
	if(ret < 0){
		printf("init error!\n");
		goto error;
	}
	u8 port2 = 5;
	mode = COMPORT_MODE_NORMAL;
	ret = comport_init(port2, mode);
	if(ret < 0){
		printf("init error!\n");
		goto error;
	}
	comport_config_t fig = {COMPORT_VERIFY_NO, 8,  1, 20, 9600, COMPORT_RTSCTS_DISABLE};  //������ʽ
	ret = comport_setconfig (port1, &fig);
	if(ret<0){
		printf("port1 setconfig error!\n");
		goto error;
	}
	ret = comport_setconfig (port2, &fig);
	if(ret<0){
		printf("port2 setconfig error!\n");
		goto error;
	}

	/***************��������1*****************/
	u8 buf[6]={6,8,5,0,1,6};    //��port1�������ݵ�port2
	ret = comport_send(port1, buf, 6);
	if(ret < 0){
		printf("test1:send error!\n");
		goto error;
	}
	sleep(3);    				//�ȴ�д�뻺��
	u8 buf_rev[20];
	memset(buf_rev,0,20);
	ret = comport_flush(port2, COMPORT_FLUSH_ALL);   //���port2���ͽ��ջ���
	ref = comport_recv(port2, buf_rev, 6);
	assert((ret==0) && (ref<0),"test1:flush error");

	/***************��������2*****************/
	ret = comport_send(port1, buf, 6);
	if(ret < 0){
		printf("test2:send error!\n");
		goto error;
	}
	sleep(3);
	ret = comport_flush(port2, COMPORT_FLUSH_RD);   //���port2���ջ���
	ref = comport_recv(port2, buf_rev, 6);
	assert((ret==0) && (ref<0),"test2:flush error");

	/***************��������3*****************/
	ret = comport_send(port1, buf, 6);
	if(ret < 0){
		printf("test3:send error!\n");
		goto error;
	}
	sleep(3);
	ret = comport_flush(port1, COMPORT_FLUSH_WR);   //���port1���ͻ���
	assert(ret==0,"test3:flush error");

	/***************��������4(�߽����)*****************/
	port1 = 0;
	mode = COMPORT_MODE_NORMAL;
	ret = comport_init(port1, mode);
	if(ret < 0){
		printf("test4:init error!\n");
		goto error;
	}
	ret = comport_flush(port1, COMPORT_FLUSH_ALL);   //���port1���ͽ��ջ���
	assert(ret==0,"test4:flush error");

	/***************��������5(�߽����)*****************/
	port1 = 6;
	mode = COMPORT_MODE_NORMAL;
	ret = comport_init(port1, mode);
	if(ret < 0){
		printf("test5:init error!\n");
		goto error;
	}
	ret = comport_flush(port1, COMPORT_FLUSH_ALL);   //���port1���ͽ��ջ���
	assert(ret==0,"test5:flush error");

	/***************��������6(�������)*****************/
	port1 = 10;                                      //�ӿڴ���
	ret = comport_flush(port1, 12);
	assert(ret== -ERR_INVAL,"test6:flush error");

	/***************��������7(�������)*****************/
	port1 = 2;
	ret = comport_flush(port1, 12);                  //mode�ӿڴ���
	assert(ret== -ERR_INVAL,"test7:flush error");

	/***************��������8(�������)*****************/
	port1 = 4;
	ret = comport_flush(port1, COMPORT_FLUSH_RD);   //���δ�򿪴���
	assert(ret== -ERR_NOINIT,"test8:flush error");


	finaltest();
error:
	exit(0);
}
