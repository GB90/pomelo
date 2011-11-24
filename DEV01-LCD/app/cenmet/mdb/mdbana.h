/**
* mdbana.h -- ��Ʒ�������
* 
* 
* ����ʱ��: 2010-5-11
* ����޸�ʱ��: 2010-5-11
*/

#ifndef _MDB_ANA_H
#define _MDB_ANA_H

//mid һ���ʾ������0~3
//metpid һ���ʾ������1~4

//��Ҫ���㼰�����ı�����
typedef struct {
	unsigned int enepa;  //�����й��ܵ���, 0.001kWh
	unsigned int enena;  //�����й��ܵ���, 0.001kWh

	unsigned int enepi;  //�����޹��ܵ���, 0.01kvarh
	unsigned int eneni;  //�����޹��ܵ���, 0.01kvarh

	int pwra[4];  //�й�����, 0.0001kW
	int pwra_1;  //���й�����(һ�β�), 0.01kW
	int pwri;  //�޹�����, 0.0001kvar
	int pwri_1;  //���޹�����(һ�β�), 0.01kvar
	int pwrv;  //�����ڹ���, 0.0001kVA
	short pwrf;  //�ܹ�������, 0.001
	unsigned short vol[3];  //��ѹ, 0.1V, ABC
	short amp[4];  //����, 0.01A, ABCN

	int pwrav;  //15����ƽ���й�����

	unsigned short vol_unb;  //��ѹ��ƽ���� 0.1%
	unsigned short amp_unb;  //������ƽ���� 0.1%

	unsigned int failmask;
	unsigned int firstfail;
} mdbana_t;

#ifndef DEFINE_MDBANA
extern const mdbana_t MdbAnalyze[MAX_CENMETP];
#endif

#define mdbana(mid)    (MdbAnalyze[mid])

#define MBMSK_ENEPA    0x00000001
#define MBMSK_ENENA    0x00000002
#define MBMSK_ENEPI    0x00000004
#define MBMSK_ENENI    0x00000008

#define MBMSK_PWRABLK    0x000000f0
#define MBMSK_PWRAS    0x00000010
#define MBMSK_PWRAA    0x00000020
#define MBMSK_PWRAB    0x00000040
#define MBMSK_PWRAC    0x00000080

#define MBMSK_PWRI    0x00000100
#define MBMSK_PWRV    0x00001000
#define MBMSK_PWRF    0x00010000

#define MBMSK_VOLBLK    0x00700000
#define MBMSK_VOLA    0x00100000
#define MBMSK_VOLB    0x00200000
#define MBMSK_VOLC    0x00400000

#define MBMSK_AMPBLK    0x07000000
#define MBMSK_AMPA    0x01000000
#define MBMSK_AMPB    0x02000000
#define MBMSK_AMPC    0x04000000
#define MBMSK_AMPN    0x08000000

#define mrd_fail(mid, mask)    (mdbana(mid).failmask & (mask))
#define mrd_firstfail(mid, mask)    (mdbana(mid).firstfail & (mask))

unsigned int GetMdbAnaFailMask(unsigned short itemid);
void ClearMdbAnaFailMask(unsigned short mid, unsigned int mask);
void SetMdbAnaFailMask(unsigned short mid, unsigned int mask);

void UpdateMdbAna(unsigned char metpid, unsigned int itemid, const unsigned char *buf, int len, unsigned char flag);


#endif /*_MDB_ANA_H*/

