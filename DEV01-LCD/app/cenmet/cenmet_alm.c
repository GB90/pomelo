/**
* cenmet_alm.c -- ��Ƹ澯���
* 
* 
* ����ʱ��: 2010-6-12
* ����޸�ʱ��: 2010-6-14
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/debug.h"
#include "include/sys/schedule.h"
#include "include/sys/timeal.h"
#include "include/lib/bcd.h"
#include "include/lib/align.h"
#include "include/param/capconf.h"
#include "include/param/hardware.h"
#include "include/param/meter.h"
#include "include/param/metp.h"
#include "include/param/mix.h"
#include "include/monitor/runstate.h"
#include "include/monitor/alarm.h"
#include "cenmet_proto.h"
#include "cenmet_comm.h"
#include "mdb/mdbcur.h"
#include "mdb/mdbana.h"
#include "include/debug/shellcmd.h"

#define CENMETALARM_DEBUG		1

#ifndef CENMETALARM_DEBUG
	#define TestPrint			DebugPrint
#else
	#define TestPrint(a,...)
#endif

#define ALARM_MID(mid)		(mid+1)

#define CLR_ALMBUF(pbuf)    {memset((unsigned char *)(pbuf), 0, sizeof(alarm_t));}

#define MALMFLAG_DEALING	0x8000
#define MALMFLAG_ALMED		0x4000
#define MALMFLAG_IDX		0x0fff
#define MALMTIME_MASK		0x3fff
#define MALMTIME_MAX		(MALMTIME_MASK+1)
typedef struct {
	unsigned short flag;
	unsigned short time; 
} cenmet_alm_t;


//ֻ����ض���������Ч
typedef struct {
	cenmet_alm_t diff[MAX_DIFFA];    //��澯
	cenmet_alm_t vol_rph;    //��ѹ��·������
	cenmet_alm_t ctsc2[3];    //ABC��CT ���β��·
	cenmet_alm_t ctoc2[3];    //ABC��CT ���β࿪·
	//cenmet_alm_t ampr[3];    //ABC�����������
} malm1_stat_t;

static const char *name_malm1[] = {
	"��澯1",
	"��澯2", 
	"��ѹ��·������",
};

// ʾ���ж�
typedef struct {
	unsigned char ene_fly;    //������
	unsigned char ene_stop;    //���ͣ��
	unsigned char ene_diff;    //�����
	unsigned char ene_del;    //ʾ���½�
	unsigned char prg_chg;    //�����ʱ�����
	unsigned char prd_chg;    //���ʱ�λ���ʸ���
	unsigned char day_chg;    //������ո���
	unsigned char pls_chg;    //������峣������
	unsigned char clk_chg;    //���ʱ���쳣
	unsigned char bei_chg;    //����������ʸ���
	unsigned char bat_low;    //����ص�ѹ��
	unsigned char need_zero;    //�����������

	unsigned int bak_enepa;    //9010
	unsigned int bak_enena;    //9020
	unsigned short cnt_enestop;
	unsigned char bak_prd[24];    //C33F, ���ʱ��
	unsigned char bak_prgcnt[2];    //B212, ��̴���
	unsigned char bak_cpyday[2];    //C117, �Զ�������
	unsigned char bak_pls[6];    //C030, C031, �й����峣��
	unsigned char bak_bei[4];    //C035, C036, ����������
	unsigned char bak_need[2];   //B213, �����������
	unsigned char bak_vbrcnt[2];  //B310, �ܶ������
} malm2_stat_t;

static const char *name_malm2[]  = {
	"������",
	"���ͣ��",
	"�����",
	"ʾ���½�",
	"�����ʱ�����",
	"���ʱ�λ���ʸ���",
	"������ո���",
	"������峣������",
	"���ʱ���쳣",
	"����������ʸ���",
	"����ص�ѹ��",
	"�����������",
};

//��ÿ����������Ч
typedef struct {
	cenmet_alm_t volbr[3];    //ABC���ѹ����
	cenmet_alm_t volls[3];    //ABC���ѹʧѹ
	cenmet_alm_t volov[3];   //ABC��ѹ��ѹ
	cenmet_alm_t ampov[3];   //ABC������������
	cenmet_alm_t amplm[3];   //ABC����������
	cenmet_alm_t pwrvov;    //���ڹ���Խ������
	cenmet_alm_t pwrvlm;    //���ڹ���Խ����
	cenmet_alm_t unb_vol;    //��ѹ��ƽ��
	cenmet_alm_t unb_amp;    //������ƽ��
	cenmet_alm_t zamp_ab;    //�������ƫ��
	cenmet_alm_t amprev[3];    //����������
} malm3_stat_t;

static const char *name_malm3[] = {
	"A���ѹ����",
	"B���ѹ����",
	"C���ѹ����",
	"A���ѹʧѹ",
	"B���ѹʧѹ",
	"C���ѹʧѹ",
	"A���ѹ��ѹ",
	"B���ѹ��ѹ",
	"C���ѹ��ѹ",
	"A�����Խ������",
	"B�����Խ������",
	"C�����Խ������",
	"A�����Խ����",
	"B�����Խ����",
	"C�����Խ����",
	"���ڹ���Խ������",
	"���ڹ���Խ����",
	"��ѹ��ƽ��",
	"������ƽ��",
	"�������ƫ��",
	"A�����������",
	"B�����������",
	"C�����������",
};

static malm1_stat_t malm1_stat;
static malm2_stat_t malm2_stat[MAX_CENMETP];
static malm3_stat_t malm3_stat[MAX_CENMETP];

DECLARE_INIT_FUNC(CenMetAlarmInit);
int CenMetAlarmInit(void)
{
	unsigned short mid, idx, baseidx;
	unsigned char i, mask;
	cenmet_alm_t *pmalm;

	memset((unsigned char *)&malm1_stat, 0, sizeof(malm1_stat_t));
	memset((unsigned char *)malm2_stat, 0, sizeof(malm2_stat_t)*MAX_CENMETP);
	memset((unsigned char *)malm3_stat, 0, sizeof(malm3_stat_t)*MAX_CENMETP);

	pmalm = malm1_stat.diff;
	idx = 0;
	mask = 1;
	baseidx = 0;
	for(i=0; i<9; i++) {
		pmalm->flag = (baseidx++)&MALMFLAG_IDX;

		if(RunState.malmflag[idx]&mask) pmalm->flag |= MALMFLAG_ALMED;

		mask <<= 1;
		if(0 == mask) {
			mask = 1;
			idx++;
		}
		pmalm++;
	}

	for(mid=0; mid<MAX_CENMETP; mid++) {
		pmalm = malm3_stat[mid].volbr;
		idx = 4*(mid+1);
		mask = 1;
		baseidx = 32*(mid+1);

		for(i=0; i<23; i++) {
			pmalm->flag = (baseidx++)&MALMFLAG_IDX;

			if(RunState.malmflag[idx]&mask) pmalm->flag |= MALMFLAG_ALMED;

			mask <<= 1;
			if(0 == mask) {
				mask = 1;
				idx++;
			}
			pmalm++;
		}
	}

	SET_INIT_FLAG(CenMetAlarmInit);
	return 0;
}

/**
* @brief ����澯״̬
* @param flag 0-����澯, 1-���ø澯
* @param palm �澯����ָ��
*/
static inline void SaveAlarmState(unsigned char flag, const cenmet_alm_t *palm)
{
	unsigned char mask;
	unsigned short idx;

	idx = (palm->flag&MALMFLAG_IDX) >> 3;
	mask = 1 << (palm->flag&0x07);

	if(flag) RunStateModify()->malmflag[idx] |= mask;
	else RunStateModify()->malmflag[idx]  &= ~mask;
}

