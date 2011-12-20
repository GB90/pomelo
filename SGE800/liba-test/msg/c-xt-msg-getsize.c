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
    �ļ���         ��  c-tx-msg-getsize.c
    ����	       ��  ���ļ�����ƽ̨��msg_getsize�����Ĳ���
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
#include "../include/xj_assert.h"
#include "../include/error.h"
#include "../include/thread.h"

msg_t message_send[3]= {{1,2,3,4}, {2,6,7,8}, {3,2,3,4}};

int main()
{
	int ret,i;
	//�����ĳ�ʼ��
	inittest();
	ret = thread_init();                //�߳�ģ���ʼ��
	if (ret){
		printf("init thread fail!\n");
		goto error;
	}

	//��������6
	u8 id = 2;
	ret = msg_getsize(id);
	assert(ret == -ERR_NOINIT,"test1:msg getsize error!");

	ret = msg_init();                  //��Ϣģ���ʼ��
	if (ret){
		printf("msg init fail!\n");
		goto error;
	}
	//�������Ի���
	id = 2;
	u8 prio = 5;
	for(i=0;i<2;i++){
		ret = msg_send(id, &message_send[i], prio);   //������Ϣ
		if (ret){
			printf("test3:send fail!\n");
			goto error;
		}
		prio++;
	}

	//id = 31(�߽����)
	prio = 5;
	id = 31;
	for(i=0;i<1;i++){
		ret = msg_send(id, &message_send[i], prio);   //������Ϣ
		if (ret){
			printf("test4:send fail!\n");
			goto error;
		}
		prio++;
	}
	//id = 0(�߽����)
	prio = 5;
	id = 0;
	for(i=0;i<1;i++){
		ret = msg_send(id, &message_send[i], prio);   //������Ϣ
		if (ret){
			printf("test3:send fail!\n");
			goto error;
		}
		prio++;
	}
	//��������1
	id = 2;
	ret = msg_getsize(id);
	assert(ret == 2,"test1:msg getsize error!");

	//��������2
	ret = msg_clear(id);
	if(ret){
		printf("test2:clear msg error!");
		goto error;
	}
	ret = msg_getsize(id);
	assert(ret == 0,"test2:msg getsize error!");

	//��������3
	id = 0;
	ret = msg_getsize(id);
	assert(ret == 1,"test3:msg getsize error!");

	//��������4
	id = 31;
	ret = msg_getsize(id);
	assert(ret == 1,"test4:msg getsize error!");

	//��������5
	id = 32;
	ret = msg_getsize(id);
	assert(ret == -ERR_NODEV,"test5:msg getsize error!");

	finaltest();
error:
	exit(0);
}
