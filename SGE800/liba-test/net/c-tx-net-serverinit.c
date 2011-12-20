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
    �ļ���      ��  c-tx-net-serverinit.c
    ����        ��  ���ļ�����ƽ̨��net_server_init������ȫ����������
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
	//�����ĳ�ʼ��

	//��������1
	ret = net_server_init(3333,2);
	assert(ret == 0,"net_server_init Use Case 1 error");

	//��������2-����
	ret = net_server_init(0,2);
	assert(ret == 0,"net_server_init Use Case 2 error");


	//��������3-����
	ret = net_server_init(23,2);
	assert(ret == -ERR_INVAL,"net_server_init Use Case 3 error");

	//��������4-�߽�
	ret = net_server_init(65535,2);
	assert(ret == 0,"net_server_init Use Case 4 error");

	//��������5-�߽�
	ret = net_server_init(4444,65535);
	assert(ret == 0,"net_server_init Use Case 5 error");

	//��������5-�߽�
	ret = net_server_init(5555,0);
	assert(ret == 0,"net_server_init Use Case 5 error");

	finaltest();
	exit(0);
}
