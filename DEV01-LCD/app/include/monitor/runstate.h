#ifndef _RUNSTATE_H
#define _RUNSTATE_H

typedef struct {
	unsigned char head[2];
	unsigned char cur[2];
	unsigned char snd[2];
	unsigned char stat[8];
} alarm_stat_t;

typedef struct {
	unsigned char ver[4];
	unsigned char flag[4];
} softchg_stat_t;

#define LEN_MALM_STAT	36

typedef struct {
	unsigned char battery;
	unsigned char pwroff;	//1-���е�
	unsigned char isig_stat;
	unsigned char isig_chg;
	
	unsigned char batcharge;
	unsigned char batbad;
	unsigned char commflow; //1-��ͨ����������
	unsigned char outgrp;  //���

	alarm_stat_t alarm;
	softchg_stat_t softchg;
	unsigned char cnt_snderr[2];
	unsigned char flag_acd;
	unsigned char unused2;

	unsigned char timepoweroff[8];//��¼ͣ��ʱ��

	unsigned char malmflag[LEN_MALM_STAT];  // 4+4*MAX_CENMETP
} runstate_t;

#ifndef DEFINE_RUNSTATE
extern const runstate_t RunState;
#endif

runstate_t *RunStateModify(void);

void SaveRunState(void);

#endif /*_RUNSTATE_H*/

