/**
* curveday.h -- �����ն�������ʷ���ݽṹ
* 
* 
* ����ʱ��: 2010-5-12
* ����޸�ʱ��: 2010-5-12
*/

#ifndef _CURVE_RMD_H
#define _CURVE_RMD_H

#include "cenmet/mdb/mdbconf.h"
	
typedef struct {//��������
	unsigned char rdtime[5];	 //����ʱ��, ��ʱ������
	unsigned char fenum;

	//F9
	unsigned char enepa[5*MAXNUM_METPRD];  //�����й�����, 0~4, 0.0001kWh
	unsigned char enepi[4*MAXNUM_METPRD];  //�����޹�����, 0~4, 0.01kvarh
	unsigned char enepi1[4*MAXNUM_METPRD];	//�����޹�һ���޵���, 0~4, 0.01kvarh
	unsigned char enepi4[4*MAXNUM_METPRD];	//�����޹������޵���, 0~4, 0.01kvarh
	//F10
	unsigned char enena[5*MAXNUM_METPRD];  //�����й�����, 0~4, 0.0001kWh
	unsigned char eneni[4*MAXNUM_METPRD];  //�����޹�����, 0~4, 0.01kvarh
	unsigned char eneni2[4*MAXNUM_METPRD];	//�����޹������޵���, 0~4, 0.01kvarh
	unsigned char eneni3[4*MAXNUM_METPRD];	//�����޹������޵���, 0~4, 0.01kvarh
	//F11
	unsigned char dmnpa[3*MAXNUM_METPRD];  //���������й��������,0~4,0.0001kW
	unsigned char dmntpa[4*MAXNUM_METPRD];	//���������й������������ʱ��,0~4,��ʱ����
	unsigned char dmnpi[3*MAXNUM_METPRD];  //���������޹��������,0~4,0.0001kvar
	unsigned char dmntpi[4*MAXNUM_METPRD];	//���������޹������������ʱ��,0~4,��ʱ����
	//F12
	unsigned char dmnna[3*MAXNUM_METPRD];  //���·����й��������,0~4,0.0001kW
	unsigned char dmntna[4*MAXNUM_METPRD];	//���·����й������������ʱ��,0~4,��ʱ����
	unsigned char dmnni[3*MAXNUM_METPRD];  //���·����޹��������,0~4,0.0001kvar
	unsigned char dmntni[4*MAXNUM_METPRD];	//���·����޹������������ʱ��,0~4,��ʱ����
} db_metrmd_t;
	
#endif /*_CURVE_RMD_H*/
	
