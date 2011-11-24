/**
* ir.c -- ���������ͨ��
* 
* 
* ����ʱ��: 2010-6-12
* ����޸�ʱ��: 2010-6-12
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/debug.h"
#include "include/sys/task.h"
#include "include/sys/schedule.h"
#include "include/sys/uart.h"
#include "include/uplink/svrnote.h"
#include "uplink_pkt.h"
#include "uplink_dl.h"
#include "svrcomm.h"

#define IR_UART		0

/**
* @brief ����ͨ������(��������)
*/
void IrActiveTask(void)
{
	unsigned long ev;

	UplinkClearState(UPLINKITF_IR);

	while(1) 
	{
		SvrCommPeekEvent(SVREV_NOTE, &ev);

		if(ev&SVREV_NOTE) 
		{
			SvrNoteProc(UPLINKITF_IR);
		}

		if(!UplinkRecvPkt(UPLINKITF_IR)) 
		{
			SvrMessageProc(UPLINKITF_IR);
		}

		Sleep(10);
	}

	return;
}

/**
* @brief ����ͨ������(����������)
*/
static void *IrPassiveTask(void *arg)
{
	UplinkClearState(UPLINKITF_IR);

	Sleep(100);

	while(1) 
	{
		if(!UplinkRecvPkt(UPLINKITF_IR)) {
			SvrMessageProc(UPLINKITF_IR);
		}

		Sleep(10);
	}

	return 0;
}

/**
* @brief ��������ͨ������
* @param mode 0����������, 1��������
* @return �ɹ�����0, ʧ�ܷ���1
*/
DECLARE_INIT_FUNC(UplinkIrStart);
int UplinkIrStart(int mode)
{
	if(UartOpen(IR_UART)) 
	{
		printf("can not open uart %d\n", IR_UART);
		return 1;
	}

	UartSet(IR_UART, 1200, 8, 1, 'e');

	if(mode) 
	{
		SysCreateTask(IrPassiveTask, NULL);
		printf("ir passive task started\n");
	}

	SET_INIT_FLAG(UplinkIrStart);

	return 0;
}

/**
* @brief �Ӻ����ͨ�Žӿڶ�ȡһ���ֽ�
* @param buf �����ַ�ָ��
* @return �ɹ�0, ����ʧ��
*/
int IrGetChar(unsigned char *buf)
{
	if(UartRecv(IR_UART, buf, 1) > 0) return 0;
	else return 1;
}

/**
* @brief ������ͨ�Žӿڷ�������
* @param buf ���ͻ�����ָ��
* @param len ����������
* @return �ɹ�0, ����ʧ��
*/
int IrRawSend(const unsigned char *buf, int len)
{
	return(UartSend(IR_UART, buf, len));
}

