/**
* cascade.h -- ����ͨ��
* 
* 
* ����ʱ��: 2010-6-14
* ����޸�ʱ��: 2010-6-14
*/

#ifndef _UPLINK_CASCADE_H
#define _UPLINK_CASCADE_H

#define CASCADE_NONE		0
#define CASCADE_SERVER		1
#define CASCADE_CLIENT		2

#ifndef DEFINE_CASCADE
extern const int CascadeMode;
#endif

extern int CascadeNoted;

int CascadeInit(void);
void CascadeClientTask(void);

#endif /*_UPLINK_CASCADE_H*/

