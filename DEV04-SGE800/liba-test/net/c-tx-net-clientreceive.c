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
    �ļ���      ��  c-tx-net-clientreceive.c
    ����        ��  ���ļ�����ƽ̨��net_client_receive������ȫ����������
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
	u8 buf[10];
	u16 len;

	//�����ĳ�ʼ������Ҫ�������˵����
	ret = net_client_init(8);
	p_err(ret);
	ret = net_client_connect("192.168.2.100",3333);
	p_err(ret);

	//��������1,�������˷����ַ���hello
	ret = net_client_receive(buf,255,&len,NET_BLOCK);
	buf[len]='\0';
	assert((ret == 0)&&(strcmp(buf,"hello")==0),"net_client_receive Use Case 1 error");

	//��������2-����
	ret = net_client_receive(buf,255,&len,NET_NONBLOCK);
	assert(ret == -ERR_TIMEOUT,"net_client_receive Use Case 2 error");

	//��������3-����
	ret = net_client_receive(buf,255,&len,2);
	assert(ret == -ERR_INVAL,"net_client_receive Use Case 3 error");

	//��������4-���ǣ��������˷��ʹ���8�ֽ�����
	printf("Use Case 4 start, please send >8 Byte data\n");
	ret = net_client_receive(buf,8,&len,NET_BLOCK);
	assert((ret == 0)&&(len == 8),"net_client_receive Use Case 4 error");

	finaltest();
	exit(0);
}
