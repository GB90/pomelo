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
    �ļ���      ��  c-tx-net-clientdisconnect.c
    ����        ��  ���ļ�����ƽ̨��net_client_disconnect������ȫ����������
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
	ret = net_client_init(5);
	p_err(ret);
	ret = net_client_connect("192.168.2.100",3333);
	p_err(ret);
	//��������1
	ret = net_client_disconnect();
	assert(ret == 0,"net_client_disconnect Use Case 1 error");

	//��������2-����
	ret = net_client_disconnect();
	assert(ret == -ERR_BUSY,"net_client_disconnect Use Case 2 error");

	finaltest();
	exit(0);
}
