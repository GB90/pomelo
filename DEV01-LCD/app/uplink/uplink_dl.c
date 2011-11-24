/**
* uplink_dl.c -- ����ͨ��������·��
* 
* 
* ����ʱ��: 2010-5-18
* ����޸�ʱ��: 2010-5-18
*/

#include <stdio.h>

#include "include/debug.h"
#include "include/sys/schedule.h"
#include "include/lib/align.h"
#include "include/param/term.h"
#include "include/param/unique.h"
#include "include/param/mix.h"
#include "include/monitor/runstate.h"
#include "uplink_pkt.h"
#include "uplink_dl.h"
#include "svrcomm.h"
#include "cascade.h"


static struct uplink_timer_t {
	int cnt;
	int max;
	int flag;
} UplinkTimer[UPLINKITF_NUM];

static struct uplink_fsm_t {
	unsigned char *pbuf;
	unsigned short cnt;
	unsigned short maxlen;
	unsigned char stat;
} UplinkFsm[UPLINKITF_NUM];


/**
* @brief ������ݰ��ĺϷ���
* @param itf �ӿڱ��
* @param pkt ���ݰ�ָ��
* @return 0�Ϸ�, ����Ƿ�
*/
static int CheckPacket(unsigned char itf, const uplink_pkt_t *pkt)
{
	unsigned short i, len;
	const unsigned char *puc;
	unsigned char chk;

	len = UPLINKAPP_LEN(pkt);
	
	if(len > UPLINK_RCVMAX(itf)) return 1;
	len += LEN_UPLINKHEAD;

	puc = &pkt->ctrl;
	chk = 0;
	for(i=0; i<len; i++) chk += *puc++;

	if(chk != *puc++) return 1;
	if(UPLINK_TAIL != *puc) return 1;

	if(pkt->area[0] != ParaUni.addr_area[0] || pkt->area[1] != ParaUni.addr_area[1])
		goto mark_cascade;

	if((0xff == pkt->sn[0]) && (0xff == pkt->sn[1])) {
		//pkt->grp &= 0xfe;
		return 0;
	}

	if(pkt->grp&0x01) {//���ַ
		if(RunState.outgrp) return 1;

		//pkt->grp &= 0xfe;
		if((0 == pkt->sn[0]) && (0 == pkt->sn[1])) return 1;

		for(i=0; i<16; i+=2) {
			if((pkt->sn[0] == ParaTerm.grpaddr.addr[i]) &&
				(pkt->sn[1] == ParaTerm.grpaddr.addr[i+1]))
				break;
		}
		if(i >= 16) return 1;
	}
	else {
		if(pkt->sn[0] != ParaUni.addr_sn[0] || pkt->sn[1] != ParaUni.addr_sn[1])
			return 1;
			//goto mark_cascade;
	}

	if(pkt->ctrl&UPCTRL_DIR) return 1;
	
	return 0;

mark_cascade:
	if(CASCADE_SERVER != CascadeMode || itf != SvrCommInterface) return 1;
	if(pkt->grp&0x01) return 1;

	len = ParaMix.cascade.num;
	if(len > MAX_CASCADE || len == 0) return 1;

	{
		unsigned int addr1, addr2;

		addr1 = MAKE_LONG(pkt->area);
		for(i=0; i<len; i++) {
			addr2 = MAKE_LONG(ParaMix.cascade.addr+(i<<2));

			if(addr1 == addr2) break;
		}
		if(i >= len) return 1;

	}

	if(0 == (pkt->ctrl&UPCTRL_PRM)) return 1;
	else if(0 == pkt->afn || UPAFN_CASCADE == pkt->afn) return 1;

	CascadeForwardPkt(UPLINKITF_CASCADE, (uplink_pkt_t *)pkt);
	return 1;
}

