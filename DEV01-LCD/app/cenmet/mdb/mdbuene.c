/**
* mdbuene.c -- �õ���
* 
* 
* ����ʱ��: 2010-5-13
* ����޸�ʱ��: 2010-5-13
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFINE_MDBUENE

#include "include/debug.h"
#include "include/param/capconf.h"
#include "include/param/metp.h"
#include "include/param/meter.h"
#include "include/lib/bcd.h"
#include "mdbconf.h"
#include "mdbcur.h"
#include "mdbana.h"
#include "mdbuene.h"

mdbuene_imm_t MdbUseEneImm[MAX_CENMETP];
mdbuene_t MdbUseEne[MAX_CENMETP];

/**
* @brief �õ������ݿ��ʼ��
*/
void MdbUseEneInit(void)
{
	int mid;

	memset(MdbUseEneImm, 0, sizeof(MdbUseEneImm));
	memset(MdbUseEne, 0, sizeof(MdbUseEne));

	for(mid=0; mid<MAX_CENMETP; mid++) {
		memset(MdbUseEne[mid].enepa_bak, 0xff, 16*(MAXNUM_FEEPRD+1));
	}
}

/**
* @brief ����õ���
* @brief flag 0-������õ���, 1-������õ���
*/
void ResetMdbUseEne(int flag)
{
	unsigned short mid;

	if(0 == flag) {
		for(mid=0; mid<MAX_CENMETP; mid++) {
			memset(MdbUseEne[mid].enepa_day, 0, 4*(MAXNUM_FEEPRD+1)*4);
		}
	}
	else {
		for(mid=0; mid<MAX_CENMETP; mid++) {
			memset(MdbUseEne[mid].enepa_mon, 0, 4*(MAXNUM_FEEPRD+1)*4);
		}
	}
}

#define UPDATE_USEENE(bak, cur, use)    { \
		if((0xffffffff == bak) || (cur < bak)) { \
			bak = cur; use = 0; } \
		else { \
			use = cur - bak; bak = cur; }}

static int inline EneIsEmpty(const unsigned char *puc, int len)
{
	int i;

	for(i=0; i<len;i ++) {
		if(*puc++ != FLAG_MDBEMPTY) return 0;
	}

	return 1;
}

/**
* @brief �����õ������ݿ�
* @param mid ������� 0~
*/
static void UpdateMdbUseEneSingle(unsigned short mid)
{
	unsigned short i;
	unsigned int ul;
	const unsigned char *puc;
	unsigned int uene;
	mdbuene_t *p;

	p = &MdbUseEne[mid];

	puc = MdbCurrent[mid].enepa;
	for(i=0; i<(MAXNUM_FEEPRD+1); i++, puc+=5) {
		if(EneIsEmpty(puc, 5)) continue;

		ul = BcdToUnsigned(&puc[4], 1);
		ul *= 10000000;//��8��0����10
		uene = BcdToUnsigned(puc, 4);
		uene /= 10;
		ul += uene;

		UPDATE_USEENE(p->enepa_bak[i], ul, uene);

		p->enepa_day[i] += uene;
		p->enepa_mon[i] += uene;

		if(0 == i) {
			MdbUseEneImm[mid].enepa_1m = uene;
			MdbUseEneImm[mid].enepa_15m += uene;
		}
	}

	puc = MdbCurrent[mid].enepi;
	for(i=0; i<(MAXNUM_FEEPRD+1); i++,puc+=4) {
		if(EneIsEmpty(puc, 4)) continue;

		ul = BcdToUnsigned(puc, 4);
		UPDATE_USEENE(p->enepi_bak[i], ul, uene);

		p->enepi_day[i] += uene;
		p->enepi_mon[i] += uene;

		if(0 == i) {
			MdbUseEneImm[mid].enepi_1m = uene;
			MdbUseEneImm[mid].enepi_15m += uene;
		}

	}

	puc = MdbCurrent[mid].enena;
	for(i=0; i<(MAXNUM_FEEPRD+1); i++,puc+=5) {
		if(EneIsEmpty(puc, 5)) continue;

		ul = BcdToUnsigned(&puc[4], 1);
		ul *= 10000000;
		uene = BcdToUnsigned(puc, 4);
		uene /= 10;
		ul += uene;
		UPDATE_USEENE(p->enena_bak[i], ul, uene);

		p->enena_day[i] += uene;
		p->enena_mon[i] += uene;

		if(0 == i) {
			MdbUseEneImm[mid].enena_1m = uene;
			MdbUseEneImm[mid].enena_15m += uene;
		}
	}

	puc = MdbCurrent[mid].eneni;
	for(i=0; i<(MAXNUM_FEEPRD+1); i++,puc+=4) {
		if(EneIsEmpty(puc, 4)) continue;

		ul = BcdToUnsigned(puc, 4);
		UPDATE_USEENE(p->eneni_bak[i], ul, uene);

		p->eneni_day[i] += uene;
		p->eneni_mon[i] += uene;

		if(0 == i) {
			MdbUseEneImm[mid].eneni_1m = uene;
			MdbUseEneImm[mid].eneni_15m += uene;
		}
	}
}

