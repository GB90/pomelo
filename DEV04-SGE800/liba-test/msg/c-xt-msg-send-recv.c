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
    �ļ���         ��  c-tx-msg-send-recv.c
    ����	       ��  ���ļ�����ƽ̨��msg_send������msg_recv�����Ĳ���
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
#include "../include/msg.h"
#include "../include/thread.h"
#include "../include/error.h"
#include "../include/xj_assert.h"

msg_t message_send1 = {1,2,3,4};
msg_t message_send2 = {2,6,7,8};
msg_t message_send3 = {3,2,3,4};
u8 message = 0;

void thread_test1()
{
	int ref,i;
	u8 id;
	u8 flag;
	msg_t message_rev;
	msg_t message_send;
	id = 2;
	//printf("block rev start!\n");
	ref = msg_recv(id, &message_rev, 0);                          //���õȴ�
	/*************��������1********************/
	assert((message_rev.type==message_send1.type) && (message_rev.bPara==message_send1.bPara)
		 &&(message_rev.lPara==message_send1.lPara) && (message_rev.wPara==message_send1.wPara)
		 && (ref==0), "test1:send-blockrev msg fail!");

	/*************��������2********************/
	memset(&message_rev, 0, sizeof(msg_t));
	flag = 1;
	while(flag){   //ѭ������
		ref = msg_recv(id, &message_rev, MSG_RECV_NONBLOCK);      //���ȴ�
		if (ref == -ERR_TIMEOUT){
			flag = 1;
			sleep(1);
		}else if(ref<0){
			flag = 0;
			assert(ref==0,"test2:send-nonblockrev msg fail!");
		}else{
			flag = 0;   //ֹͣѭ������
			assert((message_rev.type==message_send2.type) && (message_rev.bPara==message_send2.bPara)
					&&(message_rev.lPara==message_send2.lPara) && (message_rev.wPara==message_send2.wPara)
					&& (ref==0), "test2��send-nonblockrev msg fail!");
		}
	}
	/*************��������3********************/
	u8 timeout = 10;
	flag = 1;
	while(flag){
		system("date");
		ref = msg_recv(id, &message_rev, timeout);       //��ʱ�ȴ�
		if (ref == -ERR_TIMEOUT){
			//printf("test3:timeout\n");
			system("date");
			flag = 1;
			sleep(1);
		}else if(ref<0){
			flag = 0;
			assert(ref==0,"test3��send-timeoutblockrev msg fail!");
		}else{
			flag = 0;
			assert((message_rev.type==message_send3.type) && (message_rev.bPara==message_send3.bPara)
					&&(message_rev.lPara==message_send3.lPara) && (message_rev.wPara==message_send3.wPara)
					&& (ref==0), "test3��send-timeoutblockrev msg fail!");
			system("date");
		}
	}
	/*************��������7********************/
	id = 31;
	ref = msg_recv(id, &message_rev, 0);
	assert((message_rev.type==message_send3.type) && (message_rev.bPara==message_send3.bPara)
			&&(message_rev.lPara==message_send3.lPara) && (message_rev.wPara==message_send3.wPara)
			&& (ref==0), "test7��send-rev msg fail!");

	/*************��������8********************/
	id = 0;
	ref = msg_recv(id, &message_rev, 0);
	assert((message_rev.type==message_send2.type) && (message_rev.bPara==message_send2.bPara)
			&&(message_rev.lPara==message_send2.lPara) && (message_rev.wPara==message_send2.wPara)
			&& (ref==0), "test8��send-nonblockrev msg fail!");

	/*************��������4�����ܲ��ԣ�***************/
	id = 5;
	for (i = 1; i<10; i++){
		message++;
		message_send.type = message;
		ref = msg_send(id, &message_send, 5);      //������Ϣ
		if (ref){
			printf("test4:send msg fail!\n");
		}
	}

	sleep(5);

}

void thread_test2()
{
	int ret,i;
	u8 id;
	msg_t message_send;
	/*************��������4�����ܲ��ԣ�***************/
	id = 5;
	for (i = 1; i<10; i++){
		message++;
		message_send.type = message;
		ret = msg_send(id, &message_send, 5);      //������Ϣ
		if (ret){
			printf("test2:send msg2 fail!\n");
		}
		sleep(1);
	}
	while(1){

	}

}