/**
* @brief �����ݰ���ʽ��Ϊ����ͨ�Ÿ�ʽ
* @param itf �ӿڱ��
* @param pkt ���ݰ�ָ��
* @return �ɹ��������ݰ�����, ʧ�ܷ���-1
*/
static int MakePacket(unsigned char itf, uplink_pkt_t *pkt)
{
	int i, len;
	unsigned char *puc;
	unsigned char chk;
	unsigned short us;

	len = UPLINKAPP_LEN(pkt) & 0xffff;
	AssertLogReturn(len > UPLINK_SNDMAX(itf), -1, "invalid len(%d)\n", len);
	len += LEN_UPLINKHEAD;

	us = (unsigned short)len;
	us <<= 2;
	us |= 0x0002;

	pkt->head = pkt->dep = UPLINK_HEAD;
	pkt->len1[0] = (unsigned char)us;
	pkt->len1[1] = (unsigned char)(us>>8);
	pkt->len2[0] = pkt->len1[0];
	pkt->len2[1] = pkt->len1[1];
	//pkt->grp = 0;

	puc = (unsigned char *)&pkt->ctrl;
	chk = 0;
	for(i=0; i<len; i++) chk += *puc++;

	*puc++ = chk;
	*puc = UPLINK_TAIL;
	len += LEN_UPLINKDL;

	return(len);
}

/**
* @brief ���ý��ն�ʱ��
* @param itf �ӿڱ��
* @param max ��ʱʱ��(100ms)
*/
static void SetUpTimer(unsigned char itf, int max)
{
	UplinkTimer[itf].cnt = 0;
	UplinkTimer[itf].max = max;
	UplinkTimer[itf].flag = 1;
}

/**
* @brief ֹͣ��ʱ��
*/
static void StopUpTimer(unsigned char itf)
{
	UplinkTimer[itf].flag = 0;
}

/**
* @brief �������״̬
* @param itf �ӿڱ��
*/
void UplinkClearState(unsigned char itf)
{
	UplinkFsm[itf].pbuf = UPLINK_RCVBUF(itf);
	UplinkFsm[itf].stat = 0;
	UplinkFsm[itf].cnt = 0;
	UplinkFsm[itf].maxlen = 0;

	StopUpTimer(itf);
}

/**
* @brief ���ն�ʱ������
*/
static void UpTimerProc(unsigned char itf)
{
	if(!UplinkTimer[itf].flag) 
	{
		UplinkFsm[itf].pbuf = UPLINK_RCVBUF(itf);//ָ����ջ�����
		UplinkFsm[itf].stat = 0;
		UplinkFsm[itf].cnt = 0;
		UplinkFsm[itf].maxlen = 0;
		return;
	}

	UplinkTimer[itf].cnt++;
	if(UplinkTimer[itf].cnt >= UplinkTimer[itf].max) {
		UplinkClearState(itf);
	}

	return;
}

#define UPLINK_NEEDPRINT(itf)    (((itf) > UPLINKITF_ETHMTN) && (LOGITF_SVRCOMM != GetLogInterface()))
//#define UPLINK_NEEDPRINT(itf)    ((itf) > UPLINKITF_ETHMTN)

/**
* @brief ��ӡͨ��֡����
* @param pkt ͨ��ָ֡��
* @param prompt ��ʾ�ַ���ָ��
* @param applen ͨ��֡Ӧ�ò㳤��
*/
static void UplinkPrintPkt(const uplink_pkt_t *pkt, const char *prompt, unsigned short applen)
{
	unsigned short len;
	int logolvl = GetLogType();
	int i;
	unsigned char *test_print;

	if(logolvl < LOGTYPE_SHORT) return;

	len = applen;
	PrintLog(0, "%s: C=%02XH, AFN=%02X, len=%d\r\n", prompt, pkt->ctrl, pkt->afn, len);

	if(logolvl != LOGTYPE_UPLINK) return;

	len += MINLEN_UPLINKPKT;

	PrintHexLog(logolvl, (unsigned char *)pkt, len);
	test_print = (unsigned char *)pkt;
	for(i=0;i<len;i++)
	{
		printf("%02x",test_print[i]);
	}
	printf("\n");
}

