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
    �ļ���         ��  c-tx-thread-create.c
    ����	       ��  ���ļ�����ƽ̨��thread_create�����Ĳ���
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
#include "../include/thread.h"
#include "../include/xj_assert.h"
#include "../include/error.h"


void thread_test1(void)
{
	int i;
	//printf("thread_test1\n");
	for(i=1;i<100;i++)
	{
	}
	while(1){
		sleep(1);
	}

}
void thread_test2(u8 *num)
{
	int i;
	int n;
	for(i=1;i<100;i++)
	{
		n++;
	}
	printf("num = %d\n",*num);
	while(1){
		sleep(1);
	}

}

int main()
{
	int ret;
	//�����ĳ�ʼ��
	inittest();
	u8 id1,id2;

	/*********��������10**************/
	id1 = 2;
	ret = thread_create(id1, (void *)thread_test1, NULL, THREAD_MODE_NORMAL, 0);             //������ͨ�߳�
	assert(ret==-ERR_NOINIT,"test10:thread create error!");

	ret = thread_init();                //��ʼ��ģ�黷��
	if(ret){
		printf("thread init error!\n");
		goto error;
	}

	/*********��������1**************/
	id1 = 2;
	ret = thread_create(id1, (void *)thread_test1, NULL, THREAD_MODE_NORMAL, 0);             //������ͨ�߳�
	assert(ret==0,"test1:thread create error!");

	/*********��������2**************/
	id2 = 3;
	u8 test_num = 10;
	ret = thread_create(id2, (void *)thread_test2, &test_num, THREAD_MODE_NORMAL, 0);   //������ͨ�̣߳�������
	assert(ret==0,"test1:thread create error!");

	/*********��������3**************/
	id1 = 5;
	u8 prio = 3;
	ret = thread_create(id1, (void *)thread_test1, NULL, THREAD_MODE_REALTIME, prio);       //����ʵʱ�߳�
	assert(ret==0,"test3:thread create error!");

	/*********��������4**************/
	id2 = 6;
	prio = 5;
	ret = thread_create(id2, (void *)thread_test2, &test_num, THREAD_MODE_REALTIME, prio);   //����ʵʱ�̣߳�������
	assert(ret==0,"test4:thread create error!");

	/*********��������5**************/
	id1 = 1;
	ret = thread_create(id1, (void *)thread_test1, NULL, THREAD_MODE_NORMAL, 0);
	assert(ret==0,"test5:thread create error!");

	/*********��������6**************/
	id2 = 31;
	ret = thread_create(id2, (void *)thread_test2, &test_num, THREAD_MODE_NORMAL, 0);
	assert(ret==0,"test6:thread create error!");

	/*********��������7**************/
	id1 = 7;
	ret = thread_create(id1, (void *)thread_test1, NULL, THREAD_MODE_REALTIME, 1);
	assert(ret==0,"test7:thread create error!");

	/*********��������8**************/
	id1 = 8;
	ret = thread_create(id1, (void *)thread_test1, NULL, THREAD_MODE_REALTIME, 99);
	assert(ret==0,"test8:thread create error!");

	/*********��������9**************/
	id2 = 9;
	ret = thread_create(32, (void *)thread_test2, &test_num, THREAD_MODE_NORMAL, 0);   //������ͨ�̣߳�������
	assert(ret==-ERR_INVAL,"test9:thread create error!");

	/*********��������11**************/
	id2 = 8;
	ret = thread_create(id2, (void *)thread_test2, &test_num, THREAD_MODE_NORMAL, 0);   //����ͬһ��ͨ�̣߳�������
	assert(ret==-ERR_BUSY,"test11:thread create error!");

	/*********��������12**************/
	id2 = 10;
	ret = thread_create(id2, (void *)thread_test2, &test_num, 8, 0);   //������ͨ�̣߳�������
	assert(ret==-ERR_INVAL,"test12:thread create error!");

	/*********��������13**************/
	id1 = 11;
	ret = thread_create(id1, (void *)thread_test1, NULL, THREAD_MODE_NORMAL, 1);             //������ͨ�߳�
	assert(ret==-ERR_INVAL,"test13:thread create error!");

	/*********��������14**************/
	id1 = 12;
	ret = thread_create(id1, (void *)thread_test1, NULL, THREAD_MODE_REALTIME, 0);             //������ͨ�߳�
	assert(ret==-ERR_INVAL,"test14:thread create error!");

	sleep(5);
	finaltest();

error:
	exit(0);
}

