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
	�ļ�		��  adc.c
	����		��  ���ļ�������adת��ģ��Ľӿ�
	�汾		��  0.1
	����		��  ·ȽȽ
	��������	��  2010.01
******************************************************************************/
//������ͷ�ļ�
#include "private/config.h"
	
//ģ�����ÿ���
#ifdef CFG_ADC_MODULE	
//C��ͷ�ļ�
#include <stdio.h>
#include <fcntl.h> 		//open ��־	
#include <sys/ioctl.h>	//ioctl
#include <string.h> 	//memset
#include <unistd.h>		//close
#include <pthread.h>	//pthread�⺯��
	
//�ṩ���û���ͷ�ļ�
#include "include/adc.h"
#include "include/error.h"
	
//��������ͷ�ļ�
#include "private/drvlib/adclib.h"
#include "private/drvlib/adilib.h"
#include "private/debug.h"
	
/*************************************************
  ��̬ȫ�ֱ������궨��
*************************************************/
#define MAX_ADCCHN 8			//���io������
static pthread_mutex_t mutex;			//������
static u8 	adc_chn[CFG_ADC_NUM];		//ADCͨ����Ӧ������ͨ��
static u8 	adc_count = 0;				//ģ��򿪼���
static int 	adc_fd, adi_fd;			//�ⲿ���ڲ�ad�ļ�������
//static  u8 	adc_type = DS_ADC_IRQ;		//Ĭ�϶�ȡ��ʽΪ�ж�
static  u16 adc_timeout = 1;			//Ĭ��100ms��ʱ

/*************************************************
  API
*************************************************/
/******************************************************************************
*	����:	adc_init
*	����:	ADת��ģ���ʼ��
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_CFG		-	���ó���
			-ERR_BUSY		-	�Ѿ���
			-ERR_SYS		-	ϵͳ����
			-ERR_NOFILE		-	û�д�·��
 ******************************************************************************/
int adc_init(void)
{
	int ret = -1;
	u8 arg = 0;		//�ڲ�id����ͨ���������
	
	if(CFG_ADC_NUM > MAX_ADCCHN){
		ret = -ERR_CFG; 	//���ó���
		goto err;
	}
	if(adc_count == 1){
		ret = -ERR_BUSY;		//�Ѿ���
		goto err;
	}	
	if (pthread_mutex_init(&mutex, NULL)) {	//��ʼ��������
		ret = -ERR_SYS;
		goto err;
	}
	memset(adc_chn,0xff,CFG_ADC_NUM);	//��ʼ��adcͨ��ֵΪ��Ч
	//����adcͨ��
#ifdef CFG_ADC_0
	adc_chn[0] = CFG_ADC_0 ;
#endif
#ifdef CFG_ADC_1
	adc_chn[1] = CFG_ADC_1 ;
#endif
#ifdef CFG_ADC_2
	adc_chn[2] = CFG_ADC_2;
#endif
#ifdef CFG_ADC_3
	adc_chn[3] = CFG_ADC_3 ;
#endif
#ifdef CFG_ADC_4
	adc_chn[4] = CFG_ADC_4 ;
	arg |= 1<<0;
#endif
#ifdef CFG_ADC_5
	adc_chn[5] = CFG_ADC_5 ;
	arg |= 1<<1;
#endif
#ifdef CFG_ADC_6
	adc_chn[6] = CFG_ADC_6 ;
	arg |= 1<<2;
#endif
#ifdef CFG_ADC_7
	adc_chn[7] = CFG_ADC_7 ;
	arg |= 1<<3;
#endif

	if(adc_chn[0] != 0xff || adc_chn[1] != 0xff || adc_chn[2] != 0xff || adc_chn[3] != 0xff  ){
		adc_fd = open("/dev/tlv1504", O_RDONLY);	//���ⲿADC����
		if (adc_fd < 0){
			ret = -ERR_NOFILE;		//û�д�·��
			goto err;
		}
	}
	//���ڲ�ADC����
	if(adc_chn[4] != 0xff || adc_chn[5] != 0xff || adc_chn[6] != 0xff || adc_chn[7] != 0xff  ){
		adi_fd = open("/dev/atmel_adc", O_RDONLY);	//���ⲿADC����
		if (adi_fd < 0){
			ret = -ERR_NOFILE;		//û�д�·��
			goto err;
		}
		ret = ioctl(adi_fd, DS_ADI_CH, arg);
		if (ret < 0){
			ret = -ERR_SYS;
			goto err;
		}
	}
//	//�������ݶ�ȡ��ʽ
//#if CFG_ADC_TYPE
//	adc_type = DS_ADC_IRQ;
//#else
//	adc_type = DS_ADC_QUERY;
//#endif
//	ret = ioctl(fd, DS_ADC_IRQ_QUERY, adc_type);
//	if (ret < 0){
//		ret = -ERR_SYS;		//û�д�·��
//		goto err;
//	}

	adc_count = 1;
	adc_timeout = 1;	//��ʱʱ��Ĭ��Ϊ100ms
	ret = 0;
err:
	return ret;
}