void thread_test3()
{
	int ret,i;
	u8 id;
	msg_t message_send;

	/*************��������4�����ܲ��ԣ�***************/
	id = 5;
	for (i = 1; i<10; i++){
		message++;
		message_send.type = message;
		ret = msg_send(id, &message_send, 5);      //������Ϣ
		if (ret){
			printf("test2:send msg2 fail!\n");
		}
		sleep(1);
	}

	while(1){

	}

}

int main()
{
	int ret,ref,i;
	msg_t message_rev;
	//�����ĳ�ʼ��
	inittest();
	u8 thread_id1 = 7;
	ret = thread_init();                //�߳�ģ���ʼ��
	if (ret){
		printf("init thread fail!\n");
		goto error;
	}
	ret = msg_init();                  //��Ϣģ���ʼ��
	if (ret){
		printf("msg init fail!\n");
		goto error;
	}
	u8 id1 = 2;
	ret = thread_create(thread_id1, (void *)thread_test1, NULL, THREAD_MODE_NORMAL, 0);   //���������߳�
	if (ret){
		printf("thread create fail!\n");
		goto error;
	}
	ret = msg_send(id1, &message_send1, 5);      //���͵�һ����Ϣ
	if (ret){
		assert(ret==0,"test1:send msg1 fail!");   //����1
		goto error;
	}
	sleep(1);
	ret = msg_send(id1, &message_send2, 5);      //���͵ڶ�����Ϣ
	if (ret){
		assert(ret==0,"test2:send msg2 fail!");   //����2
		goto error;
	}
	sleep(1);
	ret = msg_send(id1, &message_send3, 5);      //���͵�������Ϣ
	if (ret){
		assert(ret==0,"test3:send msg3 fail!");   //����3
		goto error;
	}


	/*******��������7************************/
	id1 = 31;
	ret = msg_send(id1, &message_send3, 3);
	if (ret){
		assert(ret==0,"test7:send msg3 fail!");
		goto error;
	}
	/*******��������8************************/
	id1 = 0;
	ret = msg_send(id1, &message_send2, 8);
	if (ret){
		assert(ret==0,"test8:send msg2 fail!");
		goto error;
	}
	/*******��������9************************/
	id1 = 32;
	ret = msg_send(id1, &message_send3, 3);
	ref = msg_recv(id1, &message_rev, 0);
	assert((ret==-ERR_NODEV)&&(ref==-ERR_NODEV),"test9:send-rev fail!");

	/*******��������10************************/
	id1 = 5;
	ret = msg_send(id1, &message_send2, 100);
	assert(ret==-ERR_INVAL,"test10:send-rev fail!");


	thread_id1 = 5;
	ret = thread_create(thread_id1, (void *)thread_test2, NULL, THREAD_MODE_NORMAL, 0);
	if (ret){
		printf("thread create fail!\n");
		goto error;
	}

	thread_id1 = 6;
	ret = thread_create(thread_id1, (void *)thread_test3, NULL, THREAD_MODE_NORMAL, 0);
	if (ret){
		printf("thread create fail!\n");
		goto error;
	}

	//��������4���ܲ���
	id1 = 5;
	u8 flag;
	for(i=1;i<28;i++){
		ref = msg_recv(id1, &message_rev, 0);                          //���õȴ�
		if (message_rev.type == i){
			flag = 1;
		}else{
			flag = 0;
			i = 28;
		}
	}
	assert(flag == 1,"test4:send-rev fail!");

	//��������5���ܲ���
	id1 = 6;
	for(i=1;i<65;i++){
		ret = msg_send(id1, &message_send1, 5);      //������Ϣ--ʹ��Ϣ������
		if (ret){
			printf("test5:send msg fail!\n");
		}
	}
	ret = msg_send(id1, &message_send1, 5);
	assert(ret==-ERR_NOMEM,"test5:send msg fail!");

	id1 = 28;
	u8 timeout = 3;
	ret = msg_recv(id1, &message_rev, timeout);       //��ʱ�ȴ�
	assert(ret== -ERR_TIMEOUT,"test6:send-recv fail!");



	sleep(5);
	finaltest();
error:
	exit(0);
	}
