/**
* imet_save.h -- �ڲ������������ݴ���
* 
* 
* ����ʱ��: 2010-6-11
* ����޸�ʱ��: 2010-6-11
*/

#ifndef _IMET_SAVE_H
#define _IMET_SAVE_H

struct imetene_save {
	unsigned char year;
	unsigned char month;
	unsigned char unuse[2];

	unsigned int enepa;  //�����й�������, 0.01kWh
	unsigned int enepi;  //�����޹�������, 0.01kVarh
	unsigned int enepi1;  //һ�����޹�������, 0.01kVarh
	unsigned int enepi4;  //�������޹�������, 0.01kVarh
	unsigned int enepaa[3];  //����ABC���й�������, 0.01kWh
	unsigned int enepia[3];  //����ABC���޹�������, 0.01kVarh

	unsigned int enena;  //�����й�������, 0.01kWh
	unsigned int eneni;  //�����޹�������, 0.01kVarh
	unsigned int eneni2;  //�������޹�������, 0.01kVarh
	unsigned int eneni3;  //�������޹�������, 0.01kVarh
	unsigned int enenaa[3];  //����ABC���й�������, 0.01kWh
	unsigned int enenia[3];  //����ABC���޹�������, 0.01kVarh
};
extern struct imetene_save IMetEneSave, IMetEneSaveLm;

void UpdateIMetEneLmMdb(void);
void UpdateIMetEneMdb(void);

void SaveIMetEne(void);
int LoadIMetSave(void);

#endif /*_IMET_SAVE_H*/