/**
* @brief ��������֡(100ms����һ��)
* @param itf �ӿڱ��
* @return ����0��ʾ���յ�һ����ȷ��֡, ����1��ʾû��
*/
int UplinkRecvPkt(unsigned char itf)
{
	struct uplink_fsm_t *fsm = &UplinkFsm[itf];
	//�õ������շ��Ľӿ�
	const uplinkitf_t *pitf = &UplinkInterface[itf];
	uplink_pkt_t *pkt;
	unsigned char uc;
	unsigned short len1, len2;

	UpTimerProc(itf);//��ʼ������

	//ѭ�����õײ����ݽ��պ�����֪�����ݽ������
	//if(UPLINKITF_GPRS == itf)
		//printf("GPRS UplinkRecvPkt...\n");
	//if(itf == UPLINKITF_GPRS)
		//printf("UplinkRecvPkt	GPRS\n");

	//if(ParaMixSave.mix.bactcomm)	return 1;
	
	while(!(*pitf->getchar)(&uc)) 
	{
		//if(FAALITF_GPRS == itf) print_logo(0, "r=%02X,s=%d\r\n", uc, fsm->stat);

		switch(fsm->stat) 
		{
		case 0:
			if(UPLINK_HEAD == uc) 
			{
				printf("UplinkRecvUPLINK_HEAD\n");
				UplinkClearState(itf);
				*(fsm->pbuf)++ = uc;//���յ�һ���ֽ�
				fsm->stat = 1;//״̬�����뵽��һ���ֽڵĽ���
				fsm->maxlen = 4;//stat_1Ҫ���յ��ֽ���
				SetUpTimer(itf, pitf->timeout);
			}
			//if(UPLINKITF_GPRS == itf)
				//printf("UplinkRecvPkt case 0\n");
			break;

		case 1:
			//printf("UplinkRecvPkt case 1\n");
			*(fsm->pbuf)++ = uc;
			fsm->cnt++;
			if(fsm->cnt >= fsm->maxlen) fsm->stat = 2;
			break;

		case 2:

			if(UPLINK_HEAD != uc) 
			{
				UplinkClearState(itf);
				break;
			}
			*(fsm->pbuf)++ = uc;
			
			//@change later: ���Ϲ�Լ�ж�
			pkt = (uplink_pkt_t *)UPLINK_RCVBUF(itf);
			len1 = ((unsigned short)pkt->len1[1]<<8) + (unsigned short)pkt->len1[0];
			len1 >>= 2;
			len2 = ((unsigned short)pkt->len2[1]<<8) + (unsigned short)pkt->len2[0];
			len2 >>= 2;
			if((len1 != len2) || (len1 > pitf->rcvmax) || (len1 < LEN_UPLINKHEAD)) {
				UplinkClearState(itf);
				break;
			}

			len2 -= LEN_UPLINKHEAD;
			UPLINKAPP_LEN(pkt) = len2;

			len1 += 1;
			fsm->cnt = 0;
			fsm->maxlen = len1;//stat_3Ҫ���յ��ֽ���

			len1 /= 200;
			len1 *= 10;
			len1 += pitf->timeout;//�������ݰ��ĳ��ȵõ���ʱʱ��
			SetUpTimer(itf, len1);
			fsm->stat = 3;
			break;

		case 3:
			//printf("UplinkRecvPkt case 0\n");
			*(fsm->pbuf)++ = uc;
			fsm->cnt++;
			if(fsm->cnt >= fsm->maxlen) fsm->stat = 4;
			break;

		case 4:
			if(UPLINK_TAIL != uc) 
			{
				UplinkClearState(itf);
				break;
			}

			*(fsm->pbuf)++ = uc;

			StopUpTimer(itf);
			pkt = (uplink_pkt_t *)UPLINK_RCVBUF(itf);
			if(UPLINK_NEEDPRINT(itf)) UplinkPrintPkt(pkt, "RECV", UPLINKAPP_LEN(pkt));

			if(CheckPacket(itf, pkt)) 
			{
				if(UPLINK_NEEDPRINT(itf)) PrintLog(LOGTYPE_SHORT, "recv chk err\n");
				UplinkClearState(itf);
				//printf("UplinkRecvPkt case 5\n");
			}
			else {
				UplinkClearState(itf);
				//printf("UplinkRecvPkt case 6\n");
				printf("UplinkRecvPkt ok\n");
				printf("itf = %d\n",itf);
				return 0;
			}
			break;

		default:
			UplinkClearState(itf);
			break;
		}
	}

	return 1;
}