/**
* @brief �����쳣��ʼʱ���뵱ǰʱ��Ĳ���
* @param palm �澯����ָ��
* @return ����(��)
*/
static inline unsigned short AlarmTimeDiff(const cenmet_alm_t *palm)
{
	unsigned short us1, us2;
	utime_t utime;

	utime = UTimeReadCurrent() / 60;

	us1 = utime&MALMTIME_MASK;
	us2 = palm->time&MALMTIME_MASK;
	if(us1 < us2) {
		us1 += MALMTIME_MAX;
		us1 -= us2;
	}
	else us1 -= us2;

	return us1;
}

/**
* @brief �����쳣
* @param palm �澯����ָ��
* @param alarmtime �쳣�澯����ʱ��(��)
* @return 0-����Ҫ���͸澯, 1-��Ҫ���͸澯
*/
static int AbnorAlarm(cenmet_alm_t *palm, unsigned char alarmtime)
{
	unsigned short diff;
	utime_t utime;

	if(0 == alarmtime) alarmtime = 5;

	if(palm->flag&MALMFLAG_ALMED) {
		palm->flag &= ~MALMFLAG_DEALING;
		return 0;
	}

	if(palm->flag&MALMFLAG_DEALING) {
		diff = AlarmTimeDiff(palm);
		if(diff >= (unsigned short)alarmtime) {
			palm->flag |= MALMFLAG_ALMED;
			SaveAlarmState(1, palm);
			return 1;
		}
	}
	else {
		utime = UTimeReadCurrent() / 60;
		palm->time = utime & MALMTIME_MASK;
		palm->flag |= MALMFLAG_DEALING;
	}

	return 0;
}

/**
* @brief �쳣�ָ�
* @param palm �澯����ָ��
* @param alarmtime �쳣�澯����ʱ��(��)
* @return 0-����Ҫ���͸澯, 1-��Ҫ���͸澯
*/
static int NormalAlarm(cenmet_alm_t *palm, unsigned char alarmtime)
{
	unsigned short diff;
	utime_t utime;

	if(0 == alarmtime) alarmtime = 5;

	if(0 == (palm->flag&MALMFLAG_ALMED)) {
		palm->flag &= ~MALMFLAG_DEALING;
		return 0;
	}

	if(palm->flag&MALMFLAG_DEALING) {
		diff = AlarmTimeDiff(palm);
		if(diff >= (unsigned short)alarmtime) {
			palm->flag &= ~MALMFLAG_ALMED;
			SaveAlarmState(0, palm);
			return 1;
		}
	}
	else {
		utime = UTimeReadCurrent() / 60;
		palm->time = utime & MALMTIME_MASK;
		palm->flag |= MALMFLAG_DEALING;
	}

	return 0;
}

extern unsigned char IMetStatus[2];
extern unsigned short IMetMetpid;

/**
* @brief ��ѹ������澯���
* @param pbuf �澯������ָ��
* @param mid ���ID
*/
static void VolRphAlarmProc(alarm_t *pbuf, unsigned short mid)
{
	int rtn;
	unsigned char flag;

	if(PWRTYPE_3X3W == metp_pwrtype(mid)) {
		if(MdbAnalyze[mid].vol[0] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[2] <= ParaCenMetp[mid].limit.vol_lack) return;
	}
	else if(PWRTYPE_3X4W == metp_pwrtype(mid)) {
		if(MdbAnalyze[mid].vol[0] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[1] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[2] <= ParaCenMetp[mid].limit.vol_lack) return;
	}
	else return;

	if(IMetStatus[0]&0x02) {
		rtn = AbnorAlarm(&malm1_stat.vol_rph, 5);
		flag = ALMFLAG_ABNOR;
	}
	else {
		rtn = NormalAlarm(&malm1_stat.vol_rph, 5);
		flag = ALMFLAG_NORMAL;
	}

	if(rtn) {
		CLR_ALMBUF(pbuf);
		pbuf->erc = 11;
		pbuf->len = 19;
		DEPART_SHORT(ALARM_MID(mid), pbuf->data);
		if(ALMFLAG_ABNOR == flag) pbuf->data[1] |= 0x80;

		smallcpy(pbuf->data+2, MdbCurrent[mid].phase_arc, 12);
		smallcpy(pbuf->data+14, MdbCurrent[mid].enepa, 5);

		MakeAlarm(flag, pbuf);
	}
}

