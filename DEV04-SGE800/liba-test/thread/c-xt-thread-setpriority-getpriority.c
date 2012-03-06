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
    �ļ���         ��  c-xt-thread-setpriority-getpriority.c
    ����	       ��  ���ļ�����ƽ̨��thread-setpriority-getpriority�����Ĳ���
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

u8 test_prio=0;
void thread_test1(void)
{
	int i;
	printf("thread_test1\n");
	if (test_prio== 0){
		test_prio = 1;
	}
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
	if (test_prio== 0){
		test_prio = 2;
	}
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
	int ret,ret1;
	//�����ĳ�ʼ��
	inittest();
	u8 id1,id2;
	ret = thread_init();                //��ʼ��ģ�黷��
	if(ret){
		printf("thread init error!\n");
		goto error;
	}

	id1 = 2;
	u8 test_num = 20;
	u8 prio = 5;
	ret = thread_create(id1, (void *)thread_test2, &test_num, THREAD_MODE_REALTIME, prio);   //����ʵʱ�߳�
	if(ret){
		printf("thread create error!\n");
		goto error;
	}
	/*********��������1**************/
	u8 prio_set = 6;
	u8 prio_get;
	ret = thread_setpriority(id1, prio_set);    //�������ȼ�
	ret1 = thread_getpriority(id1, &prio_get);  //��ȡ���ȼ�
	assert((ret==0)&&(ret1==0)&&(prio_set==prio_get),"test1:thread set/get prior error!");

	/********��������2*************/
	id1 = 5;
	prio = 3;
	ret = thread_create(id1, (void *)thread_test1, NULL, THREAD_MODE_REALTIME, prio);       //����ʵʱ�߳�
	if (ret<0){
		printf("test2:create thread fail!\n");
	}
	//�ж����ĸ��߳���ִ�У�����Ӧ����thread_test2�߳���ִ��
	assert(test_prio=2,"test2:prio test fail!");

	/*********��������3**************/
	id2 = 3;
	ret = thread_create(id2, (void *)thread_test2, &test_num, THREAD_MODE_NORMAL, 0);   //������ͨ�̣߳�������
	if (ret<0){
		printf("test3:create thread fail!\n");
		goto error;
	}
	ret1 = thread_getpriority(id2, &prio_get);  //��ȡ���ȼ�
	assert((ret1==0)&&(0==prio_get),"test3:thread set/get prior error!");


	/*********��������4**************/
	id1 = 2;
	prio_set = 1;
	ret = thread_setpriority(id1, prio_set);    //�������ȼ�
	ret1 = thread_getpriority(id1, &prio_get);  //��ȡ���ȼ�
	assert((ret==0)&&(ret1==0)&&(prio_set==prio_get),"test4:thread set/get prior error!");

	/*********��������5**************/
	prio_set = 99;
	ret = thread_setpriority(id1, prio_set);    //�������ȼ�
	ret1 = thread_getpriority(id1, &prio_get);  //��ȡ���ȼ�
	assert((ret==0)&&(ret1==0)&&(prio_set==prio_get),"test5:thread set/get prior error!");

	/*********��������6**************/
	id2 = 31;
	prio = 5;
	ret = thread_create(id2, (void *)thread_test2, &test_num, THREAD_MODE_REALTIME, prio);   //������ͨ�̣߳�������
	if (ret<0){
		printf("test6:create thread fail!\n");
		goto error;
	}
	prio_set = 1;
	ret = thread_setpriority(id2, prio_set);    //�������ȼ�
	ret1 = thread_getpriority(id2, &prio_get);  //��ȡ���ȼ�
	assert((ret==0)&&(ret1==0)&&(prio_set==prio_get),"test6:thread set/get prior error!");

	/*********��������7**************/
	ret = thread_setpriority(32, prio_set);    //�������ȼ�
	ret1 = thread_getpriority(32,&prio_get);  //��ȡ���ȼ�
	assert(((ret==-ERR_INVAL)&&(ret1==-ERR_INVAL)),"test7:thread set/get prior error!");

	/*********��������8**************/
	id1 = 2;
	prio_set = 0;
	ret = thread_setpriority(id1, prio_set);    //�������ȼ�
	assert(ret=-ERR_INVAL,"test8:thread set/get prior error!");

	/*********��������9**************/
	id1 = 2;
	prio_set = 100;
	ret = thread_setpriority(id1, prio_set);    //�������ȼ�
	assert(ret=-ERR_INVAL,"test9:thread set/get prior error!");

	/*********��������10**************/
	id2 = 10;
	ret = thread_setpriority(id2, prio_set);    //�������ȼ�
	ret1 = thread_getpriority(id2, &prio_get);  //��ȡ���ȼ�
	assert(((ret==-ERR_NODEV)&&(ret1==-ERR_NODEV)),"test10:thread set/get prior error!");

	/*********��������11**************/
	id2 = 3;
	prio_set = 1;
	ret = thread_setpriority(id2, prio_set);    //��ͨ�߳��������ȼ�
	assert(ret=-ERR_INVAL,"test9:thread set/get prior error!");

	/*********��������12**************/
	id2 = 0;
	prio_set = 10;
	ret = thread_setpriority(id2, prio_set);    //�������߳����ȼ�
	ret1 = thread_getpriority(id2, &prio_get);  //��ȡ���ȼ�
	assert(((ret==0)&&(ret1==0)&&(prio_get==10)),"test12:thread set/get prior error!");

	while(1){

	}



	finaltest();
error:
	exit(0);
}

