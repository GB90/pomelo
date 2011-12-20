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
    �ļ���         ��  c-tx-thread-cancle.c
    ����	       ��  ���ļ�����ƽ̨��thread_cancle�����Ĳ���
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
	printf("thread_test1\n");
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
	printf("thread_test2\n");
	for(i=1;i<100;i++)
	{
		num++;
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
	ret = thread_init();                //��ʼ��ģ�黷��
	if(ret){
		printf("thread init error!\n");
		goto error;
	}

	id1 = 2;
	ret = thread_create(id1, (void *)thread_test1, NULL, THREAD_MODE_NORMAL, 0);             //������ͨ�߳�
	if(ret){
		printf("thread1 create error!\n");
		goto error;
	}
	id2 = 3;
	u8 test_num = 10;
	u8 prio = 5;
	ret = thread_create(id2, (void *)thread_test2, &test_num, THREAD_MODE_REALTIME, prio);   //����ʵʱ�߳�
	if(ret){
		printf("thread1 create error!\n");
		goto error;
	}
	sleep(5);
	/*********��������1**************/
	ret = thread_cancel(id1);              //��ͨ�߳�ȡ��
	assert(ret==0,"test1:thread cancel fail!");

	/*********��������2**************/
	ret = thread_cancel(id2);              //ʵʱ�߳�ȡ��
	assert(ret==0,"test2:thread cancel fail!");

	/*********��������3**************/
	id1 = 1;
	ret = thread_create(id1, (void *)thread_test1, NULL, THREAD_MODE_NORMAL, 0);             //������ͨ�߳�
	if(ret){
		printf("thread1 create error!\n");
		goto error;
	}
	ret = thread_cancel(id1);              //��ͨ�߳�ȡ��
	assert(ret==0,"test3:thread cancel fail!");

	/*********��������4**************/
	id1 = 31;
	ret = thread_create(id1, (void *)thread_test1, NULL, THREAD_MODE_NORMAL, 0);             //������ͨ�߳�
	if(ret){
		printf("thread1 create error!\n");
		goto error;
	}
	ret = thread_cancel(id1);              //��ͨ�߳�ȡ��
	assert(ret==0,"test4:thread cancel fail!");

	/*********��������5**************/
	id1 = 32;
	ret = thread_cancel(id1);              //��ͨ�߳�ȡ��
	assert(ret==-ERR_INVAL,"test5:thread cancel fail!");

	/*********��������6**************/
	id1 = 6;
	ret = thread_cancel(id1);              //��ͨ�߳�ȡ��
	assert(ret==-ERR_NODEV,"test6:thread cancel fail!");

	sleep(5);
	finaltest();

error:
	exit(0);
}