/**
* @brief ������ݱ�����
* @param pbuf �澯������ָ��
* @param mid ���ID
*/
static void MetChangeProc(alarm_t *pbuf, unsigned short mid)
{
	unsigned char balm = 0;
	unsigned char buf[32];
	int len;

	{
		unsigned char metattr;
		unsigned short metid;

		if(EMPTY_CENMETP(mid)) return;
		metid = CenMetpMap[mid].metid;
		if(ParaMeter[metid].proto == METTYPE_ACSAMP || ParaMeter[metid].proto == METTYPE_PLC) return;
		GetCMetAttr(mid, &metattr);
		if(METATTR_DL645LIKE != metattr) return;
	}

	{
		utime_t utime, utimecur;

		if(!CenMetReadTime(mid, (sysclock_t *)buf)) {
			utime = SysClockToUTime((sysclock_t *)buf);
			utimecur = UTimeReadCurrent();
			if(utime < utimecur) utime = utimecur - utime;
			else utime = utime - utimecur;
			utime /= 60;

			if(utime > ParaMix.metabnor.time) {
				if(0 == malm2_stat[mid].clk_chg) {
					malm2_stat[mid].clk_chg = 1;

					CLR_ALMBUF(pbuf);
					pbuf->erc = 12;
					pbuf->len = 2;
					DEPART_SHORT(ALARM_MID(mid), pbuf->data);
					pbuf->data[1] |= 0x80;
					MakeAlarm(ALMFLAG_ABNOR, pbuf);
				}
			}
			else {
				if(malm2_stat[mid].clk_chg) {
					malm2_stat[mid].clk_chg = 0;

					CLR_ALMBUF(pbuf);
					pbuf->erc = 12;
					pbuf->len = 2;
					DEPART_SHORT(ALARM_MID(mid), pbuf->data);
					MakeAlarm(ALMFLAG_NORMAL, pbuf);
				}
			}
		}
	}

	balm = 0;

	//���Ƿѹ
	//if(0 == malm2_stat[mid].bat_low) {
		if(!CenMetRead(mid, 0xc020, buf, &len)) {
			if(buf[0]&0x04) {
				if(0 == malm2_stat[mid].bat_low) {
					malm2_stat[mid].bat_low = 1;
					balm |= 0x10;
				}
			}
			else malm2_stat[mid].bat_low = 0;
		}
	//}

	//��̴����仯
	//if(0 == malm2_stat[mid].prg_chg) {
		if(!CenMetRead(mid, 0xb212, buf, &len)) {
			TestPrint(1, "prg_chg: %02X0%2X--%02X%02X\n",
					malm2_stat[mid].bak_prgcnt[1], malm2_stat[mid].bak_prgcnt[0],
					buf[1], buf[0]);
			if(HexIsEmpty(malm2_stat[mid].bak_prgcnt, 2, 0))
				smallcpy(malm2_stat[mid].bak_prgcnt, buf, 2);
			else {
				if(0 != HexComp(malm2_stat[mid].bak_prgcnt, buf, 2)) {
					malm2_stat[mid].prg_chg = 1;
					balm |= 0x01;
				}
				smallcpy(malm2_stat[mid].bak_prgcnt, buf, 2);
			}
		}
	//}

	//������������仯
	//if(0 == malm2_stat[mid].need_zero) {
		if(!CenMetRead(mid, 0xb213, buf, &len)) {
			TestPrint(1, "need_zero: %02X0%2X--%02X%02X\n",
					malm2_stat[mid].bak_need[1], malm2_stat[mid].bak_need[0],
					buf[1], buf[0]);
			if(HexIsEmpty(malm2_stat[mid].bak_need, 2, 0))
				smallcpy(malm2_stat[mid].bak_need, buf, 2);
			else {
				if(0 != HexComp(malm2_stat[mid].bak_need, buf, 2)) {
					malm2_stat[mid].need_zero = 1;
					balm |= 0x01;
				}

				smallcpy(malm2_stat[mid].bak_need, buf, 2);
			}
		}
	//}

	//��������仯
	if(!CenMetRead(mid, 0xb310, buf, &len)) {
		TestPrint(1, "vbrcnt: %02X0%2X--%02X%02X\n",
					malm2_stat[mid].bak_vbrcnt[1], malm2_stat[mid].bak_vbrcnt[0],
					buf[1], buf[0]);
		if(HexIsEmpty(malm2_stat[mid].bak_vbrcnt, 2, 0))
			smallcpy(malm2_stat[mid].bak_vbrcnt, buf, 2);
		else {
			if(0 != HexComp(malm2_stat[mid].bak_vbrcnt, buf, 2)) {
				balm |= 0x02;
			}

			smallcpy(malm2_stat[mid].bak_vbrcnt, buf, 2);
		}
	}
	
	if(balm) {
		CLR_ALMBUF(pbuf);
		pbuf->erc = 13;
		pbuf->len = 3;
		DEPART_SHORT(ALARM_MID(mid), pbuf->data);
		pbuf->data[1] |= 0x80;
		pbuf->data[2] = balm;

		MakeAlarm(ALMFLAG_ABNOR, pbuf);
	}

	balm = 0;	

	//����ʱ�α仯
	//if(0 == malm2_stat[mid].prd_chg) {
		if(!CenMetRead(mid, 0xc33f, buf, &len)) {
			if(HexIsEmpty(malm2_stat[mid].bak_prd, 24, 0))
				smallcpy(malm2_stat[mid].bak_prd, buf, 24);
			else {
				if(0 != HexComp(malm2_stat[mid].bak_prd, buf, 24)) {
					balm |= 0x01;
					malm2_stat[mid].prd_chg = 1;
				}

				smallcpy(malm2_stat[mid].bak_prd, buf, 24);
			}
		}
	//}


	//�����ո���
	//if(0 == malm2_stat[mid].day_chg) {
		if(!CenMetRead(mid, 0xc117, buf, &len)) {
			TestPrint(1, "day_chg: %02X0%2X--%02X%02X\n",
					malm2_stat[mid].bak_cpyday[1], malm2_stat[mid].bak_cpyday[0],
					buf[1], buf[0]);
			if(HexIsEmpty(malm2_stat[mid].bak_cpyday, 2, 0))
				smallcpy(malm2_stat[mid].bak_cpyday, buf, 2);
			else {
				if(0 != HexComp(malm2_stat[mid].bak_cpyday, buf, 2)) {
					balm |= 0x04;
					malm2_stat[mid].day_chg = 1;
				}

				smallcpy(malm2_stat[mid].bak_cpyday, buf, 2);
			}
		}
	//}

	//���峣���仯
	//if(0 == malm2_stat[mid].pls_chg) {
		if((!CenMetRead(mid, 0xc030, buf, &len)) && 
			(!CenMetRead(mid, 0xc031, &buf[3], &len)))
		{
			if(HexIsEmpty(malm2_stat[mid].bak_pls, 6, 0))
				smallcpy(malm2_stat[mid].bak_pls, buf, 6);
			else {
				if(0 != HexComp(malm2_stat[mid].bak_pls, buf, 6)) {
					balm |= 0x08;
					malm2_stat[mid].pls_chg = 1;
				}

				smallcpy(malm2_stat[mid].bak_pls, buf, 6);
			}
		}
	//}

	//���������ʱ仯
	//if(0 == malm2_stat[mid].bei_chg) {
		if((!CenMetRead(mid, 0xc040, buf, &len)) && 
			(!CenMetRead(mid, 0xc041, &buf[2], &len)))
		{
			if(HexIsEmpty(malm2_stat[mid].bak_bei, 4, 0))
				smallcpy(malm2_stat[mid].bak_bei, buf, 4);
			else {
				if(0 != HexComp(malm2_stat[mid].bak_bei, buf, 4)) {
					balm |= 0x10;
					malm2_stat[mid].bei_chg = 1;
				}

				smallcpy(malm2_stat[mid].bak_bei, buf, 4);
			}
		}
	//}

	if(!balm) return;

	CLR_ALMBUF(pbuf);
	pbuf->erc = 8;
	pbuf->len = 3;
	DEPART_SHORT(ALARM_MID(mid), pbuf->data);
	pbuf->data[1] |= 0x80;
	pbuf->data[2] = balm;
	MakeAlarm(ALMFLAG_ABNOR, pbuf);
}

