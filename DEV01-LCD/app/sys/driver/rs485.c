/**
* rs485.c -- RS485�����ӿ�
* 
* 
* ����ʱ��: 2010-4-23
* ����޸�ʱ��: 2010-5-9
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "include/debug.h"
#include "include/debug/statistic.h"
#include "include/sys/uart.h"
#include "include/sys/rs485.h"
#include "include/sys/gpio.h"
#include "include/sys/mutex.h"

#define XR2751_PORTNUM		2

static int Xr2751Fid[XR2751_PORTNUM] = {-1, -1};

static int Rs485Port[5] = {0, 1, 1,1,1};

static sys_mutex_t Rs485Mutex[XR2751_PORTNUM];

typedef struct {
	unsigned int baudrate;
	unsigned char databits;
	unsigned char stopbits;
	char parity;
} xr2751_uart_t;

#define RS485_RCVBUF_SIZE		1024
struct rs485_rcvst {
	unsigned char buf[RS485_RCVBUF_SIZE];
	int len;
	int head;
};
static struct rs485_rcvst Rs485RecvBuffer[XR2751_PORTNUM];

DECLARE_STATISTIC(STATISGROUP_SYS, rs4850_sendbytes, 0);
DECLARE_STATISTIC(STATISGROUP_SYS, rs4850_recvbytes, 0);
DECLARE_STATISTIC(STATISGROUP_SYS, rs4851_sendbytes, 0);
DECLARE_STATISTIC(STATISGROUP_SYS, rs4851_recvbytes, 0);

/**
* @brief ��ʼ��RS485�˿�
* @return 0�ɹ�, ����ʧ��
*/
DECLARE_INIT_FUNC(Rs485Init);
int Rs485Init(void)
{
	int port, failed, fd;
	char dev[16];
	xr2751_uart_t uart;

	failed = 0;

	GpioSetDirect(GPIO_485_SEL1, 1);
	GpioSetValue(GPIO_485_SEL1, 0);
	GpioSetDirect(GPIO_485_SEL0, 1);
	GpioSetValue(GPIO_485_SEL0, 0);

	/*if(UartOpen(Rs485InternalPort)) {
		ErrorLog("can not open rs485 int port\n");
		failed = 1;
	}
	else {
		UartSet(Rs485InternalPort, 1200, 8, 1, 'E');
	}
	GpioSetUartMode(Rs485InternalPort,1);*/

	for(port=0; port<XR2751_PORTNUM; port++) {
		SysInitMutex(&Rs485Mutex[port]);

		sprintf(dev, "/dev/xr2751/%d", port);
		fd = open(dev, O_RDWR);
		if(-1 == fd) {
			printf("can not open xr2751 %d\n", port);
			failed = 1;
		}
		else {
			Xr2751Fid[port] = fd;

			uart.baudrate = 1200;
			uart.databits = 8;
			uart.stopbits = 1;
			uart.parity = 'E';
			ioctl(fd, 1, &uart);
		}
	}

	SET_INIT_FLAG(Rs485Init);

	return failed;
}

/**
* @brief ��סRs485�˿�
* @param port �˿ں�
*/
void Rs485Lock(unsigned int port)
{
	unsigned int porti;

	AssertLogReturnVoid(port>=5, "invalid port(%d)\n", port);

	porti = Rs485Port[port];

	SysLockMutex(&Rs485Mutex[porti]);
/*
	if(1 == porti) {
		if(1 == port) {
			GpioSetValue(GPIO_485_SEL1, 0);
			GpioSetValue(GPIO_485_SEL0, 1);
		}
		else {
			GpioSetValue(GPIO_485_SEL0, 0);
			GpioSetValue(GPIO_485_SEL1, 1);
		}
	}
*/
}

/**
* @brief ����Rs485�˿�
* @param port �˿ں�
*/
void Rs485Unlock(unsigned int port)
{
	AssertLogReturnVoid(port>=5, "invalid port(%d)\n", port);

	port = Rs485Port[port];

	SysUnlockMutex(&Rs485Mutex[port]);
}

/**
* @brief ��λRS485�˿�
* @param port RS485�˿ں�
* @return 0�ɹ�, ����ʧ��
*/
int Rs485Reset(unsigned int port)
{
	AssertLogReturn(port>=5, 1, "invalid port(%d)\n", port);

	port = Rs485Port[port];

	AssertLogReturn(-1 == Xr2751Fid[port], 1, "invalid fid(%d)\n", Xr2751Fid[port]);

	ioctl(Xr2751Fid[port], 10, 0);

	return 0;
}

