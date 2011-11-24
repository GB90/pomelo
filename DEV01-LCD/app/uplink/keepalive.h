/**
* keepalive.h -- �����������������
* 
* 
* ����ʱ��: 2010-5-19
* ����޸�ʱ��: 2010-5-19
*/

#ifndef _SVRCOMM_KEEPALIVE_H
#define _SVRCOMM_KEEPALIVE_H

#define KEEPALIVE_FLAG_LOGONFAIL    0
#define KEEPALIVE_FLAG_LOGONOK    1

int KeepAliveInPeriod(void);
void ClearKeepAlive(void);
int KeepAliveProc(void);
int RefreshKeepAlive(void);
void SetKeepAlive(unsigned char flag);

#endif /*_SVRCOMM_KEEPALIVE_H*/