/**
* @brief ���ɵ�ѹ�����澯
* @param pbuf �澯������ָ��
* @param erc �¼�ID
* @param mid ���ID
* @param flag ��־λ 0-�ָ�, 1-�쳣
* @param abc ��λ, 0-a, 1-b , 2-c
*/
static inline void VolAmpAlarm(alarm_t *pbuf, unsigned char erc, unsigned short mid, unsigned char flag, unsigned char abc)
{
	unsigned char almflag;

	CLR_ALMBUF(pbuf);
	pbuf->erc = erc;
	pbuf->len = 20;

	DEPART_SHORT(ALARM_MID(mid), pbuf->data);
	if(flag) {
		pbuf->data[1] |= 0x80;
		almflag = ALMFLAG_ABNOR;
	}
	else almflag = ALMFLAG_NORMAL;

	pbuf->data[2] = abc;
	smallcpy(pbuf->data+3, MdbCurrent[mid].vol, 6);
	smallcpy(pbuf->data+9, MdbCurrent[mid].amp, 6);
	smallcpy(pbuf->data+15, MdbCurrent[mid].enepa, 5);

	MakeAlarm(almflag, pbuf);
}

/**
* @brief ��Ƶ�ѹ�澯���
* @param pbuf �澯������ָ��
* @param mid ���ID
*/
static void VolAlarmProc(alarm_t *pbuf, unsigned short mid)
{
	unsigned int mask;
	unsigned char abc, almabc;
	int amp_limit, i;

	if(metp_pwrtype(mid) == PWRTYPE_3X3W) {
		if(mrd_fail(mid, (MBMSK_VOLA|MBMSK_VOLC))) return;

		if((MdbAnalyze[mid].vol[0] <= ParaCenMetp[mid].limit.vol_lack) &&
			(MdbAnalyze[mid].vol[2] <= ParaCenMetp[mid].limit.vol_lack))
		{
			if(AbnorAlarm(&malm3_stat[mid].volbr[1], ParaCenMetp[mid].limit.time_volless)) {
				almabc = 2;
				almabc |= 0x40;
				VolAmpAlarm(pbuf, 10, mid, 1, almabc);
			}

			return;
		}
		else {
			if(NormalAlarm(&malm3_stat[mid].volbr[1], ParaCenMetp[mid].limit.time_volless)) {
				almabc = 2;
				almabc |= 0x40;
				VolAmpAlarm(pbuf, 10, mid, 0, almabc);
			}
		}
	}

	mask = MBMSK_VOLA;

	for(abc=0; abc<3; abc++) {
		if(mrd_fail(mid, (mask<<abc))) continue;

		if(metp_pwrtype(mid) == PWRTYPE_1X) {
			if(abc) continue;
		}
		else if(metp_pwrtype(mid) == PWRTYPE_3X3W) {
			if(1 == abc) continue;
		}

		if(MdbAnalyze[mid].vol[abc] <= ParaCenMetp[mid].limit.vol_lack) { //
			amp_limit = (int)ParaCenMetp[mid].base.amp_rating & 0xffff;
			amp_limit /= 20;  // 5%
			i = MdbAnalyze[mid].amp[abc];
			if(i < 0) i *= -1;

			if(i < amp_limit) {  //����
				if(AbnorAlarm(&malm3_stat[mid].volbr[abc], ParaCenMetp[mid].limit.time_volless)) {
					almabc = 1<<abc;
					almabc |= 0x40;
					VolAmpAlarm(pbuf, 10, mid, 1, almabc);
				}
			}
		}
		else {
			if(NormalAlarm(&malm3_stat[mid].volbr[abc], ParaCenMetp[mid].limit.time_volless)) {
				almabc = 1<<abc;
				almabc |= 0x40;
				VolAmpAlarm(pbuf, 10, mid, 0, almabc);
			}

			if(MdbAnalyze[mid].vol[abc] <= ParaCenMetp[mid].limit.vol_less) { //ʧѹ
				if(AbnorAlarm(&malm3_stat[mid].volls[abc], ParaCenMetp[mid].limit.time_volless)) {
					almabc = 1<<abc;
					almabc |= 0x80;
					VolAmpAlarm(pbuf, 10, mid, 1, almabc);
				}
			}
			else if(MdbAnalyze[mid].vol[abc] >= ParaCenMetp[mid].limit.restore_volless) {
				if(NormalAlarm(&malm3_stat[mid].volls[abc], ParaCenMetp[mid].limit.time_volless)) {
					almabc = 1<<abc;
					almabc |= 0x80;
					VolAmpAlarm(pbuf, 10, mid, 0, almabc);
				}
			}
		}
	}
}

/**
* @brief ��Ƶ�������澯���
* @param pbuf �澯������ָ��
* @param mid ���ID
*/
static void AmpRevAlarmProc(alarm_t *pbuf, unsigned short mid)
{
	unsigned int mask;
	unsigned char abc, almabc;

	if(metp_pwrtype(mid) == PWRTYPE_3X3W) {
		//ʧѹ��ѹ����
		if(MdbAnalyze[mid].vol[0] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[0] >= ParaCenMetp[mid].limit.vol_over) return;
		if(MdbAnalyze[mid].vol[2] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[2] >= ParaCenMetp[mid].limit.vol_over) return;
	}
	else if(metp_pwrtype(mid) == PWRTYPE_3X4W) {
		//ʧѹ��ѹ����
		if(MdbAnalyze[mid].vol[0] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[0] >= ParaCenMetp[mid].limit.vol_over) return;
		if(MdbAnalyze[mid].vol[1] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[1] >= ParaCenMetp[mid].limit.vol_over) return;
		if(MdbAnalyze[mid].vol[2] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[2] >= ParaCenMetp[mid].limit.vol_over) return;
	}

	//��������
	if(malm1_stat.vol_rph.flag & (MALMFLAG_ALMED|MALMFLAG_DEALING)) return;

	mask = MBMSK_AMPA;

	for(abc=0; abc<3; abc++) {
		if(metp_pwrtype(mid) == PWRTYPE_3X3W) {
			if(1 == abc) continue;
		}
		else if(metp_pwrtype(mid) != PWRTYPE_3X4W) {
			if(abc) break;
		}

		if(mrd_fail(mid, (mask<<abc))) continue;

		if(MdbAnalyze[mid].amp[abc] < -5) {
			if(AbnorAlarm(&malm3_stat[mid].amprev[abc], ParaCenMetp[mid].limit.time_amplimit)) {
				almabc = 1<<abc;
				almabc |= 0xc0;
				VolAmpAlarm(pbuf, 9, mid, 1, almabc);
			}
		}
		else if(MdbAnalyze[mid].amp[abc] > 5) {
			if(NormalAlarm(&malm3_stat[mid].amprev[abc], ParaCenMetp[mid].limit.time_amplimit)) {
				almabc = 1<<abc;
				almabc |= 0xc0;
				VolAmpAlarm(pbuf, 9, mid, 0, almabc);
			}
		}
	}
}

