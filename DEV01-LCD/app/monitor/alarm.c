/**
* alarm.c -- �¼��澯�ӿ�
* 
* 
* ����ʱ��: 2010-5-15
* ����޸�ʱ��: 2010-5-15
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/environment.h"
#include "include/debug.h"
#include "include/sys/syslock.h"
#include "include/sys/timeal.h"
#include "include/sys/bin.h"
#include "include/lib/bcd.h"
#include "include/monitor/runstate.h"
#include "include/monitor/alarm.h"
#include "include/cenmet/qrycurve.h"
#include "include/param/term.h"
#include "include/uplink/svrnote.h"
#include "include/debug/shellcmd.h"
#include "include/screen/scr_show.h"

#define ALARM_SAVEPATH	DATA_PATH
#define ALARM_MAGIC		0x9631f810

#define ALMNUM_PERFILE		32
#define ALARM_BASEMASK		((unsigned char)~(ALMNUM_PERFILE-1))

static alarm_t AlarmBufferImportant[256];
static alarm_t AlarmBufferNormal[256];

static int LockIdAlarm = -1;

/**
* @brief ����¼�����
* @param flag �¼���־, ����ALMFLAG_ABNOR...
* @param erc �¼�ID
* @param prank �¼��ȼ�����ָ��, 1-��Ҫ�¼�, 0-һ���¼�
* @return ����1��ʾ�¼���Ч(�豣��), ����0��ʾ��Ч
*/
static int GetAlarmAttribute(unsigned char flag, unsigned char erc, unsigned char *prank)
{
	unsigned char idx, mask;

	*prank = 0;

	if((0 == erc) || (erc > 64))  return 0;
	erc -= 1;

	idx = erc>>3;
	mask = 1<<(erc&0x07);
	if(flag&0x0f) RunStateModify()->alarm.stat[idx] &= ~mask;
	else RunStateModify()->alarm.stat[idx] |= mask;

	if(mask & (ParaTerm.almflag.valid[idx])) {
		if(mask & (ParaTerm.almflag.rank[idx])) *prank = 1;
		return 1;
	}

	return 0;
}

/**
* @brief �¼���ѯ��ʼָ��Ϸ����ж�
* @param idx �¼���������(��Ҫ�¼�,һ���¼�)
* @param pm ��ѯ��ʼָ��
* @return ����2��ʾ��ĩβ, ����1��ʾ�Ƿ�ָ��, ����0��ʾ�Ϸ�
*/
static int InvalidPm(unsigned char idx, unsigned char pm)
{
	if(RunState.alarm.cur[idx] == pm) return 2;
	else if(RunState.alarm.cur[idx] == RunState.alarm.head[idx]) {
		return 2;
	}
	else if(RunState.alarm.cur[idx] > RunState.alarm.head[idx]) {
		if(pm > RunState.alarm.cur[idx] || pm < RunState.alarm.head[idx]) return 1;
	}
	else {
		if(pm > RunState.alarm.cur[idx] && pm < RunState.alarm.head[idx]) return 1;
	}

	return 0;
}

/**
* @brief ����¼������ļ���
* @param idx �¼���������(��Ҫ�¼�,һ���¼�)
* @param offset ƫ��ֵ
* @filename �����ļ���
*/
static void AlarmFileName(unsigned char idx, unsigned char offset, char *filename)
{
	char c;
	if(0 == idx) c = 'i';
	else c = 'n';

	sprintf(filename, ALARM_SAVEPATH "%c%d.alm", c, offset/ALMNUM_PERFILE);
}

