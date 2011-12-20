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
    �ļ���      ��  c-tx-net-serverdisconnect.c
    ����        ��  ���ļ�����ƽ̨��net_server_disconnect������ȫ����������
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

	//�����ĳ�ʼ��,�ÿͻ�������
	ret = net_server_init(3333,100);
	p_err(ret);
	ret = net_server_listen(&id,NET_LISTEN);
	p_err(ret);
	ret = net_server_connect(&id);
	p_err(ret);

	//��������1
	ret = net_server_disconnect(id);
	assert(ret == 0,"net_server_disconnect Use Case 1 error");

	//��������2
	ret = net_server_disconnect(id);
	assert(ret == -ERR_BUSY,"net_server_disconnect Use Case 2 error");

	//��������3
	ret = net_server_disconnect(8);
	assert(ret == -ERR_INVAL,"net_server_disconnect Use Case 3 error");

	finaltest();
	exit(0);
}