/**
* @brief ���ɵ�ѹ������ƽ��澯
* @param pbuf �澯������ָ��
* @param mid ���ID
* @param flag 0-�ָ�, 1-�쳣
* @param mask �澯����
*/
static inline void VaUnbAlarm(alarm_t *pbuf, unsigned short mid, unsigned char flag, unsigned char mask)
{
	unsigned char almflag;

	CLR_ALMBUF(pbuf);
	pbuf->erc = 17;
	pbuf->len = 19;

	DEPART_SHORT(ALARM_MID(mid), pbuf->data);
	if(flag) {
		pbuf->data[1] |= 0x80;
		almflag = ALMFLAG_ABNOR;
	}
	else almflag = ALMFLAG_NORMAL;

	pbuf->data[2] = mask;
	UnsignedToBcd(MdbAnalyze[mid].vol_unb, pbuf->data+3, 2);
	UnsignedToBcd(MdbAnalyze[mid].amp_unb, pbuf->data+5, 2);
	smallcpy(pbuf->data+7, MdbCurrent[mid].vol, 6);
	smallcpy(pbuf->data+13, MdbCurrent[mid].amp, 6);

	MakeAlarm(almflag, pbuf);
}

/**
* @brief ��ѹ������ƽ��澯���
* @param pbuf �澯������ָ��
* @param mid ���ID
*/
static void UnbAlarmProc(alarm_t *pbuf, unsigned short mid)
{
	unsigned short gate_alarm, gate_restore;


	if(metp_pwrtype(mid) == PWRTYPE_3X3W) {
		//ʧѹ��ѹ����
		if(MdbAnalyze[mid].vol[0] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[0] >= ParaCenMetp[mid].limit.vol_over) return;
		if(MdbAnalyze[mid].vol[2] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[2] >= ParaCenMetp[mid].limit.vol_over) return;
	}
	else if(metp_pwrtype(mid) == PWRTYPE_3X4W) {
		//ʧѹ��ѹ����
		if(MdbAnalyze[mid].vol[0] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[0] >= ParaCenMetp[mid].limit.vol_over) return;
		if(MdbAnalyze[mid].vol[1] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[1] >= ParaCenMetp[mid].limit.vol_over) return;
		if(MdbAnalyze[mid].vol[2] <= ParaCenMetp[mid].limit.vol_lack) return;
		if(MdbAnalyze[mid].vol[2] >= ParaCenMetp[mid].limit.vol_over) return;
	}
	else return;

	TestPrint(LOGTYPE_SHORT, "unbvol=%d,%d\n", MdbAnalyze[mid].vol_unb, ParaCenMetp[mid].limit.vol_unb);
	TestPrint(LOGTYPE_SHORT, "unbamp=%d,%d\n", MdbAnalyze[mid].amp_unb, ParaCenMetp[mid].limit.amp_unb);

	gate_alarm = ParaCenMetp[mid].limit.vol_unb;
	if(0 == gate_alarm) gate_alarm = 500;
	gate_restore = ParaCenMetp[mid].limit.restore_volunb;
	if(0 == gate_restore) gate_restore = 300;

	if(MdbAnalyze[mid].vol_unb >= gate_alarm) {
		if(AbnorAlarm(&malm3_stat[mid].unb_vol, ParaCenMetp[mid].limit.time_volunb)) {
			VaUnbAlarm(pbuf, mid, 1, 0x01);
		}
	}
	else if(MdbAnalyze[mid].vol_unb < gate_restore) {
		if(NormalAlarm(&malm3_stat[mid].unb_vol, ParaCenMetp[mid].limit.time_volunb)) {
			VaUnbAlarm(pbuf, mid, 0, 0x01);
		}
	}

	gate_alarm = ParaCenMetp[mid].limit.amp_unb;
	if(0 == gate_alarm) gate_alarm = 500;
	gate_restore = ParaCenMetp[mid].limit.restore_ampunb;
	if(0 == gate_restore) gate_restore = 300;

	if(MdbAnalyze[mid].amp_unb >= gate_alarm) {
		if(AbnorAlarm(&malm3_stat[mid].unb_amp, ParaCenMetp[mid].limit.time_ampunb)) {
			VaUnbAlarm(pbuf, mid, 1, 0x02);
		}
	}
	else if(MdbAnalyze[mid].amp_unb < gate_restore) {
		if(NormalAlarm(&malm3_stat[mid].unb_amp, ParaCenMetp[mid].limit.time_ampunb)) {
			VaUnbAlarm(pbuf, mid, 0, 0x02);
		}
	}
}

/**
* @brief ��ѹԽ�޸澯���
* @param pbuf �澯������ָ��
* @param mid ���ID
*/
static void VolOverAlarmProc(alarm_t *pbuf, unsigned short mid)
{
	unsigned char i;
	unsigned int mask;

	mask = MBMSK_VOLA;

	for(i=0; i<3; i++) {
		if(mrd_fail(mid, (mask<<i))) continue;

		if(MdbAnalyze[mid].vol[i] >= ParaCenMetp[mid].limit.vol_over) {
			if(AbnorAlarm(&malm3_stat[mid].volov[i], ParaCenMetp[mid].limit.time_volover)) {
				CLR_ALMBUF(pbuf);
				pbuf->erc = 24;
				pbuf->len = 9;

				DEPART_SHORT(ALARM_MID(mid), pbuf->data);
				pbuf->data[1] |= 0x80;
				pbuf->data[2] = 1<<i;
				pbuf->data[2] |= 0x40;
				smallcpy(pbuf->data+3, MdbCurrent[mid].vol, 6);

				MakeAlarm(ALMFLAG_ABNOR, pbuf);
			}
		}
		else if(MdbAnalyze[mid].vol[i] < ParaCenMetp[mid].limit.restore_volover) {
			if(NormalAlarm(&malm3_stat[mid].volov[i], ParaCenMetp[mid].limit.time_volover)) {
				CLR_ALMBUF(pbuf);
				pbuf->erc = 24;
				pbuf->len = 9;

				DEPART_SHORT(ALARM_MID(mid), pbuf->data);
				pbuf->data[2] = 1<<i;
				pbuf->data[2] |= 0x40;
				smallcpy(pbuf->data+3, MdbCurrent[mid].vol, 6);

				MakeAlarm(ALMFLAG_NORMAL, pbuf);
			}
		}
	}
}

