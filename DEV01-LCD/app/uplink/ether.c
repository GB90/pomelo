/**
* ether.c -- ��̫��ͨ��(�ͻ���ģʽ)
* 
* 
* ����ʱ��: 2010-5-19
* ����޸�ʱ��: 2010-5-19
*/

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <sys/ioctl.h>

#include "include/debug.h"
#include "include/sys/schedule.h"
#include "include/lib/align.h"
#include "include/param/term.h"
#include "include/uplink/svrnote.h"
#include "uplink_pkt.h"
#include "uplink_dl.h"
#include "svrcomm.h"
#include "keepalive.h"

//static int SockEther = -1;

int SockEther = -1;
#define CLOSE_SOCKET(sock)   { \
	if((sock) >= 0) { \
		close(sock); \
		sock = -1; \
	}}

/**
* @brief ���ӵ�������
* @return �ɹ�0, ����ʧ��
*/
//static int EtherConnect(void)
int EtherConnect(void)
{
	static int ServerSel = 0;
	static int CountFail = 0;
	//unsigned char ipmain[4] = {0x7F,0x00,0x00,0x01};
	//unsigned short portmain = 20502;

	struct sockaddr_in addr;
	unsigned long ip;
	unsigned short port;
	//int ctlflag;

	CLOSE_SOCKET(SockEther);

	if(ParaTerm.uplink.proto)  //UDP
		SockEther = socket(AF_INET, SOCK_DGRAM, 0);
	else  //TCP
		SockEther = socket(AF_INET, SOCK_STREAM, 0);
	if(SockEther < 0) {
		printf("create socket errror\n");
		PrintLog(LOGTYPE_ALARM, "create socket errror.\n");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if(0 == ServerSel) {
		port = ParaTerm.svrip.portmain;
		addr.sin_port = htons(port);
		ip = addr.sin_addr.s_addr = MAKE_LONG(ParaTerm.svrip.ipmain);
	}
	else {
		port = ParaTerm.svrip.portbakup;
		addr.sin_port = htons(port);
		ip = addr.sin_addr.s_addr = MAKE_LONG(ParaTerm.svrip.ipbakup);
	}

	PrintLog(LOGTYPE_SHORT, "connect to %d.%d.%d.%d, %d...\n",
			ip&0xff, (ip>>8)&0xff, (ip>>16)&0xff, (ip>>24)&0xff, port);

	if(connect(SockEther, (struct sockaddr *)&addr, sizeof(addr)) == 0) {

		printf("connect succeed\n");
		printf("SockEther = %d\n",SockEther);
		PrintLog(LOGTYPE_SHORT, "connect succeed.\r\n");
		CountFail = 0;
		return 0;
	}

	PrintLog(LOGTYPE_SHORT, "connect failed.\n");
	CLOSE_SOCKET(SockEther);

	CountFail++;
	if(CountFail > 10) {
		CountFail = 0;

		if(0 == ServerSel) {
			ip = MAKE_LONG(ParaTerm.svrip.ipbakup);
			if(ip) ServerSel = 1;
		}
		else {
			ServerSel = 0;
		}
	}

	return 1;

	/*ctlflag = fcntl(sock_ether, F_GETFL);
	ctlflag |= O_NONBLOCK;
	fcntl(sock_ether, F_SETFL, ctlflag);*/
}

/**
* @brief ��������Ͽ�����
* @return �ɹ�0, ����ʧ��
*/
static int EtherDisconnect(void)
{
	CLOSE_SOCKET(SockEther);
	return 0;
}

/**
* @brief ��½������
* @return �ɹ�0, ����ʧ��
*/
static int EtherLogon(void)
{
	SetKeepAlive(KEEPALIVE_FLAG_LOGONFAIL);

	SvrCommLineState = LINESTAT_OFF;
	printf("EtherLogon\n");
	//printf("EtherLogon\n");
	//printf("EtherLogon\n");
	//printf("EtherLogon\n");
	//printf("EtherLogon\n");
	if(UplinkLogon(UPLINKITF_ETHER)) 
	{
		EtherDisconnect();
		return 1;
	}

	SvrCommLineState = LINESTAT_ON;
	SetKeepAlive(KEEPALIVE_FLAG_LOGONOK);
	return 0;
}

/**
* @brief �����������������
* @return �ɹ�0, ����ʧ��
*/
static int EtherKeepAlive(void)
{
	int rtn;

	PrintLog(LOGTYPE_SHORT, "ether keep alive...\n");

	printf("SvrCommLineState = %d\n",SvrCommLineState);
	if(LINESTAT_ON == SvrCommLineState) 
	{
		rtn = UplinkLinkTest(UPLINKITF_ETHER);
		if((UPRTN_FAIL == rtn) || (UPRTN_TIMEOUT == rtn)) 
		{
			SvrCommLineState = LINESTAT_OFF;
			EtherDisconnect();
			//svr_lineled(0);
		}
		else if(UPRTN_OK == rtn) 
		{
			return 0;
		}
		else 
		{
			SvrMessageProc(UPLINKITF_ETHER);
			return 0;
		}
	}

	if(!EtherConnect()) 
	{
		Sleep(50);
		return(EtherLogon());
	}

	EtherDisconnect();
	SvrCommLineState = LINESTAT_OFF;
	return 1;
}

/**
* @brief ��̫��ͨ������
*/
void EtherTask(void)
{
	unsigned long ev;

	UplinkClearState(UPLINKITF_ETHER);
	SvrCommLineState = LINESTAT_OFF;

	//while(2 == ParaTerm.uplink.mode) Sleep(200);  //server mode

	EtherKeepAlive();

	while(1) 
	{
		SvrCommPeekEvent(SVREV_NOTE, &ev);

		if(ev&SVREV_NOTE) //�����ϴ��¼�
		{
			if(!RefreshKeepAlive()) EtherKeepAlive();
			if(LINESTAT_ON == SvrCommLineState) SvrNoteProc(UPLINKITF_ETHER);
		}

		if(LINESTAT_ON == SvrCommLineState)
		{
			if(!UplinkRecvPkt(UPLINKITF_ETHER))  //�������¼�
			{
				Sleep(20);
				SvrMessageProc(UPLINKITF_ETHER);
			}
		}
		if(!KeepAliveProc()) //�����Ҫ������·����
		{
			EtherKeepAlive();
		}

		Sleep(10);
	}
}

static unsigned char ether_recvbuf[2048];
static int ether_recvlen = 0;
static int ether_recvhead = 0;

/**
* @brief ����̫��ͨ�Žӿڶ�ȡһ���ֽ�
* @param buf �����ַ�ָ��
* @return �ɹ�0, ����ʧ��
*/
int EtherGetChar(unsigned char *buf)
{

	if(SockEther < 0) return 1;

	if(ether_recvlen <= 0) 
	{
		//ether_recvlen = recv(sock_ether, ether_recvbuf, 2048, 0);
		ether_recvlen = recv(SockEther, ether_recvbuf, 2048, MSG_DONTWAIT);
		if(((ether_recvlen < 0) && (errno != EWOULDBLOCK)) ||
				(ether_recvlen == 0)) {
			PrintLog(LOGTYPE_SHORT, "connection corrupted(%d,%d).\n", ether_recvlen, errno);
			CLOSE_SOCKET(SockEther);
			SvrCommLineState = LINESTAT_OFF;
			return 1;
		}
		else if(ether_recvlen < 0) {
			return 1;
		}
		else {
			ether_recvhead = 0;
			printf("EtherGetChar..............\n");	
		}
	}

	*buf = ether_recvbuf[ether_recvhead++];
	ether_recvlen--;
	return 0;
}

/**
* @brief ����̫��ͨ�Žӿڷ�������
* @param buf ���ͻ�����ָ��
* @param len ����������
* @return �ɹ�0, ����ʧ��
*/
int EtherRawSend(const unsigned char *buf, int len)
{
	//int i, buflen;
	//printf("EtherRawSend1\n");
	//printf("SockEther = %d\n",SockEther);
	if(SockEther < 0) return 1;

	if(send(SockEther, buf, len, MSG_NOSIGNAL) < 0) 
	{
		//printf("EtherRawSend2\n");
		DebugPrint(1, "send fail\r\n");
		goto mark_failend;
	}
	//printf("EtherRawSend3\n");
	return 0;

#if 0
	//wait until send buffer empty
	for(i=0; i<100; i++) {
		osh_sleep(10);

		if(ioctl(sock_ether, SIOCOUTQ, &buflen)) goto mark_failend;

		//debug_print(1, "buflen = %d\r\n", buflen);
		if(0 == buflen) return 0;
	}
#endif

mark_failend:
	PrintLog(LOGTYPE_SHORT, "connection corrupted.\r\n");
	 printf("connection corrupted\n");
	CLOSE_SOCKET(SockEther);
	SvrCommLineState = LINESTAT_OFF;
	return 1;
}

