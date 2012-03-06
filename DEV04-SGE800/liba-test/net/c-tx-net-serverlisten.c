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
    �ļ���      ��  c-tx-net-serverlisten.c
    ����        ��  ���ļ�����ƽ̨��net_server_listen������ȫ����������
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
	u8 id;
	//�����ĳ�ʼ��
	ret = net_server_init(3333,2);
	p_err(ret);

	//��������1
	ret = net_server_listen(&id,NET_LISTEN);
	assert(ret == 0,"net_server_listen Use Case 1 error");

	//��������2
	ret = net_server_listen(&id,NET_LISTEN_SELECT);
	assert((ret == 0)&&(id == 0xff),"net_server_listen Use Case 2 error");

	//��������3-�߽�
	ret = net_server_listen(&id,2);
	assert(ret == -ERR_INVAL,"net_server_listen Use Case 3 error");

	//��������4-�߽�
	ret = net_server_listen(&id,0xff);
	assert(ret == -ERR_INVAL,"net_server_listen Use Case 4 error");

	finaltest();
	exit(0);
}
