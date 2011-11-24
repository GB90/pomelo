/**
* gpio.c -- GPIO�����ӿ�
* 
* 
* ����ʱ��: 2010-4-23
* ����޸�ʱ��: 2010-4-23
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>


#include "include/debug.h"
#include "include/sys/gpio.h"
#include "include/sys/schedule.h"
#include "include/lib/bcd.h"
#include "include/sys/uart.h"
typedef union {
	unsigned long ul;
	struct {
		unsigned char pin;
		unsigned char value;
		unsigned char plus[2];
	} ch;
} gpio_arg_t;

static int FidGpio = -1;

const unsigned char read_time_frame[] = {0x68, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x68, 0x11, 0x4, 0x33, 0x34, 0x33, 0x37, 0x4c, 0x16, 0xfe}; //��ʱ��
#define AMP_PORT	0x06

/**
* @brief GPIO�����ӿڳ�ʼ��
* @return 0�ɹ�, ����ʧ��
*/
DECLARE_INIT_FUNC(GpioInit);

void amp_port_init()
{
	UartOpen(AMP_PORT);//��Ӧ����0
	Sleep(10);
	UartSet(AMP_PORT, 38400, 8, 1, 'E');
}


int GpioInit(void)
{
	amp_port_init();
	//if(-1 == FidGpio) 
	//{
	//	printf("can not open gpio driver\n");
	//	return 1;
	//}

	SET_INIT_FLAG(GpioInit);

	return 0;
}

/**
* @brief ����GPIO�ܽŷ���
* @param io �ܽź�
* @param direct ����, 0����, 1���
*/
void GpioSetDirect(unsigned char io, int direct)
{
	gpio_arg_t arg;

	AssertLog(-1==FidGpio, "invalid fid(%d)\n", FidGpio);

	arg.ch.pin = io;
	arg.ch.value = direct;

	ioctl(FidGpio, 1, arg);
}

/**
* @brief ����GPIO�ܽ������˲�
* @param io �ܽź�
* @param ison 0�ر�, 1��
*/
void GpioSetDeglitch(unsigned char io, int ison)
{
	gpio_arg_t arg;

	AssertLog(-1==FidGpio, "invalid fid(%d)\n", FidGpio);

	arg.ch.pin = io;
	arg.ch.value = ison;

	ioctl(FidGpio, 2, arg);
}

/**
* @brief ����GPIO�ܽ������ƽ
* @param io �ܽź�
* @param value 0�͵�ƽ, 1�ߵ�ƽ
*/
void GpioSetValue(unsigned char io, int value)
{
	gpio_arg_t arg;

	AssertLog(-1==FidGpio, "invalid fid(%d)\n", FidGpio);

	arg.ch.pin = io;
	arg.ch.value = value;

	ioctl(FidGpio, 0, arg);
}

/**
* @brief ��ȡGPIO�ܽ������ƽ
* @param io �ܽź�
* @return 0�͵�ƽ, 1�ߵ�ƽ
*/
int GpioGetValue(unsigned char io)
{
	gpio_arg_t arg;
	unsigned int cmd;

	AssertLog(-1==FidGpio, "invalid fid(%d)\n", FidGpio);

	cmd = (unsigned int)io;
	cmd |= 0x100;

	ioctl(FidGpio, cmd, &arg);

	return(arg.ch.value);
}

/**
* @brief ����CPU�ڲ�UARTģʽ(����)
* @param port UART�˿ں�
* @param mode ģʽ,1ΪRS485ģʽ
*/
void GpioSetUartMode(int port, int mode)
{
	gpio_arg_t arg;

	if((port < 0) || (port >= 4)) {
		ErrorLog("invalid port(%d)\n", port);
		return;
	}

	AssertLog(-1==FidGpio, "invalid fid(%d)\n", FidGpio);

	arg.ch.pin = 0;
	arg.ch.value = port;
	arg.ch.plus[0] = mode;

	ioctl(FidGpio, 3, arg);
}





int recieve_read_time_ack(unsigned char *buf) 
{
	int timeout;  // ��ʱ����������
	char c;
	unsigned char *pdata;
	int len = 0;
	
	pdata = buf;
	timeout = 0;
	for(timeout=0;timeout<3;timeout++)
	{
		while(UartRecv(AMP_PORT,&c, 1))
		{
			*pdata++ = c;
			len++;
			if(len>30)
				break;
		}
		Sleep(10);
	}
	return len;
}



