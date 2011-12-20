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
    �ļ���      ��  dcsigtest.c
    ����        ��  ���ļ����ڵ��ԺͲ���ƽ̨��dcsig
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2010.01													 
******************************************************************************/


//C��ͷ�ļ�
#include <stdio.h>		//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep

//ƽ̨��ͷ�ļ�
#include "../include/dcsig.h"

int main()
{
	int ret;	
	char func;
	int pin;
	u32 result;
	u16 timeout;

	ret = dcsig_init();
	if(ret < 0 ){
		printf("init error %d!\n",ret);
		goto error;
	}
	
	printf("+----------------------------------------+\n");
	printf("| 	 ARM  Linux  dcsig lib test		|\n");
	printf("| 	  Write on 2010.04.22 ROY		|\n");
	printf("+----------------------------------------+\n"); 
help:
	printf("\n Please select the function to test ( Main Menu):\n ");
	printf("\n  h: help ");
	printf("\n  s: set timeout value");
	printf("\n  g: get timeout value");
	printf("\n  r: get dcsig result");
	printf("\n  c: close dcsig modoul");
	printf("\n ");

	while(1){
	 printf("please select function ");
	 scanf("%s",&func);
	 printf("\n ");
	 switch(func)
	 {
		 case 'h':	//�����Գ������   
			goto help;
			break;			
		case 'r':	//�����Գ��򣬻�ȡadת�����
		
			printf("please input channel 0~8 \n  4 refm\n  5 mid\n  6 refp\n  7 read cfr\n  8 read FIFO\n");
			// 4 for test voltage=refm 
			// 5 for test voltage=(refm+refp)/2
			// 6 for test voltage=refp
			// 7 read cfr
			// 8 read FIFO
			scanf("%d",&pin);
			ret = dcsig_read(pin,&result);
			if(ret < 0 )
				goto help;
			printf("channel %d dcsig result is %d\n",pin,result);
			break;		
		case 's':	
			printf("please input wait time value \n");
			scanf("%d",&pin);
			ret = dcsig_setwaittime(pin);
			if(ret < 0 )
				goto help;
			break;
		case 'g':				
			ret = dcsig_getwaittime(&timeout);
			if(ret < 0 )
				goto help;
			printf("wait time is %d\n",timeout);
			break;
		case 'c':
			dcsig_close();
			exit(1);						   
			break;	
		
		default: 
			dcsig_close();
		 	exit(1); 
		}
	} 
	   

error:
	printf ("system error\n");
	dcsig_close();
	return 0;
}