/**
* @brief ���ɵ���Խ�޸澯
* @param pbuf �澯������ָ��
* @param mid ���ID
* @param balm 0-�ָ�, 1-�쳣
* @param flag �澯��־
*/
static inline void AmpOverAlarm(alarm_t *pbuf, unsigned short mid, unsigned char balm, unsigned char flag)
{
	CLR_ALMBUF(pbuf);
	pbuf->erc = 25;
	pbuf->len = 9;

	DEPART_SHORT(ALARM_MID(mid), pbuf->data);
	if(balm) {
		balm = ALMFLAG_ABNOR;
		pbuf->data[1] |= 0x80;
	}
	else balm = ALMFLAG_NORMAL;

	pbuf->data[2] = flag;
	smallcpy(pbuf->data+3, MdbCurrent[mid].amp, 6);

	MakeAlarm(balm, pbuf);
}

/**
* @brief ����Խ�޸澯���
* @param pbuf �澯������ָ��
* @param mid ���ID
*/
static void AmpOverAlarmProc(alarm_t *pbuf, unsigned short mid)
{
	unsigned char i, flag;
	unsigned int mask;

	mask = MBMSK_AMPA;

	for(i=0; i<3; i++) {
		if(mrd_fail(mid, (mask<<i))) continue;

		if(MdbAnalyze[mid].amp[i] >= ParaCenMetp[mid].limit.amp_over) {
			if(AbnorAlarm(&malm3_stat[mid].ampov[i], ParaCenMetp[mid].limit.time_ampover)) {
				flag = 1<<i;
				flag |= 0x40;
				AmpOverAlarm(pbuf, mid, 1, flag);
			}
		}
		else {
			if(MdbAnalyze[mid].amp[i] < ParaCenMetp[mid].limit.restore_ampover) {
				if(NormalAlarm(&malm3_stat[mid].ampov[i], ParaCenMetp[mid].limit.time_ampover)) {
					flag = 1<<i;
					flag |= 0x40;
					AmpOverAlarm(pbuf, mid, 0, flag);
				}
			}

			if(MdbAnalyze[mid].amp[i] >= ParaCenMetp[mid].limit.amp_limit) {
				if(AbnorAlarm(&malm3_stat[mid].amplm[i], ParaCenMetp[mid].limit.time_amplimit)) {
					flag = 1<<i;
					flag |= 0x80;
					AmpOverAlarm(pbuf, mid, 1, flag);
				}
			}
			else if(MdbAnalyze[mid].amp[i] < ParaCenMetp[mid].limit.restore_amplimit) {
				if(NormalAlarm(&malm3_stat[mid].amplm[i], ParaCenMetp[mid].limit.time_amplimit)) {
					flag = 1<<i;
					flag |= 0x80;
					AmpOverAlarm(pbuf, mid, 0, flag);
				}
			}
		}
	}
}

/**
* @brief ����Խ�޸澯���
* @param pbuf �澯������ָ��
* @param mid ���ID
*/
static void PwrOverAlarmProc(alarm_t *pbuf, unsigned short mid)
{
	unsigned char flag, bnor;
	unsigned int pwrv, limit, restore;

	if(mrd_fail(mid, MBMSK_PWRAS)) return;

	//pwrv = abs(MdbAnalyze[mid].pwra[0])+abs(MdbAnalyze[mid].pwri);
	pwrv = MdbAnalyze[mid].pwrv;

	limit = ParaCenMetp[mid].limit.pwr_over;
	restore = ParaCenMetp[mid].limit.restore_pwrover;
	
	if(pwrv >= limit) {
		if(AbnorAlarm(&malm3_stat[mid].pwrvov, ParaCenMetp[mid].limit.time_pwrover)) {
			flag = 0x40;
			bnor = 1;
			goto mark_alarm;
		}
	}
	else {
		if(pwrv < restore) {
			if(NormalAlarm(&malm3_stat[mid].pwrvov, ParaCenMetp[mid].limit.time_pwrover)) {
				flag = 0x40;
				bnor = 0;
				goto mark_alarm;
			}
		}

		limit = ParaCenMetp[mid].limit.pwr_limit;
		restore = ParaCenMetp[mid].limit.restore_pwrlimit;

		if(pwrv >= limit) {
			if(AbnorAlarm(&malm3_stat[mid].pwrvlm, ParaCenMetp[mid].limit.time_pwrlimit)) {
				flag = 0x80;
				bnor = 1;
				goto mark_alarm;
			}
		}
		else if(pwrv < restore) {
			if(NormalAlarm(&malm3_stat[mid].pwrvlm, ParaCenMetp[mid].limit.time_pwrlimit)) {
				flag = 0x80;
				bnor = 0;
				goto mark_alarm;
			}
		}
	}

	return;

mark_alarm:
	CLR_ALMBUF(pbuf);
	pbuf->erc = 26;
	pbuf->len = 9;

	DEPART_SHORT(ALARM_MID(mid), pbuf->data);
	if(bnor) {
		bnor = ALMFLAG_ABNOR;
		pbuf->data[1] |= 0x80;
	}
	else bnor = ALMFLAG_NORMAL;

	pbuf->data[2] = flag;
	UnsignedToBcd(pwrv, pbuf->data+3, 3);
	UnsignedToBcd(limit, pbuf->data+6, 3);

	MakeAlarm(bnor, pbuf);
}

/**
* @brief ���µ���������
*/
static void UpdateEneBakup(unsigned short mid)
{
	malm2_stat[mid].bak_enepa = MdbAnalyze[mid].enepa;
	malm2_stat[mid].bak_enena = MdbAnalyze[mid].enena;
}

/**
* @brief �жϵ��ʾ���½�(�����ж�, prev, bak, cur)
* @return 1�½�, 0-û��
*/
static int EneLessJudge(unsigned int *prev, unsigned int bak, unsigned int cur)
{
	unsigned int ul;

	if(0 == *prev) {
		*prev = bak;
		return 0;
	}

	if((bak > *prev) && (bak > cur) && (*prev <= cur)) {
		*prev = cur;
		return 0;
	}

	if((*prev > bak) && (*prev > cur)) {
		ul = *prev - bak;
		if(ul >= 50) { // >0.05kWh
			*prev = bak;
			return 1;
		}
	}

	*prev = bak;
	return 0;
}

