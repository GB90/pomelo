/**
* mdb.c -- ���ģ���ʼ��
* 
* 
* ����ʱ��: 2010-5-15
* ����޸�ʱ��: 2010-5-15
*/

#include <stdio.h>

#include "include/debug.h"
#include "ad_calib.h"

extern int AlarmInit(void);
extern int RunStateInit(void);
extern int MonitorTaskInit(void);

/**
* @brief ���ģ���ʼ��
* @return �ɹ�0, ����ʧ��
*/
DECLARE_INIT_FUNC(MonitorInit);
int MonitorInit(void)
{
	#if 1
	printf("monitor init...\n");

	//RunStateInit();
	AlarmInit();

	//LoadAdCalib();

	MonitorTaskInit();

	SET_INIT_FLAG(MonitorInit);
	#endif
	return 0;
}
