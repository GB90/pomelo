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
    �ļ���      ��  c-tx-net-serverreceive.c
    ����        ��  ���ļ�����ƽ̨��net_server_receive������ȫ����������
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
	u16 len;
	unsigned char buf[10];
	//�����ĳ�ʼ��
	ret = net_server_init(3333,5);
	p_err(ret);
	ret = net_server_listen(&id,NET_LISTEN);
	p_err(ret);
	ret = net_server_connect(&id);
	p_err(ret);

	//��������1,�ͻ��˷���hello
	printf("send hello\n");
	ret = net_server_receive(id,buf,100,&len,NET_BLOCK);
	buf[len]='\0';
	assert((ret == 0)&&(strcmp(buf,"hello")==0),"net_server_receive Use Case 1 error");

	//��������2,�ͻ�����һ�η���hello
	printf("send hello\n");
	sleep(2);
	ret = net_server_receive(id,buf,100,&len,NET_NONBLOCK);
	buf[len]='\0';
	assert((ret == 0)&&(strcmp(buf,"hello")==0),"net_server_receive Use Case 2 error");

	//��������3-����
	ret = net_server_receive(id,buf,100,&len,2);
	assert(ret == -ERR_INVAL,"net_server_receive Use Case 3 error");

	//��������4-����
	ret = net_server_receive(id,buf,100,&len,NET_NONBLOCK);
	assert(ret == -ERR_TIMEOUT,"net_server_receive Use Case 4 error");

	//��������5,�ͻ�����һ�η���hello
	printf("send hello\n");
	ret = net_server_receive(id,buf,4,&len,NET_BLOCK);
	assert((ret == 0)&&(len==4),"net_server_receive Use Case 5 error");

	//��������6-�߽�
	ret = net_server_receive(8,buf,100,&len,NET_BLOCK);
	assert(ret == -ERR_INVAL,"net_server_receive Use Case 6 error");

	finaltest();
	exit(0);
}