/**
* @brief �����¼�
* @param idx �¼���������(��Ҫ�¼�,һ���¼�)
* @param pbuf �����¼�������ָ��
*/
static void SaveAlarm(unsigned char idx, const alarm_t *pbuf)
{
	alarm_t *pbase;
	char filename[32];
	unsigned char idxbase;
	runstate_t *pstat = RunStateModify();

	AssertLogReturnVoid(idx>=2, "invalid idx(%d)\n", idx);

	if(0 == idx) pbase = AlarmBufferImportant;
	else pbase = AlarmBufferNormal;

	LockSysLock(LockIdAlarm);

	if(RunState.alarm.cur[idx] == RunState.alarm.head[idx])  //empty
		pstat->alarm.cur[idx] = pstat->alarm.head[idx] = 0;

	//pbase[RunState.alarm.cur[idx]] = *pbuf;
	memcpy(pbase+RunState.alarm.cur[idx], pbuf, sizeof(alarm_t));

	AlarmFileName(idx, RunState.alarm.cur[idx], filename);

	idxbase = RunState.alarm.cur[idx] & ALARM_BASEMASK;
	SaveBinFile(filename, ALARM_MAGIC, (unsigned char *)&pbase[idxbase], sizeof(alarm_t)*ALMNUM_PERFILE);

	pstat->alarm.cur[idx]++;
	if(RunState.alarm.cur[idx] == RunState.alarm.head[idx]) pstat->alarm.head[idx]++;

	UnlockSysLock(LockIdAlarm);
}

/**
* @brief ����¼�����
* @param idx �¼���������(��Ҫ�¼�,һ���¼�)
*/
void ClearAlarm(unsigned char idx)
{
	runstate_t *pstat = RunStateModify();

	AssertLogReturnVoid(idx>=2, "invalid idx(%d)\n", idx);

	pstat->alarm.head[idx] = 0;
	pstat->alarm.cur[idx] = 0;
	pstat->alarm.snd[idx] = 0;
}

static const char *AlarmName[] = {
	"δ֪", // 0
	"���ݳ�ʼ��!",
	"������ʧ!",
	"�������!",
	"״̬����λ!",  // 4
	"ң����բ!",
	"������բ!",
	"�����բ!", 
	"���������!", // 8
	"������·�쳣!",
	"��ѹ��·�쳣!",
	"�����쳣!",
	"���ʱ�䳬��!", //12
	"������!", 
	"�ն�ͣ/�ϵ�!", 
	"г��Խ��!",
	"ֱ��ģ����Խ��!", // 16
	"��ѹ/������ƽ��!", 
	"������Ͷ������!", 
	"����������!",
	"��Ϣ��֤����!", // 20
	"�ն˹���!", 
	"�����Խ��!", 
	"��ظ澯!",
	"��ѹԽ��!", // 24
	"����Խ��!", 
	"���ڹ���Խ��!",
	"���ʾ���½�!",
	"��������!", // 28
	"������!", 
	"���ͣ��!", 
	"485����ʧ��!", 
	"ͨ����������!", //32
	"���״̬�ֱ�λ!", 
	"CT�쳣!",
	"����δ֪���!",
};
#define ALARMNAME_NUM		(sizeof(AlarmName)/sizeof(AlarmName[0]))

/**
* @brief �����¼��澯
* @param flag �¼���־(����ALMFLAG_IMPORTANT...)
* @param pbuf �����¼�������ָ��
*/
void MakeAlarm(unsigned char flag, alarm_t *pbuf)
{
	unsigned char bvalid, rank, dbid;

	DebugPrint(LOGTYPE_ALARM, "make alarm %d\n", pbuf->erc);

	AssertLogReturnVoid(pbuf->len>MAXLEN_ALMDATA, "invalid len(%d)\n", pbuf->len);

	bvalid = GetAlarmAttribute(flag, pbuf->erc, &rank);
	if(!bvalid) return;

	if(pbuf->erc < ALARMNAME_NUM) 
		PrintLog(LOGTYPE_ALARM, "!alarm %d, r=%d(%s)\r\n", pbuf->erc, rank, AlarmName[pbuf->erc]);
	else PrintLog(LOGTYPE_ALARM, "!alarm %d, r=%d\r\n", pbuf->erc, rank);

	if(14 != pbuf->erc) {  //ͣ��/�ϵ�
		sysclock_t clock;

		SysClockReadCurrent(&clock);
		pbuf->min = clock.minute;
		pbuf->hour = clock.hour;
		pbuf->day = clock.day;
		pbuf->mon = clock.month;
		pbuf->year = clock.year;
		HexToBcd(&pbuf->min, 5);
	}

	if(flag&ALMFLAG_IMPORTANT) dbid = 0;
	else if(flag&ALMFLAG_NOIMPORTANT) dbid = 1;
	else if(rank) dbid = 0;
	else dbid = 1;

	SaveAlarm(dbid, pbuf);

	if(0 == dbid) {
		if((flag&0x0f) == ALMFLAG_ABNOR) ScreenSetHeadFlag(SCRHEAD_FLAG_ALM, pbuf->erc);
		else ScreenSetHeadFlag(SCRHEAD_FLAG_CLRALM, pbuf->erc);

		RunStateModify()->flag_acd = 1;
		SvrCommNote(SVRNOTEID_ALARM);
	}

	if(pbuf->erc && pbuf->erc < ALARMNAME_NUM) {
		//DebugPrint(0, "display alarm %d, %s\n", pbuf->erc, AlarmName[pbuf->erc]);
		ScreenDisplayAlarm(AlarmName[pbuf->erc]);
	}

	SaveRunState();	
}

