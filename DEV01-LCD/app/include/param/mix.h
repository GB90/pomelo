/**
* mix.h -- �ۺϲ���ͷ�ļ�
* 
* 
* ����ʱ��: 2010-5-8
* ����޸�ʱ��: 2010-5-8
*/

#ifndef _PARAM_MIX_H
#define _PARAM_MIX_H

#include "include/param/commport.h"
#include "include/param/capconf.h"

//F35 �ն�̨�����г����ص㻧����
typedef struct {
	unsigned char num;    //<=20
	unsigned char reserv;
	unsigned short metid[MAX_IMPORTANT_USER];  //�ص㻧�ĵ��ܱ�/��������װ�����
} cfg_impuser_t;

//F37 �ն˼���ͨ�Ų���
#define MAX_CASCADE		3
typedef struct {
	unsigned char port;  //�ն˼���ͨ�Ŷ˿ں�
	unsigned char frame;   //�ն˼���ͨ�ſ�����
	unsigned char timeout;  //���յȴ����ĳ�ʱʱ��, 100ms
	unsigned char timeout_byte;  //���յȴ��ֽڳ�ʱʱ��, 10ms
	unsigned char retry;  //������(����վ)����ʧ���ط�����
	unsigned char cycle;  //����Ѳ������, minute
	unsigned char flag;  //����/��������־, 0-����վ, 1-��������
	unsigned char num;  //������/�������ն˸���
	unsigned char addr[MAX_CASCADE*4];  //�������ն˵�ַ
} cfg_cascade_t;

//F21 �ն˵���������ʱ�κͷ�����
typedef struct {
	unsigned char period[48];
	unsigned char fenum;
	unsigned char reserv[3];
} cfg_feprd_t;

//F59 ���ܱ��쳣�б���ֵ�趨
typedef struct {
	unsigned char diff;  //������������ֵ
	unsigned char fly;  //���ܱ������ֵ
	unsigned char stop;  //���ܱ�ͣ����ֵ
	unsigned char time;  //���ܱ�Уʱ��ֵ
} cfg_metabnor_t;

//F60 г����ֵ
typedef struct {
	unsigned short vol_sum;  //�ܻ����ѹ����������, 0.1%
	unsigned short vol_odd;  //���г����ѹ����������
	unsigned short vol_even;  //ż��г����ѹ����������
	unsigned short vol_2;  //2��г����ѹ����������
	unsigned short vol_4;
	unsigned short vol_6;
	unsigned short vol_8;
	unsigned short vol_10;
	unsigned short vol_12;
	unsigned short vol_14;
	unsigned short vol_16;
	unsigned short vol_18;
	unsigned short vol_3;  //3��г����ѹ����������
	unsigned short vol_5;
	unsigned short vol_7;
	unsigned short vol_9;
	unsigned short vol_11;
	unsigned short vol_13;
	unsigned short vol_15;
	unsigned short vol_17;
	unsigned short vol_19;
	unsigned short amp_sum;  //�ܻ��������Чֵ����, 0.01A
	unsigned short amp_2;  //2��г��������Чֵ����
	unsigned short amp_4;
	unsigned short amp_6;
	unsigned short amp_8;
	unsigned short amp_10;
	unsigned short amp_12;
	unsigned short amp_14;
	unsigned short amp_16;
	unsigned short amp_18;
	unsigned short amp_3;  //3��г��������Чֵ����
	unsigned short amp_5;
	unsigned short amp_7;
	unsigned short amp_9;
	unsigned short amp_11;
	unsigned short amp_13;
	unsigned short amp_15;
	unsigned short amp_17;
	unsigned short amp_19;
} cfg_syntony_t;

typedef struct {
	unsigned char bactsend;  //��ֹ�����ϱ�, 1-��ֹ,0-����ֹ
	unsigned char bactcomm;  //��ֹ����վͨ��, 1-��ֹ,0-����ֹ
	unsigned char reserv[3];
	cfg_feprd_t feprd;
	cfg_impuser_t impuser;
	unsigned int upflow_max;  //F36 �ն�����ͨ�������������� byte(��ͨ������) 0��ʾ�������
	cfg_cascade_t cascade;
	cfg_metabnor_t metabnor;
	cfg_syntony_t syntony;
} para_mix_t;

typedef struct {
	para_commport_t commport[MAX_COMMPORT];
	para_mix_t mix;
} save_mix_t;

#ifndef DEFINE_PARAMIX
extern const save_mix_t ParaMixSave;
#define ParaMix		(ParaMixSave.mix)
#endif

#endif /*_PARAM_MIX_H*/