/**
* @brief ����һ��ͨ��֡
* @param itf �ӿڱ��
* @param pkt ͨ��ָ֡��
* @return �ɹ�����0, ����ʧ��
*/
int UplinkSendPkt(unsigned char itf, uplink_pkt_t *pkt)
{
	int len;
	unsigned short lenapp;
	unsigned char seq;

	//printf("ParaMixSave.mix.bactcomm = %d",ParaMixSave.mix.bactcomm);

	if(pkt->afn == 0x02 && pkt->data[0] == 0x00 && pkt->data[1] == 0x00 && pkt->data[2] == 0x04 && pkt->data[3] == 0x00)	goto OUT;
	if(enable_comm_with_station_flag)
	{
		enable_comm_with_station_flag = 0;
		goto OUT;
	}
	if(ParaMixSave.mix.bactcomm)	return 1;

OUT:
	pkt->ctrl |= UPCTRL_DIR;
	pkt->area[0] = ParaUni.addr_area[0];
	pkt->area[1] = ParaUni.addr_area[1];
	pkt->sn[0] = ParaUni.addr_sn[0];
	pkt->sn[1] = ParaUni.addr_sn[1];

	lenapp = UPLINKAPP_LEN(pkt);
	seq = pkt->seq;
	len = MakePacket(itf, pkt);

	if(-1 == len) {
		UPLINKAPP_LEN(pkt) = lenapp;
		pkt->seq = seq;
		return 1;
	}

	if(UPLINK_NEEDPRINT(itf)) UplinkPrintPkt(pkt, "SEND", lenapp);

	if((*UplinkInterface[itf].rawsend)((unsigned char *)pkt, len)) {
		UPLINKAPP_LEN(pkt) = lenapp;
		pkt->seq = seq;
		return 1;
	}

	UPLINKAPP_LEN(pkt) = lenapp;
	pkt->seq = seq;
	return 0;
}

/**
* @brief ת��һ������ͨ��֡(��ģʽ)
* @param itf �ӿڱ��
* @param pkt ͨ��ָ֡��
* @return �ɹ�����0, ����ʧ��
*/
int CascadeForwardPkt(unsigned char itf, uplink_pkt_t *pkt)
{
	int len;

	len = UPLINKAPP_LEN(pkt) & 0xffff;
	AssertLogReturn(len > UPLINK_SNDMAX(itf), 1, "invalid len(%d)\n", len);
	len += MINLEN_UPLINKPKT;

	pkt->len1[0] = pkt->len2[0];
	pkt->len1[1] = pkt->len2[1];

	return(*UplinkInterface[itf].rawsend)((unsigned char *)pkt, len);
}

/**
* @brief ����һ������ͨ��֡(��ģʽ)
* @param itf �ӿڱ��
* @param pkt ͨ��ָ֡��
* @return �ɹ�����0, ����ʧ��
*/
int CascadeSvrSendPkt(unsigned char itf, uplink_pkt_t *pkt)
{
	int len;
	unsigned short lenapp;
	unsigned char seq;

	pkt->ctrl &= ~UPCTRL_DIR;

	lenapp = UPLINKAPP_LEN(pkt);
	seq = pkt->seq;
	len = MakePacket(itf, pkt);

	if(-1 == len) {
		UPLINKAPP_LEN(pkt) = lenapp;
		pkt->seq = seq;
		return 1;
	}

	if(UPLINK_NEEDPRINT(itf)) UplinkPrintPkt(pkt, "CASCADE SEND", lenapp);

	if((*UplinkInterface[itf].rawsend)((unsigned char *)pkt, len)) {
		UPLINKAPP_LEN(pkt) = lenapp;
		pkt->seq = seq;
		return 1;
	}

	UPLINKAPP_LEN(pkt) = lenapp;
	pkt->seq = seq;
	return 0;
}


