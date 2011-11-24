/**
* serial.c -- ��������ͨ��
* 
* 
* ����ʱ��: 2010-6-8
* ����޸�ʱ��: 2010-6-8
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

#define SERIAL_UART		4

static int SerialStarted = 0;

/**
* @brief ����ͨ������(��������)
*/
void SerialActiveTask(void)
{
	unsigned long ev;

	if(!SerialStarted) 
	{
		ErrorLog("Serial not start, return\n");
		return;
	}

	UplinkClearState(UPLINKITF_SERIAL);

	while(1) 
	{
		SvrCommPeekEvent(SVREV_NOTE, &ev);//�ȴ���������

		if(ev&SVREV_NOTE) //�������������
		{
			SvrNoteProc(UPLINKITF_SERIAL);
		}

		if(!UplinkRecvPkt(UPLINKITF_SERIAL)) //��������
		{
			SvrMessageProc(UPLINKITF_SERIAL);
		}

		Sleep(10);
	}

	return;
}

/**
* @brief ����ͨ������(����������)
*/
static void *SerialPassiveTask(void *arg)
{
	if(!SerialStarted) {
		ErrorLog("Serial not start, return\n");
		return 0;
	}

	UplinkClearState(UPLINKITF_SERIAL);

	Sleep(100);
	printf("serial passive task started\n");

	while(1) 
	{
		if(!UplinkRecvPkt(UPLINKITF_SERIAL)) 
		{
			SvrMessageProc(UPLINKITF_SERIAL);
		}

		Sleep(10);
	}


	#if 0
	unsigned char buf[1024];
	int i;
	while(1) {
		i = UartRecv(SERIAL_UART, buf, 1024);
		if(i > 0) {
			printf("recv %d bytes: ", i);
			if(i >= 4) printf("%02X %02X -- %02X %02X\r\n", buf[0], buf[1], buf[i-2], buf[i-1]);
			else if(i >= 2) printf("%02X %02X\r\n", buf[0], buf[1]);
			else printf("%02X\r\n", buf[0]);
		}

		Sleep(200);
	}
	#endif
	return 0;
}

/**
* @brief ��������ͨ������
* @param mode 0����������, 1��������
* @param baud ���ڲ�����
* @return �ɹ�����0, ʧ�ܷ���1
*/
int UplinkSerialStart(int mode, unsigned int baud)
{
	if(UartOpen(SERIAL_UART)) 
	{
		printf("can not open uart %d\n", SERIAL_UART);
		return 1;
	}
	else
	{
		printf("open uart %d succ\n", SERIAL_UART);
	}

	//UartSet(SERIAL_UART, baud, 8, 1, 'n');
	UartSet(SERIAL_UART, baud, 8, 1, 'E');
	
	//system("stty raw -echo");
	SerialStarted = 1;//���ڴ򿪱�־
	if(mode) 
	{
		SysCreateTask(SerialPassiveTask, NULL);
	}

	//SerialStarted = 1;//���ڴ򿪱�־

	return 0;
}

/**
* @brief �Ӵ���ͨ�Žӿڶ�ȡһ���ֽ�
* @param buf �����ַ�ָ��
* @return �ɹ�0, ����ʧ��
*/
int SerialGetChar(unsigned char *buf)
{
	if(UartRecv(SERIAL_UART, buf, 1) > 0) return 0;
	else return 1;
}

/**
* @brief �򴮿�ͨ�Žӿڷ�������
* @param buf ���ͻ�����ָ��
* @param len ����������
* @return �ɹ�0, ����ʧ��
*/
int SerialRawSend(const unsigned char *buf, int len)
{
	return(UartSend(SERIAL_UART, buf, len));
}


