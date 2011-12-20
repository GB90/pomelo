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
    �ļ���      ��  keytest.c
    ����        ��  ���ļ����ڵ��ԺͲ���ƽ̨��key
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2010.04
******************************************************************************/


//C��ͷ�ļ�
#include <stdio.h>		//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep
#include <pthread.h>	//pthread�⺯��

//ƽ̨��ͷ�ļ�
#include "../include/key.h"
#include "../include/gpio.h"
#include "../include/pinio.h"
#define		GPIO_HIGH  PIN_PB1


void p_err(int exp)
{
	//_TestCount ++;
	if (exp != 0)
	{
	//	_ErrorCount ++;
//		printf(msg);
		switch(exp)
		{
			case -1:
				printf(" error: ERR_SYS\n");
				break;
			case -2:
				printf(" error: ERR_NODEV/ERR_NOFILE\n");
				break;
			case -3:
				printf(" error: ERR_TIMEOUT\n");
				break;
			case -4:
				printf(" error: ERR_INVAL\n");
				break;
			case -5:
				printf(" error: ERR_NOFUN\n");
				break;
			case -6:
				printf(" error: ERR_BUSY\n");
				break;
			case -7:
				printf(" error: ERR_NOINIT\n");
				break;
			case -8:
				printf(" error: ERR_NOMEM\n");
				break;
			case -9:
				printf(" error: ERR_NODISK\n");
				break;
			case -10:
				printf(" error: ERR_NORECORD\n");
				break;
			case -11:
				printf(" error: ERR_CFG\n");
				break;
			case -12:
				printf(" error: ERR_NOCFG\n");
				break;
			case -13:
				printf(" error: ERR_DEVUNSTAB\n");
				break;
			case -14:
				printf(" error: ERR_DISCONNECT\n");
				break;
			case -80:
				printf(" error: ERR_OTHER\n");
				break;
			default:
				printf("\n");
				break;
		}

	}
}

//void * thread_period(void *arg)
//{
//	u8 id;
//	fd_set rfds;
//	struct timeval tv;
//	int fd = 1;
//
//	id = *((u8 *)arg);
//	tv.tv_sec = 0;
//	FD_ZERO (&rfds);
//	FD_SET (fd, &rfds);
//
//	//�����߳����ԣ��յ�cancel�������˳�
//	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
//
//
//	while(1){
//		printf("key %d persist down\n",(id-100));
//		tv.tv_sec = 1;
//		tv.tv_usec = 0;
//		select (0, NULL, NULL, NULL, &tv);
//	}
//	pthread_exit(0);
//}
int main()
{
	int ret;	
	//pthread_t idkey;
	ret = key_init();
	if(ret < 0 ){
		p_err(ret);
	}

	printf("+----------------------------------------+\n");
	printf("| 	 ARM  Linux  key lib test	 |\n");
	printf("| 	  Write on 2010.04.21 ROY	 |\n");
	printf("+----------------------------------------+\n"); 

	while(1){
		ret = key_get(20);
		printf("ret = %d ",ret);

		if(ret == 0){
			printf("no key \n");;
		}else if(ret < 0){
			p_err(ret);
		}else if(ret < 100){
			printf("key %d down\n",ret);
		}else if(ret < 200){
			printf("key %d persist down\n",ret-100);
			// pthread_create(&idkey, NULL, thread_period, (void *)&ret);
//		}else if(ret > 200){
//			pthread_cancel(idkey);
//			pthread_join(idkey,NULL);
//			printf("key %d persist down over\n",ret-200);
		}
	}
	exit(0);
}

