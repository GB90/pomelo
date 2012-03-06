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
    �ļ���      ��  c-tx-net-serversend.c
    ����        ��  ���ļ�����ƽ̨��net_server_send������ȫ����������
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
	u8 id;
	unsigned char buf[10]="hello";
	//�����ĳ�ʼ��
	ret = net_server_init(3333,5);
	p_err(ret);
	ret = net_server_listen(&id,NET_LISTEN);
	p_err(ret);
	ret = net_server_connect(&id);
	p_err(ret);

	//��������1
	ret = net_server_send(id,buf,5);
	assert(ret == 5,"net_server_send Use Case 1 error");

	//��������2-����
	ret = net_server_send(8,buf,5);
	assert(ret == -ERR_INVAL,"net_server_send Use Case 2 error");

	//��������3-����
	ret = net_server_send(id,buf,0);
	assert(ret == -ERR_INVAL,"net_server_send Use Case 3 error");

	//��������4-����
	system("ifconfig eth0 down");
	ret = net_server_send(id,buf,6);
	assert(ret == -ERR_DISCONNECT,"net_server_send Use Case 4 error");
	system("ifconfig eth0 up");

	finaltest();
	exit(0);
}