/**
* @brief ����¼�����
* @param ec �����¼�����ָ��
*/
void GetAlarmEc(unsigned char *ec)
{
	unsigned short us;
	int i;

	for(i=0; i<2; i++) {
		if(RunState.alarm.head[i] == RunState.alarm.cur[i]) 
			ec[i] = 0;
		else if(RunState.alarm.head[i] < RunState.alarm.cur[i])
			ec[i] = RunState.alarm.cur[i] - RunState.alarm.head[i];
		else {
			us = 256;
			us -= (unsigned short)(RunState.alarm.head[i])&0xff;
			us += (unsigned short)(RunState.alarm.cur[i])&0xff;
			ec[i] = (unsigned char)us;
		}
	}
}

/**
* @brief �¼��ӿڳ�ʼ��
* @return �ɹ�0, ����ʧ��
*/
DECLARE_INIT_FUNC(AlarmInit);
int AlarmInit(void)
{
	int idx, offset;
	char filename[32];
	alarm_t *pbuf;

	memset(AlarmBufferImportant, 0, sizeof(AlarmBufferImportant));
	memset(AlarmBufferNormal, 0, sizeof(AlarmBufferNormal));

	for(idx=0; idx<2; idx++) {
		if(0 == idx) pbuf = AlarmBufferImportant;
		else pbuf = AlarmBufferNormal;

		DebugPrint(0, "  load ");
		for(offset=0; offset<256; offset+=ALMNUM_PERFILE,pbuf+=ALMNUM_PERFILE) {
			AlarmFileName(idx, offset, filename);

			DebugPrint(0, "%c%d", (idx)?'n':'i', offset/ALMNUM_PERFILE);
			if(ReadBinFile(filename, ALARM_MAGIC, (unsigned char *)pbuf, sizeof(alarm_t)*ALMNUM_PERFILE) > 0) {
				DebugPrint(0, " ok, ");
			}
			else {
				DebugPrint(0, " fail, ");
			}
		}
		DebugPrint(0, "\n");
	}

	LockIdAlarm = RegisterSysLock();

	SET_INIT_FLAG(AlarmInit);

	return 0;
}

/**
* @brief ���¼�����ת��������ͨ�Ÿ�ʽ
* @param palm �¼�����ָ��
* @param buf ���������ָ��
* @return ���뻺�������ֽ���
*/
static int FormatAlarm(alarm_t *palm, unsigned char *buf)
{
	int len;

	if(palm->len > MAXLEN_ALMDATA) return 0;

	len = palm->len;
	len += 7;

	memcpy(buf, &palm->erc, len);
	buf[1] += 5;

	return(len);
}

/**
* @brief �������ͨ��ƫ��ֵ
* @param dbid �¼���������(��Ҫ�¼�,һ���¼�)
* @param pm ������ƫ��ֵ
* @return ����ͨ��ƫ��ֵ
*/
static inline unsigned char GetAlarmOffset(unsigned char dbid, unsigned char pm)
{
	if(RunState.alarm.head[dbid] <= pm)
		pm -= RunState.alarm.head[dbid];
	else 
		pm += ((unsigned short)256 - (unsigned short)RunState.alarm.head[dbid]);

	return(pm);
}

