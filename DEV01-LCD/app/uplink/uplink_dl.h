/**
* uplink_dl.h -- ����ͨ��������·��
* 
* 
* ����ʱ��: 2010-5-18
* ����޸�ʱ��: 2010-5-18
*/

#ifndef _UPLINK_DL_H
#define _UPLINK_DL_H

//����ͨ���ŵ����
#define UPLINKITF_SERIAL		0   //����
#define UPLINKITF_ETHMTN		1   //��̫��������
#define UPLINKITF_GPRS			2   //GPRS
#define UPLINKITF_ETHER			3  //��̫��
#define UPLINKITF_SMS			4   //����
#define UPLINKITF_IR			5  //����
#define UPLINKITF_CASCADE		6  //����

#define UPLINKITF_NUM			7

#define UPLINKATTR_NOECHO    0x01    //�������Ͳ�����Ӧ(������ŵ�)

typedef struct {
	unsigned char *rcvbuf;  //���ջ���
	unsigned char *sndbuf;    //���ͻ���
	int (*rawsend)(const unsigned char *buf, int len);    //���ͺ���
	int (*getchar)(unsigned char *buf);    //���պ���
	int (*linestat)(void);
	int timeout;   //���ճ�ʱ

	int rcvmax;    //������������󳤶�
	int sndmax;    //������������󳤶�
	int sndnor;     //��������һ�㳤������

	unsigned int attr;   //ͨ������
} uplinkitf_t;

extern const uplinkitf_t UplinkInterface[UPLINKITF_NUM];
#define UPLINK_RCVBUF(itf)		(UplinkInterface[itf].rcvbuf)
#define UPLINK_SNDBUF(itf)		(UplinkInterface[itf].sndbuf)
#define UPLINK_RCVMAX(itf)		(UplinkInterface[itf].rcvmax)
#define UPLINK_TIMEOUT(itf)		(UplinkInterface[itf].timeout)
#define UPLINK_SNDMAX(itf)		(UplinkInterface[itf].sndmax)
#define UPLINK_SNDNOR(itf)		(UplinkInterface[itf].sndnor)
#define UPLINK_ATTR(itf)		(UplinkInterface[itf].attr)

void UplinkClearState(unsigned char itf);
int UplinkRecvPkt(unsigned char itf);
int UplinkSendPkt(unsigned char itf, uplink_pkt_t *pkt);
int CascadeForwardPkt(unsigned char itf, uplink_pkt_t *pkt);
int CascadeSvrSendPkt(unsigned char itf, uplink_pkt_t *pkt);

#define UPRTN_OK    0      //���ͳɹ�
#define UPRTN_FAIL    1    //����ʧ��
#define UPRTN_OKRCV    2    //���ͳɹ�, �յ������
#define UPRTN_FAILRCV    3    //����֮ǰ�յ������
#define UPRTN_TIMEOUT    4   //���ͳ�ʱ

//flag = 0, not wait echo, 1-wait echo
int UplinkActiveSend(unsigned char itf, unsigned char flag, uplink_pkt_t *psnd);
int UplinkDelay(unsigned char itf, const uplink_pkt_t *pkt);

int UplinkLogon(unsigned char itf);
int UplinkLinkTest(unsigned char itf);

//unsigned char faal_datomid(unsigned char *da);
//int UplinkCheckEchoPkt(uplink_pkt_t *pkt);

#endif /*_UPLINK_DL_H*/