/**
* @brief �����õ������ݿ�
*/
void UpdateMdbUseEne(void)
{
	unsigned short mid;

	//PrintLog(LOGTYPE_DOWNLINK, "UpdateMdbUseEne.........\n");
	for(mid=0; mid<MAX_CENMETP; mid++) {
		if(EMPTY_CENMETP(mid)) continue;

		UpdateMdbUseEneSingle(mid);
	}
}

/**
* @brief ����15�����õ���(ÿ15���ӵ���һ��)
* @param mid ������� 0~
*/
void UpdateMdbUseEne15Min(unsigned short mid)
{
	MdbUseEne[mid].uenepa += MdbUseEneImm[mid].enepa_15m;
	MdbUseEne[mid].uenepi += MdbUseEneImm[mid].enepi_15m;
	MdbUseEne[mid].uenena += MdbUseEneImm[mid].enena_15m;
	MdbUseEne[mid].ueneni += MdbUseEneImm[mid].eneni_15m;
}

/**
* @brief ���15�����õ���(ÿ15���ӵ���һ��)
* @param mid ������� 0~
*/
void ClearMdbUseEne15Min(unsigned short mid)
{
	MdbUseEneImm[mid].enepa_15m = 0;
	MdbUseEneImm[mid].enepi_15m = 0;
	MdbUseEneImm[mid].enena_15m = 0;
	MdbUseEneImm[mid].eneni_15m = 0;

	MdbUseEne[mid].uenepa = 0;
	MdbUseEne[mid].uenepi = 0;
	MdbUseEne[mid].uenena = 0;
	MdbUseEne[mid].ueneni = 0;
}

/**
* @brief ��ȡ��ǰ����õ�������
* @param metpid �������, 1~MAX_CENMETP
* @param itemid ��������
* @param buf ������ָ��
* @param len ����������
* @return �ɹ�����ʵ�ʶ�ȡ����, ʧ�ܷ��ظ���, �޴��������-2, �������������-1
*/
int ReadMdbUseEne(unsigned short metpid, unsigned short itemid, unsigned char *buf, int len)
{
	unsigned int multi, ul;
	int i;
	unsigned int *pul;

	if((metpid==0) ||(metpid > MAX_CENMETP)) return -2;
	metpid -= 1;

	switch(itemid) {
	case 0x0501: multi = 10; pul = MdbUseEne[metpid].enepa_day; break;
	case 0x0502: multi = 100; pul = MdbUseEne[metpid].enepi_day; break;
	case 0x0504: multi = 10; pul = MdbUseEne[metpid].enena_day; break;
	case 0x0508: multi = 100; pul = MdbUseEne[metpid].eneni_day; break;
	case 0x0510: multi = 10; pul = MdbUseEne[metpid].enepa_mon; break;
	case 0x0520: multi = 100; pul = MdbUseEne[metpid].enepi_mon; break;
	case 0x0540: multi = 10; pul = MdbUseEne[metpid].enena_mon; break;
	case 0x0580: multi = 100; pul = MdbUseEne[metpid].eneni_mon; break;
	default: return -2;
	}

	if(len < 21) return -1;

	*buf++ = MAXNUM_FEEPRD;

	for(i=0; i<(MAXNUM_FEEPRD+1); i++) {
		ul = pul[i];
		ul *= multi;
		UnsignedToBcd(ul, buf, 4);
		buf += 4;
	}

	return 21;
}

