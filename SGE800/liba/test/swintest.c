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
    �ļ���      ��  swintest.c
    ����        ��  ���ļ����ڵ��ԺͲ���ƽ̨��adc
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2010.04
******************************************************************************/


//C��ͷ�ļ�
#include <stdio.h>		//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep
#include <string.h>

//ƽ̨��ͷ�ļ�
#include "../include/swin.h"
#include "../include/gpio.h"
#include "../include/pinio.h"

#define		GPIO_PULSE  PIN_PB1

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

/******************************************************************************
*	����:	pulse_make
*	����:	�������������
*	����:
*	����:
*	˵��:���ݺ궨��GPIO_PULSE������ܽŲ�������Ϊperiod (ms)������num��
 ******************************************************************************/
void pulse_make(u32 num, u32 period)
{
	int i;
	fd_set rfds;
	struct timeval tv;
	int fd_t = 1;

	tv.tv_sec = 0;
	tv.tv_usec = period/2*1000;
	FD_ZERO (&rfds);
	FD_SET (fd_t, &rfds);
	for(i = 0; i < num; i ++){
		gpio_output_set(GPIO_PULSE,1);
		tv.tv_usec = period/2*1000;
		select (0, NULL, NULL, NULL, &tv);
		gpio_output_set(GPIO_PULSE,0);
		tv.tv_usec = period/2*1000;
		select (0, NULL, NULL, NULL, &tv);
	}
}
int main()
{
	int ret,i=0;
	char func;
	int pin;
	u32 mode[8]={0};
	swin_time_t result;
	rtc_time_t	time;

	ret = swin_init();
	if(ret < 0 ){
		p_err(ret);
		goto error;
	}
	//gpio��ʼ��
	ret = gpio_init();
	if(ret < 0 ){
		p_err(ret);
		goto error;
	}
	ret = gpio_output_set(GPIO_PULSE,0);
	if(ret < 0 ){
		p_err(ret);
		goto error;
	}
	ret = gpio_set(GPIO_PULSE,GPIO_OUT,0,0);
	if(ret < 0 ){
		p_err(ret);
		goto error;
	}
//	ret = gpio_input_get(GPIO_PULSE,&val);
//	if(ret < 0 ){
//		p_err(ret);
//		goto error;
//	}
//	printf("input get val = %x\n",val);

	printf("+----------------------------------------+\n");
	printf("| 	 ARM  Linux  swin lib test		|\n");
	printf("| 	  Write on 2010.04.16 ROY		|\n");
	printf("+----------------------------------------+\n"); 
help:
	printf("\n********* (Main Menu)*********\n ");
	printf("\n  h: help ");
	printf("\n  s: set swin");
	printf("\n  p: process ");
	printf("\n  r: read swin");
	printf("\n  g: get swin status");
	printf("\n  c: close swin modoul");
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
		case 's':	//�����Գ�������ģʽ
			printf("please input id 0~7 \n");
			scanf("%d",&pin);
			printf("please input mode 1-remote signal, 2-pusle\n");
			scanf("%u",&mode[pin]);
			ret = swin_set(pin,mode[pin]);
			if(ret < 0 ){
				p_err(ret);
				goto help;
			}
			printf("set swin done\n");
			break;

		case 'p':		//������
			pulse_make(5,100);
			ret = swin_process();
			if(ret < 0 ){
				p_err(ret);
				goto help;
			}
			printf("ret = %x\n",ret);
			break;

		case 'r':		//��ȡ���
			printf("please input id \n");
			scanf("%d",&pin);
			ret = swin_read(pin,&result);
			if(ret < 0 ){
				p_err(ret);
				goto help;
			}
			printf("num = %d\n",result.num);
			if((mode[pin] == REMSIG) && (result.num > 0)){
				for(i=0; i<result.num; i++){
					memcpy(&time,&result.jump[i].time,sizeof time);
					if(result.jump[i].polar == 1){
						printf("get positive change time is: %d.%d.%d %d:%d:%d %d\n",
								2000+time.year,time.mon,time.day,
								time.hour,time.min,time.sec,time.wday);
					}else{
						printf("get nagetive change time is: %d.%d.%d %d:%d:%d %d\n",
								2000+time.year,time.mon,time.day,
								time.hour,time.min,time.sec,time.wday);
					}
				}
			}
			break;
		case 'g':	//��ȡio��״̬
			printf("please input id \n");
			scanf("%d",&pin);
			ret = swin_check(pin);
			if(ret < 0 ){
				p_err(ret);
				goto help;
			}

			printf("swin status is %d\n",ret);
			break;
		case 'c':
			swin_close();
			exit(1);						   
			break;	
		
		default: 
			swin_close();
		 	exit(1); 
		}
	} 
	   

error:
	printf ("system error\n");
	swin_close();
	return 0;
}

