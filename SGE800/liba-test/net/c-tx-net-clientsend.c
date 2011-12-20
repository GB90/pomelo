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
    �ļ���      ��  c-tx-net-client_send.c
    ����        ��  ���ļ�����ƽ̨��net_client_send������ȫ����������
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep
#include <string.h>
//ƽ̨��ͷ�ļ�
#include "../include/net.h"
#include "../include/xj_assert.h"
#include "../include/error.h"

int main()
{
	int ret;
	u8 buf[10]="hello!";

	//�����ĳ�ʼ��
	ret = net_client_init(5);
	p_err(ret);
	ret = net_client_connect("192.168.2.100",3333);
	p_err(ret);

	//��������1,�������� �����ַ���hello
	ret = net_client_send(buf,6);
	assert(ret == 6,"net_client_send Use Case 1 error");

//	//��������5-�߽磬�������� �����ַ���hello
//	ret = net_client_send(buf,0xffff);
//	assert(ret == 0,"net_client_send Use Case 5 error");

	//��������2-����
	ret = net_client_send(buf,0);
	assert(ret == -ERR_INVAL,"net_client_send Use Case 2 error");

	//��������4-����
	ret = net_client_disconnect();
	p_err(ret);
	ret = net_client_send(buf,6);
	assert(ret == -ERR_TIMEOUT,"net_client_send Use Case 4 error");

	//��������3-����
	ret = net_client_init(5);
	p_err(ret);
	ret = net_client_connect("192.168.2.100",3333);
	p_err(ret);
	system("ifconfig eth0 down");		 //�Ͽ�����
	sleep(2);
	ret = net_client_send(buf,6);
	assert(ret == 6,"net_client_send Use Case 3 error");
	p_err(ret);

	p_err(ret);

	system("ifconfig eth0 up");			 //��������

	finaltest();
	exit(0);
}
