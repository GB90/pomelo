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
    �ļ���         ��  c-tx-msg-send-recvprio.c
    ����	       ��  ���ļ�����ƽ̨��msg_send������msg_recvprio�����Ĳ���
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
#include "../include/thread.h"
#include "../include/error.h"
#include "../include/msg.h"
#include "../include/xj_assert.h"

msg_t message_send[3]= {{1,2,3,4}, {2,6,7,8}, {3,2,3,4}};
u8 message = 0;
void thread_test1()
{
	int ref;
	u8 id,i;
	msg_t message_rev;

	u8 result;
	printf("rev test thread1 start!\n");
	/*************�������ܲ�������1********************/
	id = 3;
	for(i=3;i>0;i--){
		memset(&message_rev, 0 ,sizeof(msg_t));
		ref = msg_recv_prio(id, &message_rev, 0);              //���õȴ�
		if ((message_rev.type==message_send[i-1].type) && (message_rev.bPara==message_send[i-1].bPara)
			 &&(message_rev.lPara==message_send[i-1].lPara) && (message_rev.wPara==message_send[i-1].wPara)
			 && (ref==0)){
			result = 1;
		}else{
			result = 0;
			i = 0;
		}
	}
	assert(result == 1,"test1:send-revpiro msg fail!");

	/*************��������7********************/
	id = 31;
	memset(&message_rev, 0 ,sizeof(msg_t));
	ref = msg_recv_prio(id, &message_rev, 0);
	assert((message_rev.type==message_send[1].type) && (message_rev.bPara==message_send[1].bPara)
			&&(message_rev.lPara==message_send[1].lPara) && (message_rev.wPara==message_send[1].wPara)
			&& (ref==0), "test7��send-rev msg fail!");

	/*************��������8********************/
	id = 0;
	memset(&message_rev, 0 ,sizeof(msg_t));
	ref = msg_recv_prio(id, &message_rev, 0);
	assert((message_rev.type==message_send[1].type) && (message_rev.bPara==message_send[1].bPara)
			&&(message_rev.lPara==message_send[1].lPara) && (message_rev.wPara==message_send[1].wPara)
			&& (ref==0), "test8:send-rev msg fail!");

	/*************��������9********************/
	id = 7;
	memset(&message_rev, 0 ,sizeof(msg_t));
	ref = msg_recv_prio(id, &message_rev, 0);
	assert((message_rev.type==message_send[1].type) && (message_rev.bPara==message_send[1].bPara)
			&&(message_rev.lPara==message_send[1].lPara) && (message_rev.wPara==message_send[1].wPara)
			&& (ref==0), "test8��send-rev msg fail!");

	/*************��������10********************/
	id = 32;  //Խ��
	memset(&message_rev, 0 ,sizeof(msg_t));
	ref = msg_recv_prio(id, &message_rev, 0);
	assert(ref==-ERR_NODEV,"test10:send-rev msg fail!");

	while(1){
	}

}
void thread_test2()
{
	int ref;
	u8 id,i,flag;
	u8 result = 0;
	msg_t message_rev;

	printf("rev test thread2 start!\n");
	/*************�������ܲ�������2********************/
	id = 4;
	result = 0;
	for(i=3;i>0;i--){
		flag = 1;
		while(flag){   //ѭ������
			memset(&message_rev, 0 ,sizeof(msg_t));
			ref = msg_recv_prio(id, &message_rev, MSG_RECV_NONBLOCK);      //���ȴ�
			if (ref == -ERR_TIMEOUT){
				flag = 1;
				sleep(1);
			}else if(ref<0){
				flag = 0;
				assert(ref==0,"test2:send-rev msg fail!");
				i = 1;
				printf("a\n");
			}else{
				flag = 0;   //ֹͣѭ������
				if ((message_rev.type==message_send[i-1].type) && (message_rev.bPara==message_send[i-1].bPara)
					 &&(message_rev.lPara==message_send[i-1].lPara) && (message_rev.wPara==message_send[i-1].wPara)
					 && (ref==0)){
					result = 1;
				}else{
					result = 0;
					i = 1;
				}
			}
		}
	}
	assert(result == 1,"test2:send-revprio msg fail!");
	while(1){

	}
}

void thread_test3()
{
	int ref;
	u8 id,i,flag,prio;
	u8 result = 0;
	msg_t message_rev;

	id = 5;
	u8 timeout = 10;
	printf("rev test thread3 start!\n");

	/*************�������ܲ�������2********************/
	for(i=3;i>0;i--){
		flag = 1;
		while(flag){   //ѭ������
			memset(&message_rev, 0 ,sizeof(msg_t));
			ref = msg_recv_prio(id, &message_rev, timeout);       //��ʱ�ȴ�
			if (ref == -ERR_TIMEOUT){
				flag = 1;
				sleep(1);
			}else if(ref<0){
				flag = 0;
				assert(ref==0,"test3:send-nonblockrev msg fail!");
				i=1;
			}else{
				flag = 0;   //ֹͣѭ������
				if ((message_rev.type==message_send[i-1].type) && (message_rev.bPara==message_send[i-1].bPara)
					 &&(message_rev.lPara==message_send[i-1].lPara) && (message_rev.wPara==message_send[i-1].wPara)
					 && (ref==0)){
					result = 1;
				}else{
					result = 0;
					i = 1;
					printf("ff\n");
				}
			}
		}
	}
	assert(result == 1,"test3:send-revprio msg fail!");

	//��������4���ܲ���
	sleep(5);
	printf("rev\n");
	id = 11;
	prio = 8;
	for(i=1;i<5;i++){
		memset(&message_rev, 0, sizeof(message_rev));
		ref = msg_recv_prio(id, &message_rev, 0);                          //���õȴ�
		if (message_rev.type == prio){
			flag = 1;
			prio--;
		}else{
			flag = 0;
		}
		printf("prio_rev = %d\n",message_rev.type);
	}
	assert(flag == 1,"test4:send-rev fail!");
	while(1){

	}
}

