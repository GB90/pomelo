#ifndef _MDB_TGRP_H
#define _MDB_TGRP_H

#include "cenmet/mdb/mdbconf.h"
#include "include/lib/datatype_gb.h"

typedef struct {
	int pwra;    //��ǰ�ܼ��й�����, 0.01kW
	int pwri;    //��ǰ�ܼ��޹�����, 0.01kvar

	int uenea_15min;   // 15 �����õ���, kWh
	int uenei_15min;   // 15 �����õ���, kvarh

	int uenea_1min;   // 1�����õ���, kWh
	int uenei_1min;   // 1 �����õ���, kvarh

	int uenea;    //�й��õ���(������)
	int uenei;    //�޹��õ���(������)
} mdbtgrp_imm_t;

typedef struct {
	int enea_day[MAXNUM_FEEPRD+1];    //�����ܼ��й�������, kWh
	int enei_day[MAXNUM_FEEPRD+1];    //�����ܼ��޹�������, kvarh
	int enea_mon[MAXNUM_FEEPRD+1];     //�����ܼ��й�������, kWh
	int enei_mon[MAXNUM_FEEPRD+1];      //�����ܼ��޹�������, kvarh

	int enea_res;   //ʣ�²����й�����, 0.001kWh
	int enei_res;   //ʣ�²����޹�����, 0.01kvarh

	gene_t ene_bec;   //�ն˵�ǰʣ�����/��
	int pwr_flt;   //��ǰ�����¸��غ��й����ʶ���ֵ
} mdbtgrp_t;

#ifndef DEFINE_MDBTGRP
extern const mdbtgrp_imm_t MdbTGrpImm[MAX_TGRP];
#define mdbtgrp_imm(tid)	(MdbTGrpImm[tid])
extern const mdbtgrp_t MdbTGrp[MAX_TGRP];
#define mdbtgrp(tid)		(MdbTGrp[tid])
#endif

int MdbTGrpInit(void);

void UpdateMdbTGrpPower(void);
void UpdateTGrpEne(void);
void MdbTGrpUEneFrezEnd(void);
void AddMdbTGrpUEne15Min(void);
void ClearMdbTGrp15Minute(void);
void SetMdbTGrpPwrflt(unsigned char tid, int pwr);

int ReadMdbTGrp(unsigned short metpid, unsigned short itemid, unsigned char *buf, int len);

#endif /*_MDB_TGRP_H*/

