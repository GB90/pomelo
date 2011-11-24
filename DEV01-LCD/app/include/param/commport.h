/**
* commport.h -- ͨ�Ŷ˿ڲ���ͷ�ļ�
* 
* 
* ����ʱ��: 2010-5-8
* ����޸�ʱ��: 2010-5-8
*/

#ifndef _PARAM_COMMPORT_H
#define _PARAM_COMMPORT_H

#define COMMFRAME_BAUD_300		0
#define COMMFRAME_BAUD_600		0x20
#define COMMFRAME_BAUD_1200		0x40
#define COMMFRAME_BAUD_2400		0x60
#define COMMFRAME_BAUD_4800		0x80
#define COMMFRAME_BAUD_7200		0xa0
#define COMMFRAME_BAUD_9600		0xc0
#define COMMFRAME_BAUD_19200	0xe0

#define COMMFRAME_STOPBIT_1		0
#define COMMFRAME_STOPBIT_2		0x10

#define COMMFRAME_NOCHECK		0
#define COMMFRAME_HAVECHECK		0x08

#define COMMFRAME_ODDCHECK		0
#define COMMFRAME_EVENCHECK		0x04

#define COMMFRAME_DATA_5		0
#define COMMFRAME_DATA_6		1
#define COMMFRAME_DATA_7		2
#define COMMFRAME_DATA_8		3

//DL698��Լӳ��˿�
//#define COMMPORT_PLC		0  //�ز��˿�//F33����ʱ��Ӧ�˿�1
#define COMMPORT_PLC		30  //�ز��˿�//F33����ʱ��Ӧ�˿�1
#define COMMPORT_RS485_1	1  //�����˿� 
#define COMMPORT_RS485_2	2  //���ܱ�˿�//F33����ʱ��Ӧ�˿�3
#define COMMPORT_RS485_3	3  //����˿�

#define PLC_PORT				(COMMPORT_PLC + 1)
//#define PLC_PORT				31
//#define CEN_METER_PORT		(COMMPORT_RS485_2 + 1)
#define CEN_METER_PORT		2


//#define MAX_COMMPORT		4
#define MAX_COMMPORT		31

//ʵ��Ӳ��485�˿�
#define COMMPORT_CASCADE	(COMMPORT_RS485_1-1)
#define COMMPORT_CENMET		(COMMPORT_RS485_2-1)
#define COMMPORT_485BUS		(COMMPORT_RS485_3-1)

#define RDMETFLAG_ENABLE		0x0001  //��"1"�������Զ�������"0" Ҫ���ն˸��ݳ���ʱ���Զ�����
#define RDMETFLAG_ALL			0x0002  //��"1"Ҫ���ն�ֻ���ص����"0"Ҫ���ն˳����б�
#define RDMETFLAG_FREZ			0x0004  //Ҫ���ն˲��ù㲥���᳭����"0"��Ҫ��
#define RDMETFLAG_CHECKTIME		0x0008  //��"1"Ҫ���ն˶�ʱ�Ե��㲥Уʱ����"0"��Ҫ��
#define RDMETFLAG_FINDMET		0x0010  //��"1"Ҫ���ն���Ѱ����������ĵ����"0"��Ҫ��
#define RDMETFLAG_RDSTATUS		0x0020  //��"1"Ҫ���ն˳���"���״̬��"����"0"��Ҫ��

//F33 �ն˳������в�������
//F34 ���ն˽ӿڵ�ͨ��ģ��Ĳ�������
#define MAXNUM_PERIOD	24
typedef struct {
	unsigned char hour_start;
	unsigned char min_start;
	unsigned char hour_end;
	unsigned char min_end;
}cfg_period_t;

typedef struct {
	unsigned short flag;  //̨�����г������п�����
	unsigned char time_hour;	//������-ʱ��:ʱ
	unsigned char time_minute;  //������-ʱ��:��
	unsigned int dateflag;  //������-����   �ܱ�
	unsigned char cycle;  //������ʱ��, ��, 1~60  �ܱ�
	unsigned char chktime_day;     //�Ե��㲥Уʱ��ʱʱ��:��
	unsigned char chktime_hour;    //�Ե��㲥Уʱ��ʱʱ��:ʱ
	unsigned char chktime_minute;  //�Ե��㲥Уʱ��ʱʱ��:��
	unsigned char periodnum;	//������ʱ����
	unsigned char reserv;
	cfg_period_t period[MAXNUM_PERIOD];  //������ʱ���  �ܱ�
	
	unsigned int baudrate;  //���ն˽ӿڶ�Ӧ�˵�ͨ�����ʣ�bps��
	unsigned char frame;  //���ն˽ӿڶ˵�ͨ�ſ�����
} para_commport_t;

const para_commport_t *GetParaCommPort(unsigned int port);

#endif /*_PARAM_COMMPORT_H*/