typedef struct {
	unsigned char p0[2];
	unsigned char fn[2];
	unsigned char pm;
	unsigned char pn;
} almqry_t;

typedef struct {
	unsigned char p0[2];
	unsigned char fn[2];
	unsigned char ec[2];
	unsigned char pm;
	unsigned char pn;
} almsnd_t;

/**
* @brief �������͸澯
* @param sendbuffer ���ͻ�����ָ��
* @return �ɹ�0, ����ʧ��
*/
int ActiveSendAlarm(qrycurve_buffer_t *sendbuffer)
{
	alarm_t *palm = AlarmBufferImportant;
	almsnd_t *psend = (almsnd_t *)QRYCBUF_DATA(sendbuffer);
	unsigned char dbid = 0;
	int rtn;

	printf("ActiveSendAlarm...................");

	RunStateModify()->flag_acd = 0;

	rtn = InvalidPm(dbid, RunState.alarm.snd[dbid]);
	if(rtn == 1) {
		RunStateModify()->alarm.snd[dbid] = RunState.alarm.head[dbid];
	}
	else if(rtn == 2) return 1;  //empty

	psend->p0[0] = psend->p0[1] = 0;
	psend->fn[1] = 0;
	if(0 == dbid) psend->fn[0] = 0x01;
	else psend->fn[0] = 0x02;
	GetAlarmEc(psend->ec);
	psend->pm = GetAlarmOffset(dbid, RunState.alarm.snd[dbid]);
	psend->pn = psend->pm + 1;

	rtn = FormatAlarm(&palm[RunState.alarm.snd[dbid]], (unsigned char *)(psend+1));
	if(rtn <= 0) return 1;

	if(QRYCBUF_MAXLEN(sendbuffer) < (rtn+sizeof(almsnd_t))) {
		ErrorLog("too short len(%d)\n", QRYCBUF_MAXLEN(sendbuffer));
		return 1;
	}

	QRYCBUF_ADD(sendbuffer, rtn+sizeof(almsnd_t));
	return 0;
}

/**
* @brief ��ѯ�澯
* @param recvbuf ���ղ�ѯѡ�����ָ��
* @param sendbuffer ���ͻ�����ָ��
* @return �ɹ�0, ����ʧ��
*/
int QueryAlarm(const unsigned char *recvbuf, qrycurve_buffer_t *sendbuffer)
{
	const almqry_t *pqry = (const almqry_t *)recvbuf;
	almsnd_t *psend;
	alarm_t *palm;
	unsigned char *pbuf;
	unsigned char dbid, base;
	int datalen, num, i, rtn;

	/*DebugPrint(0, "query alarm %02X%02X:%02X%02X %d-%d\n",
		pqry->p0[0], pqry->p0[1], pqry->fn[0], pqry->fn[1], pqry->pm, pqry->pn);
	DebugPrint(0, "head=%d, cur=%d, snd=%d\n", 
		RunState.alarm.head[0], RunState.alarm.cur[0], RunState.alarm.snd[0]);*/

	//if(pqry->p0[0] || pqry->p0[1] || pqry->fn[1]) return 1;  // ? this is true, complier bug?...
	if(pqry->p0[0] | pqry->p0[1] | pqry->fn[1]) return 1;  // change to this...

	if(1 == pqry->fn[0]) {
		dbid = 0;
		palm = AlarmBufferImportant;
	}
	else if(2 == pqry->fn[0]) {
		dbid = 1;
		palm = AlarmBufferNormal;
	}
	else return 1;

	if(pqry->pm == pqry->pn) return 1;

	if(pqry->pn > pqry->pm) num = (int)(pqry->pn - pqry->pm)&0xff;
	else num = (int)((unsigned short)256 + (unsigned short)pqry->pn - (unsigned short)pqry->pm)&0xff;
	base = pqry->pm + RunState.alarm.head[dbid];
	if(InvalidPm(dbid , base)) return 1;

	psend = (almsnd_t *)QRYCBUF_DATA(sendbuffer);
	psend->p0[0] = psend->p0[1] = 0;
	psend->fn[1] = 0;
	psend->fn[0] = pqry->fn[0];
	GetAlarmEc(psend->ec);
	psend->pm = psend->pn = pqry->pm;
	datalen = sizeof(almsnd_t);
	pbuf = (unsigned char *)(psend+1);

	//DebugPrint(0, "start query %d alarm, dbid=%d, base=%d\n", num, dbid, base);

	for(i=0; i<num; i++) {
		if((datalen + MAXLEN_ALMDATA) > QRYCBUF_MAXLEN(sendbuffer)) {
			unsigned char bakpn = psend->pn;

			if(datalen <= sizeof(almsnd_t)) {
				ErrorLog("too short len(%d)\n", datalen);
				return 1;
			}
			QRYCBUF_ADD(sendbuffer, datalen);
			QRYCBUF_NEXT(sendbuffer);
			if(NULL == sendbuffer) return 1;

			psend = (almsnd_t *)QRYCBUF_DATA(sendbuffer);
			psend->p0[0] = psend->p0[1] = 0;
			psend->fn[1] = 0;
			psend->fn[0] = pqry->fn[0];
			GetAlarmEc(psend->ec);
			psend->pm = psend->pn = bakpn;
			datalen = sizeof(almsnd_t);
			pbuf = (unsigned char *)(psend+1);
		}

		rtn = FormatAlarm(&palm[base], pbuf);
		//DebugPrint(0, "fomat alarm %d, len=%d\n", base, rtn);
		if(rtn <= 0) {
			base++;
			if(base == RunState.alarm.cur[dbid]) break;
			continue;
		}

		base++;
		psend->pn++;
		datalen += rtn;
		pbuf += rtn;
		if(base == RunState.alarm.cur[dbid]) break;
	}

	//DebugPrint(0, "datalen=%d, min=%d\n", sendbuffer->datalen, sizeof(almsnd_t));
	if(datalen <= sizeof(almsnd_t)) return 1;
	QRYCBUF_ADD(sendbuffer, datalen);
	return 0;
}

