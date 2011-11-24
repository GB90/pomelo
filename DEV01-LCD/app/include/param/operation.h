/**
* operation.h -- ��������ͷ�ļ�
* 
* 
* ����ʱ��: 2010-5-7
* ����޸�ʱ��: 2010-5-7
*/

#ifndef _PARAM_OPERATION_H
#define _PARAM_OPERATION_H

#define POERR_OK			0
#define POERR_INVALID		1
#define POERR_FATAL		2

int WriteParam(unsigned char *buf, int len, int *pactlen);

typedef struct {
	unsigned char *buf;   //���뻺����ָ��
	int len;   //����������
	int actlen;   //��Ч���ݳ���(�ɺ�������)
} para_readinfo_t;
int ReadParam(unsigned char *buf, int len, int *pactlen, para_readinfo_t *readinfo);

void ClearSaveParamFlag(void);
void SaveParam(void);

void ClearAllData(void);
void ClearAllParam(void);
int get_meter_param_change_flag(void);
void clear_meter_param_change_flag(void);
void set_meter_param_change_flag(void);
#endif /*_PARAM_OPERATION_H*/