/**
* @brief ��ȡ��ʱʱ����ط�����
* @param ptime ��ʱʱ��ָ��(100ms)
* @param retry �ط�����ָ��
*/
static void inline GetTimeout(int *ptime, int *retry)
{
	int i;

	i = (int)(ParaTerm.tcom.rsnd)&0x0fff;
	if(0 == i) i = 30;
	*ptime = i*10;

	i = (int)(ParaTerm.tcom.rsnd)&0x3000;
	i >>= 12;
	*retry = i+1;
}

/**
* @brief �ж��Ƿ��Ӧ����
* @pkt ͨ��ָ֡��
* @return �ǻ�Ӧ���ķ���1, ���򷵻�0
*/
static int IsEchoPkt(uplink_pkt_t *pkt)
{
	unsigned char ctrl;

	ctrl = pkt->ctrl;
	if(0 == (ctrl&UPCTRL_PRM)) return 1;
	else if(0 == pkt->afn) return 1;
	else if(CASCADE_CLIENT == CascadeMode) 
	{
		if(UPAFN_CASCADE == pkt->afn) return 1;
	}

	return 0;
}

/**
* @brief ���ͱ��Ĳ��ȴ���Ӧ
* @param itf �ӿڱ��
* @param pkt ͨ��ָ֡��
* @return �ɹ�����0,���򷵻ش������
*/
static int UplinkSendWait(unsigned char itf, uplink_pkt_t *psnd)
{
	//uplinkitf_t *pitf = (uplinkitf_t *)&UplinkInterface[itf];
	int i, j;
	int times, retry;
	uplink_pkt_t *prcv;

	//printf("UplinkSendWait1\n");
	psnd->seq |= UPSEQ_CON;

	prcv = (uplink_pkt_t *)UPLINK_RCVBUF(itf);

	//if(!UplinkRecvPkt(itf)) 
	//{
		//if(!IsEchoPkt(prcv)) return UPRTN_FAILRCV;
	//}

	GetTimeout(&times, &retry);
	//printf("times = %d retry = %d\n",times,retry);
	//times = 1000;
	times = 50;
	if(CASCADE_CLIENT == CascadeMode) retry = 1;

	for(i=0; i<retry; i++) 
	{
		//if(!(*pitf->linestat)()) return UPRTN_FAIL;

		if(UplinkSendPkt(itf, psnd)) return UPRTN_FAIL;

		for(j=0; j<times; j++) 
		{
			if(!UplinkRecvPkt(itf)) 
			{
				if(!IsEchoPkt(prcv)) return UPRTN_OKRCV;
				else return UPRTN_OK;
			}

			Sleep(10);
		}
	}
	printf("UplinkSendWait2\n");
	return UPRTN_TIMEOUT;
}

/**
* @brief ��ʱһ��ʱ��(���ݽӿں�֡����)
* @param itf �ӿڱ��
* @param pkt ����ָ֡��
* @return �ɹ�����0,���򷵻ش������
*/
int UplinkDelay(unsigned char itf, const uplink_pkt_t *pkt)
{
	uplinkitf_t *pitf = (uplinkitf_t *)&UplinkInterface[itf];
	int i, timeout;

	timeout = (int)UPLINKAPP_LEN(pkt)&0xffff;
	if(timeout > UPLINK_SNDMAX(itf)) return UPRTN_FAIL;
	timeout += LEN_UPLINKHEAD;

	timeout >>= 7;
	timeout *= UPLINK_TIMEOUT(itf);
	timeout += UPLINK_TIMEOUT(itf)*3;
	timeout /= 10;

	for(i=0; i<timeout; i++) {
		if(!(*pitf->linestat)()) return UPRTN_FAIL;

		if(!UplinkRecvPkt(itf)) {
			pkt = (uplink_pkt_t *)UPLINK_RCVBUF(itf);
			if(pkt->ctrl & UPCTRL_PRM) {
				if(IsSvrMsgAfn(pkt->afn)) return UPRTN_OKRCV;
			}
		}

		Sleep(10);
	}

	return UPRTN_OK;
}

