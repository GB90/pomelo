/**
* unique.h -- �Զ������
* 
* 
* ����ʱ��: 2010-5-18
* ����޸�ʱ��: 2010-5-18
*/

#ifndef _PARA_UNIQUE_H
#define _PARA_UNIQUE_H

typedef struct {
	unsigned char uplink;   //����ͨ�Žӿ�
	unsigned char unuse[3];
	char cmply[2];
	unsigned char addr_area[2];  //�ն˵�ַ-������
	unsigned char addr_sn[2];   //�ն˵�ַ-���

	unsigned char addr_mac[6];  //�ն�MAC��ַ

	char manuno[14];  //�ն��ⲿ�������
	char manuno_inner[12];  //�ն��ڲ��������
	unsigned int gprs_mod_baud;
} para_uni_t;

#ifndef DEFINE_PARAUNI
extern const para_uni_t ParaUni;
#endif

#endif /*_PARA_UNIQUE_H*/

