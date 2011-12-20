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
    �ļ���      ��  c-tx-net-clientconnect.c
    ����        ��  ���ļ�����ƽ̨��net_client_connect������ȫ����������
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep

//ƽ̨��ͷ�ļ�
#include "../include/net.h"
#include "../include/xj_assert.h"
#include "../include/error.h"

int main()
{
	int ret;

	//�����ĳ�ʼ��,����ǰ�����˿ں�Ϊ3333��4444��0��0xffff�ķ�������
	inittest();
	ret = net_client_init(2);
	p_err(ret);

	//��������2-����ʱ��5555�˿��޷�������
	ret = net_client_connect("192.168.2.100",5555);
	assert(ret == -ERR_TIMEOUT,"net_client_connect Use Case 2 error");

	//��������1
	ret = net_client_connect("192.168.2.100",3333);
	assert(ret == 0,"net_client_connect Use Case 1 error");
	ret = net_client_disconnect();
	p_err(ret);

	//��������4-�߽�
	ret = net_client_init(2);
	p_err(ret);
	ret = net_client_connect("192.168.2.100",0);
	assert(ret == -ERR_TIMEOUT,"net_client_connect Use Case 4 error");

	//��������5-�߽�
	ret = net_client_connect("192.168.2.100",65535);
	assert(ret == 0,"net_client_connect Use Case 5 error");

	ret = net_client_disconnect();
	p_err(ret);

	//��������3-��������Ͽ�
	ret = net_client_init(2);
	p_err(ret);
	system("ifconfig eth0 down");
	ret = net_client_connect("192.168.2.100",4444);
	assert(ret == -ERR_DISCONNECT,"net_client_connect Use Case 3 error");
	system("ifconfig eth0 up");

	finaltest();
	exit(0);
}
