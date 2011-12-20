/*****************************************************************************
	��̵����ɷ����޹�˾			��Ȩ��2008-2015

	��Դ���뼰������ĵ�Ϊ���������̵����ɷ����޹�˾�������У�δ��������
	�ɲ��������޸Ļ򷢲�������׷����صķ������Ρ�

						���������̹ɷ����޹�˾
						www.xjgc.com
						(0374) 321 2924
*****************************************************************************/


/******************************************************************************
	��Ŀ����	��  SGE800���������ն�ƽ̨
	�ļ�		��  gpio.c
	����		��  ���ļ�������IO����ģ��Ľӿ�
	�汾		��  0.1
	����		��  ·ȽȽ
	��������	��  2009.12
******************************************************************************/
//������ͷ�ļ�
#include "private/config.h"

//ģ�����ÿ���
#ifdef CFG_GPIO_MODULE

//C��ͷ�ļ�
#include <stdio.h>
#include <fcntl.h> 		//ioctl
#include <string.h> 	//memset
#include <unistd.h>		//close
#include <pthread.h>	//pthread�⺯��
#include <sys/ioctl.h>

//�ṩ���û���ͷ�ļ�
#include "include/gpio.h"
#include "include/error.h"

//��������ͷ�ļ�
#include "private/drvlib/gpiolib.h"

/*************************************************
  ��̬ȫ�ֱ������궨��
*************************************************/
#define PINBASE 32			//���뵽������io�ڵ�ַ��ַ
#define MAX_PIN 96			//���io������
static int fd;				//����gpio�����ļ�������

/*************************************************
  �ṹ���Ͷ���
*************************************************/
static struct io_attr{
	u8 active;		//��¼gpio�ļ���״̬
	u8 mode;		//����ģʽ��1�����0�����
	u8 od;			//OD��ʹ�ܱ�־��1��ʾʹ��
	u8 pu;			//��������ʹ�ܱ�־��1��ʾʹ��
	pthread_mutex_t mutex;	//������
}gpio[MAX_PIN];	//����gpio����������	

static u8 gpio_count = 0;		//ģ��򿪼���

/*************************************************
  API
*************************************************/
/******************************************************************************
*	����:	gpio_init
*	����:	GPIOģ���ʼ��
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_NOFILE		-	û�д�·��
			-ERR_BUSY		-	�豸æ���Ѿ���
			-ERR_SYS		-	��ʼ����ʧ�ܣ����ڴ治�㣻�Ѿ���ʼ����������û���٣���������ַ��Ч��
*	˵��:	��
 ******************************************************************************/
int gpio_init(void)
{
	int ret = -1;
	int i;
	if(gpio_count == 1){
		ret = -ERR_BUSY;		//�Ѿ���
		goto err;
	}
	
	fd = open("/dev/atmel_gpio", O_RDWR | O_NOCTTY);
	if (fd < 0){
   		ret = -ERR_NOFILE;		//û�д�·��
   		goto err;
	}
	gpio_count = 1;
	memset(gpio, 0, MAX_PIN*sizeof( struct io_attr));
	//��ʼ��������
	for(i = 0; i < MAX_PIN; i ++){
		if (pthread_mutex_init(&gpio[i].mutex, NULL)) {
			ret = -ERR_SYS;
			goto err;
		}
	}

	ret = 0;
err:
	return ret;
}


/******************************************************************************
*	����:	gpio_set
*	����:	����IO����
*	����:	io				-	IO�ڱ��
			mode			-	����ģʽ������������
			od				-	OD��ʹ�ܱ�־��1��ʾʹ��
			pu				-	��������ʹ�ܱ�־��1��ʾʹ��
*	����:	0				-	�ɹ�
			-ERR_INVAL 	 	-	��������
			-ERR_NODEV 	 	-	�޴��豸
			-ERR_SYS 	 	-	ϵͳ����ioctl����ʧ��
			-ERR_NOINIT  	-	��û�г�ʼ����
*	˵��:	��
 ******************************************************************************/
