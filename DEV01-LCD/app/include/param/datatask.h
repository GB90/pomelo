/**
* datatask.h -- �������ͷ�ļ�
* 
* 
* ����ʱ��: 2010-5-8
* ����޸�ʱ��: 2010-5-8
*/

#ifndef _PARAM_DATATASK_H
#define _PARAM_DATATASK_H

#include "include/param/capconf.h"

//F65 ��ʱ�ϱ�1��������������
//F66 ��ʱ�ϱ�2��������������

#define MAX_TASK_DUID		64

typedef struct {
	unsigned char da[2];    //��Ϣ��
	unsigned char dt[2];    //��Ϣ��
} task_duid_t;

#define TASKTYPE_CLS1    1
#define TASKTYPE_CLS2    2

typedef struct {
	unsigned char valid;
	unsigned char dev_snd;   //��ʱ�ϱ�����
	unsigned char mod_snd;  //0~3���α�ʾ�֡�ʱ���ա���
	unsigned char base_year;  //�ϱ���׼ʱ��: ��
	unsigned char base_month;
	unsigned char base_day;
	unsigned char base_hour;
	unsigned char base_minute;
	unsigned char base_second;
	unsigned char base_week;
	unsigned char freq; //�������ݳ�ȡ����
	unsigned char num;
	task_duid_t duid[MAX_TASK_DUID];
} para_task_t;

typedef struct {
	para_task_t cls1[MAX_DTASK_CLS1];
	para_task_t cls2[MAX_DTASK_CLS2];
} save_dtask_t;

#ifndef DEFINE_PARATASK
extern const save_dtask_t ParaTask;
#define ParaTaskCls1	(ParaTask.cls1)
#define ParaTaskCls2	(ParaTask.cls2)
#endif

#endif /*_PARAM_DATATASK_H*/

