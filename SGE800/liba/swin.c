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
	�ļ�		��  swin.c
	����		��  ���ļ�������������ģ��Ľӿ�
	�汾		��  0.1
	����		��  ·ȽȽ
	��������	��  2009.12
******************************************************************************/
//������ͷ�ļ�
#include "private/config.h"
	
//ģ�����ÿ���
#ifdef CFG_SWIN_MODULE
	
//C��ͷ�ļ�
#include <stdio.h>
#include <fcntl.h> 		//open ��־	
#include <sys/ioctl.h>		//ioctl
#include <string.h> 		//memset
#include <unistd.h>		//close
	
//�ṩ���û���ͷ�ļ�
#include "include/swin.h"
#include "include/timer.h"
#include "include/error.h"
	
//��������ͷ�ļ�
#include "private/drvlib/pulselib.h"
#include "private/drvlib/timerlib.h"
#include "private/drvlib/gpiolib.h"
#include "private/debug.h"
	
/*************************************************
  ��̬ȫ�ֱ������궨��
*************************************************/
#define PINBASE 32						//���뵽������io�ڵ�ַ��ַ
#define MAX_SWINPIN (8 + 4)				//���io������,8·��ͨ��4·ȷ��
#define SWIN_BUFSIZE 1024				//������������С����λ�ֽ�
#define CUR_SWINPIN (CFG_SWIN_NUM + 4)	//��ǰio������,4·ȷ��

#define CFG_CHECK_FILTER_NUM	3		//��鵱ǰio��״̬��������
#define CFG_CHECK_FILTER_TIME	20		//��鵱ǰio��״̬����ʱ��

static int fd;							//�����������ļ�������
static int t_fd;						//��ʱ���������ļ�������
static int g_fd;						//gpio�����ļ�������

static u8 swin_count = 0;				//ģ��򿪼���
static u16 swin_buf[SWIN_BUFSIZE];		//�����������
static pthread_mutex_t mutex;			//������

static swin_time_t	swin_result[CUR_SWINPIN];		//������
static u8	swin_pin[CUR_SWINPIN];		//�ܽ�io��ַ
static u32	swin_num[CUR_SWINPIN];		//�ܽ��������
static u16	high[CUR_SWINPIN], low[CUR_SWINPIN];	//�����иߵ�ƽ���ص�ƽ��������
static u8	swin_flag[CUR_SWINPIN];		//��ǰ״̬������ߵ�ƽ���͵�ƽ
static u8	swin_fun[CUR_SWINPIN];		//��ǰ���ܣ�1-ң��remsig��0-����pulse
static	u8	pulse_change[CUR_SWINPIN] = {0}, remsig_change[CUR_SWINPIN] = {0};

/*************************************************
  API
*************************************************/
/******************************************************************************
*	����:	swin_init
*	����:	ң�ż�������ģ���ʼ��
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_CFG		-	���ó���
			-ERR_BUSY		-	�Ѿ���
			-ERR_SYS		-	ϵͳ����
			-ERR_NOFILE		-	û�д�·��
 * ˵��:	��
 ******************************************************************************/
