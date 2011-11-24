/**
* syscall_init.c -- ϵͳ����ģ���ʼ��
* 
* 
* ����ʱ��: 2008-3-31
* ����޸�ʱ��: 2010-5-6
*/

#include <stdio.h>

#include "include/debug.h"

extern int SysCallInit(void);
extern int DriverInit(void);
extern int DriverHighInit(void);
extern int StorageInit(void);

/**
* @brief ϵͳ����ģ���ʼ������
* @return ����0��ʾ�ɹ�, ����ʧ��
*/
DECLARE_INIT_FUNC(SysInit);
int SysInit(void)
{
	printf("System init...\n");
	if(DriverInit()) return 1;
	if(SysCallInit()) return 1;
	//if(DriverHighInit()) return 1;
	if(StorageInit()) return 1;

	SET_INIT_FLAG(SysInit);

	return 0;
}