int gpio_set(u8 io, u8 mode, u8 od, u8 pu)
{
	int ret = -1;	
	
	if(gpio_count == 0)
		return -ERR_NOINIT;
	if((io < 0) || (io >= MAX_PIN)){		//gpio��ַ��Χ
		return -ERR_NODEV;		
	}
	//��û�����
	if (pthread_mutex_lock (&gpio[io].mutex)) {
		return  -ERR_NOINIT;		
	}
	
	if(mode == GPIO_OUT || mode == GPIO_IN)		//�����������
		gpio[io].mode = mode;		
	else{
		ret = -ERR_INVAL;
		goto err;
	}

	if(od == GPIO_ODD || od == GPIO_ODE)		//����od��ʹ��
		gpio[io].od = od;			
	else{
		ret = -ERR_INVAL;
		goto err;
	}
	
	if(pu == GPIO_PUD || pu == GPIO_PUE)		//������������ʹ��
		gpio[io].pu = 0;		
	else{
		ret = -ERR_INVAL;
		goto err;
	}
	
	gpio[io].active = 1;
	
	if(mode == GPIO_OUT){		//�����������		
		ret = ioctl(fd, SETO, io + PINBASE); 
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}
	else if(mode == GPIO_IN){		
		ret = ioctl(fd, SETI, io + PINBASE); 
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}
	
	if(od == GPIO_ODD){		//����od��ʹ��		
		ret = ioctl(fd, ODD, io + PINBASE); 
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}
	else if(od == GPIO_ODE){		
		ret = ioctl(fd, ODE, io + PINBASE); 
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}
	
	if(pu == GPIO_PUD){		//������������ʹ��		
		ret = ioctl(fd, PUD, io + PINBASE); 
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}
	else if(pu == GPIO_PUE){		
		ret = ioctl(fd, PUE, io + PINBASE); 
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}
	ret = 0;
err:	//����
	if (pthread_mutex_unlock (&gpio[io].mutex)) {
		ret = -ERR_OTHER;		
	}
	return ret;
}


/******************************************************************************
*	����:	gpio_output_set
*	����:	���ģʽ�����������ƽ״̬
*	����:	io				-	IO�ڱ��
			val				-	���״̬
*	����:	0				-	�ɹ�
			-ERR_INVAL		-	��������
			-ERR_NODEV		-	�޴��豸
			-ERR_NOFUN		-	�޴˹���
			-ERR_NOCFG		-	û������
			-ERR_SYS		-	ϵͳ����
			-ERR_NOINIT		-	��û�г�ʼ����
			-ERR_OTHER		-	���������̻߳������Ĵ���
*	˵��:	��
 ******************************************************************************/
int gpio_output_set(u8 io, u8 val)
{
	int ret = -1;
	
	if(gpio_count == 0)
		return -ERR_NOINIT;

	if((io < 0) || (io >= MAX_PIN))		//gpio��ַ��Χ��Ч���ж�
		return -ERR_NODEV;
	
	//��û�����
	if (pthread_mutex_lock (&gpio[io].mutex)) {
		return  -ERR_NOINIT;		
	}
	
//	if(gpio[io].active == 0){				//�˶˿�û������
//		ret = -ERR_NOCFG ;
//		goto err;
//	}
//	if(gpio[io].mode != GPIO_OUT){			//�˶˿�û������Ϊ�������
//		ret = -ERR_NOFUN;
//		goto err;
//	}
	
	if(val == 0){			//�������	
		ret = ioctl(fd, OCLR, io + PINBASE); 
		if(ret < 0){			
			ret = -ERR_SYS;
			goto err;
		}
	}
	else if(val == 1){		
		ret = ioctl(fd, OSET, io + PINBASE); 
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}else{
		ret = -ERR_INVAL;
		goto err;
	}
	ret = 0;
err:	//����
	if (pthread_mutex_unlock (&gpio[io].mutex)) {
		ret = -ERR_OTHER;		
	}
	return ret;
}


/******************************************************************************
*	����:	gpio_output_get
*	����:	���ģʽ�»�ȡ�����ƽ״̬
*	����:	io				-	IO�ڱ��
			val				-	���״̬�����ݴ�����
*	����:	0				-	�ɹ�
			-ERR_NODEV		-	�޴��豸
			-ERR_NOFUN		-	�޴˹���
			-ERR_NOCFG		-	û������
			-ERR_SYS		-	ϵͳ����
			-ERR_NOINIT		-	��û�г�ʼ����
			-ERR_OTHER		-	���������̻߳������Ĵ���
*	˵��:	��
 ******************************************************************************/