int swin_init(void)
{
	int ret = -1;
	int i = 0;
	char *dev[]={"/dev/atmel_tc0","/dev/atmel_tc1","/dev/atmel_tc2",
				"/dev/atmel_tc3","/dev/atmel_tc4","/dev/atmel_tc5"};
	
	if(CFG_SWIN_NUM > MAX_SWINPIN){
		ret = -ERR_CFG; 	//���ó���
		goto err;
	}
	if(swin_count == 1){
		ret = -ERR_BUSY;		//�Ѿ���
		goto err;
	}
	//��ʼ��������
	if (pthread_mutex_init(&mutex, NULL)) {
		ret = -ERR_SYS;
		goto err;
	}

	//��ʼ������
	memset(swin_num, 0, CUR_SWINPIN*sizeof(u32));
	memset(swin_buf, 0, SWIN_BUFSIZE*sizeof(u16));
	memset(high, 0, CUR_SWINPIN*sizeof(u16));
	memset(low, 0, CUR_SWINPIN*sizeof(u16));
	memset(swin_flag, 0, CUR_SWINPIN*sizeof(u8));
	memset(swin_fun, 0, CUR_SWINPIN*sizeof(u8));
	memset(pulse_change, 0, CUR_SWINPIN*sizeof(u8));
	memset(remsig_change, 0, CUR_SWINPIN*sizeof(u8));

	//����gpio�����ַ
#ifdef CFG_SWIN_0
	swin_pin[SWIN0] = CFG_SWIN_0 + PINBASE;
#endif
#ifdef CFG_SWIN_1
	swin_pin[SWIN1] = CFG_SWIN_1 + PINBASE;
#endif
#ifdef CFG_SWIN_2
	swin_pin[SWIN2] = CFG_SWIN_2 + PINBASE;
#endif
#ifdef CFG_SWIN_3
	swin_pin[SWIN3] = CFG_SWIN_3 + PINBASE;
#endif
#ifdef CFG_SWIN_4
	swin_pin[SWIN4] = CFG_SWIN_4 + PINBASE;
#endif
#ifdef CFG_SWIN_5
	swin_pin[SWIN5] = CFG_SWIN_5 + PINBASE;
#endif
#ifdef CFG_SWIN_6
	swin_pin[SWIN6] = CFG_SWIN_6 + PINBASE;
#endif
#ifdef CFG_SWIN_7
	swin_pin[SWIN7] = CFG_SWIN_7 + PINBASE;
#endif

	swin_pin[CFG_SWIN_NUM] = CFG_SWIN_DOOR + PINBASE;       //�Žӵ��źŶ�ӦIO
	swin_fun[CFG_SWIN_NUM] = REMSIG;
	swin_pin[CFG_SWIN_NUM+1] = CFG_SWIN_LID_TOP + PINBASE;    //������źŶ�ӦIO
	swin_fun[CFG_SWIN_NUM+1] = REMSIG;
	swin_pin[CFG_SWIN_NUM+2] = CFG_SWIN_LID_MID + PINBASE;    //���и��źŶ�ӦIO
	swin_fun[CFG_SWIN_NUM+2] = REMSIG;
	swin_pin[CFG_SWIN_NUM+3] = CFG_SWIN_LID_TAIL + PINBASE;   //��β���źŶ�ӦIO
	swin_fun[CFG_SWIN_NUM+3] = REMSIG;

	//�򿪶�ʱ������
	t_fd = open(dev[CFG_SWIN_TIMER], O_RDWR | O_NOCTTY );
   	if (t_fd < 0){
   		ret = -ERR_NOFILE;		//û�д�·��
		goto err;
	}
	//���ö�ʱ������ɼ�����
	ret = ioctl(t_fd, SET_PULSE, 0);
	if (ret < 0){
   		ret = -ERR_SYS;
		goto err;
	}
	//���ö�ʱ��ʱ��
	ret = ioctl(t_fd, SET_CLOCK, SCK32KIHZ); 
	if (ret < 0){
   		ret = -ERR_SYS;
		goto err;
	}
	//���ö�ʱ����ʱʱ��
	ret = ioctl(t_fd, SET_DELAY, CFG_SWIN_FILTER_TIME);
	if (ret < 0){
   		ret = -ERR_SYS;
		goto err;
	}
	//�����忪��ģ������
	fd = open("/dev/atmel_ps", O_RDONLY );//| O_NONBLOCK
	if (fd < 0){
   		ret = -ERR_NOFILE;		//û�д�·��
		goto err;
	}  
	
	ret = ioctl(fd, SET_TC, CFG_SWIN_TIMER);
	if (ret < 0){
   		ret = -ERR_SYS;
		goto err;
	}
	//����Ҫ����io��
	for(i = 0; i < CUR_SWINPIN; i ++){
		ret = ioctl(fd, ADD_IO, swin_pin[i]);
		if (ret < 0){
	   		ret = -ERR_SYS;		//û�д�·��
			goto err;
		} 		
	}
	//��ʼ��rtc
	ret = rtc_init();
	if((ret < 0) && (ret != -ERR_BUSY)){
		goto err;
	}

	//��gpioģ��
	g_fd = open("/dev/atmel_gpio", O_RDWR | O_NOCTTY);
	if (g_fd < 0){
		ret = -ERR_NOFILE;		//û�д�·��
		goto err;
	}

	 //����
	ret = ioctl(fd, PSTART, 0);
	if (ret < 0){
   		ret = -ERR_SYS;	
		goto err;
	}	

	swin_count = 1;
	ret = 0;
err:
	return ret;
}

/******************************************************************************
*	����:	swin_set
*	����:	���ÿ���ģʽ
*	����:	id				-	����ͨ����
			mode			-	1-ң��,0-����
*	����:	0				-	�ɹ�
			-ERR_INVAL		-	��������
			-ERR_NODEV		-	�޴��豸
*	˵��:
 ******************************************************************************/