/******************************************************************************
*	����:	adc_setwaittime
*	����:	����ADת����ʱʱ��
*	����:	timeout			-	��ʱʱ��
*	����:	0				-	�ɹ�
			-ERR_INVAL		-	��������
*	˵��:	timeout��λ��100ms
 ******************************************************************************/
int adc_setwaittime(u16 timeout)
{
	if(timeout <= 0 ){
		return -ERR_INVAL;
	}
	adc_timeout = timeout;
	return 0;	
}

/******************************************************************************
*	����:	adc_getwaittime
*	����:	��ȡADת����ʱʱ��
*	����:	timeout			-	��ʱʱ�䣨���ݴ���ָ�룩
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
*	˵��:	timeout��λ��100ms
 ******************************************************************************/
int adc_getwaittime(u16 *timeout)
{
	*timeout = adc_timeout;
	return 0;
}	

/******************************************************************************
*	����:	adc_read
*	����:	��ȡת�����
*	����:	id				-	ADͨ����
			result			-	ADת����������ݴ���ָ�룩
*	����:	0				-	�ɹ�
			-ERR_TIMEOUT	-	��ʱ
			-ERR_NODEV 		-	�޴��豸
			-ERR_NOINIT		-	��û�г�ʼ����
			-ERR_OTHER:		-	���������̻߳������Ĵ���
*	˵��:	result��ADת����ԭʼֵ��
 ******************************************************************************/
int adc_read(u8 id, u16 *result)
{
	int ret = -1;
	int adc_result;
	int timeout= adc_timeout;

	if((id < 0) || (id >= MAX_ADCCHN)){		//din��Χ���
		return -ERR_NODEV;		
	}	
	//��ȡ����
	do{
		if(id <= 3){	//�ⲿAD��ȡ
			
			ret = read(adc_fd, &adc_result, adc_chn[id]);
			if (ret < 0){
				timeout --;
			} 
		}else{			//�ڲ�AD��ȡ			
			ret = read(adi_fd, &adc_result, adc_chn[id]);
			if (ret < 0){
				timeout --;
			}
		}
		usleep(100000);	//��ʱ100ms
	}while((ret < 0) && (timeout > 0));
	if (ret < 0){
		ret = -ERR_TIMEOUT;
		goto err;
	} 
	*result = adc_result;
	ret = 0;
err:	
	if (pthread_mutex_unlock (&mutex)) {
		ret = -ERR_OTHER;	
	}	
	return ret;
}

/******************************************************************************
*	����:	adc_close
*	����:	adcģ��رպ���
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_NOINIT		-	û�г�ʼ��
			-ERR_OTHER		-	���������̻߳������Ĵ���
*	˵��:	��
 ******************************************************************************/
int adc_close(void)
{
	int ret = -1;
		
	if(adc_count == 0){
		return -ERR_NOINIT;
	}
	ret = close(adc_fd);
	if(ret < 0){
		return -ERR_SYS;	
	}
	ret = close(adi_fd);
	if(ret < 0){
		return -ERR_SYS;
	}
	adc_count = 0;
	if (pthread_mutex_destroy(&mutex)) {
		ret = -ERR_OTHER;
	}
	ret = 0;
	return ret;
}
#endif
