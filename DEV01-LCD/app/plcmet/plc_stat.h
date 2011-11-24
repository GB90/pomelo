/**
* plc_stat.h -- �ز�ͨ��״̬
* 
* 
* ����ʱ��: 2010-5-21
* ����޸�ʱ��: 2010-5-21
*/

#ifndef _PLC_STAT_H
#define _PLC_STAT_H

#include "include/param/capconf.h"

typedef struct {
	unsigned short metid;
	unsigned char routes;  //�м�·�ɼ���
	unsigned char phase;  //�ز�������λ
	unsigned char quality;  //�ز��ź�Ʒ��
	unsigned char okflag;  //���һ�γ���ɹ�/ʧ�ܱ�־ , 1/0
	unsigned char failcount;  //�������ʧ���ۼƴ���
	unsigned char unuse;
	sysclock_t oktime;  //���һ�γ���ɹ�ʱ��
	sysclock_t failtime;  //���һ�γ���ʧ��ʱ��
} plc_state_t;
extern plc_state_t PlcState[MAX_METER];//F170 PARA

//����ͨ����Ϣ
typedef struct {
	unsigned char routes;  //�м�·�ɼ���
	unsigned char phase;  //�ز�������λ
	unsigned char quality;  //�ز��ź�Ʒ��
	unsigned short ctime;  //ͨ��ʱ�� 100ms
} plc_sinfo_t;

int PlcStateSave(void);

int ReadPlMdbState(unsigned short metpid, unsigned short itemid, unsigned char *buf, int len);

#define MAX_PLCYCSTAT		13
//�����ִ�ͳ��
struct pl_cycstat{
	unsigned short ok_mets;  //����ɹ�����
	unsigned char time_start[6];  //�ִο�ʼʱ��(BCD)
	unsigned char time_end[6];  //�ִν���ʱ��(BCD)
	unsigned short rd_mets;  //�ܹ���������
};
extern struct pl_cycstat PlCycState[MAX_PLCYCSTAT];

int PlStatisticSave(void);

#endif /*_PLC_STAT_H*/

