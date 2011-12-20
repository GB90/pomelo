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
    �ļ���      ��  comporttest.c
    ����        ��  ���ļ����ڵ��ԺͲ���ƽ̨��comport
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2010.01													 
******************************************************************************/


//C��ͷ�ļ�
#include <stdio.h>		//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep

//ƽ̨��ͷ�ļ�
#include "../include/comport.h"

int main()
{
	int ret=-1;	
	char func;
	char coms;
	int mode;
	int pin;
	u8 buf[50];
	u8 bufs[50]="hello,kitty!";
	int flush_mode;
	comport_config_t com_s={COMPORT_VERIFY_NO,8,1,0,9600,0};
	comport_config_t com_g;
	
	
	printf("+----------------------------------------+\n");
	printf("| 	 ARM  Linux  comport lib test		|\n");
	printf("| 	  Write on 2010.01.06 ROY		|\n");
	printf("+----------------------------------------+\n"); 
help:
	printf("\n Please select the function to test ( Main Menu):\n ");
	printf("\n  h: help ");
	printf("\n  i: init comport");
	printf("\n  s: config comport");
	printf("\n  g: get config of comport");
	printf("\n  r: read  comport");
	printf("\n  w: write single char comport");
	printf("\n  z: write string comport");
	printf("\n  f: flush comport buffer");
	printf("\n  c: close comport modoul");	
	printf("\n  ret = %d\n ",ret);

	while(1){
	 printf("please select function:");
	 scanf("%s",&func);
	 printf("\n");
	 switch(func)
	 {
		 case 'h':	//�����Գ������   
			goto help;
			break;			
		case 'r':	//�����Գ��򣬶�ȡ��������
			
			ret = comport_recv(pin,buf,50);
			if(ret < 0 )
				goto help;
			buf[ret]= '\0';
			printf("read %d byte:%s\n",ret,buf);
			
			break;		
		case 'w':	//���ַ�д����
			printf("please input charactor:");
			scanf("%s",&coms);
			printf("\n");
			buf[0]=coms;
			ret = comport_send(pin,buf,1);
			if(ret < 0 )
				goto help;
			printf("send %d byte:%c\n",ret,coms);
			break;
		case 'z':	//�ַ���д����

			ret = comport_send(pin,bufs,sizeof bufs);
			if(ret < 0 )
				goto help;
			printf("send %d byte:%c\n",ret,coms);
			break;
		case 'i':	//��ʼ������
			printf("please input port 0~7 \n");
			scanf("%d",&pin);
			printf("please input mode 0-232 1-485 \n");
			scanf("%d",&mode);
			ret = comport_init(pin,mode);
			if(ret < 0 ){
				printf("init error %d!\n",ret);
				goto help;
			}			
			printf("init successfully\n");

			break;
		case 's':	//���ô���
			ret = comport_setconfig(pin,&com_s);
			if(ret < 0 )
				goto help;
			printf("config successfully\n");
			break;
		case 'g':	//��ȡ���ò���			
			ret = comport_getconfig(pin,&com_g);
			if(ret < 0 )
				goto help;
			printf("get band is %d\n",com_g.baud);
			break;
		case 'f':	//��մ�����������������
			printf("please input flush mode: 0 wr,1 r,2 w \n");
			scanf("%d",&flush_mode);
			ret = comport_flush(pin,flush_mode);
			if(ret < 0 )
				goto help;		
			printf("flush successfully\n");
			break;
		case 'c':
			comport_close(pin);
			exit(1);						   
			break;		
		default: 
			comport_close(pin);
		 	exit(1); 
		}
	} 
	   


	printf ("system error\n");
	comport_close(pin);
	return 0;
}