int main(void)
{
	int ref;
	u8 id,i;
	msg_t message_rev;
	msg_t message_s={1,2,3,4};

	ref = thread_init();                //ģ���ʼ��
	if (ref){
		printf("init thread fail!\n");
		goto error;
	}
	/*********��������11*******************/
	id = 2;
	ref = msg_recv_prio(id, &message_rev, 0);
	assert(ref==-ERR_NOINIT,"test11: send-revprio fail!");  //δ��ʼ������

	ref = msg_init();            		//��ʼ����Ϣģ��
	if (ref){
		printf("init msg fail!\n");
		goto error;
	}
	/*********��������1********************/
	u8 prio = 5;
	id = 3;
	for(i=3;i<6;i++){
		ref = msg_send(id, &message_send[i-3], prio);   //������Ϣ
		if (ref){
			printf("test1:send fail!\n");
		}
		prio++;
	}
	u8 id_test;
	id_test = 6;
	ref = thread_create(id_test, (void *)thread_test1, NULL, THREAD_MODE_NORMAL, 0);   //���������߳�
	if (ref){
		printf("test1:thread create fail!\n");
		goto error;
	}
	/*********��������2********************/
	prio = 5;
	id = 4;
	for(i=0;i<3;i++){
		ref = msg_send(id, &message_send[i], prio);   //������Ϣ
		if (ref){
			printf("test2:send fail!\n");
		}
		prio++;
	}
	id_test = 7;
	ref = thread_create(id_test, (void *)thread_test2, NULL, THREAD_MODE_NORMAL, 0);   //���������߳�
	if (ref){
		printf("test2:thread create fail!\n");
		goto error;
	}
	/*********��������3********************/
	prio = 5;
	id = 5;
	for(i=0;i<3;i++){
		ref = msg_send(id, &message_send[i], prio);   //������Ϣ
		if (ref){
			printf("test3:send fail!\n");
			goto error;
		}
		prio++;
	}

	id_test = 8;
	ref = thread_create(id_test, (void *)thread_test3, NULL, THREAD_MODE_NORMAL, 0);   //���������߳�
	if (ref){
		printf("test3:thread create fail!\n");
		goto error;
	}
	//��������4��5��6���ܲ���

	/*******��������7************************/
	prio = 5;
	id = 31;
	for(i=3;i<5;i++){
		ref = msg_send(id, &message_send[i-3], prio);   //������Ϣ
		if (ref){
			printf("test7:send fail!\n");
			goto error;
		}
		prio+=5;
	}
	/*******��������8************************/
	prio = 5;
	id = 0;
	for(i=3;i<5;i++){
		ref = msg_send(id, &message_send[i-3], prio);   //������Ϣ
		if (ref){
			printf("test8:send fail!\n");
			goto error;
		}
		prio+=5;
	}
	/*******��������9************************/
	prio = 0;
	id = 7;
	for(i=3;i<5;i++){
		ref = msg_send(id, &message_send[i-3], prio);   //������Ϣ
		if (ref){
			printf("test9:send fail!\n");
			goto error;
		}
		prio+=99;
	}

/*************��������4�����ܲ��ԣ�***************/

	id = 11;
	prio = 5;
	message_s.type = prio;
	ref = msg_send(id, &message_s, prio);      //������Ϣ
	if (ref){
		printf("test4:send msg fail!\n");
	}
	prio = 7;
	message_s.type = prio;
	ref = msg_send(id, &message_s, prio);      //������Ϣ
	if (ref){
		printf("test4:send msg fail!\n");
	}
	prio = 6;
	message_s.type = prio;
	ref = msg_send(id, &message_s, prio);      //������Ϣ
	if (ref){
		printf("test4:send msg fail!\n");
	}
	prio = 8;
	message_s.type = prio;
	ref = msg_send(id, &message_s, prio);      //������Ϣ
	if (ref){
		printf("test4:send msg fail!\n");
	}
	printf("send ok!\n");

	//��������5���ܲ���
	id = 6;
	for(i=1;i<65;i++){
		ref = msg_send(id, &message_send[1], 5);      //������Ϣ--ʹ��Ϣ������
		if (ref){
			printf("test5:send msg fail!\n");
		}
	}
	ref = msg_send(id, &message_send[1], 5);
	assert(ref==-ERR_NOMEM,"test5:send msg fail!");

	//��������6���ܲ���
	id = 28;
	u8 timeout = 3;
	ref = msg_recv(id, &message_rev, timeout);       //��ʱ�ȴ�
	assert(ref== -ERR_TIMEOUT,"test6:send-recv fail!");

	sleep(15);
	finaltest();
error:
	exit(0);

}
