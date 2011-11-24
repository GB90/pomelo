/**
* meter.h -- ��Ʋ���ͷ�ļ�
* 
* 
* ����ʱ��: 2010-5-7
* ����޸�ʱ��: 2010-5-7
*/

#ifndef _PARAM_METER_H
#define _PARAM_METER_H

#include "include/param/capconf.h"

/*
Լ��:
ǰ8����ֻ�������๦�ܱ��
�����ֻ�������ز����
���������������
*/

//F10, �ն˵��ܱ�/�����������ò���
#define METTYPE_DL645			1
#define METTYPE_ACSAMP		2
#define METTYPE_DL645_2007		30
#define METTYPE_PLC			31

#define METBAUD_MASK		0xe0
#define METPORT_MASK		0x1f
#define CENMETPORT_MASK	0x02

typedef struct {
	unsigned short index;  //���������,������, �����ط�����
	unsigned short metp_id;    //�����������, 1~2040, 0��ʾ��Ч
	unsigned char portcfg;    //ͨ�����ʼ��˿ں�
	unsigned char proto;    //ͨ�Ź�Լ����, 1-DL645-1997, 2-��������,30-DL645-2007
	unsigned char addr[6];    //ͨ�ŵ�ַ
	unsigned char pwd[6];    //ͨ������
	unsigned char prdnum;    //���ܷ��ʸ���
	unsigned char intdotnum;    //�й�����ʾֵ����λ��С��λ����
	unsigned char owneraddr[6];    //�����ɼ�����ַ
	unsigned char userclass;    //�û������
	unsigned char metclass;    //�������
} para_meter_t;

#ifndef DEFINE_PARAMETER
extern const para_meter_t ParaMeter[MAX_METER];
#endif

#endif /*_PARAM_METER_H*/