/**
* @brief ������������
* @param itf �ӿڱ��
* @param flag ���ͱ�־, 0-���ȴ���Ӧ, 1-�ȴ���Ӧ
* @return �ɹ�����0,���򷵻ش������
*/
int UplinkActiveSend(unsigned char itf, unsigned char flag, uplink_pkt_t *psnd)
{
	static unsigned char fseq = 0;

	psnd->seq = fseq&0x0f;
	fseq++;
	psnd->seq |= UPSEQ_SPKT;
	psnd->ctrl |= UPCTRL_PRM;
	psnd->grp = 0;


	//if((!flag) || (UPLINK_ATTR(itf)&UPLINKATTR_NOECHO))
		//return(UplinkSendPkt(itf, psnd));
	//else {
		psnd->seq |= UPSEQ_CON;
		return(UplinkSendWait(itf, psnd));
	//}
}

/**
* @brief ��½��������
* @param itf �ӿڱ��
* @return �ɹ�����0,����ʧ��
*/
int UplinkLogon(unsigned char itf)
{
	uplink_pkt_t *pkt = (uplink_pkt_t *)UPLINK_SNDBUF(itf);
	uplink_duid_t *pduid;

	printf("UplinkLogon1...");
	pduid = (uplink_duid_t *)pkt->data;
	pkt->ctrl = UPCMD_LINKTEST|UPCTRL_PRM;
	pkt->afn = UPAFN_LINKTEST;
	pduid->da[0] = pduid->da[1] = 0;   //p0
	pduid->dt[0] = 1;    //F1
	pduid->dt[1] = 0;
	UPLINKAPP_LEN(pkt) = 4;

	if(UplinkActiveSend(itf, 1, pkt)) goto MARK_FLOGFAIL;
	printf("UplinkLogon2...");
	UplinkPrintPkt(pkt, "RECV", UPLINKAPP_LEN(pkt));
	pkt = (uplink_pkt_t *)UPLINK_RCVBUF(itf);
	pduid = (uplink_duid_t *)pkt->data;
	if((0 != pduid->dt[1]) || (2 == pduid->dt[0])) goto MARK_FLOGFAIL;
	printf("UplinkLogon3...");
	PrintLog(LOGTYPE_SHORT, "logon ok.\r\n");
	printf("logon ok...");
	SvrSendFileInfo(itf);
	return 0;

MARK_FLOGFAIL:
	PrintLog(LOGTYPE_SHORT, "logon fail.\r\n");
	printf("logon fail...");
	return 1;
}

/**
* @brief ��·���
* @param itf �ӿڱ��
* @return �ɹ�����0,���򷵻ش������
*/
int UplinkLinkTest(unsigned char itf)
{
	uplink_pkt_t *pkt = (uplink_pkt_t *)UPLINK_SNDBUF(itf);
	uplink_duid_t *pduid;
	int rtn;

	pduid = (uplink_duid_t *)pkt->data;
	pkt->ctrl = UPCMD_LINKTEST|UPCTRL_PRM;
	pkt->grp = 0;
	pkt->afn = UPAFN_LINKTEST;
	pduid->da[0] = pduid->da[1] = 0;   //p0
	pduid->dt[0] = 0x04;    //F3
	pduid->dt[1] = 0;
	UPLINKAPP_LEN(pkt) = 4;

	//@change later;
	/*if(stat_acd)
	{
		unsigned char *paux;

		paux = pkt->data;
		paux += UPLINKAPP_LEN(pkt);

		pkt->ctrl &= 0xef;
		pkt->ctrl |= 0x20;
		get_alarmec(paux);
		UPLINKAPP_LEN(pkt) += 2;
	}*/

	rtn = UplinkActiveSend(itf, 1, pkt);
	if(UPRTN_FAIL == rtn) {
		PrintLog(LOGTYPE_SHORT, "link test fail\r\n");
	}
	else if(UPRTN_OK == rtn) {
		PrintLog(LOGTYPE_SHORT, "link test ok.\r\n");
	}

	return(rtn);
}

