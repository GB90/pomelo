/**
* operation.c -- ����ģ���ʼ��
* 
* 
* ����ʱ��: 2010-5-8
* ����޸�ʱ��: 2010-5-8
*/

#include <stdio.h>

#include "include/debug.h"
#include "operation_inner.h"

extern int ParamSaveInit(void);
extern void MappingCenMetp(void);

/**
* @brief ����ģ���ʼ��
* @return 0�ɹ�, ����ʧ��
*/
DECLARE_INIT_FUNC(ParamInit);
int ParamInit(void)
{
	printf("load param ...\n");

	LoadParaUni();
	LoadParaTerm();
	SetParaNetAddr();
	LoadParaMeter();
	LoadParaCenMetp();
	LoadParaPlcMetp();
	LoadParaChildEnd();
	LoadParaMix();
	LoadParaDataUse();
	LoadParaTask();
	LoadParaHardw();
	LoadParaRoute();
	MappingCenMetp();
	
	if(ParamSaveInit()) return 1;

	SET_INIT_FLAG(ParamInit);

	return 0;
}