/**
* @brief ʾ���½��澯���
* @param pbuf �澯������ָ��
* @param mid ���ID
*/
static void EneLessAlarmProc(alarm_t *pbuf, unsigned short mid)
{
	static unsigned int enepabak[MAX_CENMETP] = {0};
	static unsigned int enenabak[MAX_CENMETP] = {0};

	unsigned char balm1, balm2, balm;
	unsigned int ul;

	balm1 = balm2 = balm = 0;

	if((0 !=  malm2_stat[mid].bak_enepa) && (!mrd_fail(mid, MBMSK_ENEPA))) {
		if(EneLessJudge(&enepabak[mid], malm2_stat[mid].bak_enepa, MdbAnalyze[mid].enepa)) balm1 = 1;
		else balm1 = 2;
	}

	if((0 !=  malm2_stat[mid].bak_enena) && (!mrd_fail(mid, MBMSK_ENENA))) {
		if(EneLessJudge(&enenabak[mid], malm2_stat[mid].bak_enena, MdbAnalyze[mid].enena)) balm2 = 1;
		else balm2 = 2;
	}

	if(1 == balm1) {
		if(0 == (malm2_stat[mid].ene_del&0x01)) {
			balm = 1;
			malm2_stat[mid].ene_del |= 0x01;
		}
	}
	else if(1 == balm2) {
		if(0 != (malm2_stat[mid].ene_del&0x02)) {
			balm = 1;
			malm2_stat[mid].ene_del |= 0x02;
		}
	}
	else malm2_stat[mid].ene_del = 0;

	if(balm) {
		CLR_ALMBUF(pbuf);
		pbuf->erc = 27;
		pbuf->len = 12;

		DEPART_SHORT(ALARM_MID(mid), pbuf->data);
		pbuf->data[1] |= 0x80;
		ul = enepabak[mid];
		UnsignedToBcd(ul/10, pbuf->data+3, 4);
		ul = (ul%10)*10;
		UnsignedToBcd(ul, pbuf->data+2, 1);

		ul = malm2_stat[mid].bak_enepa;
		UnsignedToBcd(ul/10, pbuf->data+8, 4);
		ul = (ul%10)*10;
		UnsignedToBcd(ul, pbuf->data+7, 1);

		MakeAlarm(ALMFLAG_ABNOR, pbuf);
	}
}

/**
* @brief ���ͣ�߸澯���
* @param pbuf �澯������ָ��
* @param mid ���ID
*/
static void EneStopAlarmProc(alarm_t *pbuf, unsigned short mid)
{
	int pwra;
	unsigned short dev;

	dev = ParaMixSave.commport[COMMPORT_RS485_2].cycle;
	if(0 == dev) dev = 15;

	if((0 == malm2_stat[mid].bak_enepa) || (0 == malm2_stat[mid].bak_enena)) return;
	if(mrd_fail(mid, MBMSK_ENEPA|MBMSK_ENENA)) return;

	if((malm2_stat[mid].bak_enepa != MdbAnalyze[mid].enepa) || 
			(malm2_stat[mid].bak_enena != MdbAnalyze[mid].enena)) {
		malm2_stat[mid].ene_stop = 0;
		return;
	}

	if(malm2_stat[mid].ene_stop) return;

	pwra = MdbAnalyze[mid].pwra[0];
	if(pwra < 0) pwra *= -1;
	
	if(pwra < 400/*0.4kW*/) {
		if(malm2_stat[mid].cnt_enestop >= dev)
			malm2_stat[mid].cnt_enestop -= dev;
	}
	else malm2_stat[mid].cnt_enestop += dev;

	dev = (unsigned short)(ParaMix.metabnor.stop);
	dev *= 15;

	if(malm2_stat[mid].cnt_enestop >= dev) {
		malm2_stat[mid].ene_stop = 1;
		malm2_stat[mid].cnt_enestop = 0;

		CLR_ALMBUF(pbuf);
		pbuf->erc = 30;
		pbuf->len = 8;

		DEPART_SHORT(ALARM_MID(mid), pbuf->data);
		pbuf->data[1] |= 0x80;
		smallcpy(&pbuf->data[1], MdbCurrent[mid].enepa, 5);
		pbuf->data[6] = ParaMix.metabnor.stop;

		MakeAlarm(ALMFLAG_ABNOR, pbuf);
	}
}

/**
* @brief ������ߵ���(��������2��)
* @param mid ���ID
* @param flag 0-�����й�����, 1-�����й�����
* @param puene ���ص��õ���
* @return 0����ɹ�, 1-ʧ��
*/
static int EneFlyCalculate(unsigned short mid, unsigned char flag, unsigned int *puene)
{
	static unsigned char prevok[MAX_CENMETP] = {0};
	static unsigned char prevok_neg[MAX_CENMETP] = {0};

	unsigned char *pok;
	unsigned int chgene, curene, uene;

	if(0 == flag) {
		pok = &prevok[mid];
		chgene = malm2_stat[mid].bak_enepa;
		curene = MdbAnalyze[mid].enepa;
		if(mrd_fail(mid, (MBMSK_ENEPA|MBMSK_PWRAS))) {
			*pok = 0;
			return 1;
		}
	}
	else
	{
		pok = &prevok_neg[mid];
		chgene = malm2_stat[mid].bak_enena;
		curene = MdbAnalyze[mid].enena;
		if(mrd_fail(mid, (MBMSK_ENENA|MBMSK_PWRAS))) {
			*pok = 0;
			return 1;
		}
	}

	TestPrint(LOGTYPE_SHORT, "ene fly: cur=%d, chg=%d, ok=%d\r\n", curene, chgene, *pok);

	if(0 == *pok) {
		*pok = 1;
		return 1;
	}

	uene = chgene;
	if(uene > curene) return 1;
	uene = curene - uene;

	*puene = uene;
	return 0;
}

/**
* @brief �������ж�
* @param uene ����õ���(ʾ�ȼ���)
* @param pwr ����õ���(���ʼ���)
* @return 0-����, 1-����
*/
static int EneFlyJudge(unsigned int uene, unsigned int pwr)
{
	unsigned int ul, dev;

	dev = ParaMixSave.commport[COMMPORT_RS485_2].cycle;
	if(0 == dev) dev = 15;

	//15�����õ����������0.1kWh
	ul = dev * 100 / 15;
	if(uene < ul) return 0;

	dev = ParaMix.metabnor.fly;
	if(dev < 15) dev = 100;

	ul = pwr * dev /10;

	if(ul >= uene) return 0;
	else return 1;
}

