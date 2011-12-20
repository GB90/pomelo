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
    �ļ���      ��  gpiotest.c
    ����        ��  ���ļ����ڵ��ԺͲ���ƽ̨��gpio
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2009.12													 
******************************************************************************/


//C��ͷ�ļ�
#include <stdio.h>		//printf
#include <stdlib.h>		//exit

#include <unistd.h>		//sleep

//ƽ̨��ͷ�ļ�
#include "../include/gpio.h"
#include "../include/pinio.h"

u32 pin;
static struct io_attr{
	u8 active;	//��¼gpio�ļ���״̬
	u8 mode;	//����ģʽ��1�����0�����
	u8 od;		//OD��ʹ�ܱ�־��1��ʾʹ��
	u8 pu;		//��������ʹ�ܱ�־��1��ʾʹ��
}gpio;

/******************************************************************************
 * ����:	thread1
 * ����:	�̣߳���һ��io�ڷ�������
 * ����:	 			
 * ����:	
 * ˵��:	��
 ******************************************************************************/

void* thread1 (void *arg) {
	int ret;
	while(1){
		ret = gpio_output_set(pin,1);
		if(ret < 0 )
			goto error;
		usleep(10000);
		ret = gpio_output_set(pin,0);
		if(ret < 0 )
			goto error;
		usleep(10000);		
			} 
error:	
	return NULL;
}

int main()
{
	int ret;
	u32 tmp;
	u32 val;
	u8 oval;		

	pthread_t tid;	
	char func;	
	ret = gpio_init();
	if(ret < 0 )
		goto error;
	
	
	printf("+----------------------------------------+\n");
	printf("| 	 ARM  Linux  GPIO lib test		|\n");
	printf("| 	  Write on 2009.12.24 ROY		|\n");
	printf("+----------------------------------------+\n"); 
help:
	printf("\n Please select the function to test ( Main Menu):\n ");
	printf("\n  h: help ");
	printf("\n  s: set gpio");
	printf("\n  o: set output val");
	printf("\n  g: get output val");
	printf("\n  i: get input val ");  
	printf("\n  t: test output pulse");	
	printf("\n  c: close gpio");	
	printf("\n ");

	for(;;){
	 printf("please select function \n");
	 scanf("%s",&func);
	 switch(func)
	 {
		 case 'h':	//�����Գ������   
			goto help;
			break;

		 case 's':	//�����Գ�������io������
		 	do{
			 	printf("please input pin 0~95 \n");
				scanf("%d",&pin);
		 	}while((pin < 0) | (pin > 95));		//����io������

			do{
			 	printf("please input pin 0 output,1 input \n");
				scanf("%d",&tmp);
				
		 	}while((tmp!= 0) && (tmp != 1))	;	//����io�ڹ���
			gpio.mode = tmp;
			do{
			 	printf("please input pin 0 od gate disable, 1 od gate enable\n");
				scanf("%d",&tmp);
		 	}while((tmp!= 0) && (tmp != 1))	;	//����io��od��
		 	gpio.od = tmp;

			do{
			 	printf("please input pin 0 pull up res disable, 1 pull up res enable\n");
				scanf("%d",&tmp);
		 	}while((tmp != 0) && (tmp != 1));		//����io��pull up res
			gpio.pu = tmp;
			
			ret = gpio_set(pin, gpio.mode, gpio.od, gpio.pu);			
			if(ret < 0 )
				goto error;
							
			break;

		 case 'o':	//�����Գ�������io�������ֵ
		 	do{
			 	printf("please input val 0 or 1? \n");
				scanf("%d",&val);
		 	}while((val!=0) && (val !=1));
			ret = gpio_output_set(pin,val);
			if(ret < 0 )
				goto error;				  
			break;
			
		case 'g':	//�����Գ��򣬻�ȡio�������ֵ
			
			ret = gpio_output_get(pin,&oval);
			if(ret < 0 )
				goto error;
			printf("output val is %x\n",oval);
			break;	
			
		case 'i':	//�����Գ��򣬻�ȡio�������ֵ
			
			ret = gpio_input_get(pin,&oval);
			if(ret < 0 )
				goto help;
			printf("input val is %x\n",oval);
			break;	
		case 't':	//�����Գ���io���������
			//����thread1�����߳�
			pthread_create (&tid, NULL, thread1, NULL);			
			printf("please chech out the %d pin output\n",pin);
			break; 
		
		case 'c':
			gpio_close();
			exit(1);						   
			break;	
		
		default: 
			  gpio_close();
			  exit(1); 
		}
	} 
	   

error:
	printf ("system error\n");
	gpio_close();
	return 0;
}
