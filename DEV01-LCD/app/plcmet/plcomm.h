/**
* plccomm.h -- �ز�ͨ�Žӿ�ͷ�ļ�
* 
* 
* ����ʱ��: 2010-4-24
* ����޸�ʱ��: 2010-4-24
*/

#ifndef _PLCOMM_H
#define _PLCOMM_H

#define PLC_UART_PORT		5
   
//�ز�·�ɱ����5���м̣�
#define PLC_ROUTENUM    5
typedef struct {
	unsigned char level;  //·�ɼ���
	unsigned char phase;   //0-δָ��, 1-a, 2-b,3-c
	unsigned char addr[PLC_ROUTENUM*6];
} plc_route_t;

//Ŀ������
typedef struct {
	unsigned short metid;  // ��1��ʼ, 0��Ч
	unsigned char portcfg;
	unsigned char proto;
	unsigned char src[6];
	unsigned char dest[6];
	plc_route_t route;
} plc_dest_t;

typedef struct {
	unsigned short itemid;
	unsigned char *pwd;
	int pwdlen;
	const unsigned char *command;
	int cmdlen;
} plwrite_config_t; 

#define PLCOMM_BUF_LEN		272
unsigned char *GetPlCommBuffer(void);

void MakePlcDest(unsigned short metid, plc_dest_t *dest);

#define PLCHKTIME_POLL		1   //��ѯ��ʽ�ϱ�
#define PLCHKTIME_BROCAST	2   //�㲥��ʽ�ϱ�


//���ش�����
#define PLCERR_INVALID		-1
#define PLCERR_TIMEOUT		-2

int PlcRead(const plc_dest_t *dest, unsigned short itemid, unsigned char *buf, int len);
int PlcWrite(const plc_dest_t *dest, const plwrite_config_t *pconfig);
int PlcCheckTime(void);

#ifndef DEFINE_PLCOMM
extern const int PlcTimeChecking;
#endif

#endif /*_PLCOMM_H*/

