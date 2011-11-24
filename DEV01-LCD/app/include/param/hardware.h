/**
* hardware.h -- Ӳ������
* 
* 
* ����ʱ��: 2010-5-8
* ����޸�ʱ��: 2010-5-8
*/

#ifndef _PARAM_HARDWARE_H
#define _PARAM_HARDWARE_H

#include "include/param/capconf.h"

#define PLSTYP_POSA    0    //�����й�
#define PLSTYP_POSI    1    //�����޹�
#define PLSTYP_NEGA    2    //�����й�
#define PLSTYP_NEGI    3    //�����޹�
//F11, �ն��������ò���
typedef struct {
	unsigned char metp_id;    //�����������, 1~16
	unsigned char type;    //��������
	unsigned short consk;    //���峣��
} para_pulse_t;

//F12, ״̬���������
typedef struct {
	unsigned char flagin;    //�����־λ
	unsigned char flagattr;    //���Ա�־λ
	unsigned char reserv[2];
} para_isig_t;

//F13, ��ѹ/����ģ�������ò���,����

#define TGRFLG_OP    0x80
#define TGRFLG_DIRECT    0x40
#define TGRFLG_METP    0x3f
#define MAX_TGRP_METP		15
//F14, �ն��ܼ������ò���
//ע��!!�ܼ���Ĳ�����Ŵ�0~3, ������1~4
typedef struct {
	unsigned char num;
	unsigned char flag[MAX_TGRP_METP];
} para_tgrp_t;

//F15, �й��ܵ������Խ���¼���������
typedef struct {
	unsigned char tgrp_con;    //�Ա��ܼ����
	unsigned char tgrp_ref;    //�����ܼ����
	unsigned char flag;    //��־,
	                      //D0~D1=0-60���ӵ���, 1-30���ӵ���, 2-15���ӵ���
	                      //D7=0-��ԶԱ�, =1-���ԶԱ�
	unsigned char percent;    //�Խ����Բ�ֵ, %
	int absdiff;    //�Խ�޾��Բ�ֵ,kWh
} para_diffa_t;

typedef struct {
	para_pulse_t pulse[MAX_PULSE];
	para_isig_t isig;
	para_tgrp_t tgrp[MAX_TGRP];
	para_diffa_t diffa[MAX_DIFFA];
} para_hardw_t;

#ifndef DEFINE_PARAHARDWARE
extern const para_hardw_t ParaHardw;
#define ParaPulse		(ParaHardw.pulse)
#define ParaIsig		(ParaHardw.isig)
#define ParaTGrp		(ParaHardw.tgrp)
#define ParaDiffa		(ParaHardw.diffa)
#endif

#define EMPTY_TGRP(tid)		(ParaHardw.tgrp[tid].num == 0)

#endif /*_PARAM_HARDWARE_H*/

