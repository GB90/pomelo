/**
* mdbstic.h -- ͳ������
* 
* 
* ����ʱ��: 2010-5-14
* ����޸�ʱ��: 2010-5-14
*/

#ifndef _MDB_STIC_H
#define _MDB_STIC_H

typedef struct {
	unsigned char min;
	unsigned char hour;
	unsigned char day;
	unsigned char mon;
} stictime_t;

typedef struct {
	unsigned int pwramax[4];    //����й����� 0.1W
	stictime_t pwramax_time[4];    //����й����ʷ���ʱ��
	unsigned short pwrzero_time[4];    //�й�����Ϊ��ʱ��, ��

	unsigned int dmmax[4];   //����������� 0.1W
	stictime_t dmtime[4];   //���������������ʱ��

	unsigned short vol_overtime[3];    //��ѹ�ۼ�ʱ��
	unsigned short vol_lesstime[3];    //Ƿѹ�ۼ�ʱ��
	unsigned short vol_uptime[3];    //Խ�����ۼ�ʱ��
	unsigned short vol_lowtime[3];    //Խ�����ۼ�ʱ��
	unsigned short vol_oktime[3];    //�ϸ��ۼ�ʱ��

	unsigned short volmax[3];
	unsigned short volmin[3];
	stictime_t volmax_time[3];
	stictime_t volmin_time[3];
	unsigned int volsum[3];    //ƽ����ѹ
	unsigned short volsnum[3];

	unsigned short ampunb_time;   //������ƽ���ۼ�ʱ��
	unsigned short volunb_time;   //��ѹ��ƽ���ۼ�ʱ��
	unsigned short ampunb_max;    //������ƽ�����ֵ,%
	unsigned short volunb_max;    //��ѹ��ƽ�����ֵ,%
	stictime_t ampunbmax_time;    //������ƽ�����ֵ����ʱ��
	stictime_t volunbmax_time;    //��ѹ��ƽ�����ֵ����ʱ��

	unsigned short ampover_time[3];   //�����ۼ�ʱ��
	unsigned short ampup_time[3];   //����Խ���ۼ�ʱ��
	unsigned short zampup_time;   //�������Խ���ۼ�ʱ��
	unsigned short ampmax[4];    //�������ֵ
	stictime_t ampmax_time[4];   //�������ֵ����ʱ��

	unsigned short pwrvov_time;    //���ڹ���Խ�������ۼ�ʱ��
	unsigned short pwrvup_time;    //���ڹ���Խ�����ۼ�ʱ��

	unsigned short pwrf_time[3];   //�������������ۼ�ʱ��

	unsigned short load_max;  //���������ֵ,%
	unsigned short load_min;  //��������Сֵ,%
	stictime_t loadmax_time;  //���������ֵ����ʱ��
	stictime_t loadmin_time;  //��������Сֵ����ʱ��
} metpstic_t;

typedef struct {
	unsigned short pwr_time;   //����ʱ��
	unsigned short rst_cnt;   //��λ����
	unsigned char sw_cnt[4];   //��բ�ۼƴ���
	unsigned int comm_bytes;  //�ն�����վͨ������
} termstic_t;

typedef struct {
	int pwrmax;
	stictime_t pwrmax_time;
	int pwrmin;
	stictime_t pwrmin_time;

	unsigned short pwrzero_time;
} tgrpstic_day_t;

typedef struct {
	int pwrmax;
	stictime_t pwrmax_time;
	int pwrmin;
	stictime_t pwrmin_time;

	unsigned short pwrzero_time;

	//@add later: δ�������ۼ�
	unsigned short pcov_time;   //�����ʶ�ֵ���ۼ�ʱ�估�ۼƵ���
	unsigned short ecov_time;  //���µ�����ֵ���ۼ�ʱ�估�ۼƵ���
	int pcov_ene;
	int ecov_ene;
} tgrpstic_mon_t;

typedef struct {
	metpstic_t metp_day[MAX_CENMETP];
	metpstic_t metp_mon[MAX_CENMETP];

	tgrpstic_day_t tgrp_day[MAX_TGRP];
	tgrpstic_mon_t tgrp_mon[MAX_TGRP];

	termstic_t term_day;
	termstic_t term_mon;

	unsigned char year;
	unsigned char mon;
	unsigned char day;
	unsigned char reserv;
} mdbstic_t;

#ifndef DEFINE_MDBSTIC
extern const mdbstic_t MdbStic;
#endif

void UpdateMdbStic(void);
void MdbSticEmptyDay(void);
void MdbSticEmptyMonth(void);
void MdbSticInit(void);

void UpdateSticResetCount(void);
void UpdateSticComm(unsigned int bytes);

void BakupSticMetp(unsigned char flag, unsigned short mid, unsigned char *buf);
void BakupSticTerm(unsigned char flag, unsigned char *buf);

void BakupSticTGrpDay(unsigned short tid, unsigned char *buf);
void BakupSticTGrpMon(unsigned short tid, unsigned char *buf);

#endif /*_MDB_STIC_H*/

