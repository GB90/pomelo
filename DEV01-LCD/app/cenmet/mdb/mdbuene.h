/**
* mdbuene.h -- �õ���
* 
* 
* ����ʱ��: 2010-5-13
* ����޸�ʱ��: 2010-5-13
*/

#ifndef _MDB_UENE_H
#define _MDB_UENE_H

#include "cenmet/mdb/mdbconf.h"

typedef struct {
	unsigned int enepa_15m;    // 15�����õ���, 0.001kWh
	unsigned int enepi_15m;  //0.01kvarh
	unsigned int enena_15m;
	unsigned int eneni_15m;

	unsigned int enepa_1m;    // 1�����õ���
	unsigned int enepi_1m;
	unsigned int enena_1m;
	unsigned int eneni_1m;
} mdbuene_imm_t;

typedef struct {
	//F41~F44
	unsigned int enepa_day[MAXNUM_FEEPRD+1];     //���������й�������, 0.001kWh
	unsigned int enepi_day[MAXNUM_FEEPRD+1];     //���������޹�������, 0.01kvarh
	unsigned int enena_day[MAXNUM_FEEPRD+1];     //���շ����й�������, 0.001kWh
	unsigned int eneni_day[MAXNUM_FEEPRD+1];     //���շ����޹�������, 0.01kvarh

	//F45~F48
	unsigned int enepa_mon[MAXNUM_FEEPRD+1];     //���������й�������, 0.001kWh
	unsigned int enepi_mon[MAXNUM_FEEPRD+1];     //���������޹�������, 0.01kvarh
	unsigned int enena_mon[MAXNUM_FEEPRD+1];     //���·����й�������, 0.001kWh
	unsigned int eneni_mon[MAXNUM_FEEPRD+1];     //���·����޹�������, 0.01kvarh

	unsigned int enepa_bak[MAXNUM_FEEPRD+1];     //���������й�������, 0.001kWh
	unsigned int enepi_bak[MAXNUM_FEEPRD+1];     //���������޹�������, 0.01kvarh
	unsigned int enena_bak[MAXNUM_FEEPRD+1];     //���ݷ����й�������, 0.001kWh
	unsigned int eneni_bak[MAXNUM_FEEPRD+1];     //���ݷ����޹�������, 0.01kvarh

	unsigned int uenepa;
	unsigned int uenepi;
	unsigned int uenena;
	unsigned int ueneni;
} mdbuene_t;

#ifndef DEFINE_MDBUENE
extern const mdbuene_imm_t MdbUseEneImm[MAX_CENMETP];
extern const mdbuene_t MdbUseEne[MAX_CENMETP];
#endif

void MdbUseEneInit(void);
void ResetMdbUseEne(int flag);

void UpdateMdbUseEne();
void UpdateMdbUseEne15Min(unsigned short mid);
void ClearMdbUseEne15Min(unsigned short mid);

int ReadMdbUseEne(unsigned short metpid, unsigned short itemid, unsigned char *buf, int len);

#endif /*_MDB_UENE_H*/

