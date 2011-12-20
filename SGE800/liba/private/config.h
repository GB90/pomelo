#ifndef _CONFIG_H
#define _CONFIG_H

#include "../include/pinio.h"

//��������
//#define CFG_DEBUG 1

#ifdef CFG_DEBUG
#define CFG_DEBUG_DIN
#define CFG_DEBUG_PULSE
#define CFG_DEBUG_POWERCHECK
#define CFG_DEBUG_ADC
#define CFG_DEBUG_RTC
#define CFG_DEBUG_TIMER
#define CFG_DEBUG_GPIO

#define CFG_DEBUG_COMPORT
#define CFG_DEBUG_NET
#define CFG_DEBUG_GSERIAL

#define CFG_DEBUG_DBS
#define CFG_DEBUG_FILE

#define CFG_DEBUG_THREAD
#define CFG_DEBUG_MSG
#endif

//ADת��ģ������
#define CFG_ADC_MODULE				1			//�Ƿ�����
#define CFG_ADC_NUM					8			//��ǰADCͨ������
#define CFG_ADC_0					0			//0��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_ADC_1					1			//1��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_ADC_2					2			//2��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_ADC_3					3			//3��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_ADC_4					4			//4��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_ADC_5					5			//5��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_ADC_6					6			//6��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_ADC_7					7			//7��ͨ����Ӧ��Ӳ��ͨ��

//��ʱ��ģ������
#define CFG_TIMER_MODULE			1			//�Ƿ�����
#define CFG_TIMER_PWM_S				1			//�������ʱ��Դ���߾���
#define CFG_TIMER_MEASURE_S			2			//Ƶ�ʲ���ʱ��Դ���;���

//RTCģ������
#define CFG_RTC_MODULE				1				//�Ƿ�����

//IO����ģ������
#define CFG_GPIO_MODULE				1				//�Ƿ�����

//�����ϵ���ģ������
#define CFG_POWERCHECK_MODULE			1			//�Ƿ�����
#define CFG_POWERCHECK_IO				PIN_PB27	//���������io��

//����ģ��
#define CFG_NET_MODULE              1
#define CFG_NET_SERVPORT            3333                 //�˿ں�
#define CFG_NET_BACKLOG             8                    //����
#define CFG_NET_MAXSIZE             8                    //�������ͻ�����--���Ϊ64

//��������	
#define CFG_COMPORT_MODULE		1			//�Ƿ�����

//USB�ӿ����⴮������	
#define CFG_GSERIAL_MODULE		1			//�Ƿ�����

//���ݿ�����
#define CFG_DBS_MODULE				1				//�Ƿ�����
#define CFG_DBS_ENV_FLAG     		DB_INIT_CDB |DB_CREATE |DB_PRIVATE|DB_INIT_MPOOL|DB_THREAD      //���ݿ⻷������
#define CFG_DBS_MPOOL_SIZE    		4*1024*1024          //�����ڴ�ش�С��Ĭ��256K
#define CFG_DBS_ENV_HOME      		"/mnt/local"         //���ô򿪻�����Ŀ¼
#define CFG_DBS_UNM_MAX       		16                   //����֧�ֵ����ݿ����
#define CFG_DBS_ARITH         		DB_HASH              //���ݿ���ʹ�õ��㷨
#define CFG_DBS_RECORD_MAX    		128                  //�������ݼ�¼����ֽ���
#define CFG_DBS_KEYSIZE_MAX   		8                    //�ؼ�������ֽ���

//�ļ�����ģ������
#define CFG_FILE_MODULE				1				//�Ƿ�����
#define CFG_FILE_NAME_MAX			32				//�ļ�������󳤶�
#define CFG_FILE_DATA_MAX			0x1000			//�ļ����ݶ�д����󳤶�

//�̲߳���ģ������
#define CFG_THREAD_MODULE			1				//�Ƿ�����
#define CFG_THREAD_MAX				32				//���֧���߳���

//��Ϣ����ģ������
#define CFG_MSG_MODULE				1				//�Ƿ�����
#define CFG_MSG_MAX					32				//���֧����Ϣ������
#define CFG_MSG_SIZE				64				//��Ϣ�����������ɵ���Ϣ��

//ͬ��ģ��
#define CFG_SYN_MODULE              1
#define CFG_MUTEX_MAX               64                   //֧�ֵĻ����������
#define CFG_SEM_MAX                 64                   //֧�ֵ��ź��������
#define CFG_RWLOCK_MAX              64                   //֧�ֵĶ�д�������

//�̵���ģ������
#define CFG_RELAY_MODULE			1				//�Ƿ�����
#define CFG_RELAY_NUM				3				//��ǰRELAYͨ������

#define CFG_RELAY_0					PIN_PB22			//0��ͨ����Ӧ��Ӳ��ͨ��
//#undef  CFG_RELAY_D0
#define CFG_RELAY_D0				PIN_PB19		//0��ͨ��˫���Ӧ��Ӳ��ͨ��
#define CFG_RELAY_TYPE0				0				//IO�ߵ���̵�������֮�����ϵ��1�߶�����0�Ͷ���

#define CFG_RELAY_1					PIN_PB23			//1��ͨ����Ӧ��Ӳ��ͨ��
//#undef  CFG_RELAY_D1
#define CFG_RELAY_D1				PIN_PB20		//1��ͨ��˫���Ӧ��Ӳ��ͨ��
#define CFG_RELAY_TYPE1				0				//IO�ߵ���̵�������֮�����ϵ��1�߶�����0�Ͷ���

#define CFG_RELAY_2					PIN_PB24			//2��ͨ����Ӧ��Ӳ��ͨ��
//#undef  CFG_RELAY_D2
#define CFG_RELAY_D2				PIN_PB21		//2��ͨ��˫���Ӧ��Ӳ��ͨ��
#define CFG_RELAY_TYPE2				0				//IO�ߵ���̵�������֮�����ϵ��1�߶�����0�Ͷ���

//#define CFG_RELAY_3					PIN_PB3			//3��ͨ����Ӧ��Ӳ��ͨ��
////#undef  CFG_RELAY_D3
//#define CFG_RELAY_D3				PIN_PB13		//3��ͨ����Ӧ��Ӳ��ͨ��
//#define CFG_RELAY_TYPE3				1				//IO�ߵ���̵�������֮�����ϵ��1�߶�����0�Ͷ���
//
//#define CFG_RELAY_4					PIN_PB4			//4��ͨ����Ӧ��Ӳ��ͨ��
////#undef  CFG_RELAY_D4
//#define CFG_RELAY_D4				PIN_PB14		//4��ͨ����Ӧ��Ӳ��ͨ��
//#define CFG_RELAY_TYPE4				1				//IO�ߵ���̵�������֮�����ϵ��1�߶�����0�Ͷ���
//
//#define CFG_RELAY_5					PIN_PB5			//5��ͨ����Ӧ��Ӳ��ͨ��
////#undef  CFG_RELAY_D5
//#define CFG_RELAY_D5				PIN_PB15		//5��ͨ����Ӧ��Ӳ��ͨ��
//#define CFG_RELAY_TYPE5				1				//IO�ߵ���̵�������֮�����ϵ��1�߶�����0�Ͷ���
//
//#define CFG_RELAY_6					PIN_PB6			//6��ͨ����Ӧ��Ӳ��ͨ��
////#undef  CFG_RELAY_D6
//#define CFG_RELAY_D6				PIN_PB16		//6��ͨ����Ӧ��Ӳ��ͨ��
//#define CFG_RELAY_TYPE6				1				//IO�ߵ���̵�������֮�����ϵ��1�߶�����0�Ͷ���
//
//#define CFG_RELAY_7					PIN_PB7			//7��ͨ����Ӧ��Ӳ��ͨ��
////#undef  CFG_RELAY_D7
//#define CFG_RELAY_D7				PIN_PB17		//7��ͨ����Ӧ��Ӳ��ͨ��
//#define CFG_RELAY_TYPE7				1				//IO�ߵ���̵�������֮�����ϵ��1�߶�����0�Ͷ���

//״̬��ģ������
#define CFG_LED_MODULE			1				//�Ƿ�����
#define CFG_LED_NUM				8				//��ǰLEDͨ������

#define CFG_LED_0				PIN_PB0			//0��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_LED_TYPE0			1				//IO�ߵ���LED����֮�����ϵ��1������0����
#define CFG_LED_1				PIN_PB1			//1��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_LED_TYPE1			1				//IO�ߵ���LED����֮�����ϵ��1������0����

#define CFG_LED_2				PIN_PB2			//2��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_LED_TYPE2			1				//IO�ߵ���LED����֮�����ϵ��1������0����

#define CFG_LED_3				PIN_PB3			//3��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_LED_TYPE3			1				//IO�ߵ���LED����֮�����ϵ��1������0����

#define CFG_LED_4				PIN_PB4			//4��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_LED_TYPE4			1				//IO�ߵ���LED����֮�����ϵ��1������0����

#define CFG_LED_5				PIN_PB5			//5��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_LED_TYPE5			1				//IO�ߵ���LED����֮�����ϵ��1������0����

#define CFG_LED_6				PIN_PB6			//6��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_LED_TYPE6			1				//IO�ߵ���LED����֮�����ϵ��1����������

#define CFG_LED_7				PIN_PB7			//7��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_LED_TYPE7			1				//IO�ߵ���LED����֮�����ϵ��1������0����

//ң����������ģ������
#define CFG_SWIN_MODULE				1			//�Ƿ�����

#define CFG_SWIN_DOOR				PIN_PB5		//�Žӵ��źŶ�ӦIO
#define CFG_SWIN_LID_TOP			PIN_PB6		//������źŶ�ӦIO
#define CFG_SWIN_LID_MID			PIN_PB7		//���и��źŶ�ӦIO
#define CFG_SWIN_LID_TAIL			PIN_PB5		//��β���źŶ�ӦIO

#define CFG_SWIN_NUM				1			//ң��������������
#define CFG_SWIN_0					PIN_PB11		//0��ң�Ż�����ͨ����Ӧ��io��
//#define CFG_SWIN_1					PIN_PA5		//1��ң�Ż�����ͨ����Ӧ��io��
//#define CFG_SWIN_2					PIN_PA6		//2��ң�Ż�����ͨ����Ӧ��io��
//#define CFG_SWIN_3					PIN_PA7		//3��ң�Ż�����ͨ����Ӧ��io��
//#define CFG_SWIN_4					PIN_PB8		//4��ң�Ż�����ͨ����Ӧ��io��
//#define CFG_SWIN_5					PIN_PB9		//1��ң�Ż�����ͨ����Ӧ��io��
//#define CFG_SWIN_6					PIN_PB10	//2��ң�Ż�����ͨ����Ӧ��io��
//#define CFG_SWIN_7					PIN_PB11	//3��ң�Ż�����ͨ����Ӧ��io��

#define CFG_SWIN_TIMER				5			//���ڲ�����������Ķ�ʱ����
#define CFG_SWIN_FILTER_TIME		5			//���������������λms��
#define CFG_SWIN_FILTER_NUM			5			//������������

//����ģ������
#define CFG_KEY_MODULE			1				//�Ƿ�����
#define CFG_KEY_NUM				2				//��ǰKEYͨ������

#define CFG_KEY_LONG			3000			//������ʼ��Ӧ�������msΪ������λ����ע�ⲻҪ���ڰ�����ȡapi�ĳ�ʱ��
#define CFG_KEY_PER				500				//�������ؼ������msΪ������λ����ע�ⲻҪС�ڰ���ɨ��ʱ��
#define CFG_KEY_SCAN_DELAY		50				//����ɨ����ʱ����λms���Ƽ�20~100ms�䡣
#define CFG_KEY_MODE			1				//������Ӧģʽ��1-���·��أ�0-�ͷŷ���

#define CFG_KEY_1				PIN_PB11		//1��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_KEY_TYPE1			0				//IO�ߵ��밴���Ƿ���֮�����ϵ��1�߰��£�0�Ͱ���

#define CFG_KEY_2				PIN_PB27		//2��ͨ����Ӧ��Ӳ��ͨ��
#define CFG_KEY_TYPE2			1				//IO�ߵ��밴���Ƿ���֮�����ϵ��1�߰��£�0�Ͱ���

//#define CFG_KEY_3				PIN_PB3			//3��ͨ����Ӧ��Ӳ��ͨ��
//#define CFG_KEY_TYPE3			1				//IO�ߵ��밴���Ƿ���֮�����ϵ��1�߰��£�0�Ͱ���
//
//#define CFG_KEY_4				PIN_PB4			//4��ͨ����Ӧ��Ӳ��ͨ��
//#define CFG_KEY_TYPE4			1				//IO�ߵ��밴���Ƿ���֮�����ϵ��1�߰��£�0�Ͱ���
//
//#define CFG_KEY_5				PIN_PB5			//5��ͨ����Ӧ��Ӳ��ͨ��
//#define CFG_KEY_TYPE5			1				//IO�ߵ��밴���Ƿ���֮�����ϵ��1�߰��£�0�Ͱ���
//
//#define CFG_KEY_6				PIN_PB6			//6��ͨ����Ӧ��Ӳ��ͨ��
//#define CFG_KEY_TYPE6			1				//IO�ߵ��밴���Ƿ���֮�����ϵ��1�߰��£�0�Ͱ���
//
//#define CFG_KEY_7				PIN_PB7			//7��ͨ����Ӧ��Ӳ��ͨ��
//#define CFG_KEY_TYPE7			1				//IO�ߵ��밴���Ƿ���֮�����ϵ��1�߰��£�0�Ͱ���

//ֱ��ģ����ģ������
#define CFG_DCSIG_MODULE				1			//�Ƿ�����
#define CFG_DCSIG_NUM					4			//��ǰͨ������
#define CFG_DCSIG_BIT					10			//adת��λ��
#define CFG_DCSIG_REFL					0			//�ο���ѹ��
#define CFG_DCSIG_REFH					2			//�ο���ѹ��

#endif /* _CONFIG_H */
