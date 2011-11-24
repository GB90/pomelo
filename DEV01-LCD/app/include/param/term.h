/**
* term.h -- �ն˲���ͷ�ļ�
* 
* 
* ����ʱ��: 2010-5-6
* ����޸�ʱ��: 2010-5-6
*/

#ifndef _PARAM_TERM_H
#define _PARAM_TERM_H

//F1, �ն�ͨ�Ų�������
typedef struct {
	unsigned char rts;		//�ն���������ʱʱ��
	unsigned char delay;	//�ն���Ϊ����վ��������ʱʱ��
	unsigned short rsnd;	//�ն˵ȴ��Ӷ�վ��Ӧ�ĳ�ʱʱ����ط�����
	unsigned char flagcon;	//��Ҫ��վȷ�ϵ�ͨ�ŷ���(CON=1)�ı�־
	unsigned char cycka;	//��������, ��
} cfg_tcom_t;

//F2, �ն��м�ת������, ����

//F3, ��վIP��ַ�Ͷ˿ں�
typedef struct {
	unsigned char ipmain[4];
	unsigned short portmain;
	unsigned char ipbakup[4];
	unsigned short portbakup;
	char apn[20];
} cfg_svrip_t;

//F4, ��վ�绰����Ͷ������ĺ���
typedef struct {
	unsigned char phone[8];
	unsigned char sms[8];
} cfg_smsc_t;

//F5, �ն���������
typedef struct {
	unsigned char art;    //�����㷨���
	unsigned char pwd[2];    //��Կ
} cfg_pass_t;

//F6, �ն����ַ����
typedef struct {
	unsigned char addr[16];
} cfg_grpaddr_t;

//F7, �ն�IP��ַ�Ͷ˿�
typedef struct {
	unsigned char ipterm[4];
	unsigned char maskterm[4];
	unsigned char ipgatew[4];
	unsigned char ipproxy[4];
	unsigned short portproxy;
	unsigned char proxy_type;
	unsigned char proxy_connect;
	char username[32];
	char pwd[32];
	unsigned short portlisten;
} cfg_termip_t;

//F8, �ն�����ͨ�Ź�����ʽ
typedef struct {
	unsigned char proto;  //0-TCP, 1-UDP
	unsigned char mode;   //0-mix, 1-client, 2-server
	unsigned char clientmode;   //0-��������, 1-��������, 2-ʱ������
	unsigned char countdail;  //�ز�����
	unsigned short timedail;  //�ز���� second
	unsigned char timedown;  //��ͨ���Զ�����ʱ�� minute
	unsigned char unsued;
	unsigned int onlineflag;  //����ʱ�α�־
} cfg_uplink_t;

//F9, �ն��¼���¼����
typedef struct {
	unsigned char valid[8];   //�¼���Ч
	unsigned char rank[8];    //�¼��ȼ�, 1-��Ҫ�¼�, 0-һ���¼�
} cfg_almflag_t;

//F16, ����ר���û���, ����
typedef struct {
	char user[32];
	char pwd[32];
} cfg_vpn_t;

typedef struct {
	cfg_tcom_t tcom;    //F1
	//F2
	cfg_svrip_t svrip;    //F3
	cfg_smsc_t smsc;    //F4
	cfg_pass_t pwd;    //F5
	cfg_grpaddr_t grpaddr;    //F6
	cfg_termip_t termip;    //F7
	cfg_uplink_t uplink;    //F8
	cfg_almflag_t almflag;    //F9
	cfg_vpn_t vpn;   //F16
} para_term_t;

#ifndef DEFINE_PARATERM
extern const para_term_t ParaTerm;
#endif

#endif /*_PARAM_TERM_H*/

