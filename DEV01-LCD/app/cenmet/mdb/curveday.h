/**
* curveday.h -- �ն�������ʷ���ݽṹ
* 
* 
* ����ʱ��: 2010-5-12
* ����޸�ʱ��: 2010-5-12
*/

#ifndef _CURVE_DAY_H
#define _CURVE_DAY_H

#include "cenmet/mdb/mdbconf.h"

typedef struct {//��������
	unsigned char rdtime[5];    //����ʱ��, ��ʱ������
	unsigned char fenum;

	//F1
	unsigned char enepa[5*MAXNUM_METPRD];  //�����й�����, 0~4, 0.0001kWh
	unsigned char enepi[4*MAXNUM_METPRD];  //�����޹�����, 0~4, 0.01kvarh
	unsigned char enepi1[4*MAXNUM_METPRD];  //�����޹�һ���޵���, 0~4, 0.01kvarh
	unsigned char enepi4[4*MAXNUM_METPRD];  //�����޹������޵���, 0~4, 0.01kvarh
	//F2
	unsigned char enena[5*MAXNUM_METPRD];  //�����й�����, 0~4, 0.0001kWh
	unsigned char eneni[4*MAXNUM_METPRD];  //�����޹�����, 0~4, 0.01kvarh
	unsigned char eneni2[4*MAXNUM_METPRD];  //�����޹������޵���, 0~4, 0.01kvarh
	unsigned char eneni3[4*MAXNUM_METPRD];  //�����޹������޵���, 0~4, 0.01kvarh
	//F3
	unsigned char dmnpa[3*MAXNUM_METPRD];  //���������й��������,0~4,0.0001kW
	unsigned char dmntpa[4*MAXNUM_METPRD];  //���������й������������ʱ��,0~4,��ʱ����
	unsigned char dmnpi[3*MAXNUM_METPRD];  //���������޹��������,0~4,0.0001kvar
	unsigned char dmntpi[4*MAXNUM_METPRD];  //���������޹������������ʱ��,0~4,��ʱ����
	//F4
	unsigned char dmnna[3*MAXNUM_METPRD];  //���·����й��������,0~4,0.0001kW
	unsigned char dmntna[4*MAXNUM_METPRD];  //���·����й������������ʱ��,0~4,��ʱ����
	unsigned char dmnni[3*MAXNUM_METPRD];  //���·����޹��������,0~4,0.0001kvar
	unsigned char dmntni[4*MAXNUM_METPRD];  //���·����޹������������ʱ��,0~4,��ʱ����
	//F5
	unsigned char enepa_day[4*MAXNUM_METPRD];     //���������й�������, 0.0001kWh
	//F6
	unsigned char enepi_day[4*MAXNUM_METPRD];     //���������޹�������, 0.0001kvarh
	//F7
	unsigned char enena_day[4*MAXNUM_METPRD];     //���շ����й�������, 0.0001kWh
	//F8
	unsigned char eneni_day[4*MAXNUM_METPRD];     //���շ����޹�������, 0.0001kvarh
	//F153
	unsigned char enepa_abc[5*3];	//ABC�������й�����ʾֵ, 0.0001kWh
	//F154
	unsigned char enepi_abc[4*3];	//ABC�������޹�����ʾֵ, 0.01kvarh
	//F155
	unsigned char enena_abc[5*3];	//ABC�෴���й�����ʾֵ, 0.0001kWh
	//F156
	unsigned char eneni_abc[4*3];	//ABC�������޹�����ʾֵ, 0.01kvarh
} db_metday_t;

typedef struct {//��������
	//F25
	unsigned char pwra_max[6*4];    //��������й����ʼ�����ʱ��, kW, ��ʱ��
	unsigned char pwra_zero[2*4];    //�����й�����Ϊ��ʱ��, min
	//F26
	unsigned char dm_max[6*4];    //�����������������ʱ��, kW, ��ʱ��
	//F27
	unsigned char vol_extime[10*3];   //��ѹԽ���ۼ�ʱ��, min
	unsigned char vol_maxmin[10*3];   //��ѹ���ֵ��Сֵ������ʱ��V, ��ʱ��
	unsigned char vol_avg[2*3];   //ƽ����ѹ, V
	//F28
	unsigned char amp_unb[2];   //������ƽ��Խ���ۼ�ʱ��, min
	unsigned char vol_unb[2];   //������ƽ��Խ���ۼ�ʱ��, min
	unsigned char ampunb_max[5];  //������ƽ�����ֵ������ʱ��, %, ��ʱ��
	unsigned char volunb_max[5];  //��ѹ��ƽ�����ֵ������ʱ��, %, ��ʱ��
	//F29
	unsigned char amp_extime[4*3];    //����Խ���ۼ�ʱ��, min
	unsigned char zamp_extime[2];    //�������Խ���ۼ�ʱ��, min
	unsigned char amp_max[5*4];    //�������ֵ������ʱ��, A, ��ʱ��
	//F30
	unsigned char pwrv_extime[4];    //���ڹ���Խ���ۼ�ʱ��, min
	//F31
	unsigned char load[10];   //�ո�����ͳ��, %, ��ʱ��
	//F43
	unsigned char pwrf_ptime[6];   //�������������ۼ�ʱ��
	//F32
	unsigned char rdtime[5];
	unsigned char volbr_cnt[8];    //�������,SABC
	unsigned char volbr_times[12];  //�����ۼ�ʱ��,SABC, min
	unsigned char volbr_timestart[16];    //���һ�ζ�����ʼʱ��,SABC,��ʱ����
	unsigned char volbr_timeend[16];    //���һ�ζ������ʱ��,SABC,��ʱ����

	unsigned char reserv;
} db_msticday_t;

typedef struct {//��������
	//F49
	unsigned char pwr_time[2];    //�ն��չ���ʱ��, min
	unsigned char rest_cnt[2];    //�ն��ո�λ����
	//F50
	unsigned char sw_time[4];	 //��բ�ۼƴ���
	//F53
	unsigned char comm_bytes[4];  //�ն�����վ��ͨ������, �ֽ�
} db_termday_t;//�ն�������

typedef struct {
	//F57
	unsigned char pwra_time[5*2];   //�����С���ʼ�����ʱ��
	unsigned char pwra_zero[2];    //�й�����Ϊ0ʱ��, min
	//F58
	unsigned char fenum58;
	unsigned char enepa[4*MAXNUM_METPRD];    //�й�����
	//F59
	unsigned char fenum59;
	unsigned char enepi[4*MAXNUM_METPRD];    //�޹�����

	unsigned char reserv[2];
} db_tgrpday_t;

#endif /*_CURVE_DAY_H*/