extern int check_recieve_read_imet_pak(unsigned char *buf,unsigned char len);

/*
int check_recieve_read_imet_pak(unsigned char *buf,unsigned char len) 
{
	//unsigned char *pbuf = buf;
	
	if(buf[0] != 0x68)	return 1;
	if(buf[7] != 0x68)	return 1;

	return 0;
}
*/
/**
* @brief ��ȡ�ⲿʱ��
* @param ʱ�ӱ���ָ��
* @return 0�ɹ�, ����ʧ��
*/
int ExtClockRead(extclock_t *clock)
{
	unsigned char rcv_buf_tmp[30];
	unsigned char i = 0;
	int rcv_len = 0;
	extclock_t *clock_tmp;
	
	clock_tmp = clock;
	memset(rcv_buf_tmp,0x00,30);
	
	UartSend(AMP_PORT,read_time_frame,strlen(read_time_frame));
	rcv_len = recieve_read_time_ack(rcv_buf_tmp); 
	if(rcv_len>0)
	{
		//printf("rcv_buf_tmp = ");
		//for(i=0;i<30;i++)
		//{
		//	printf("%02x ",rcv_buf_tmp[i]);
		//}
		//printf("\n");
		if(check_recieve_read_imet_pak(rcv_buf_tmp,rcv_len))		return 1;
		for(i=14;i<21;i++)
		{
			rcv_buf_tmp[i] -= 0x33;
			//printf("rcv_buf_tmp[%d] = %02x ",i,rcv_buf_tmp[i]);
		}
		BcdToHex(&rcv_buf_tmp[14],7);
		//printf("\n");
		clock_tmp->century = 20;
		clock_tmp->year = rcv_buf_tmp[20];
		clock_tmp->month = rcv_buf_tmp[19];
		clock_tmp->day = rcv_buf_tmp[18];
		clock_tmp->week = rcv_buf_tmp[17];
		clock_tmp->hour = rcv_buf_tmp[16];
		clock_tmp->minute = rcv_buf_tmp[15];
		clock_tmp->second = rcv_buf_tmp[14];
		return 0;
	}
	return 1;
}


#if 0
/**
* @brief �����ⲿʱ��
* @param ʱ�ӱ���ָ��
* @return 0�ɹ�, ����ʧ��
*/
int ExtClockWrite(const extclock_t *clock)
{
	unsigned char write_time_frame[30];
	unsigned char *pdata = write_time_frame;
	unsigned char check_sum = 0;
	unsigned char i = 0;
	unsigned char *pbuf;
	unsigned char rcv_buf_tmp[30];
	int rcv_len = 0;

	printf("ExtClockWrite1\n");
	memset(write_time_frame,0x00,sizeof(write_time_frame));
	rcv_len = 0;
	memset(rcv_buf_tmp,0x00,sizeof(rcv_buf_tmp));
	memcpy(pdata,read_time_frame,8);
	pdata += 8;
	//*pdata++ = 0x11;
	*pdata++ = 0x14;
	*pdata++ = 0x08;
	*pdata++ = 0x34;
	*pdata++ = 0x34;
	*pdata++ = 0x33;
	*pdata++ = 0x37;
	//*pdata++ = HexToBcd(&clock->day + 0x33),1);
	//*pdata++ = HexToBcd(&clock->month+ 0x33,1);
	//*pdata++ = HexToBcd(&clock->year+ 0x33,1);
	//*pdata++ = HexToBcd(&clock->week+ 0x33,1);

	*pdata++ = clock->day + 0x33;
	*pdata++ = clock->month+ 0x33;
	*pdata++ = clock->year+ 0x33;
	*pdata++ = clock->week+ 0x33;
	HexToBcd(pdata - 4,4);
	
	pbuf = write_time_frame;
	for(i=0;i<18;i++)
	{
		check_sum += *pbuf++;
	}
	*pdata++ = check_sum;
	*pdata++ = 0x16;
	UartSend(AMP_PORT,write_time_frame,20);
	rcv_len = recieve_read_time_ack(rcv_buf_tmp); 
	if(rcv_len>0)
	{
		printf("rcv_buf_tmp = ");
		for(i=0;i<30;i++)
		{
			printf("%02x ",rcv_buf_tmp[i]);
		}
		printf("\n");
		if(check_recieve_read_imet_pak(rcv_buf_tmp,rcv_len))		return 1;
	}
	printf("ExtClockWrite2\n");
	Sleep(1000);
	memset(write_time_frame,0x00,sizeof(write_time_frame));
	memcpy(pdata,read_time_frame,8);
	rcv_len = 0;
	memset(rcv_buf_tmp,0x00,sizeof(rcv_buf_tmp));
	pdata += 8;
	//*pdata++ = 0x11;
	*pdata++ = 0x14;
	*pdata++ = 0x07;
	*pdata++ = 0x35;
	*pdata++ = 0x34;
	*pdata++ = 0x33;
	*pdata++ = 0x37;
	//*pdata++ = HexToBcd(&clock->second+ 0x33,1);
	//*pdata++ = HexToBcd(&clock->minute+ 0x33,1);
	//*pdata++ = HexToBcd(&clock->hour+ 0x33,1);

	*pdata++ = clock->second+ 0x33;
	*pdata++ = clock->minute+ 0x33;
	*pdata++ = clock->hour+ 0x33;
	HexToBcd(pdata - 3,3);

	pbuf = write_time_frame;
	for(i=0;i<17;i++)
	{
		check_sum += *pbuf++;
	}
	*pdata++ = check_sum;
	*pdata++ = 0x16;

	
	UartSend(AMP_PORT,write_time_frame,19);
	rcv_len = recieve_read_time_ack(rcv_buf_tmp); 
	if(rcv_len>0)
	{
		printf("rcv_buf_tmp = ");
		for(i=0;i<30;i++)
		{
			printf("%02x ",rcv_buf_tmp[i]);
		}
		printf("\n");
		if(check_recieve_read_imet_pak(rcv_buf_tmp,rcv_len))		return 1;
	}
	printf("ExtClockWrite3\n");
	return 0;
}
#endif