int shell_clearalarm(int argc, char *argv[])
{
	ClearAlarm(0);
	ClearAlarm(1);
	RunStateModify()->flag_acd = 0;
	SaveRunState();

	system("rm -f " ALARM_SAVEPATH "*.alm");

	PrintLog(0, "�澯����ɹ�\n");
	return 0;
}
DECLARE_SHELL_CMD("almclr", shell_clearalarm, "������и澯");

#if 1
int shell_alarmtest(int argc, char *argv[])
{
	int i, num;
	sysclock_t clock;
	utime_t utime;
	alarm_t alarmbuf;

	if(4 != argc) {
		PrintLog(0, "usage: almtest month day num\n");
		return 1;
	}

	SysClockReadCurrent(&clock);

	clock.hour = 0;
	clock.minute = 0;
	clock.second = 0;

	i = atoi(argv[1]);
	if(i < 1 || i > 12) {
		PrintLog(0, "invalid month\n");
		return 1;
	}
	clock.month = i;

	i = atoi(argv[2]);
	if(i < 1 || i > 31) {
		PrintLog(0, "invalid day\n");
		return 1;
	}
	clock.day = i;

	num = atoi(argv[3]);
	if(num <= 0 || num > 1000) {
		PrintLog(0, "invalid len\n");
		return 1;
	}

	utime = SysClockToUTime(&clock);
	memset(&alarmbuf, 0, sizeof(alarmbuf));

	for(i=0; i<num; i++) {
		alarmbuf.erc = 14;
		alarmbuf.len = 5;
		alarmbuf.min = clock.minute;
		alarmbuf.hour = clock.hour;
		alarmbuf.day = clock.day;
		alarmbuf.mon = clock.month;
		alarmbuf.year = clock.year;
		HexToBcd(&alarmbuf.min, 5);
		MakeAlarm(ALMFLAG_ABNOR, &alarmbuf);

		utime += 60;
		UTimeToSysClock(utime, &clock);
	}

	PrintLog(0, "make %d alarm ok\n", num);
	return 0;
}
DECLARE_SHELL_CMD("almtest", shell_alarmtest, "�澯����");
#endif

