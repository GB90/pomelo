/**
* capconf.h -- ��������
* 
* 
* ����ʱ��: 2010-5-10
* ����޸�ʱ��: 2010-5-10
*/

#ifndef _PARAM_CAPCONF_H
#define _PARAM_CAPCONF_H

//#define MAX_METER    2040   //�����/����������
#define MAX_METER    (2040)  //�����/����������

#define MAX_METP		MAX_METER   //����������
//#define MAX_CENMETP		8     //���๦�ܲ�������, �ز���ƵĲ������ֻ�ܴ������
#define MAX_CENMETP		2     //���๦�ܲ�������, �ز���ƵĲ������ֻ�ܴ������
#define MAX_PLCMET		(MAX_METP-MAX_CENMETP)  //����ز�����
#define PLC_BASEMETP	MAX_CENMETP  //�ز�����ʼ�������(���)
//#define PLC_BASEMETP	3  //�ز�����ʼ�������(���)
//#define PLC_BASEMETP	2  //�ز�����ʼ�������(���)
//#define PLC_BASEMETP	0  //�ز�����ʼ�������(���)

#define MAX_IMPORTANT_USER		20   //����ص��û�����


#define MAX_PULSE	2  //���������
#define MAX_ISIG	2  //״̬�����������
#define MAX_TGRP	8  //�ܼ��������
#define MAX_DIFFA	2  //�й��ܵ�����������

#define MAX_DTASK_CLS1		16   // 1�����������
#define MAX_DTASK_CLS2		32  // 2�����������

#endif /*_PARAM_CAPCONF_H*/

