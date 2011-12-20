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
	�ļ�		��  led.c
	����		��  ���ļ�������״̬��ģ��Ľӿ�
	�汾		��  0.1
	����		��  ·ȽȽ
	��������	��  2010.04
******************************************************************************/
//������ͷ�ļ�
#include "private/config.h"
	
//ģ�����ÿ���
#ifdef CFG_LED_MODULE
//C��ͷ�ļ�
#include <stdio.h>
#include <fcntl.h> 		//open ��־	
#include <sys/ioctl.h>	//ioctl
#include <string.h> 	//memset
#include <unistd.h>		//close
#include <pthread.h>	//pthread�⺯��

//�ṩ���û���ͷ�ļ�
#include "include/led.h"
#include "include/error.h"
	
//��������ͷ�ļ�
#include "private/drvlib/gpiolib.h"
#include "private/debug.h"
	
/*************************************************
  ��̬ȫ�ֱ������궨��
*************************************************/

struct period_arg_t{
	u8 id;
	u32 last;
	u32 period;
};

#define PINBASE 32							//���뵽������io�ڵ�ַ��ַ
#define MAX_LED_CHN 8						//���led����

static pthread_mutex_t mutex;				//������

static u8 	led_chn[CFG_LED_NUM];			//ledͨ����Ӧ������ͨ��
static u8 	led_type[CFG_LED_NUM];
static pthread_t thread_id[CFG_LED_NUM];	//���ڶ����߳�id
static struct period_arg_t thread_arg[CFG_LED_NUM];

static u8 	led_count = 0;					//ģ��򿪼���
static int 	led_fd;						//�ļ�������
static  u8	led_period[CFG_LED_NUM];		//led���ڶ��������־
/*************************************************
  API
*************************************************/
/******************************************************************************
*	����:	led_act
*	����:	led����
*	����:	id
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_CFG		-	����IO�ߵ���led����֮�����ϵ����
*	˵��:
 ******************************************************************************/
int led_act(u8 id)
{
	int ret;

	if(led_type[id] == 1){
		ret = ioctl(led_fd, OSET, led_chn[id]);
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}else if(led_type[id] == 0){
		ret = ioctl(led_fd, OCLR, led_chn[id]);
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}else{
		ret = -ERR_CFG;
		goto err;
	}
	ret = 0;
err:
	return ret;
}


/******************************************************************************
*	����:	led_disact
*	����:	ledϨ��
*	����:	id
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_CFG		-	����IO�ߵ���led����֮�����ϵ����
*	˵��:
 ******************************************************************************/
int led_disact(u8 id)
{
	int ret;

	if(led_type[id] == 1){
		ret = ioctl(led_fd, OCLR, led_chn[id]);
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}else if(led_type[id] == 0){
		ret = ioctl(led_fd, OSET, led_chn[id]);
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}else{
		ret = -ERR_CFG;
		goto err;
	}
err:
	return ret;
}
/******************************************************************************
*	����:	led_init
*	����:	ADת��ģ���ʼ��
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_CFG		-	���ó���
			-ERR_BUSY		-	�Ѿ���
			-ERR_SYS		-	ϵͳ����
			-ERR_NOFILE		-	û�д�·��
 ******************************************************************************/