int swin_set(u8 id, u8 mode)
{
	if((id < 0) || (id >= CFG_SWIN_NUM)){		//��Χ���
		return -ERR_NODEV;
	}
	if((mode != REMSIG) && (mode != PULSE)){				//��Χ���
			return -ERR_INVAL;
	}
	swin_fun[id] = mode;
	return 0;
}

/******************************************************************************
*	����:	swin_process
*	����:	��������п���Ľ����������������ң�ű�λʱ�̣�
*	����:
*	����:	>=0				-	��־λ����16λ��ʾ��ͨ���Ƿ��б仯
			-ERR_INVAL		-	��������
			-ERR_SYS		-	ϵͳ����ioctl����ʧ�ܻ�ʵʱʱ�Ӷ�ȡʱ�����
			-ERR_NOINIT		-	����ʵʱʱ��ģ��û�г�ʼ����
			-ERR_OTHER:		-	���������̻߳������Ĵ���
*	˵��:
 ******************************************************************************/
int swin_process(void)
{
	int ret;
	int i,j,read_byte;
	u16 * p;

	p = swin_buf;
	memset(p,0,SWIN_BUFSIZE*sizeof(u16));

	if(swin_count == 0)
		return -ERR_NOINIT;
	for(j = 0; j < CUR_SWINPIN; j ++){
		if((swin_fun[j] != REMSIG) && (swin_fun[j] != PULSE)){	//��Χ���
			return -ERR_NOCFG;
		}
	}
	//��û�����
	if (pthread_mutex_lock (&mutex)) {
		return	-ERR_NOINIT;
	}
	//��ȡ����
	ret = read(fd, (void*)p, 0);		//retΪ�����ֽ���
	if (ret < 0){
   		ret = -ERR_SYS;
		goto err;
	}
	read_byte = ret;
	for(i = 0; i < read_byte/2 ; i++){				//iΪ��i�β�����jΪ��j·io��
		for(j = 0; j < CUR_SWINPIN; j ++){
			if(swin_fun[j] == REMSIG){

				//�����洢��������-ң��
				if((swin_buf[i] >> j & 0x01) == 1 ){
					if(swin_flag[j] == 0){
						high[j] ++;
						low[j] = 0;
						if(high[j] >= CFG_SWIN_FILTER_NUM){
							high[j] = 0;
							swin_flag[j] = 1;

							swin_result[j].num += 1;
							if(swin_result[j].num >= 8){	//ң���źű�λ���8��
								swin_result[j].num = 8;		//ң���źű�λ���8��
							}

							if(remsig_change[j] >= 7){	//ң���źű�λʱ��buf��ʱ������һ��
								remsig_change[j] = 7;	//���һ��ʱ��
							}
							swin_result[j].jump[remsig_change[j]].polar = 1;
							ret =  rtc_gettime (&swin_result[j].jump[remsig_change[j]].time);
							if(ret < 0 ){
								goto err;
							}
							remsig_change[j] += 1;
						}
					}
				}else{
					if(swin_flag[j] == 1){
						low[j] ++;
						high[j] = 0;
						if(low[j] >= CFG_SWIN_FILTER_NUM){
							low[j] = 0;
							swin_flag[j] = 0;
							swin_result[j].num += 1;
							if(swin_result[j].num >= 8){	//ң���źű�λ���8��
								swin_result[j].num = 8;		//ң���źű�λ���8��
							}

							if(remsig_change[j] >= 7){		//ң���źű�λʱ��buf��ʱ������һ��
								remsig_change[j] = 7;		//���һ��ʱ�估����
							}
							swin_result[j].jump[remsig_change[j]].polar = 0;
							ret =  rtc_gettime (&swin_result[j].jump[remsig_change[j]].time);
							if(ret < 0 ){
								goto err;
							}
							remsig_change[j] += 1;
						}
					}
				}

			//�����洢��������-������
			}else if(swin_fun[j] == PULSE){
				if((swin_buf[i] >> j & 0x01) == 1 ){
					if(swin_flag[j] == 0){
						high[j] ++;
						low[j] = 0;
						if(high[j] >= CFG_SWIN_FILTER_NUM){
							swin_flag[j] = 1;
							swin_num[j] += 1;
							pulse_change[j] = 1;
						}
					}
				}else{
					low[j] ++;
					high[j] = 0;
					if(low[j] >= CFG_SWIN_FILTER_NUM)
						swin_flag[j] = 0;
				}

			}
		}
	}

	ret = 0;
	for(j = 0; j < CUR_SWINPIN ; j++){
		if((swin_fun[j] == PULSE) && (pulse_change[j] == 1)){	//�������δ���ߵ������������Ӧ·��1
			ret |= 1 << j;
		}
		if((swin_fun[j] == REMSIG) && (remsig_change[j] >= 1)){//�������δ���ߵ�ң�ţ���Ӧ·��1
			ret |= 1 << j;
		}
	}

err:	//����
	if (pthread_mutex_unlock (&mutex)) {
		ret = -ERR_OTHER;
	}
	return ret;
}

