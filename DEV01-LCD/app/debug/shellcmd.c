/**
* shellcmd.c -- �����е�������
* 
* 
* ����ʱ��: 2010-5-26
* ����޸�ʱ��: 2010-5-26
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/debug.h"
#include "include/version.h"
#include "include/debug/shellcmd.h"
#include "include/sys/schedule.h"
#include "include/sys/timeal.h"
#include "include/sys/reset.h"
#include "include/sys/timer.h"
#include "include/sys/gpio.h"
#include "include/sys/adc.h"
#include "include/sys/rs485.h"
#include "include/sys/sigin.h"
#include "include/sys/cycsave.h"
#include "include/sys/uart.h"
#include "include/param/unique.h"
#include "include/param/term.h"
#include "include/cenmet/info.h"
#include "include/lib/align.h"
#include "include/plcmet/pltask.h"
#include "cenmet/cenmet_comm.h"
#include "uplink/svrcomm.h"
static int shell_debug(int argc, char *argv[])
{
	int type;

	if(argc != 2) return 1;

	type = atoi(argv[1]);
	if(argc < 0 || argc > 30) return 1;
	SetLogType(type);
	if(type) PrintLog(0, "open debug %d ok\n", type);
	else {
		PrintLog(0, "close debug ok\n");
		SetLogInterface(0);
	}
	return 0;
}
DECLARE_SHELL_CMD("debug", shell_debug, "�ı���Դ�ӡ����");

static int shell_showtime(int argc, char *argv[])
{
	sysclock_t clock;
	const char *pstr;

	SysClockRead(&clock);
	pstr = SysClockFormat(&clock);
	PrintLog(0, "��ǰʱ��: %s\n", pstr);
	return 0;
}
DECLARE_SHELL_CMD("time", shell_showtime, "��ʾ��ǰʱ��");

static int shell_showclock(int argc, char *argv[])
{
	sysclock_t clock;
	const char *pstr;
	extclock_t extclock;

	if(ExtClockRead(&extclock)) {
		PrintLog(0, "read ext clock fail\n");
		return 1;
	}

	clock.year = extclock.year;
	clock.month = extclock.month;
	clock.day = extclock.day;
	clock.hour = extclock.hour;
	clock.minute = extclock.minute;
	clock.second = extclock.second;
	clock.week = extclock.week;

	SysClockRead(&clock);
	pstr = SysClockFormat(&clock);
	PrintLog(0, "��ǰʱ��: %s\n", pstr);
	return 0;
}
DECLARE_SHELL_CMD("clock", shell_showclock, "��ʾ��ǰʵʱʱ��");

static int shell_restart(int argc, char *argv[])
{
	PrintLog(0, "ϵͳ���ڸ�λ, ��ȴ�...\n");

	SysRestart();
	return 0;
}
DECLARE_SHELL_CMD("reboot", shell_restart, "��λ�ն�");

static int shell_settime(int argc, char *argv[])
{
	int i;
	sysclock_t clock;
	utime_t utime, utimecur;
	extclock_t extclock;

	if(argc != 7) return 1;

	i = atoi(argv[1]);
	if(i < 0 || i > 99) {
		PrintLog(0, "invalid year\n");
		return 1;
	}
	clock.year = i;

	i = atoi(argv[2]);
	if(i < 1 || i > 12) {
		PrintLog(0, "invalid month\n");
		return 1;
	}
	clock.month = i;

	i = atoi(argv[3]);
	if(i < 1 || i > 31) {
		PrintLog(0, "invalid day\n");
		return 1;
	}
	clock.day = i;

	i = atoi(argv[4]);
	if(i < 0 || i > 23) {
		PrintLog(0, "invalid hour\n");
		return 1;
	}
	clock.hour = i;

	i = atoi(argv[5]);
	if(i < 0 || i > 59) {
		PrintLog(0, "invalid minute\n");
		return 1;
	}
	clock.minute = i;

	i = atoi(argv[6]);
	if(i < 0 || i > 59) {
		PrintLog(0, "invalid second\n");
		return 1;
	}
	clock.second = i;

	utimecur = UTimeReadCurrent();
	utime = SysClockToUTime(&clock);

	DebugPrint(0, "set sys clock to %s\n", SysClockFormat(&clock));
	SysClockSet(&clock);
	SysClockRead(&clock);
	DebugPrint(0, "set ext clock to %s\n", SysClockFormat(&clock));

	extclock.century = 0;
	extclock.year = clock.year;
	extclock.month = clock.month;
	extclock.day = clock.day;
	extclock.hour = clock.hour;
	extclock.minute = clock.minute;
	extclock.second = clock.second;
	extclock.week = clock.week;

	ExtClockWrite(&extclock);

	if(utimecur > utime) i = utimecur - utime;
	else i = utime - utimecur;

	if(i > 300) SysRecalAllRTimer();

	PrintLog(0, "�����ն�ʱ��ɹ�\n");
	return 0;
}
DECLARE_SHELL_CMD("settime", shell_settime, "�����ն�ʱ��");

static int shell_adc(int argc, char *argv[])
{
	int chn, value;

	if(argc != 2) {
		PrintLog(0, "adc channel\n");
		return 1;
	}

	chn = atoi(argv[1]);
	if(chn < 0 || chn >= ADC_CHN_MAX) {
		PrintLog(0, "invalid channel\n");
		return 1;
	}

	value = AdcRead(chn);
	PrintLog(0, "channel %d = %dmv\n", chn, value);

	return 0;
}
DECLARE_SHELL_CMD("adc", shell_adc, "��ȡAD��ֵ");

#if 0
static int shell_rs485test(int argc, char *argv[])
{
	int porta, portb;
	int i, j;
	unsigned char buf[48];

	if(argc != 3) {
		PrintLog(0, "rs485 porta portb\n");
		return 1;
	}

	porta = atoi(argv[1]);
	if(porta < 0 || porta >= RS485_PORTNUM) {
		PrintLog(0, "�Ƿ��˿�\n");
		return 1;
	}
	portb = atoi(argv[2]);
	if(portb < 0 || portb >= RS485_PORTNUM) {
		PrintLog(0, "�Ƿ��˿�\n");
		return 1;
	}
	if(porta > 0 && portb > 0) {
		PrintLog(0, "���ܻ�·����\n");
		return 1;
	}

	if(porta > 0) Rs485Lock(porta);
	else if(portb > 0) Rs485Lock(portb);

	while(Rs485Recv(portb, buf, 32) > 0);

	for(i=0; i<32; i++) buf[i] = i;

	Rs485Send(porta, buf, 32);
	Sleep(100);
	i = Rs485Recv(portb, buf, 48);
	if(i <= 0) {
		PrintLog(0, "û���յ�����\n");
		goto mark_end;
	}

	PrintLog(0, "�յ� %d ���ֽ�\n", i);
	PrintHexLog(0, buf, i);
	if(i < 32) PrintLog(0, "too short\n");
	else i = 32;
	for(j=0; j<i; j++) {
		if(buf[j] != (unsigned char)j) PrintLog(0, "��%d���ֽڲ���\n", j);
	}
	if(j >= 32) PrintLog(0, "��·���Գɹ�\n");

mark_end:
	if(porta > 0) Rs485Unlock(porta);
	else if(portb > 0) Rs485Unlock(portb);

	return 0;
}
#endif


int RecieveRs485(unsigned char *buf,unsigned char port_num) 
{
	int timeout;  // ��ʱ����������
	char c;
	unsigned char *pdata;
	int len = 0;
	
	pdata = buf;
	timeout = 0;
	do
	{   
    		timeout++;
		while(UartRecv(port_num,&c, 1))
		{
			*pdata++ = c;
			len++;
		} 
		if((len>=17) && (c = 0x16))
			break;
		Sleep(10);
	}while(timeout<100);	  
	printf("rcv_len = %d\n",len);
	return len;
}


static int shell_rs485test(int argc, char *argv[])
{
	int i,test_receive_len;
	unsigned char test_port = 0,test_port1 = 0;
	unsigned char test_receive_buf[32],test_send_buf[32],test_meter_addr[6];

	for(i=0;i<6;i++)
	{
		test_meter_addr[i] = 0x11;
	}
	test_receive_len = 0;
	test_port1 = atoi(argv[1]);
	memset(test_receive_buf,0x00,32);
	memset(test_send_buf,0x00,32);
	if(test_port1 != 1 && test_port1 != 2 && test_port1 != 3)
	{
		PrintLog(0, "����˿�Ϊ1,2,3\n");
		return 0;
	}
	if(test_port1 == 1)		test_port = 3;
	else if(test_port1 == 2)	test_port = 4;
	else if(test_port1 == 3)	test_port = 1;
	PrintLog(0, "test_port = %d\n",test_port);
	UartOpen(test_port);
	if(test_port != 4)
	{
		UartSet(test_port, 1200, 8, 1, 'E');
	}
	else
	{
		UartSet(test_port, 9600, 8, 1, 'E');
	}
	make_read_645_pkt(test_meter_addr, 0x9010, test_send_buf);
	//CenMetLock(test_port);
	UartSend(test_port,test_send_buf,14);
	test_receive_len = RecieveRs485(test_receive_buf,test_port);
	if(test_receive_len<=0)
	{
		//CenMetUnlock(test_port);
		PrintLog(0, "û���յ�����\n");
		return 1;
	}
	//CenMetUnlock(test_port);
	if(!check_cen_meter_rcv_pak(test_receive_buf,test_receive_len))
	{
		PrintLog(0, "�˿ڲ��Գɹ�\n");
	}
	else
	{
		PrintLog(0, "�յ�����������֡\n");
	}
	return 0;
}

DECLARE_SHELL_CMD("rs485", shell_rs485test, "RS485��·����");

static int shell_alarm(int argc, char *argv[])
{
#if 0
	int i;

	GpioSetDirect(GPIO_ALARM, 1);

	for(i=0; i<10; i++) {
		GpioSetValue(GPIO_ALARM, 1);
		Sleep(20);
		GpioSetValue(GPIO_ALARM, 0);
		Sleep(20);
	}
#endif
	PrintLog(0, "���Խ���\n");
	return 0;
}
DECLARE_SHELL_CMD("beep", shell_alarm, "���Է�����");

static int shell_sysinfo(int argc, char *argv[])
{
	sysclock_t clock;
	const char *pstr;

	GetClockSysStart(&clock);

	pstr = SysClockFormat(&clock);
	PrintLog(0, "ϵͳ����ʱ��: %s\n", pstr);

	PrintLog(0, " �� �� �� ַ: %02X%02X-%02X%02X\n", 
			ParaUni.addr_area[1], ParaUni.addr_area[0],
			ParaUni.addr_sn[1], ParaUni.addr_sn[0]);
	PrintLog(0, "  �ն�IP��ַ: %d.%d.%d.%d\n",
			ParaTerm.termip.ipterm[0], ParaTerm.termip.ipterm[1], 
			ParaTerm.termip.ipterm[2], ParaTerm.termip.ipterm[3]);
	PrintLog(0, " �ն�MAC��ַ: %02X:%02X:%02X:%02X:%02X:%02X\n", 
			ParaUni.addr_mac[0], ParaUni.addr_mac[1], ParaUni.addr_mac[2], 
			ParaUni.addr_mac[3], ParaUni.addr_mac[4], ParaUni.addr_mac[5]);
	PrintLog(0, "  �����˿ں�: %d\n", ParaTerm.termip.portlisten);
	PrintLog(0, " �� �� �� ��: %s\n", ParaUni.manuno);
	PrintLog(0, "�ڲ��������: %s\n", ParaUni.manuno_inner);
	PrintLog(0, "�ն�����״̬:");
	if(LINESTAT_ON == SvrCommLineState) 
	{
		PrintLog(0, "����\n");
	}
	else
	{
		PrintLog(0, "������\n");
	}
	return 0;
}
DECLARE_SHELL_CMD("info", shell_sysinfo, "��ӡϵͳ��Ϣ");

static int shell_isig(int argc, char *argv[])
{
	unsigned int stat;

	stat = SiginReadState();

	PrintLog(0, "ң��1: %s\n", (stat&0x01)?"��":"��");
	PrintLog(0, "ң��2: %s\n", (stat&0x02)?"��":"��");

	return 0;
}
DECLARE_SHELL_CMD("esig", shell_isig, "��ʾң��״̬");

static int shell_version(int argc, char *argv[])
{
	unsigned char buf[32];
	unsigned char i, num;

	PrintLog(0, "Ӧ�ó���汾: %d.%02d.%d (%s)\n",
		VERSION_MAJOR, VERSION_MINOR, VERSION_PROJECT, STRING_PROJECT);
	PrintLog(0, "    ��������: 20%02d-%d-%d %d:%d\n\n", 
		_mk_year, _mk_month, _mk_day, _mk_hour, _mk_minute);

	if(ReadDriverVersion(buf, 32) > 0) {
		PrintLog(0, "��������汾: %d.%02d\n", buf[0], buf[1]);
		PrintLog(0, "    ��������: 20%02x-%x-%x\n\n", buf[2], buf[3], buf[4]);
	}
	else {
		PrintLog(0, "��ȡ��������汾ʧ��\n\n");
	}

	if(PrintCMetProtoInfo(buf) > 0) {
		PrintLog(0, "  ��Э��汾: %d.%d-%04X\n", buf[5], buf[6], MAKE_SHORT(buf+7));
		PrintLog(0, "    ��������: 20%02d-%d-%d %d:%d\n", buf[0], buf[1], buf[2], buf[3], buf[4]);
		PrintLog(0, "    ������Լ: ");
		num = buf[9];
		for(i=0; i<num; i++) PrintLog(0, "%d ", buf[i+10]);
		PrintLog(0, "\n");
	}
	else {
		PrintLog(0, "��ȡ��Э��汾ʧ��\n");
	}

	return 0;
}
DECLARE_SHELL_CMD("version", shell_version, "��ʾ�汾��Ϣ");

static int shell_cycsave(int argc, char *argv[])
{
	int flag;

	if(2 != argc) {
		PrintLog(0, "usage: cycsave 0 or 1\n");
		return 1;
	}

	flag = atoi(argv[1]);
	if(flag < 0 || flag > 1) {
		PrintLog(0, "invalid flag\n");
		return 1;
	}

	SysCycleSave(flag);
	PrintLog(0, "����ɹ�\n");
	return 0;
}
DECLARE_SHELL_CMD("cycsave", shell_cycsave, "�����������ڴ����ļ�");