int gpio_output_get(u8 io, u8 *val)
{
	int ret = -1;
	u32 tmp;
	u32 *arg = NULL;
	arg = &tmp;
	tmp = io + PINBASE;
	
	if(gpio_count == 0)
		return -ERR_NOINIT;

	if((io < 0) || (io >= MAX_PIN))		//gpio��ַ��Χ
		return -ERR_NODEV;
	//��û�����
	if (pthread_mutex_lock (&gpio[io].mutex)) {
		return  -ERR_NOINIT;		
	}

	if(gpio[io].active == 0){		//�˶˿�û������
		ret = -ERR_NOCFG;
		goto err;
	}
	if(gpio[io].mode != GPIO_OUT){		//�˶˿�û������Ϊ�������
		ret = -ERR_NOFUN;
	}
	
	ret = ioctl(fd, IOGETO, arg); 
	if(ret < 0){
		ret = -ERR_SYS;
		goto err;
	}
	if((*arg < 0) || (*arg > 1)){
		ret = -ERR_SYS;
		goto err;
	}
	*val = *arg;
	ret = 0;
err:	//����
	if (pthread_mutex_unlock (&gpio[io].mutex)) {
		ret = -ERR_OTHER;		
	}
	return ret;
}


/******************************************************************************
*	����:	gpio_input_get
*	����:	����ģʽ�»�ȡ�����ƽ״̬
*	����:	io 				-	IO�ڱ��
			val				-	�����ƽ״̬�����ݴ�����
*	����:	0				-	�ɹ�
			-ERR_NODEV		-	�޴��豸
			-ERR_NOFUN		-	�޴˹���
			-ERR_NOCFG		-	û������
			-ERR_SYS		-	ϵͳ����
			-ERR_NOINIT		-	��û�г�ʼ����
*	˵��:	��
 ******************************************************************************/
int gpio_input_get(u8 io, u8 *val)
{
	
	int ret;
	int tmp;
	int *arg = NULL;
	arg = &tmp;	
	
	if((io < 0) || (io >= MAX_PIN)){		//gpio��ַ��Χ
		return -ERR_NODEV;		
	}
	
	//��û�����
	if (pthread_mutex_lock (&gpio[io].mutex)) {
		return  -ERR_NOINIT;		
	}
	
	if(gpio[io].active == 0){				//�˶˿�û������
		ret = -ERR_NOCFG;
		goto err;
	}
	if(gpio[io].mode != GPIO_IN){			//�˶˿�û������Ϊ���빦��
		ret = -ERR_NOFUN;
		goto err;
	}
	
	tmp = io + PINBASE;
	ret = ioctl(fd, IOGETI, arg); 
	if(ret < 0){
		ret = -ERR_SYS;
		goto err;
	}

	if((*arg < 0) || (*arg > 1)){
		ret = -ERR_SYS;
		goto err;
	}
	*val = *arg;
	ret = 0;
err:	//����
	if (pthread_mutex_unlock (&gpio[io].mutex)) {
		ret = -ERR_OTHER;	
	}	
	return ret;
}

/******************************************************************************
*	����:	gpio_close
*	����:	GPIOģ��رպ���
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_NOINIT		-	ģ��δ��ʼ��
			-ERR_OTHER		-	��ǰ�̲߳�ӵ�л�����������δ��ʼ��
*	˵��:	��
 ******************************************************************************/
int gpio_close(void)
{
	int ret = -1;
	int i;
	if(gpio_count == 0)
		return -ERR_NOINIT;
	ret = close(fd);
	if(ret < 0)
		return -ERR_SYS;
	gpio_count = 0;
	//���ٻ�����
	for(i = 0; i < MAX_PIN; i ++){
		if (pthread_mutex_destroy(&gpio[i].mutex)) {
			ret = -ERR_OTHER;
		}
	}
	ret = 0;
	return ret;
}


#endif /* CFG_GPIO_MODULE */
