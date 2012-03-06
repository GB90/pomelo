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
    �ļ���      ��  timertest.c
    ����        ��  ���ļ����ڵ��ԺͲ���ƽ̨��timer
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2009.12													 
******************************************************************************/


//C��ͷ�ļ�
#include <stdio.h>		//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep

//ƽ̨��ͷ�ļ�
#include "../include/timer.h"

int main()
{
	int ret;	
	char func;
	int pin;
	int mode;
	u32 interval;
	int tmp=0;
	
	printf("please input pin 0~5 \n");
	scanf("%d",&pin);
	printf("please input mode 1-heart, 2-measure,3-PWM  \n");
	scanf("%d",&mode);
	ret = timer_init(pin, mode);
	if(ret < 0 ){
		printf("init error %d!\n",ret);
		goto error;
	}
	
	printf("+----------------------------------------+\n");
	printf("| 	 ARM  Linux  timer lib test		|\n");
	printf("| 	  Write on 2010.01.05 ROY		|\n");
	printf("+----------------------------------------+\n"); 
help:
	printf("\n Please select the function to test ( Main Menu):\n ");
	printf("\n  h: help ");
	printf("\n  t: config timer heart mode");
	printf("\n  p: config timer pwm mode");
	printf("\n  m: config timer measure mode");
	printf("\n  s: start heart mode");
	printf("\n  c: close timer modoul");	
	printf("\n ");
	printf("ret = %d \n", ret);
	while(1){
	 printf("please select function \n");
	 scanf("%s",&func);
	 switch(func)
	 {
		 case 'h':	//�����Գ������   
			goto help;
			break;			
		case 't':	//�����Գ�����������ģʽ
			printf("please input val  \n");
			scanf("%d",&interval);

			ret = timer_heart_setconfig(pin,interval);
			if(ret < 0 )
				goto help;
			//��ȡ����
			ret = timer_heart_getconfig(pin,&interval);			
			if(ret < 0 )
				goto help;
			printf("get interval = %d\n",interval);
			break;	
		case 's':	//�����Գ��򣬲�������

			tmp = 0;
			ret = timer_heart_start(pin);
			if(ret < 0 )
				goto help;
			while(1){				
				ret = timer_heart_wait(pin);
				if(ret < 0 )
					goto help;
				if(++tmp == 10){
					printf("heart is %d\n",interval);
					tmp = 0;
				}
			}
			break;	
		
		case 'c':
			timer_close(pin);
			exit(1);						   
			break;	
		
		default: 
		  timer_close(pin);
		  exit(1); 
		}
	} 
	   

error:
	printf ("system error\n");	
	return 0;
}

