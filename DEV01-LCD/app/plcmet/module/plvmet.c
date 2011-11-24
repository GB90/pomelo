/**
* plvmet.c -- �����ز���
* 
* 
* ����ʱ��: 2010-7-2
* ����޸�ʱ��: 2010-7-2
*/

#include <stdio.h>
#include <string.h>

#include "include/debug.h"
#include "include/sys/schedule.h"
#include "include/sys/timeal.h"
#include "include/lib/bcd.h"
#include "plcmet/plcomm.h"
#include "plcmet/plc_stat.h"
#include "plvmet.h"

#ifdef USE_PLVMET

/**
* @brief ������
* @param dest Ŀ�ĵ�ַ
* @param itemid �������ʶ
* @param buf ����֡������ָ��
* @param len ����������
* @return �ɹ�����ʵ�����ݳ���, ʧ�ܷ���-1
*/
int PlVmetRead(const plc_dest_t *dest, unsigned short itemid, unsigned char *buf, int len)
{
	unsigned short metid;
	unsigned int ene;

	AssertLog(dest->metid > MAX_METER, "invalid metid(%d)\n", dest->metid);

	Sleep(20);

	ene = dest->dest[1] & 0xff;
	ene <<= 8;
	ene += dest->dest[0] & 0xff;
	if(ene > 9999) ene -= 9999;

	UnsignedToBcd(ene, buf, 2);
	buf[2] = 0x05;
	buf[3] = 0;

	if(!dest->metid) return 4;

	metid = dest->metid - 1;
	PlcState[metid].quality = 0;

	SysClockReadCurrent(&PlcState[metid].oktime);
	PlcState[metid].okflag = 1;

	return 4;
}

/**
* @brief д����
* @param dest Ŀ�ĵ�ַ
* @param pconfig д����
* @return �ɹ�����0, ����ʧ��
*/
int PlVmetWrite(const plc_dest_t *dest, const plwrite_config_t *pconfig)
{
	return 0;
}

/**
* @brief �㲥��ʱ
* @param flag 0-���ͽ�ʱ����, 1-��ѯ��ʱ��ʽ
* @return flag=0ʱ �ɹ�����0, ʧ�ܷ��ظ���, flag=1ʱ���ؽ�ʱ��ʽ(PLCHKTIME_POLL)
*/
int PlVmetCheckTime(const plc_dest_t *dest, int flag)
{
	if(flag) return PLCHKTIME_POLL;

	return 0;
}


#endif /*USE_PLVMET*/

