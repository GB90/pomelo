/**
* plstatic.h -- ��̬·���ز�ͨ�Žӿ�
* 
* 
* ����ʱ��: 2010-5-21
* ����޸�ʱ��: 2010-5-21
*/

#ifndef _PLSTATIC_H
#define _PLSTATIC_H

int PlStaticRead(const plc_dest_t *dest, unsigned short itemid, unsigned char *buf, int len);
int PlStaticWrite(const plc_dest_t *dest, const plwrite_config_t *pconfig);
int PlStaticCheckTime(const plc_dest_t *dest, int flag);

#endif /*_PLSTATIC_H*/

