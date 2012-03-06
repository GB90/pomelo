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
#include "private/debug.h"
	
/*************************************************
  ��̬ȫ�ֱ������궨��
*************************************************/
#define MAX_ADCCHN 8			//���io������
static pthread_mutex_t mutex;			//������
static u8 	adc_chn[CFG_ADC_NUM];		//ADCͨ����Ӧ������ͨ��
static u8 	adc_count = 0;				//ģ��򿪼���
static int 	fd;							//�ⲿad�ļ�������
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
	//����adcͨ��
#ifdef CFG_ADC_0
	adc_chn[CFG_ADC_0] = CFG_ADC_0 ;
#endif
#ifdef CFG_ADC_1
	adc_chn[CFG_ADC_1] = CFG_ADC_1 ;
#endif
#ifdef CFG_ADC_2
	adc_chn[CFG_ADC_2] = CFG_ADC_2;
#endif
#ifdef CFG_ADC_3
	adc_chn[CFG_ADC_3] = CFG_ADC_3 ;
#endif
#ifdef CFG_ADC_4
	adc_chn[CFG_ADC_4] = CFG_ADC_4 ;
#endif
#ifdef CFG_ADC_5
	adc_chn[CFG_ADC_5] = CFG_ADC_5 ;
#endif
#ifdef CFG_ADC_6
	adc_chn[CFG_ADC_6] = CFG_ADC_6 ;
#endif
#ifdef CFG_ADC_7
	adc_chn[CFG_ADC_7] = CFG_ADC_7 ;
#endif

	
	fd = open("/dev/tlv1504", O_RDONLY);	//���ⲿADC����
	if (fd < 0){
		ret = -ERR_NOFILE;		//û�д�·��
		goto err;
	}	
	//���ڲ�ADC����
	
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
	int timeout;
	timeout = adc_timeout;
	if((id < 0) || (id >= MAX_ADCCHN)){		//din��Χ���
		return -ERR_NODEV;		
	}	
	//��ȡ����
	do{
		if(id <= CHANNEL7){	//�ⲿAD��ȡ
			
			ret = read(fd, &adc_result, adc_chn[id]);
			if (ret < 0){
				timeout --;
			} 
		}else{			//�ڲ�AD��ȡ			
			adc_result = 0;//ģ��
			ret=0;
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
	ret = close(fd);
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
