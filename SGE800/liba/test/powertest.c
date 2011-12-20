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
    �ļ���      ��  adctest.c
    ����        ��  ���ļ����ڵ��ԺͲ���ƽ̨��adc
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2010.01													 
******************************************************************************/


//C��ͷ�ļ�
#include <stdio.h>		//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep

//ƽ̨��ͷ�ļ�
#include "../include/powercheck.h"

int main()
{
	int ret;	
	char func;
	int tmp;
	int timeout;
	u16 get_timeout;
	u8 io_level;
	printf("+----------------------------------------+\n");
	printf("| 	 ARM  Linux  power_check lib test	|\n");
	printf("| 	  Write on 2010.01.06 ROY		|\n");
	printf("+----------------------------------------+\n"); 
help:
	printf("\n Please select the function to test ( Main Menu):\n ");
	printf("\n  h: help ");
	printf("\n  t: set timeout value");
	printf("\n  g: get timeout value");
	printf("\n  s: set mode 0-NOBLOCK,1-UP,2-DOWN,3-UP OR DOWN");
	printf("\n  r: NOBLOCK read");
	printf("\n  e: BLOCK read");
	printf("\n  c: close adc modoul");	
	printf("\n ret = %d\n",ret);

	while(1){
	 printf("please select function ");
	 scanf("%s",&func);
	 printf("\n ");
	 switch(func)
	 {
		 case 'h':	//�����Գ������   
			goto help;
			break;			
		case 's':	//�����Գ������õ�����ģʽ
			printf("please input channel 0~3 \n  0-NOBLOCK,1-UP,2-DOWN,3-UP OR DOWN\n");
			scanf("%d",&tmp);
			ret = powercheck_init(tmp);
			if(ret < 0 )
				goto help;
			break;		
		case 't':
			printf("please input wait time value \n");
			scanf("%d",&timeout);
			ret = powercheck_setwaittime(timeout);
			if(ret < 0 )
				goto help;
			break;
		case 'g':				
			powercheck_getwaittime(&get_timeout);
			printf("wait time is %d\n",get_timeout);
			break;
		case 'r':
			while(1){
				io_level = powercheck_check();
				printf("%d\n",io_level);
				sleep(1);
			}
			break;
		case 'e':
			while(1){
				io_level = powercheck_check();
				printf("%d\n",io_level);
			}
			break;
		case 'c':
			powercheck_close();
			exit(1);						   
			break;	
		
		default: 
			powercheck_close();
		 	exit(1); 
		}
	} 
	   
	printf ("system error\n");
	powercheck_close();
	return 0;
}