int led_init(void)
{
	int ret = -1;
	int i;

	if(led_count == 1){
		ret = -ERR_BUSY;		//�Ѿ���
		goto err;
	}

	memset(led_chn, 0, CFG_LED_NUM);
	memset(led_period, 0, CFG_LED_NUM);

	//����ͨ��
#ifdef CFG_LED_0
	led_chn[LED0] = CFG_LED_0 + PINBASE;
	led_type[LED0] = CFG_LED_TYPE0;
#endif
#ifdef CFG_LED_1
	led_chn[LED1] = CFG_LED_1 + PINBASE;
	led_type[LED1] = CFG_LED_TYPE1;
#endif
#ifdef CFG_LED_2
	led_chn[LED2] = CFG_LED_2 + PINBASE;
	led_type[LED2] = CFG_LED_TYPE2;
#endif
#ifdef CFG_LED_3
	led_chn[LED3] = CFG_LED_3 + PINBASE ;
	led_type[LED3] = CFG_LED_TYPE3;
#endif
#ifdef CFG_LED_4
	led_chn[LED4] = CFG_LED_4 + PINBASE;
	led_type[LED4] = CFG_LED_TYPE4;
#endif
#ifdef CFG_LED_5
	led_chn[LED5] = CFG_LED_5 + PINBASE ;
	led_type[LED5] = CFG_LED_TYPE5;
#endif
#ifdef CFG_LED_6
	led_chn[LED6] = CFG_LED_6 + PINBASE ;
	led_type[LED6] = CFG_LED_TYPE6;
#endif
#ifdef CFG_LED_7
	led_chn[LED7] = CFG_LED_7 + PINBASE ;
	led_type[LED7] = CFG_LED_TYPE7;
#endif

	//����������
	led_fd = open("/dev/atmel_gpio", O_RDONLY);	//���ⲿADC����
	if (led_fd < 0){
		ret = -ERR_NOFILE;		//û�д�·��
		goto err;
	}	

	//���ü̵������Ĭ�ϲ�����
	for(i = 0; i < CFG_LED_NUM; i ++){
		if(led_chn[i] > 0){					//����io��Ϊ���
			ret = led_disact(i);			//���ò�����
			if(ret < 0){
				goto err;
			}
			ret = ioctl(led_fd, SETO, led_chn[i]);
			if(ret < 0){
				ret = -ERR_SYS;
				goto err;
			}
		}
	}

	if (pthread_mutex_init(&mutex, NULL)) {	//��ʼ��������
		ret = -ERR_SYS;
		goto err;
	}
	led_count = 1;
	ret = 0;
err:
	return ret;
}


/******************************************************************************
*	����:	led_thread_period
*	����:	led���������߳�
*	����:
*	����:
*	˵��:
 ******************************************************************************/
void * led_thread_period(void * arg)
{
	int ret;
	fd_set rfds;
	struct timeval tv;
	int fd = 1;
	struct period_arg_t *per_arg;

	tv.tv_sec = 0;
	FD_ZERO (&rfds);
	FD_SET (fd, &rfds);

	per_arg = (struct period_arg_t *)arg;

	led_period[per_arg->id] = 1;	//�����߳����б�־λ�����߳�ȡ���á�

	//�����߳����ԣ��յ�cancel�������˳�
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

	//�������Ϊperiod����������Ϊlast�ĵ�ƽ
	while(1){
		ret = led_act(per_arg->id);
		if(ret < 0){
			break;
		}
		tv.tv_usec = per_arg->last*1000;
		select (0, NULL, NULL, NULL, &tv);

		ret = led_disact(per_arg->id);
		if(ret < 0){
			break;
		}
		tv.tv_usec = (per_arg->period - per_arg->last) * 1000;
		select (0, NULL, NULL, NULL, &tv);

	}
	pthread_exit(0);
}

/******************************************************************************
*	����:	led_on
*	����:	led��������
*	����:	id				-	led���
*			delay			-	��ʱʱ�䣬��λΪms��Ϊ0��ʾû����ʱ
			last			-	����ʱ�䣬��λΪms��Ϊ0��ʾû�г���
			period			-	����ʱ�䣬��λΪms��Ϊ0��ʾû������
*	����:	0				-	�ɹ�
			-ERR_INVAL		-	��������
			-ERR_NOINIT		-	û�г�ʼ��
			-ERR_NODEV		-	û�д��豸
			-ERR_CFG		-	���ó���
			-ERR_SYS		-	ϵͳ����
*	˵��:	3��ʱ��ȫΪ0ʱ����ʾһֱ��, �˴�ʱ�侫�Ȳ��ߣ�Ӧ>20ms.
 ******************************************************************************/
int led_on(u8 id, u32 delay, u32 last, u32 period)
{
	int ret = -1;

	fd_set rfds;					//select��ʱ
	struct timeval tv;
	int fd = 1;

	thread_arg[id].id = id;
	thread_arg[id].last = last;
	thread_arg[id].period = period;

    FD_ZERO (&rfds);
    FD_SET (fd, &rfds);

	if(led_count == 0){
		return -ERR_NOINIT;
	}
	if((id < 0) || (id >= MAX_LED_CHN)){		//��Χ���
		return -ERR_NODEV;
	}

	if(led_chn[id] == 0){
		return -ERR_CFG;
	}

	//�ӳ�delay ms
	if(delay > 0){
		tv.tv_sec = 0;
		tv.tv_usec = delay*1000;
		select (0, NULL, NULL, NULL, &tv);

	}
	//���ڶ���
	if(period > 0){
		if((last <= 0) || (period <= last) ){
			ret = -ERR_INVAL;
			goto err;
		}

		ret = pthread_create(&thread_id[id], NULL, led_thread_period, (void *)&thread_arg[id]);
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}else{
		//����led��,�ӳ�last ms������led��
		if(last > 0){
			ret = led_act(id);
			if(ret < 0){
				goto err;
			}
			tv.tv_sec = 0;
			tv.tv_usec = last*1000;
			select (0, NULL, NULL, NULL, &tv);
			ret = led_off(id);
			if(ret < 0){
				goto err;
			}
		}
	}

	//��������Ϊ0ʱ��ֻ��led
	if((delay == 0) && (last == 0) && (period == 0)){
		ret = led_act(id);
		if(ret < 0){
			goto err;
		}
	}

	ret = 0;
err:
	return ret;
}