/**
* @brief �����ⲿʱ��
* @param ʱ�ӱ���ָ��
* @return 0�ɹ�, ����ʧ��
*/
int ExtClockWrite(const extclock_t *clock)
{
	unsigned char write_time_frame[30];
	unsigned char *pdata = write_time_frame;
	unsigned char check_sum = 0;
	unsigned char i = 0;
	unsigned char *pbuf;
	unsigned char rcv_buf_tmp[30];
	int rcv_len = 0;

	printf("ExtClockWrite1\n");
	memset(write_time_frame,0x00,sizeof(write_time_frame));
	rcv_len = 0;
	memset(rcv_buf_tmp,0x00,sizeof(rcv_buf_tmp));
	memcpy(pdata,read_time_frame,8);
	pdata += 8;
	//*pdata++ = 0x11;
	*pdata++ = 0x14;
	*pdata++ = 0x0B;
	*pdata++ = 0x33;
	*pdata++ = 0x34;
	*pdata++ = 0x33;
	*pdata++ = 0x37;
	//*pdata++ = HexToBcd(&clock->day + 0x33),1);
	//*pdata++ = HexToBcd(&clock->month+ 0x33,1);
	//*pdata++ = HexToBcd(&clock->year+ 0x33,1);
	//*pdata++ = HexToBcd(&clock->week+ 0x33,1);

	*pdata++ = clock->day + 0x33;
	*pdata++ = clock->month+ 0x33;
	*pdata++ = clock->year+ 0x33;
	*pdata++ = clock->week+ 0x33;
	*pdata++ = clock->second+ 0x33;
	*pdata++ = clock->minute+ 0x33;
	*pdata++ = clock->hour+ 0x33;
	HexToBcd(pdata - 4,7);
	
	pbuf = write_time_frame;
	for(i=0;i<18;i++)
	{
		check_sum += *pbuf++;
	}
	*pdata++ = check_sum;
	*pdata++ = 0x16;
	UartSend(AMP_PORT,write_time_frame,20);
	rcv_len = recieve_read_time_ack(rcv_buf_tmp); 
	if(rcv_len>0)
	{
		printf("rcv_buf_tmp = ");
		for(i=0;i<30;i++)
		{
			printf("%02x ",rcv_buf_tmp[i]);
		}
		printf("\n");
		if(check_recieve_read_imet_pak(rcv_buf_tmp,rcv_len))		return 1;
	}
	#if 0
	printf("ExtClockWrite2\n");
	Sleep(1000);
	memset(write_time_frame,0x00,sizeof(write_time_frame));
	memcpy(pdata,read_time_frame,8);
	rcv_len = 0;
	memset(rcv_buf_tmp,0x00,sizeof(rcv_buf_tmp));
	pdata += 8;
	//*pdata++ = 0x11;
	*pdata++ = 0x14;
	*pdata++ = 0x07;
	*pdata++ = 0x35;
	*pdata++ = 0x34;
	*pdata++ = 0x33;
	*pdata++ = 0x37;
	//*pdata++ = HexToBcd(&clock->second+ 0x33,1);
	//*pdata++ = HexToBcd(&clock->minute+ 0x33,1);
	//*pdata++ = HexToBcd(&clock->hour+ 0x33,1);

	*pdata++ = clock->second+ 0x33;
	*pdata++ = clock->minute+ 0x33;
	*pdata++ = clock->hour+ 0x33;
	HexToBcd(pdata - 3,3);

	pbuf = write_time_frame;
	for(i=0;i<17;i++)
	{
		check_sum += *pbuf++;
	}
	*pdata++ = check_sum;
	*pdata++ = 0x16;

	
	UartSend(AMP_PORT,write_time_frame,19);
	rcv_len = recieve_read_time_ack(rcv_buf_tmp); 
	if(rcv_len>0)
	{
		printf("rcv_buf_tmp = ");
		for(i=0;i<30;i++)
		{
			printf("%02x ",rcv_buf_tmp[i]);
		}
		printf("\n");
		if(check_recieve_read_imet_pak(rcv_buf_tmp,rcv_len))		return 1;
	}
	printf("ExtClockWrite3\n");
	*/
	#endif
	printf("ExtClockWrite2\n");
	return 0;
}


