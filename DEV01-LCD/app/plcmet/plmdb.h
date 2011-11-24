/**
* plmdb.h -- �ز�������
* 
* 
* ����ʱ��: 2010-5-22
* ����޸�ʱ��: 2010-5-22
*/

#ifndef _PLMDB_H
#define _PLMDB_H

#include "include/param/capconf.h"

#define PLDATA_EMPTY		0xee
#define PLTIME_EMTPY		0xeeee

#define PLMDB_DAY		0
#define PLMDB_MONTH		1
#define PLMDB_IMP		2

#define MAX_PLMET_FENUM		4
#define MAX_PLMET_ENENUM	(MAX_PLMET_FENUM+1)

#define IMP_PLMET_FREZ		3

//�ն�������
typedef struct {
	unsigned short meter_num;
	unsigned char meter_addr[6];  
	unsigned short readtime;   //Сʱ*60+����
	unsigned short read_stat;  //״̬
	unsigned char meter_ene[4];  //9010
} plmdb_day_t;

//�¶�������
typedef struct {
	unsigned short readtime;  //��*1440+Сʱ*60+����
	unsigned char state[2];  //״̬
	unsigned char unuse[3];
	unsigned char fenum;   //������
	unsigned char ene[MAX_PLMET_ENENUM*4];
} plmdb_mon_t;

//�ص��û�����
typedef struct {
	unsigned char ene[4*24];
} plmdb_imp_t;

#ifndef DEFINE_PLMDB
extern const plmdb_day_t PlMdbDay[MAX_PLCMET];
extern const plmdb_mon_t PlMdbMonth[MAX_PLCMET];
extern const plmdb_imp_t PlMdbImp[MAX_IMPORTANT_USER];
#endif

void LockPlMdb(void);
void UnlockPlMdb(void);
int ReadPlMdbDay(plmdb_day_t *pday, dbtime_t dbtime);
int ReadPlMdbMonth(plmdb_mon_t *pmon, dbtime_t dbtime);
int ReadPlMdbImp(plmdb_imp_t *pimp, dbtime_t dbtime);
void SavePlMdb(void);
int SavePlNodeInfo(void);
int SavePlCjqNodeInfo(void);
void UpdatePlMdb(unsigned short mid, unsigned short itemid, const unsigned char *buf, int len);
void SetPlMdbFenum(unsigned short mid, unsigned char fenum);

void ResetPlMdbDay(void);
void ResetPlMdbMonth(void);

#endif /*_PLMDB_H*/