/**
* @brief �����߸澯���
* @param pbuf �澯������ָ��
* @param mid ���ID
*/
static void EneFlyAlarmProc(alarm_t *pbuf, unsigned short mid)
{
	static cenmet_alm_t stat[MAX_CENMETP];

	int rtn1, rtn2;
	unsigned int uenepa, uenena, pwr, dev;

	dev = ParaMixSave.commport[COMMPORT_RS485_2].cycle;
	if(0 == dev) dev = 15;

	rtn1 = EneFlyCalculate(mid, 0, &uenepa);
	rtn2 = EneFlyCalculate(mid, 1, &uenena);
	if(rtn1 || rtn2) return;

	//�ɹ��ʼ����õ���
	pwr = abs(MdbAnalyze[mid].pwra[1]) + abs(MdbAnalyze[mid].pwra[2]) + abs(MdbAnalyze[mid].pwra[3]);

	pwr *= dev;
	pwr /= 600;   // /60 / 10 
	DebugPrint(LOGTYPE_SHORT, "pwr=%d, uenepa=%d, uenena=%d\r\n", pwr, uenepa, uenena);

	rtn1 = EneFlyJudge(uenepa, pwr);
	rtn2 = EneFlyJudge(uenena, pwr);

	if((1 == rtn1) || (1 == rtn2)) {
		if(AbnorAlarm(&stat[mid], 3)) {
			malm2_stat[mid].ene_fly = 1;

			CLR_ALMBUF(pbuf);
			pbuf->erc = 29;
			pbuf->len = 13;

			DEPART_SHORT(ALARM_MID(mid), pbuf->data);
			pbuf->data[1] |= 0x80;
			UnsignedToBcd(((malm2_stat[mid].bak_enepa%10)*10), pbuf->data+2, 1);
			UnsignedToBcd(malm2_stat[mid].bak_enepa/10, pbuf->data+3, 4);
			smallcpy(pbuf->data+7, MdbCurrent[mid].enepa, 5);
			pbuf->data[12] = ParaMix.metabnor.fly;
			HexToBcd(pbuf->data+11, 1);

			MakeAlarm(ALMFLAG_ABNOR, pbuf);
		}
	}
	else if((2 == rtn1) && (2 == rtn2)) {
		if(NormalAlarm(&stat[mid], 3)) {
			malm2_stat[mid].ene_fly = 0;
		}
	}
}

/**
* @brief 1���Ӹ澯����
*/
void CMetAlm1MinProc(void)
{
	static alarm_t almbuf;

	unsigned short mid;

	for(mid=0; mid<MAX_CENMETP; mid++) {
		if(EMPTY_CENMETP(mid)) continue;

		VolAlarmProc(&almbuf, mid);
		UnbAlarmProc(&almbuf, mid);
		VolOverAlarmProc(&almbuf, mid);
		AmpOverAlarmProc(&almbuf, mid);

		if((mid+1) == IMetMetpid) {
			VolRphAlarmProc(&almbuf, mid);
			AmpRevAlarmProc(&almbuf, mid);
		}

		PwrOverAlarmProc(&almbuf, mid);
	}
}


/**
* @brief �������ڸ澯����
*/
void CMetAlmRdmetProc(void)
{
	static alarm_t almbuf;

	unsigned short mid, metid;

	for(mid=0; mid<MAX_CENMETP; mid++) {
		if(EMPTY_CENMETP(mid)) continue;
		
		metid = CenMetpMap[mid].metid;
		if(METTYPE_ACSAMP == ParaMeter[metid].proto || METTYPE_PLC == ParaMeter[metid].proto) continue;

		MetChangeProc(&almbuf, mid);
		EneStopAlarmProc(&almbuf, mid);
		EneLessAlarmProc(&almbuf, mid);
		EneFlyAlarmProc(&almbuf, mid);
	}

	//enediff_almproc(&almbuf);

	for(mid=0; mid<MAX_CENMETP; mid++) UpdateEneBakup(mid);
}

/**
* @brief 15���Ӹ澯����
*/
void CMetAlm15MinProc(void)
{
	//@change later
	/*static alarm_t almbuf;

	diffa_almproc(&almbuf);*/
}

/**
* @brief ����澯״̬
*/
void ClearCMetAlarm(void)
{
	runstate_t *prun = RunStateModify();

	memset(prun->malmflag, 0, LEN_MALM_STAT);
	CenMetAlarmInit();
}

static const char *name_alm_stat[] = {"����", "�쳣,δ�澯", "�Ѹ澯", "����,�ָ���"};
#define MALM_STATE_NAME(flag)	(name_alm_stat[(flag)>>14])
#define NAME_NUM(name)			(sizeof(name)/sizeof(name[0]))

/**
* @brief ��ӡ�澯״̬
*/
static int shell_print_malmstat(int argc ,char *argv[])
{
	int metid, flag, i;
	cenmet_alm_t *pmalm;
	unsigned char *puc;

	if(argc < 2) {
		PrintLog(0, "usage: malm metid(1~%d) f(0,1,2,3)\n  malm clr\n", MAX_CENMETP);
		return 1;
	}

	if(0 == strcmp(argv[1], "clr")) {
		memset(RunStateModify()->malmflag, 0, LEN_MALM_STAT);
		SaveRunState();
		PrintLog(0, "�����Ƹ澯�ɹ�\n");
		return 0;
	}

	if(argc != 3) {
		PrintLog(0, "usage: malm metid(1~%d) f(0,1,2,3)\n  malm clr\n", MAX_CENMETP);
		return 1;
	}

	metid = atoi(argv[1]);
	if(metid <= 0 || metid > MAX_CENMETP) {
		PrintLog(0, "invalid metid\n");
		return 1;
	}

	flag = atoi(argv[2]);
	if(flag < 0 || flag > 3) {
		PrintLog(0, "invalid flag\n");
		return 1;
	}

	PrintLog(0, "��%d�澯״̬:\n", metid);
	metid -= 1;

	if(2 == flag) goto mark_malm2;
	else if(3 == flag) goto mark_malm3;

	pmalm = malm1_stat.diff;
	for(i=0; i<NAME_NUM(name_malm1); i++,pmalm++) {
		PrintLog(0, "%s: %s\n", name_malm1[i], MALM_STATE_NAME(pmalm->flag));
	}
	PrintLog(0, "\n");
	if(flag) return 0;

mark_malm2:
	puc = &malm2_stat[metid].ene_fly;
	for(i=0; i<NAME_NUM(name_malm2); i++,puc++) {
		PrintLog(0, "%s: %s\n", name_malm2[i], (*puc) ? name_alm_stat[2] : name_alm_stat[0]);
	}
	PrintLog(0, "\n");
	if(flag) return 0;

mark_malm3:
	pmalm = malm3_stat[metid].volbr;
	for(i=0; i<NAME_NUM(name_malm3); i++,pmalm++) {
		PrintLog(0, "%s: %s\n", name_malm3[i], MALM_STATE_NAME(pmalm->flag));
	}
	PrintLog(0, "\n");

	return 0;
}
DECLARE_SHELL_CMD("malm", shell_print_malmstat, "��ʾ��Ƹ澯״̬");