/**
* @brief ��ȡ��������汾��Ϣ
*        �汾��Ϣ����:���汾��(1), �ΰ汾��(1), ����������(3)(BCD��ʽ), ��(1)
* @param buf ������ָ��
* @param len ����������
* @return �ɹ����ض�ȡ����(6�ֽ�), ���򷵻�-1
*/
int ReadDriverVersion(unsigned char *buf, int len)
{
	AssertLog(-1==FidGpio, "invalid fid(%d)\n", FidGpio);
	AssertLogReturn(len<6, -1, "invalid len(%d)\n", len);

	if(ioctl(FidGpio, 0x200, buf)) return -1;

	return 6;
}

/**
* @brief ��ȡ��ģ����
* @return ��ģ����
*/
unsigned char ReadLeftModuleNo(void)
{
	unsigned char uc = 0;

	AssertLog(-1==FidGpio, "invalid fid(%d)\n", FidGpio);

	if(ioctl(FidGpio, 0x201, &uc)) {
		ErrorLog("read module info fail\n");
	}

	return ((uc>>5)&0x07);
}

/**
* @brief ��ȡ��ģ����
* @return ��ģ����
*/
unsigned char ReadRightModuleNo(void)
{
	unsigned char uc = 0;

	AssertLog(-1==FidGpio, "invalid fid(%d)\n", FidGpio);

	if(ioctl(FidGpio, 0x201, &uc)) {
		ErrorLog("read module info fail\n");
	}

	return (uc&0x07);
}





char get_io_value(unsigned int pin)
{
	unsigned char io_data = 0;
	int gpio_fd1 = 0;

	//gpio_fd1=open("/dev/switch ",O_RDWR);
	gpio_fd1=open("/dev/switch", O_RDWR);
	if(gpio_fd1 > 0)
	{	
		lseek( gpio_fd1, pin, 0);
		Sleep(10);
		read(gpio_fd1,&io_data,1);
		Sleep(10);
		close(gpio_fd1);
		Sleep(10);
	}
	else
	{
		printf("open fd error \r\n");
		close(gpio_fd1);
	}
	return io_data;
}


void set_io(unsigned int pin,unsigned char cmd)
{	
	int gpio_fd2 = 0;
	
	gpio_fd2 = open("/dev/switch", O_RDWR);
	if(gpio_fd2> 0)
	{	
		lseek( gpio_fd2, pin, 0);
		Sleep(10);
		write(gpio_fd2,&cmd,1);
		//write(gpio_fd2,cmd,1);
		Sleep(10);
		close(gpio_fd2);
		Sleep(10);
	}
	else
	{
		printf("open fd error \r\n");
		close(gpio_fd2);
	}
}




