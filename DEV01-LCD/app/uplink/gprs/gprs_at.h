/**
* gprs_at.h -- GPRSģ��AT�����
* 
* 
* ����ʱ��: 2010-5-19
* ����޸�ʱ��: 2010-5-19
*/

#ifndef _GPRS_AT_H
#define _GPRS_AT_H

extern unsigned long gprs_atclr;
#define GPRSAT_CLRRCV    while(GprsLineRecv((unsigned char *)&(gprs_atclr), 4) > 0)

typedef struct {
	const char *cmd;
	int keynum;
	const char *key[4];
} atlist_t;

int GprsSendAt(const atlist_t *list, int timeout);

#endif /*_GPRS_AT_H*/