/******************************************************************************
*	����:	led_off
*	����:	led��
*	����:	id				-	led��
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_CFG		-	����IO�ߵ���led����֮�����ϵ����
*	˵��:
 ******************************************************************************/
int led_off(u8 id)
{
	int ret = -1;

	if(led_count == 0){
		return -ERR_NOINIT;
	}
	if((id < 0) || (id >= MAX_LED_CHN)){		//��Χ���
		return -ERR_NODEV;
	}

	if(led_chn[id] == 0){
		return -ERR_CFG;
	}

	//���ڶ��������������̣߳����ȴ��߳̽���
	if(led_period[id] == 1){
		ret = pthread_cancel(thread_id[id]);
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
		ret = pthread_join(thread_id[id],NULL);
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
		led_period[id] = 0;
	}

	ret = led_disact(id);
	if(ret < 0){
		goto err;
	}

	ret = 0;
err:
	return ret;
}	

/******************************************************************************
*	����:	led_check
*	����:	���led״̬
*	����:	id				-	ledͨ����
*	����:	1				-	��
			0				-	��
			-ERR_TIMEOUT	-	��ʱ
			-ERR_NODEV 		-	�޴��豸
			-ERR_NOINIT		-	��û�г�ʼ����
			-ERR_OTHER:		-	���������̻߳������Ĵ���
			-ERR_SYS		-	ϵͳ����
*	˵��:
 ******************************************************************************/
int led_check(u8 id)
{
	int ret = -1;

	u32 vp = led_chn[id];

	if(led_count == 0){
			return -ERR_NOINIT;
	}
	if((id < 0) || (id >= MAX_LED_CHN)){		//din��Χ���
		return -ERR_NODEV;		
	}	
	//��û�����
	if (pthread_mutex_lock (&mutex)) {
		return	-ERR_NOINIT;
	}

	if(led_chn[id] == 0){
		return -ERR_CFG;
	}

	//��ȡio�ڵ�ƽ
	ret = ioctl(led_fd, IOGETO, &vp);
	if(ret < 0){
		ret = -ERR_SYS;
		goto err;
	}
	if(vp > 1){				//����ֵ��0�� 1����
		ret = -ERR_SYS;
		goto err;
	}

	//�ж�led״̬
	if(led_type[id] == 1){
		if(vp == 1){
			ret = 1;
		}else{
			ret = 0;
		}
	}else if(led_type[id] == 0){
		if(vp == 0){
			ret = 1;
		}else{
			ret = 0;
		}
	}else{
		ret = -ERR_CFG;
		goto err;
	}
err:	
	if (pthread_mutex_unlock (&mutex)) {
		ret = -ERR_OTHER;	
	}	
	return ret;
}

/******************************************************************************
*	����:	led_close
*	����:	ledģ��رպ���
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_NOINIT		-	û�г�ʼ��
			-ERR_OTHER		-	���������̻߳������Ĵ���
			-ERR_CFG		-	����IO�ߵ���led����֮�����ϵ����
*	˵��:	��
 ******************************************************************************/
int led_close(void)
{
	int ret = -1;
	int i;

	if(led_count == 0){
		return -ERR_NOINIT;
	}
	for(i = 0; i < CFG_LED_NUM; i ++){
		if(led_chn[i] > 0){
			ret = led_off(i);					//���ò�����
			if(ret < 0){
				return ret;
			}
		}
	}

	ret = close(led_fd);
	if(ret < 0){
		return -ERR_SYS;	
	}
	led_count = 0;
	if (pthread_mutex_destroy(&mutex)) {
		ret = -ERR_OTHER;
	}
	ret = 0;

	return ret;
}
#endif
