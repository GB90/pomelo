/**
* curvemon.h -- �¶�������ʷ���ݽṹ
* 
* 
* ����ʱ��: 2010-5-12
* ����޸�ʱ��: 2010-5-12
*/

#ifndef _CURVE_MON_H
#define _CURVE_MON_H

#include "cenmet/mdb/mdbconf.h"

typedef struct {//��������
	unsigned char rdtime[5];    //����ʱ��, ��ʱ������
	unsigned char fenum;  //������

	//F17
	unsigned char enepa[5*MAXNUM_METPRD];  //�����й�����, 0~4, 0.0001kWh
	unsigned char enepi[4*MAXNUM_METPRD];  //�����޹�����, 0~4, 0.01kvarh
	unsigned char enepi1[4*MAXNUM_METPRD];  //�����޹�һ���޵���, 0~4, 0.01kvarh
	unsigned char enepi4[4*MAXNUM_METPRD];  //�����޹������޵���, 0~4, 0.01kvarh
	//F18
	unsigned char enena[5*MAXNUM_METPRD];  //�����й�����, 0~4, 0.0001kWh
	unsigned char eneni[4*MAXNUM_METPRD];  //�����޹�����, 0~4, 0.01kvarh
	unsigned char eneni2[4*MAXNUM_METPRD];  //�����޹������޵���, 0~4, 0.01kvarh
	unsigned char eneni3[4*MAXNUM_METPRD];  //�����޹������޵���, 0~4, 0.01kvarh
	//F19
	unsigned char dmnpa[3*MAXNUM_METPRD];  //���������й��������,0~4,0.0001kW
	unsigned char dmntpa[4*MAXNUM_METPRD];  //���������й������������ʱ��,0~4,��ʱ����
	unsigned char dmnpi[3*MAXNUM_METPRD];  //���������޹��������,0~4,0.0001kvar
	unsigned char dmntpi[4*MAXNUM_METPRD];  //���������޹������������ʱ��,0~4,��ʱ����
	//F20
	unsigned char dmnna[3*MAXNUM_METPRD];  //���·����й��������,0~4,0.0001kW
	unsigned char dmntna[4*MAXNUM_METPRD];  //���·����й������������ʱ��,0~4,��ʱ����
	unsigned char dmnni[3*MAXNUM_METPRD];  //���·����޹��������,0~4,0.0001kvar
	unsigned char dmntni[4*MAXNUM_METPRD];  //���·����޹������������ʱ��,0~4,��ʱ����
	//F21
	unsigned char enepa_day[4*MAXNUM_METPRD];     //���������й�������, 0.0001kWh
	//F22
	unsigned char enepi_day[4*MAXNUM_METPRD];     //���������޹�������, 0.0001kvarh
	//F23
	unsigned char enena_day[4*MAXNUM_METPRD];     //���·����й�������, 0.0001kWh
	//F24
	unsigned char eneni_day[4*MAXNUM_METPRD];     //���·����޹�������, 0.0001kvarh
	//F157
	unsigned char enepa_abc[5*3];	//ABC�������й�����ʾֵ, 0.0001kWh
	//F158
	unsigned char enepi_abc[4*3];	//ABC�������޹�����ʾֵ, 0.01kvarh
	//F159
	unsigned char enena_abc[5*3];	//ABC�෴���й�����ʾֵ, 0.0001kWh
	//F160
	unsigned char eneni_abc[4*3];	//ABC�������޹�����ʾֵ, 0.01kvarh
} db_metmon_t;

typedef struct {//��������
	//F33
	unsigned char pwra_max[6*4];    //��������й����ʼ�����ʱ��, kW, ��ʱ��
	unsigned char pwra_zero[2*4];    //�����й�����Ϊ��ʱ��, min
	//F34
	unsigned char dm_max[6*4];    //�����������������ʱ��, kW, ��ʱ��
	//F35
	unsigned char vol_extime[10*3];   //��ѹԽ���ۼ�ʱ��, min
	unsigned char vol_maxmin[10*3];   //��ѹ���ֵ��Сֵ������ʱ��V, ��ʱ��
	unsigned char vol_avg[2*3];   //ƽ����ѹ, V
	//F36
	unsigned char amp_unb[2];   //������ƽ��Խ���ۼ�ʱ��, min
	unsigned char vol_unb[2];   //��ѹ��ƽ��Խ���ۼ�ʱ��, min
	unsigned char ampunb_max[6];  //������ƽ�����ֵ������ʱ��, %, ��ʱ����
	unsigned char volunb_max[6];  //��ѹ��ƽ�����ֵ������ʱ��, %, ��ʱ����
	//F37
	unsigned char amp_extime[4*3];    //����Խ���ۼ�ʱ��, min
	unsigned char zamp_extime[2];    //�������Խ���ۼ�ʱ��, min
	unsigned char amp_max[5*4];    //�������ֵ������ʱ��, A, ��ʱ��
	//F38
	unsigned char pwrv_extime[4];    //���ڹ���Խ���ۼ�ʱ��, min
	//F39
	unsigned char load[12];   //�¸�����ͳ��, %, ��ʱ����
	//F44
	unsigned char pwrf_ptime[6];   //�������������ۼ�ʱ��
} db_msticmon_t;

typedef struct {//��������
	//F51
	unsigned char pwr_time[2];    //�ն��¹���ʱ��, min
	unsigned char rest_cnt[2];    //�ն��¸�λ����
	//F52
	unsigned char sw_time[4];	 //��բ�ۼƴ���
	//F54
	unsigned char comm_bytes[4];  //�ն�����վ��ͨ������, �ֽ�
} db_termmon_t;

typedef struct {
	//F60
	unsigned char pwra_time[5*2];   //�����С���ʼ�����ʱ��
	unsigned char pwra_zero[2];    //�й�����Ϊ0ʱ��, min
	//F65
	unsigned char pcuene[6];    //�����ʶ�ֵ���ۼ�ʱ�估�ۼƵ���
	//F66
	unsigned char ecuene[6];    //���µ�����ֵ���ۼ�ʱ�估�ۼƵ���

	//F61
	unsigned char fenum61;
	unsigned char enepa[4*MAXNUM_METPRD];    //�й�����
	//F62
	unsigned char fenum62;
	unsigned char enepi[4*MAXNUM_METPRD];    //�޹�����

	unsigned char reserv[2];
} db_tgrpmon_t;

#endif /*_CURVE_MON_H*/

