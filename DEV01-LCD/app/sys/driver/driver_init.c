/**
* driver_init.c -- ������ʼ������
* 
* 
* ����ʱ��: 2010-4-24
* ����޸�ʱ��: 2010-5-6
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/debug.h"
#include "include/sys/schedule.h"

extern int GpioInit(void);
extern int Rs485Init(void);
extern int SiginInit(void);
extern int AdcInit(void);

/**
* @brief �����ӿ���ģ���ʼ������
* @return 0�ɹ�, ����ʧ��
*/
DECLARE_INIT_FUNC(DriverInit);
int DriverInit(void)
{
	printf("  Driver init...\n");

	system("rmmod /conf/net/ohci-hcd.ko");
	Sleep(10);
	system("insmod /conf/net/ohci-hcd.ko");
	if(GpioInit()) return 1;
	if(SiginInit()) return 1;

	SET_INIT_FLAG(DriverInit);

	return 0;
}

/**
* @brief �����ӿ���ģ���ʼ������(�߼�����)
* @return 0�ɹ�, ����ʧ��
*/
DECLARE_INIT_FUNC(DriverHighInit);
int DriverHighInit(void)
{
	printf("  Driver high init...\n");
	if(Rs485Init()) return 1;
	//if(AdcInit()) return 1;

	SET_INIT_FLAG(DriverHighInit);
	return 0;
}
