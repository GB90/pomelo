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
    �ļ���      ��  rtctest.c
    ����        ��  ���ļ����ڵ��ԺͲ���ƽ̨��rtc
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2009.12													 
******************************************************************************/


//C��ͷ�ļ�
#include <stdio.h>		//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep

//ƽ̨��ͷ�ļ�
#include "../include/rtc.h"

int main()
{
	int ret;
	rtc_time_t time;
	char func;
	int tmp;
	unsigned char stat;

	ret = rtc_init();
	if(ret < 0 ){
		printf("init error ret = %d!\n",ret);
		goto error;
	}
	
	printf("+----------------------------------------+\n");
	printf("| 	 ARM  Linux  rtc lib test		|\n");
	printf("| 	  Write on 2009.12.28 ROY		|\n");
	printf("+----------------------------------------+\n"); 
help:
	printf("\n Please select the function to test ( Main Menu):\n ");
	printf("\n  h: help ");
	printf("\n  s: set rtc time");
	printf("\n  g: get rtc time");
	printf("\n  u: get rtc status");
	printf("\n  c: close gpio");	
	printf("\n ");

	while(1){
	 printf("please select function \n");
	 scanf("%s",&func);
	 switch(func)
	 {
		 case 'h':	//�����Գ������   
			goto help;
			break;

		 case 's':	//�����Գ�������ʱ��
//		 	do{
			 	printf("please input week 1~7 \n");
				scanf("%d",&tmp);
//		 	}while((tmp < 1) | (tmp > 7));		//������
		 	time.wday = tmp;

//			do{
			 	printf("please input year 2000~2255 \n");
				scanf("%d",&tmp);				
//		 	}while((tmp < 2000) && (tmp > 2255));	//�����깦��
		 	tmp -= 2000;
			time.year = tmp;

//			do{
			 	printf("please input month 1~12 \n");
				scanf("%d",&tmp);				
//		 	}while((tmp < 1) && (tmp > 12))	;	//�����¹���
			time.mon = tmp;

//			do{
			 	printf("please input day 1~31 \n");
				scanf("%d",&tmp);				
//		 	}while((tmp < 1) && (tmp > 31))	;	//�����չ���
			time.day = tmp;

//			do{
			 	printf("please input hour 0~23 \n");
				scanf("%d",&tmp);				
//		 	}while((tmp < 0) && (tmp > 23))	;	//����ʱ����
			time.hour = tmp;
		
//			do{
			 	printf("please input minute 0~59 \n");
				scanf("%d",&tmp);				
//		 	}while((tmp < 0) && (tmp > 59))	;	//���÷ֹ���
			time.min = tmp;

//			do{
			 	printf("please input second 0~59 \n");
				scanf("%d",&tmp);				
//		 	}while((tmp < 0) && (tmp > 59))	;	//�����빦��
			time.sec = tmp;
			
			
			ret = rtc_settime(&time);			
			if(ret < 0 ){
				printf("set error!!!\n\n");
				goto help;
			}		
			break;
	
			
		case 'g':	//�����Գ��򣬻�ȡio�������ֵ
			
			ret =  rtc_gettime (&time);
			if(ret < 0 )
				goto error;
			printf("get time is: %d.%d.%d %d:%d:%d %d\n",
					2000+time.year,time.mon,time.day,
					time.hour,time.min,time.sec,time.wday);
			break;

		case 'u':	//�����Գ��򣬻�ȡio�������ֵ

			ret =  rtc_getstat (&stat);
			if(ret < 0 )
				goto error;
			printf("get status is: %d\n",stat);
			break;
		case 'c':
			rtc_close();
			exit(1);						   
			break;	
		
		default: 
		  rtc_close();
		  exit(1); 
		}
	} 
	   

error:
	printf ("system error\n");
	rtc_close();
	return 0;
}

