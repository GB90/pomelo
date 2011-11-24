/**
* pldynamic.c -- ��̬·���ز�ͨ�Žӿ�
* 
* 
* ����ʱ��: 2010-8-25
* ����޸�ʱ��: 2010-8-25
*/

#include <stdio.h>
#include <string.h>

#include "include/debug.h"
#include "include/sys/schedule.h"
#include "include/sys/timeal.h"
#include "include/param/capconf.h"
#include "plcmet/plcomm.h"
#include "plcmet/plc_stat.h"
#include "pldynamic_dl.h"
#include "plcmet/proto/plc_proto.h"

#define PER_RELAY_TIMEOUT	48
#define MAX_PLC_TIMEOUT		300

static unsigned short PlDynamicTimeout[MAX_METER];

/**
* @brief ������
* @param dest Ŀ�ĵ�ַ
* @param itemid �������ʶ
* @param buf ����֡������ָ��
* @param len ����������
* @return �ɹ�����ʵ�����ݳ���, ʧ�ܷ���-1
*/
int PlDynamicRead(const plc_dest_t *dest, unsigned short itemid, unsigned char *buf, int len)
{
	unsigned char *cache = GetPlCommBuffer();
	int applen, timeout, i;
	//unsigned char proto;
	const plcmet_prot_t *pfunc;
	unsigned short metid;

	pfunc = GetPlcMetProto(1);
	if(NULL == pfunc) return -1;

	AssertLog(dest->metid > MAX_METER, "invalid metid(%d)\n", dest->metid);

	applen = (*pfunc->makeread)(dest->dest, itemid, cache, PLCOMM_BUF_LEN);
	if(applen <= 0) return -1;

	/*if(dest->proto == 30) proto = 2;  //dl645-2007
	else proto = 1;*/

	if(PlDynamicSendPkt(dest, cache, applen, 1)) return -1;

	if(0 == dest->metid) timeout = MAX_PLC_TIMEOUT;
	else if(0 == PlDynamicTimeout[dest->metid-1]) timeout = MAX_PLC_TIMEOUT;
	else timeout = PlDynamicTimeout[dest->metid-1];

	applen = PlDynamicRecvPkt(dest, cache, PLCOMM_BUF_LEN, timeout);

	if(applen > 0) {
		applen = (*pfunc->checkread)(dest->dest, itemid, cache, applen);
		if(applen > 0) {
			for(i=0; i<len; i++) {
				if(i < applen) *buf++ = *cache++;
				else *buf++ = 0;
			}
		}
	}

	if(!dest->metid) {
		if(applen <= 0) {
			PrintLog(LOGTYPE_DOWNLINK, "proto check error(%d)\n", applen);
			return -1;
		}
		else return applen;
	}

	metid = dest->metid - 1;
	PlcState[metid].quality = 0;

	if(applen <= 0) {
		PrintLog(LOGTYPE_DOWNLINK, "proto check error(%d)\n", applen);
		SysClockReadCurrent(&PlcState[metid].failtime);
		if(PlcState[metid].okflag) PlcState[metid].failcount = 1;
		else PlcState[metid].failcount++;
		PlcState[metid].okflag = 0;

		if(PlDynamicTimeout[metid]) {
			PlDynamicTimeout[metid] += PER_RELAY_TIMEOUT*2;
			if(PlDynamicTimeout[metid] > MAX_PLC_TIMEOUT) PlDynamicTimeout[metid] = MAX_PLC_TIMEOUT;
		}

		return -1;
	}

	SysClockReadCurrent(&PlcState[metid].oktime);
	PlcState[metid].okflag = 1;

	{
		const plc_sinfo_t *pinfo = GetPlDynamicInfo();
		unsigned char routesl;

		PlcState[metid].routes = pinfo->routes;
		if(pinfo->phase > 0 && pinfo->phase < 4) PlcState[metid].phase = 1<<(pinfo->phase-1);
		else PlcState[metid].phase = 0;
		PlcState[metid].quality = pinfo->quality;

		routesl = pinfo->routes + pinfo->quality;
		PlDynamicTimeout[metid] = (unsigned short)routesl*(PER_RELAY_TIMEOUT/2);
		PlDynamicTimeout[metid] += PER_RELAY_TIMEOUT*2;

		//PlDynamicTimeout[metid] = pinfo->ctime + PER_RELAY_TIMEOUT;
		if(PlDynamicTimeout[metid] > MAX_PLC_TIMEOUT) PlDynamicTimeout[metid] = MAX_PLC_TIMEOUT;
	}

	return applen;
}

/**
* @brief д����
* @param dest Ŀ�ĵ�ַ
* @param pconfig д����
* @return �ɹ�����0, ����ʧ��
*/
int PlDynamicWrite(const plc_dest_t *dest, const plwrite_config_t *pconfig)
{
	unsigned char *cache = GetPlCommBuffer();
	int applen, timeout;
	//unsigned char proto;
	const plcmet_prot_t *pfunc;

	pfunc = GetPlcMetProto(1);
	if(NULL == pfunc) return 1;

	applen = (*pfunc->makewrite)(dest->dest, pconfig, cache, PLCOMM_BUF_LEN);
	if(applen <= 0) return 1;

	/*if(dest->proto == 30) proto = 2;  //dl645-2007
	else proto = 1;*/

	if(PlDynamicSendPkt(dest, cache, applen, 1)) return 1;

	if(0 == dest->metid) timeout = MAX_PLC_TIMEOUT;
	else if(0 == PlDynamicTimeout[dest->metid-1]) timeout = MAX_PLC_TIMEOUT;
	else timeout = PlDynamicTimeout[dest->metid-1];
	applen = PlDynamicRecvPkt(dest, cache, PLCOMM_BUF_LEN, timeout);

	if(applen <= 0) return 1;
	if((*pfunc->checkwrite)(dest->dest, pconfig->itemid, cache, applen)) {
		PrintLog(LOGTYPE_DOWNLINK, "proto check error(%d)\n", applen);
		return 1;
	}

	return 0;
}

/**
* @brief �㲥��ʱ
* @param flag 0-���ͽ�ʱ����, 1-��ѯ��ʱ��ʽ
* @return flag=0ʱ �ɹ�����0, ʧ�ܷ��ظ���, flag=1ʱ���ؽ�ʱ��ʽ(PLCHKTIME_POLL)
*/
int PlDynamicCheckTime(const plc_dest_t *dest, int flag)
{
	if(flag) return PLCHKTIME_BROCAST;

	unsigned char *cache = GetPlCommBuffer();
	int applen;
	const plcmet_prot_t *pfunc;
	sysclock_t clock;

	pfunc = GetPlcMetProto(1);
	if(NULL == pfunc) return -1;

	SysClockReadCurrent(&clock);
	applen = (*pfunc->makechktime)(&clock, cache, PLCOMM_BUF_LEN);
	if(applen <= 0) return -1;

	if(PlDynamicSendPkt(dest, cache, applen, 1)) return -1;

	return 300;
}


