/**
* pldynamic_dl.h -- ��̬̬·���ز��ײ�ͨ�Žӿ�
* 
* 
* ����ʱ��: 2010-8-25
* ����޸�ʱ��: 2010-8-25
*/

#ifndef _PLDYNAMIC_DL_H
#define _PLDYNAMIC_DL_H

const plc_sinfo_t *GetPlDynamicInfo(void);
int PlDynamicSendPkt(const plc_dest_t *dest, const unsigned char *buf, int len, unsigned char proto);
int PlDynamicRecvPkt(const plc_dest_t *dest, unsigned char *buf, int len, int timeout);

#endif /*_PLDYNAMIC_DL_H*/

