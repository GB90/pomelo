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
	�ļ�		��  relay.c
	����		��  ���ļ������˼̵���ģ��Ľӿ�
	�汾		��  0.1
	����		��  ·ȽȽ
	��������	��  2010.04
******************************************************************************/
//������ͷ�ļ�
#include "private/config.h"
	
//ģ�����ÿ���
#ifdef CFG_RELAY_MODULE
//C��ͷ�ļ�
#include <stdio.h>
#include <fcntl.h> 		//open ��־	
#include <sys/ioctl.h>	//ioctl
#include <string.h> 	//memset
#include <unistd.h>		//close
#include <pthread.h>	//pthread�⺯��

//�ṩ���û���ͷ�ļ�
#include "include/relay.h"
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

#define PINBASE 32						//���뵽������io�ڵ�ַ��ַ
#define MAX_RELAY_CHN 8					//���̵�������

static pthread_mutex_t mutex;			//������

static u8 	relay_chn[MAX_RELAY_CHN];	//�̵���ͨ����Ӧ������ͨ��
static u8 	relay_chd[MAX_RELAY_CHN];	//�̵���ͨ����Ӧ��˫������ͨ��
static u8 	relay_type[MAX_RELAY_CHN];	//�̵���ͨ����Ӧ������
static struct period_arg_t thread_arg[MAX_RELAY_CHN];	//�̴߳������

static pthread_t thread_id[MAX_RELAY_CHN];		//���ڶ����߳�id

static u8 	relay_count = 0;			//ģ��򿪼���
static int 	relay_fd;				//�ļ�������
static u8	relay_period[MAX_RELAY_CHN];		//�̵������ڶ��������־
/*************************************************
  API
*************************************************/
/******************************************************************************
*	����:	relay_act
*	����:	�̵�������
*	����:	id
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_CFG		-	����IO�ߵ���̵�������֮�����ϵ����
*	˵��:
 ******************************************************************************/