/******************************************************************************
*	����:	swin_read
*	����:	����ָ��ͨ���ŵ���������ң�ű�λʱ��
*	����:	id				-	����ͨ����
			p				-	��������ң�ű�λʱ�̣����ݴ�����
*	����:	0				-	�ɹ�
			-ERR_INVAL		-	��������
			-ERR_NODEV		-	�޴��豸
			-ERR_NOINIT		-	ģ��û�г�ʼ����
*	˵��:
 ******************************************************************************/
int swin_read(u8 id,swin_time_t *p)
{
	int ret;

	if(swin_count == 0)
		return -ERR_NOINIT;

	if((id < 0) || (id >= CUR_SWINPIN)){		//��Χ���
		return -ERR_NODEV;
	}
	if(swin_fun[id] == PULSE){
		memset(p, 0,sizeof(swin_time_t));
		p->num = swin_num[id];
		swin_num[id] = 0;
		pulse_change[id] = 0;
	}else if(swin_fun[id] == REMSIG){
		memcpy(p, &swin_result[id],sizeof(swin_time_t));
		memset(&swin_result[id], 0,sizeof(swin_time_t));
		memset(remsig_change, 0, CUR_SWINPIN*sizeof(u8));
	}else{
		ret = -ERR_INVAL;
		goto err;
	}

	ret = 0;
err:
	return ret;
}

/******************************************************************************
*	����:	swin_check
*	����:	ʵʱ��ѯ��ǰ״̬
*	����:	id				-	����ͨ����
*	����:	0				-	��ǰ״̬Ϊ�͵�ƽ
*			1				-	��ǰ״̬Ϊ�ߵ�ƽ
			-ERR_NODEV		-	�޴��豸
			-ERR_SYS		-	ϵͳ����gpio����ioctl����ʧ��
			-ERR_DEVUNSTAB	-	�豸���ȶ�
*	˵��:
 ******************************************************************************/
int swin_check(u8 id)
{
	int ret;
	u32 tmp[CFG_CHECK_FILTER_NUM];
	int i;

	fd_set rfds;
	struct timeval tv;
	int fd_t = 1;

	tv.tv_sec = 0;
	tv.tv_usec = CFG_CHECK_FILTER_TIME*1000;
	FD_ZERO (&rfds);
	FD_SET (fd_t, &rfds);

	if(swin_count == 0)
		return -ERR_NOINIT;

	if((id < 0) || (id >= CUR_SWINPIN)){		//din��Χ���
		return -ERR_NODEV;
	}

	//�˲���ȡio��״̬
	for(i = 0; i < CFG_CHECK_FILTER_NUM; i ++){
		tmp[i] = swin_pin[id];
		ret = ioctl(g_fd, IOGETI, &tmp[i]);

		if(ret < 0){
			ret = -ERR_SYS;
			goto err;
		}


		if((tmp[i] < 0) || (tmp[i] > 1)){
			ret = -ERR_SYS;
			goto err;
		}
		select (0, NULL, NULL, NULL, &tv);
	}
	for(i = 0; i < CFG_CHECK_FILTER_NUM - 1; i ++){
		if(tmp[i]  != tmp[i+1]){
			ret = -ERR_DEVUNSTAB;
			goto err;
		}
	}
	ret = tmp[0];
err:
	return ret;
}

/******************************************************************************
*	����:	swin_close
*	����:	�ر�ң��������ģ��
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_NOINIT		-	û�г�ʼ��
			-ERR_OTHER		-	���������̻߳������Ĵ���
*	˵��:	��
 ******************************************************************************/
int swin_close(void)
{
	int ret = -1;
	
	if(swin_count == 0)
		return -ERR_NOINIT;

	ret = close(fd);
	if(ret < 0)
		return -ERR_SYS;
	ret = close(t_fd);
	if(ret < 0)
		return -ERR_SYS;
	ret = close(g_fd);
	if(ret < 0)
		return -ERR_SYS;

	swin_count = 0;
	//���ٻ�����	
	if (pthread_mutex_destroy(&mutex)) {
		ret = -ERR_OTHER;
	}
	ret = 0;
	return ret;
}

#endif
