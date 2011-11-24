/**
* plvmet.c -- �����ز���
* 
* 
* ����ʱ��: 2010-7-2
* ����޸�ʱ��: 2010-7-2
*/

#ifndef _PLVMET_H
#define _PLVMET_H

#define USE_PLVMET

#ifdef USE_PLVMET
int PlVmetRead(const plc_dest_t *dest, unsigned short itemid, unsigned char *buf, int len);
int PlVmetWrite(const plc_dest_t *dest, const plwrite_config_t *pconfig);
int PlVmetCheckTime(const plc_dest_t *dest, int flag);
#endif

#endif /*_PLVMET_H*/