int relay_act(u8 id)
{
	int ret;

	if(relay_type[id] == 1){
		ret = ioctl(relay_fd, OSET, relay_chn[id]);
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
		if(relay_chd[id] > 0){		//˫��ģʽ
			ret = ioctl(relay_fd, OCLR, relay_chd[id]);
			if(ret < 0){
				ret = -ERR_SYS;
				goto err;
			}
		}
	}else if(relay_type[id] == 0){
		ret = ioctl(relay_fd, OCLR, relay_chn[id]);
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
		if(relay_chd[id] > 0){		//˫��ģʽ
			ret = ioctl(relay_fd, OSET, relay_chd[id]);
			if(ret < 0){
				ret = -ERR_SYS;
				goto err;
			}
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
*	����:	relay_disact
*	����:	�̵����Ͽ�
*	����:	id
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_CFG		-	����IO�ߵ���̵�������֮�����ϵ����
*	˵��:
 ******************************************************************************/
int relay_disact(u8 id)
{
	int ret;

	//����id��Ӧ�ļ̵�����Ϊ������
	if(relay_type[id] == 1){
		ret = ioctl(relay_fd, OCLR, relay_chn[id]);
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
		if(relay_chd[id] > 0){		//˫��ģʽ
			ret = ioctl(relay_fd, OSET, relay_chd[id]);
			if(ret < 0){
				ret = -ERR_SYS;
				goto err;
			}
		}
	}else if(relay_type[id] == 0){
		ret = ioctl(relay_fd, OSET, relay_chn[id]);
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
		if(relay_chd[id] > 0){		//˫��ģʽ
			ret = ioctl(relay_fd, OCLR, relay_chd[id]);
			if(ret < 0){
				ret = -ERR_SYS;
				goto err;
			}
		}
	}else{
		ret = -ERR_CFG;
		goto err;
	}
err:
	return ret;
}
/******************************************************************************
*	����:	relay_init
*	����:	�̵���ģ���ʼ��
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_CFG		-	���ó���
			-ERR_BUSY		-	�Ѿ���
			-ERR_SYS		-	ϵͳ����
			-ERR_NOFILE		-	û�д�·��
 ******************************************************************************/
int relay_init(void)
{
	int ret = -1;
	int i;
	if(CFG_RELAY_NUM > MAX_RELAY_CHN){
		ret = -ERR_CFG; 	//���ó���
		goto err;
	}
	if(relay_count == 1){
		ret = -ERR_BUSY;		//�Ѿ���
		goto err;
	}

	memset(relay_chn, 0, MAX_RELAY_CHN);
	memset(relay_chd, 0, MAX_RELAY_CHN);
	memset(relay_period, 0, MAX_RELAY_CHN);

	//����ͨ��
#ifdef CFG_RELAY_0
	relay_chn[RELAY0] = CFG_RELAY_0 + PINBASE;
	relay_type[RELAY0] = CFG_RELAY_TYPE0;
#endif
#ifdef CFG_RELAY_1
	relay_chn[RELAY1] = CFG_RELAY_1 + PINBASE;
	relay_type[RELAY1] = CFG_RELAY_TYPE1;
#endif
#ifdef CFG_RELAY_2
	relay_chn[RELAY2] = CFG_RELAY_2 + PINBASE;
	relay_type[RELAY2] = CFG_RELAY_TYPE2;
#endif
#ifdef CFG_RELAY_3
	relay_chn[RELAY3] = CFG_RELAY_3 + PINBASE ;
	relay_type[RELAY3] = CFG_RELAY_TYPE3;
#endif
#ifdef CFG_RELAY_4
	relay_chn[RELAY4] = CFG_RELAY_4 + PINBASE;
	relay_type[RELAY4] = CFG_RELAY_TYPE4;
#endif
#ifdef CFG_RELAY_5
	relay_chn[RELAY5] = CFG_RELAY_5 + PINBASE ;
	relay_type[RELAY5] = CFG_RELAY_TYPE5;
#endif
#ifdef CFG_RELAY_6
	relay_chn[RELAY6] = CFG_RELAY_6 + PINBASE ;
	relay_type[RELAY6] = CFG_RELAY_TYPE6;
#endif
#ifdef CFG_RELAY_7
	relay_chn[RELAY7] = CFG_RELAY_7 + PINBASE ;
	relay_type[RELAY7] = CFG_RELAY_TYPE7;
#endif

//˫���ж�
#ifdef CFG_RELAY_D0
	relay_chd[RELAY0] = CFG_RELAY_D0 + PINBASE ;
#endif
#ifdef CFG_RELAY_D1
	relay_chd[RELAY1] = CFG_RELAY_D1 + PINBASE ;
#endif
#ifdef CFG_RELAY_D2
	relay_chd[RELAY2] = CFG_RELAY_D2 + PINBASE;
#endif
#ifdef CFG_RELAY_D3
	relay_chd[RELAY3] = CFG_RELAY_D3 + PINBASE ;
#endif
#ifdef CFG_RELAY_D4
	relay_chd[RELAY4] = CFG_RELAY_D4 + PINBASE ;
#endif
#ifdef CFG_RELAY_D5
	relay_chd[RELAY5] = CFG_RELAY_D5 + PINBASE ;
#endif
#ifdef CFG_RELAY_D6
	relay_chd[RELAY6] = CFG_RELAY_D6 + PINBASE ;
#endif
#ifdef CFG_RELAY_D7
	relay_chd[RELAY7] = CFG_RELAY_D7 + PINBASE ;
#endif
	
	//����������
	relay_fd = open("/dev/atmel_gpio", O_RDONLY);	//���ⲿADC����
	if (relay_fd < 0){
		ret = -ERR_NOFILE;		//û�д�·��
		goto err;
	}	

	//���ü̵������Ĭ�ϲ�����
	for(i = 0; i < CFG_RELAY_NUM; i ++){
		if(relay_chn[i] > 0){					//����io��Ϊ���
			ret = relay_disact(i);					//���ò�����
			if(ret < 0){
				goto err;
			}
			ret = ioctl(relay_fd, SETO, relay_chn[i]);
			if(ret < 0){
				ret = -ERR_SYS;
				goto err;
			}
		}
		if(relay_chd[i] > 0){					//����˫��io��Ϊ���
			ret = ioctl(relay_fd, SETO, relay_chd[i]);
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
	relay_count = 1;
	ret = 0;
err:
	return ret;
}


/******************************************************************************
*	����:	relay_thread_period
*	����:	�̵������ڶ����߳�
*	����:
*	����:
*	˵��:
 ******************************************************************************/
void * relay_thread_period(void * arg)
{
	int ret;
	fd_set rfds;
	struct timeval tv;
	int fd = 1;
	struct period_arg_t *per_arg;

	tv.tv_sec = 0;
	FD_ZERO (&rfds);
	FD_SET (fd, &rfds);

	//memcpy(&per_arg,(struct period_arg_t *)arg,sizeof per_arg);
	per_arg = (struct period_arg_t *)arg;
	relay_period[per_arg->id] = 1;	//�����߳����б�־λ�����߳�ȡ���á�

	//�����߳����ԣ��յ�cancel�������˳�
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

	//�������Ϊperiod����������Ϊlast�ĵ�ƽ
	while(1){
		ret = relay_act(per_arg->id);
		if(ret < 0){
			break;
		}
		tv.tv_usec = per_arg->last*1000;
		select (0, NULL, NULL, NULL, &tv);

		ret = relay_disact(per_arg->id);
		if(ret < 0){
			break;
		}
		tv.tv_usec = (per_arg->period - per_arg->last) * 1000;
		select (0, NULL, NULL, NULL, &tv);

	}
	pthread_exit(0);
}

/******************************************************************************
*	����:	relay_on
*	����:	�̵�����������
*	����:	id				-	�̵������
*			delay			-	��ʱʱ�䣬��λΪms��Ϊ0��ʾû����ʱ
			last			-	����ʱ�䣬��λΪms��Ϊ0��ʾû�г���
			period			-	����ʱ�䣬��λΪms��Ϊ0��ʾû������
*	����:	0				-	�ɹ�
			-ERR_INVAL		-	��������
			-ERR_NOINIT		-	û�г�ʼ��
			-ERR_NODEV		-	û�д��豸
			-ERR_CFG		-	���ó���
			-ERR_SYS		-	ϵͳ����
*	˵��:	3��ʱ��ȫΪ0ʱ����ʾһֱ����, �˴�ʱ�侫�Ȳ��ߣ�Ӧ>20ms.
 ******************************************************************************/
int relay_on(u8 id, u32 delay, u32 last, u32 period)
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

	if(relay_count == 0){
		return -ERR_NOINIT;
	}
	if((id < 0) || (id >= MAX_RELAY_CHN)){		//��Χ���
		return -ERR_NODEV;
	}

	if(relay_chn[id] == 0){
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
		ret = pthread_create(&thread_id[id], NULL, relay_thread_period, (void *)&thread_arg[id]);

		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}else{
		//���ü̵�������,�ӳ�last ms�����ü̵���������
		if(last > 0){
			ret = relay_act(id);
			if(ret < 0){
				goto err;
			}
			tv.tv_sec = 0;
			tv.tv_usec = last*1000;
			select (0, NULL, NULL, NULL, &tv);
			ret = relay_off(id);
			if(ret < 0){
				goto err;
			}
		}
	}

	//��������Ϊ0ʱ��ֻ�����̵���
	if((delay == 0) && (last == 0) && (period == 0)){
		ret = relay_act(id);
		if(ret < 0){
			goto err;
		}
	}

	ret = 0;
err:
	return ret;
}

/******************************************************************************
*	����:	relay_off
*	����:	�̵����Ͽ�
*	����:	id				-	�̵�����
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_CFG		-	����IO�ߵ���̵�������֮�����ϵ����
*	˵��:
 ******************************************************************************/
int relay_off(u8 id)
{
	int ret = -1;

	if(relay_count == 0){
		return -ERR_NOINIT;
	}
	if((id < 0) || (id >= MAX_RELAY_CHN)){		//��Χ���
		return -ERR_NODEV;
	}

	if(relay_chn[id] == 0){
		return -ERR_CFG;
	}

	//���ڶ��������������̣߳����ȴ��߳̽���
	if(relay_period[id] == 1){
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

		relay_period[id] = 0;
	}

	ret = relay_disact(id);
	if(ret < 0){
		goto err;
	}

	ret = 0;
err:
	return ret;
}	

/******************************************************************************
*	����:	relay_check
*	����:	���̵���״̬
*	����:	id				-	�̵���ͨ����
*	����:	1				-	����
			0				-	������
			-ERR_TIMEOUT	-	��ʱ
			-ERR_NODEV 		-	�޴��豸
			-ERR_NOINIT		-	��û�г�ʼ����
			-ERR_OTHER:		-	���������̻߳������Ĵ���
			-ERR_SYS		-	ϵͳ����
*	˵��:
 ******************************************************************************/
int relay_check(u8 id)
{
	int ret = -1;
	u32 vp = relay_chn[id], vn = relay_chd[id];

	if(relay_count == 0){
			return -ERR_NOINIT;
	}
	if((id < 0) || (id >= MAX_RELAY_CHN)){		//din��Χ���
		return -ERR_NODEV;		
	}	
	//��û�����
	if (pthread_mutex_lock (&mutex)) {
		return	-ERR_NOINIT;
	}

	if(relay_chn[id] == 0){
		return -ERR_CFG;
	}
	//��ȡio�ڵ�ƽ
	ret = ioctl(relay_fd, IOGETO, &vp);
	if(ret < 0){
		ret = -ERR_SYS;
		goto err;
	}
	if(vp > 1){				//����ֵ��0�� 1����
		ret = -ERR_SYS;
		goto err;
	}

	if(vn > 0){
		ret = ioctl(relay_fd, IOGETO, &vn);
		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
			if(vn > 1){
			ret = -ERR_SYS;
			goto err;
		}
	}

	//�жϼ̵���״̬

	if(relay_type[id] == 1){
		if(vp == 1){
			ret = 1;
		}else{
			ret = 0;
		}
		if(relay_chd[id] > 0){		//˫��ģʽ,���̵�������Ϊ�ߣ�����Ϊ��ʱ������������Ϊ������
			if((vp == 1) && (vn == 0)){
				ret = 1;
			}else{
				ret = 0;
			}
		}
	}else if(relay_type[id] == 0){
		if(vp == 0){
			ret = 1;
		}else{
			ret = 0;
		}
		if(relay_chd[id] > 0){		//˫��ģʽ,���̵�������Ϊ�ߣ�����Ϊ��ʱ������������Ϊ������
			if((vp == 0) && (vn == 1)){
				ret = 1;
			}else{
				ret = 0;
			}
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
*	����:	relay_close
*	����:	�̵���ģ��رպ���
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_NOINIT		-	û�г�ʼ��
			-ERR_OTHER		-	���������̻߳������Ĵ���
			-ERR_CFG		-	����IO�ߵ���̵�������֮�����ϵ����
*	˵��:	��
 ******************************************************************************/
int relay_close(void)
{
	int ret = -1;
	int i;

	if(relay_count == 0){
		return -ERR_NOINIT;
	}
	for(i = 0; i < CFG_RELAY_NUM; i ++){
		if(relay_chn[i] > 0){
			ret = relay_off(i);					//���ò�����
			if(ret < 0){
				return ret;
			}
		}
	}

	ret = close(relay_fd);
	if(ret < 0){
		return -ERR_SYS;	
	}
	relay_count = 0;
	if (pthread_mutex_destroy(&mutex)) {
		ret = -ERR_OTHER;
	}
	ret = 0;

	return ret;
}
#endif
