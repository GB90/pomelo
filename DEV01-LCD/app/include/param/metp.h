/**
* metp.h -- ���������ͷ�ļ�
* 
* 
* ����ʱ��: 2010-5-7
* ����޸�ʱ��: 2010-5-7
*/

#ifndef _PARAM_METP_H
#define _PARAM_METP_H

#include "include/param/capconf.h"

#define PWRTYPE_3X3W	1
#define PWRTYPE_3X4W	2
#define PWRTYPE_1X		3

//F25, �������������
typedef struct {
	unsigned short pt;    //��ѹ����������
	unsigned short ct;    //��������������
	unsigned short vol_rating;    //���ѹ, 0.1V
	unsigned short amp_rating;    //�����, 0.01A
	unsigned int pwr_rating;   //�����, 0.0001kVA
	unsigned char pwrtype;    //��Դ���߷�ʽ, 
	                             // 1=��������,2=��������,3=����
	unsigned char pwrphase;   //����������, 0-unknown, 1-A, 2-B, 3-C
} cfg_mpbase_t;

//F26, ��������ֵ����
typedef struct {
	unsigned short volok_up;    //��ѹ�ϸ�����, 0.1V
	unsigned short volok_low;    //��ѹ�ϸ�����, 0.1V
	unsigned short vol_lack;    //��ѹ��������, 0.1V
	unsigned short vol_over;    //��ѹ����, 0.1V
	unsigned short vol_less;    //Ƿѹ����, 0.1V

	unsigned short amp_over;    //��������, 0.01A
	unsigned short amp_limit;    //���������, 0.01A
	unsigned short zamp_limit;    //�����������, 0.01A

	unsigned int pwr_over;    //���ڹ���������, 0.0001kVA
	unsigned int pwr_limit;    //���ڹ�������, 0.0001kVA

	unsigned short vol_unb;    //�����ѹ��ƽ����ֵ, 0.1%
	unsigned short amp_unb;    //���������ƽ����ֵ, 0.1%

	unsigned char time_volover;   //Խ�޳���ʱ��, ��
	unsigned char time_volless;
	unsigned char time_ampover;
	unsigned char time_amplimit;
	unsigned char time_zamp;
	unsigned char time_pwrover;
	unsigned char time_pwrlimit;
	unsigned char time_volunb;
	unsigned char time_ampunb;
	unsigned char time_volless_2;    //����ʧѹʱ����ֵ, min

	unsigned int restore_pwrover;
	unsigned int restore_pwrlimit;
	unsigned short restore_volover;  //��ѹԽ�޻ָ���ѹ, 0.1V, ������ͬ
	unsigned short restore_volless;
	unsigned short restore_ampover;
	unsigned short restore_amplimit;
	unsigned short restore_zamp;
	unsigned short restore_volunb;
	unsigned short restore_ampunb;
} cfg_mplimit_t;

//F27, ������ͭ��, �������
typedef struct {
	unsigned short Ra;  //A�����
	unsigned short Xa;  //A��翹
	unsigned short Ga;  //A��絼
	unsigned short Ba;  //A�����

	unsigned short Rb;  //B�����
	unsigned short Xb;  //B��翹
	unsigned short Gb;  //B��絼
	unsigned short Bb;  //B�����

	unsigned short Rc;  //C�����
	unsigned short Xc;  //C��翹
	unsigned short Gc;  //C��絼
	unsigned short Bc;  //C�����
} cfg_mpcopper_t;

//F28, �����㹦�������ֶ���ֵ
typedef struct {
// 3������, A<limit1<=B<limit2<=C
	unsigned short limit1;    //�ֶ���ֵ1, 0.1%
	unsigned short limit2;
} cfg_mppwrf_t;

//F29, �ն˵��ص��ܱ���ʾ��
typedef struct {
	unsigned char no[12];
} cfg_mpdisplay_t;

//�๦�ܲ��������
typedef struct {
	unsigned short mid;   //use for save
	unsigned char stopped;   //F30, �ն�̨�����г���ͣ��/Ͷ������
	unsigned char reserv;
	cfg_mpbase_t base;
	cfg_mplimit_t limit;
	cfg_mpcopper_t copper;
	cfg_mppwrf_t pwrf;
	unsigned char displayno[12];   //F29, �ն˵��ص��ܱ���ʾ��
} para_cenmetp_t;

//�ز����������
typedef struct {
	unsigned short mid;   //use for save
	unsigned char phase;   //����������, 0-unknown, 1-A, 2-B, 3-C
	unsigned char stopped;   //F30, �ն�̨�����г���ͣ��/Ͷ������
	unsigned char displayno[12];   //F29, �ն˵��ص��ܱ���ʾ��
} para_plcmetp_t;

//F31 �ز��ӽڵ㸽���ڵ��ַ
#define MAX_CHILDEND	20
typedef struct {
	unsigned short mid;   //use for save
	unsigned char reserv;
	unsigned char num;
	unsigned char addr[MAX_CHILDEND*6];
} para_childend_t;

//�����㵽��������ӳ��
#define METP_NONE		0
#define METP_METER		1
#define METP_PULSE		2

typedef struct {
	unsigned char type;   //����, 0-��Ч, 1-���, 2-����
	unsigned char reserv;
	unsigned short metid;   //���-��Ӧ����,����ŵȵ�
} metp_map_t;


#ifndef DEFINE_PARAMETP
extern const para_cenmetp_t ParaCenMetp[MAX_CENMETP];
extern const para_plcmetp_t ParaPlcMetp[MAX_METP];
extern const para_childend_t ParaChildEnd[MAX_METP];
extern const metp_map_t CenMetpMap[MAX_CENMETP];
#endif

void MappingCenMetp(void);

#define metp_pwrtype(mid)    (ParaCenMetp[mid].base.pwrtype)
#define EMPTY_CENMETP(mid)		(CenMetpMap[mid].type == METP_NONE)

#endif /*_PARAM_METP_H*/

