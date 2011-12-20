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
    �ļ���         ��  c-tx-msg-clear.c
    ����	       ��  ���ļ�����ƽ̨��msg_clear�����Ĳ���
    �汾              ��  0.1
    ����              ��  ����
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>			//exit
#include <unistd.h>			//sleep
#include <db.h>

//ƽ̨��ͷ�ļ�
#include "../include/msg.h"
#include "../include/thread.h"
#include "../include/xj_assert.h"
#include "../include/error.h"

msg_t message_send[3]= {{1,2,3,4}, {2,6,7,8}, {3,2,3,4}};
void thread_test1()
{
	int ref;
	u8 id;
	msg_t message_rev;
	id = 2;

	/*************��������1********************/
	u8 timeout = 5;
	system("date");
	ref = msg_recv(id, &message_rev, timeout);       //��ʱ�ȴ�
	assert(ref==-ERR_TIMEOUT,"teset1:clear msg error!");
	system("date");

	/*************��������2********************/
	system("date");
	id = 0;
	ref = msg_recv(id, &message_rev, timeout);       //��ʱ�ȴ�
	assert(ref==-ERR_TIMEOUT,"test2:clear msg error!");
	system("date");


	/*************��������3********************/
	system("date");
	id = 31;
	ref = msg_recv(id, &message_rev, timeout);       //��ʱ�ȴ�
	assert(ref==-ERR_TIMEOUT,"test3:clear msg error!");
	system("date");
	while(1){

	}
}


int main()
{
	int ret,i;
	//�����ĳ�ʼ��
	inittest();
	u8 thread_id = 7;
	ret = thread_init();                //�߳�ģ���ʼ��
	if (ret){
		printf("init thread fail!\n");
		goto error;
	}
	//��������5
	u8 id1 = 2;
	ret = msg_clear(id1);
	assert(ret == -ERR_NOINIT,"test5:clear msg error!");
	ret = msg_init();                  //��Ϣģ���ʼ��
	if (ret){
		printf("msg init fail!\n");
		goto error;
	}
	id1 = 2;
	u8 prio = 5;
	for(i=0;i<3;i++){
		ret = msg_send(id1, &message_send[i], prio);   //������Ϣ
		if (ret){
			printf("test1:send fail!\n");
		}
		prio++;
	}

	id1 = 0;
	prio = 6;
	for(i=0;i<3;i++){
		ret = msg_send(id1, &message_send[i], prio);   //������Ϣ
		if (ret){
			printf("test1:send fail!\n");
		}
		prio++;
	}

	id1 = 31;
	prio = 6;
	for(i=0;i<3;i++){
		ret = msg_send(id1, &message_send[i], prio);   //������Ϣ
		if (ret){
			printf("test1:send fail!\n");
		}
		prio++;
	}
	//��������1
	id1 = 2;
	ret = msg_clear(id1);
	if (ret){
		assert(ret == 0,"test1:clear msg error!");
		goto error;
	}
	printf("msg clear!\n");

	//��������2
	id1 = 0;
	ret = msg_clear(id1);
	if (ret){
		assert(ret == 0,"test2:clear msg error!");
		goto error;
	}
	printf("msg clear!\n");

	//��������3
	id1 = 31;
	ret = msg_clear(id1);
	if (ret){
		assert(ret == 0,"test3:clear msg error!");
		goto error;
	}
	printf("msg clear!\n");

	ret = thread_create(thread_id, (void *)thread_test1, NULL, THREAD_MODE_NORMAL, 0);   //���������߳�
	if (ret){
		printf("thread create fail!\n");
		goto error;
	}
	//��������4
	id1 = 32;
	ret = msg_clear(id1);
	assert(ret==-ERR_NODEV,"test4:clear msg error!\n");
	sleep(15);
	finaltest();
error:
	exit(0);
	}