/**
* @brief ����RS485�˿ڲ����ʺ�����
* @param port �˿ں�
* @param baud ������
* @param databits ����λ, 5~8
* @param stopbits ֹͣλ, 1~2
* @param parity У��λ
*/
void Rs485Set(unsigned int port, int baud, int databits, int stopbits, char parity)
{
	static xr2751_uart_t old_uart[XR2751_PORTNUM] = {
		{1200, 8, 1, 'E'},
		{1200, 8, 1, 'E'}
	};

	xr2751_uart_t uart;

	if(parity >= 'a' && parity <= 'z') parity = parity - 'a' + 'A';

	//printf("\n\nset rs485 %d baud=%d, %d,%d,%c\n\n", port, baud, databits, stopbits, parity);

	AssertLogReturnVoid(port>=5, "invalid port(%d)\n", port);
	port = Rs485Port[port];
	AssertLogReturnVoid(-1 == Xr2751Fid[port], "invalid fid(%d)\n", Xr2751Fid[port]);

	uart.baudrate = baud;
	uart.databits = databits;
	uart.stopbits = stopbits;
	uart.parity = parity;

	if(uart.baudrate != old_uart[port].baudrate || uart.databits != old_uart[port].databits ||
		uart.stopbits != old_uart[port].stopbits || uart.parity != old_uart[port].parity) {
		old_uart[port].baudrate = uart.baudrate;
		old_uart[port].databits = uart.databits;
		old_uart[port].stopbits = uart.stopbits;
		old_uart[port].parity = uart.parity;

		ioctl(Xr2751Fid[port], 1, &uart);
	}
	//printf("set rs485 rtn=%d\n", rtn);
}

/**
* @brief ��RS485�˿ڷ�������
* @param port �˿ں�
* @param buf ���ͻ�����
* @param len ����������
* @param 0�ɹ�, ����ʧ��
*/
int Rs485Send(unsigned int port, const unsigned char *buf, int len)
{
	AssertLogReturn(port>=5, 1, "invalid port(%d)\n", port);
	AssertLog(len<=0, "invalid len(%d)\n", len);
	port = Rs485Port[port];

	AssertLogReturn(-1 == Xr2751Fid[port], 1, "invalid fid(%d)\n", Xr2751Fid[port]);

	if(0 == port) ADD_STATISTIC(rs4850_sendbytes, len);
	else ADD_STATISTIC(rs4851_sendbytes, len);

	write(Xr2751Fid[port], (char *)buf, len);
	return 0;
}

/**
* @brief ��RS485�˿ڽ�������
* @param port �˿ں�
* @param buf ���ջ�����
* @param len ����������
* @return ʧ�ܷ���-1, �ɹ����ؽ��յ����ֽ���, ����0��ʾδ���յ�����
*/
int Rs485Recv(unsigned int port, unsigned char *buf, int len)
{
	int rtn, i;
	unsigned char *precv;

	AssertLogReturn(port>=5, -1, "invalid port(%d)\n", port);
	AssertLog(len<=0, "invalid len(%d)\n", len);
	port = Rs485Port[port];
	AssertLogReturn(-1 == Xr2751Fid[port], -1, "invalid fid(%d)\n", Xr2751Fid[port]);

	if(Rs485RecvBuffer[port].len <= 0) {
		Rs485RecvBuffer[port].len = 0;
		Rs485RecvBuffer[port].head = 0;
		rtn = read(Xr2751Fid[port], Rs485RecvBuffer[port].buf, RS485_RCVBUF_SIZE);
		if((rtn <=0) || (rtn > RS485_RCVBUF_SIZE)) return 0;
		Rs485RecvBuffer[port].len = rtn;

		if(0 == port) ADD_STATISTIC(rs4850_recvbytes, rtn);
		else ADD_STATISTIC(rs4851_recvbytes, rtn);
	}

	if(Rs485RecvBuffer[port].len > len) rtn = len;
	else rtn = Rs485RecvBuffer[port].len;

	precv =  &Rs485RecvBuffer[port].buf[Rs485RecvBuffer[port].head];
	for(i=0; i<rtn; i++) *buf++ = *precv++;
	//memcpy(buf, &Rs485RecvBuffer[port].buf[Rs485RecvBuffer[port].head], rtn);
	Rs485RecvBuffer[port].len -= rtn;
	Rs485RecvBuffer[port].head += rtn;

	return rtn;
}
