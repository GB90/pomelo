/**
* keepalive.c -- �����������������
* 
* 
* ����ʱ��: 2010-5-19
* ����޸�ʱ��: 2010-5-19
*/

#include <stdio.h>
#include <string.h>

#include "include/debug.h"
#include "include/sys/timeal.h"
#include "include/param/term.h"
#include "uplink_pkt.h"
#include "svrcomm.h"
#include "keepalive.h"

/**
* @brief ��ȡ��������
* @return ��������(100ms)
*/
static int GetCycKeepAlive(void)
{
	int i;

	i = (int)(ParaTerm.tcom.cycka)&0xff;
	i *= 600;

	return(i);
}

#define TIMEOUT_KEEPALIVE    600    // 1 minute
#define TIMEOUT_GNOWAIT    600    // 1 minute
typedef struct {
	int cnt;
	int cnt_max;
	int cnt_getcyc;
	int cnt_retry;
} keepalive_stat_t;
static keepalive_stat_t keepalive_stat;

/**
* @brief ���״̬
*/
void ClearKeepAlive(void)
{
	int cyc = GetCycKeepAlive();

	keepalive_stat.cnt = 0;
	keepalive_stat.cnt_retry = 0;
	keepalive_stat.cnt_getcyc = 0;
	keepalive_stat.cnt_max = cyc;
}

/**
* @brief ����Ƿ��ڿ�����ʱ��
* @return �����߷���1, �������߷���0
*/
int KeepAliveInPeriod(void)
{
#define TIMOUT_CHECK	600  // 1minute
	static int lastrtn = 1;
	static int count = TIMOUT_CHECK;

	if(ParaTerm.uplink.clientmode != 2) return 1;

	count++;
	if(count > TIMOUT_CHECK) {
		sysclock_t clock;
		unsigned int mask;

		count = 0;
		SysClockReadCurrent(&clock);
		mask = (unsigned int)1<<(clock.hour);
		if(mask&ParaTerm.uplink.onlineflag) lastrtn = 1;
		else lastrtn = 0;
	}

	return lastrtn;
}

/**
* @brief �������Ӵ���
* @return ����0-��Ҫ������·���Ի��ߵ�½, 1-��Ҫ������·
*/
int KeepAliveProc(void)
{
	int cnt_maxretry = (int)ParaTerm.uplink.countdail & 0xff;

	if(cnt_maxretry == 0) cnt_maxretry = 3;

	if((LINESTAT_OFF == SvrCommLineState) && (keepalive_stat.cnt_max > 1800)
		&& (keepalive_stat.cnt_retry < cnt_maxretry))
	{
		int cnt_timedail = (int)ParaTerm.uplink.timedail & 0xffff;

		cnt_timedail *= 10;
		if(cnt_timedail == 0) cnt_timedail = 600;

		keepalive_stat.cnt++;
		if(keepalive_stat.cnt > cnt_timedail) {
			/*DebugPrint(0, "cnt_timedail=%d, retry=%d, maxtry=%d, cnt_max=%d\n", 
				cnt_timedail, keepalive_stat.cnt_retry, cnt_maxretry, keepalive_stat.cnt_max);*/
			keepalive_stat.cnt_retry++;
			keepalive_stat.cnt = 0;
			keepalive_stat.cnt_getcyc = 0;
			keepalive_stat.cnt_max = GetCycKeepAlive();
			return 0;
		}
	}
	else if(keepalive_stat.cnt_max >= 100) {
		keepalive_stat.cnt++;
		if(keepalive_stat.cnt > keepalive_stat.cnt_max) {
			//DebugPrint(0, "cnt_max=%d\n", keepalive_stat.cnt_max);
			keepalive_stat.cnt = 0;
			keepalive_stat.cnt_getcyc = 0;
			keepalive_stat.cnt_max = GetCycKeepAlive();
			return 0;
		}
	}
	else {
		keepalive_stat.cnt_getcyc++;
		if(keepalive_stat.cnt_getcyc > TIMEOUT_KEEPALIVE) {
			keepalive_stat.cnt_getcyc = 0;
			keepalive_stat.cnt_max = GetCycKeepAlive();
			return 0;
		}
	}

	return 1;
}

/**
* @brief ˢ����·����
* @return �ɹ�0, ����ʧ��
*/
int RefreshKeepAlive(void)
{
	if((LINESTAT_ON == SvrCommLineState) && 
		(keepalive_stat.cnt >= TIMEOUT_GNOWAIT) &&
		SvrCommHaveTask())
	{
		keepalive_stat.cnt = 0;
		keepalive_stat.cnt_getcyc = 0;
		keepalive_stat.cnt_max = GetCycKeepAlive();
		return 0;
	}

	return 1;
}

/**
* @brief ���ñ�־
* @param flag ��־
*/
void SetKeepAlive(unsigned char flag)
{
	switch(flag) {
	case KEEPALIVE_FLAG_LOGONFAIL:
		//keepalive_stat.cnt_retry++;
		break;

	case KEEPALIVE_FLAG_LOGONOK:
		keepalive_stat.cnt_retry = 0;
		break;

	default: break;
	}
}

