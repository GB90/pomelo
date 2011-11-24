/**
* ethsvr.c -- ��̫��ͨ��(������ģʽ)
* 
* 
* ����ʱ��: 2010-5-19
* ����޸�ʱ��: 2010-5-19
*/

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/errno.h>

#include "include/debug.h"
#include "include/sys/schedule.h"
#include "include/sys/task.h"
#include "include/param/term.h"
#include "uplink_pkt.h"
#include "uplink_dl.h"
#include "svrcomm.h"
#include  "include/plcmet/pltask.h"

static int SockEthSvr = -1;
static struct sockaddr AddrEthSvr;

#define CLOSE_SOCKET(sock)   { \
	if((sock) >= 0) { \
		close(sock); \
		sock = -1; \
	}}
/**
* @brief ��̫��ͨ�������ʼ��
* @return �ɹ�0, ����ʧ��
*/
static int EthSvrNetInit(void)
{
	struct sockaddr_in addr;
	int ctlflag;

	//SockEthSvr = socket(AF_INET, SOCK_DGRAM, 0);//create socket
	//CLOSE_SOCKET(SockEthSvr);
	SockEthSvr = socket(AF_INET, SOCK_DGRAM, 0);
	//SockEthSvr =	 socket(AF_INET, SOCK_STREAM, 0);
	if(SockEthSvr < 0) 
	{
		printf("create socket error.\r\n");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));//��0
	//��ʼ��struct sockaddr_in addr
	addr.sin_family = AF_INET;
	//ParaTerm.termip.portlisten = 20501;
	addr.sin_port = htons(ParaTerm.termip.portlisten);
	//addr.sin_port = htons(20508);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//addr.sin_addr.s_addr = 0;
	//�󶨴�addr
	if(bind(SockEthSvr, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
	{
		printf("bind error(%d).\r\n", errno);
		close(SockEthSvr);
		SockEthSvr = -1;
		return 1;
	}

	printf("ether server listen at port %d...\n", ParaTerm.termip.portlisten);
	//printf("ether server listen at port %d...\n", 20508);

	ctlflag = fcntl(SockEthSvr, F_GETFL);
	ctlflag |= O_NONBLOCK;
	fcntl(SockEthSvr, F_SETFL, ctlflag);

	return 0;
}

extern int EtherConnect(void);
extern int SockEther;
/**
* @brief ��̫��ͨ������
*/
static void *EthSvrTask(void *arg)
{
	UplinkClearState(UPLINKITF_ETHMTN);
	EthSvrNetInit();

	while(1) 
	{
		while(!UplinkRecvPkt(UPLINKITF_ETHMTN)) 
		{
			SvrMessageProc(UPLINKITF_ETHMTN);
		}
		Sleep(10);
		
	}
}

/**
* @brief ��̫��ͨ�ų�ʼ��
* @return �ɹ�0, ����ʧ��
*/
DECLARE_INIT_FUNC(EthSvrInit);
int EthSvrInit(void)
{
	SysCreateTask(EthSvrTask, NULL);

	SET_INIT_FLAG(EthSvrInit);
	return 0;
}

static unsigned char EthSvrRcvBuffer[2048];
static int EthSvrRcvLen = 0;
static int EthSvrRcvHead = 0;


//extern int get_m590_sock(void);
/**
* @brief ����̫��ͨ�Žӿڶ�ȡһ���ֽ�
* @param buf �����ַ�ָ��
* @return �ɹ�0, ����ʧ��
*/
int EthSvrGetChar(unsigned char *buf)
{
	unsigned int addrlen;
	

	if(SockEthSvr < 0) return 1;

	if(EthSvrRcvLen <= 0) {
		addrlen = sizeof(AddrEthSvr);
		EthSvrRcvLen = recvfrom(SockEthSvr, EthSvrRcvBuffer, 2048, 0, &AddrEthSvr, &addrlen);
		//EthSvrRcvLen = recvfrom(SockEthSvr, EthSvrRcvBuffer, 1, 0, &AddrEthSvr, &addrlen);
		if(EthSvrRcvLen <= 0) return 1;
		else EthSvrRcvHead = 0;
		if(EthSvrRcvLen)
		{
			//printf("EthSvrGetChar..............\n");			
		}
	}

	*buf = EthSvrRcvBuffer[EthSvrRcvHead++];
	EthSvrRcvLen--;
	return 0;



/*


	int rcv_len = -1;

	//rcv_len = recv(SockEthSvr, buf, 1, MSG_DONTWAIT);
	rcv_len = recv(SockEthSvr, EthSvrRcvBuffer, 2048, MSG_DONTWAIT);
	if(rcv_len>0)
	{
		printf("EthSvrGetChar..............\n");	
		return 0;
	}
	else
	{
		return 1;
	}

*/


	

	
}

/**
* @brief ����̫��ͨ�Žӿڷ�������
* @param buf ���ͻ�����ָ��
* @param len ����������
* @return �ɹ�0, ����ʧ��
*/
int EthSvrRawSend(const unsigned char *buf, int len)
{
	if(SockEthSvr < 0) return 1;

	sendto(SockEthSvr, buf, len, 0, &AddrEthSvr, sizeof(AddrEthSvr));
	printf("EthSvrRawSend..............\n");	
	return 0;
}
