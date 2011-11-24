/**
* datatask.c -- �������
* 
* 
* ����ʱ��: 2010-5-8
* ����޸�ʱ��: 2010-5-8
*/

#include <stdio.h>
#include <string.h>

#define DEFINE_PARATASK

#include "param_config.h"
#include "include/debug.h"
#include "include/param/datatask.h"
#include "include/sys/bin.h"
#include "include/param/operation.h"
#include "operation_inner.h"
#include "include/lib/bcd.h"
#include "include/uplink/datatask_timer.h"
#include "include/cenmet/sndtime.h"

save_dtask_t ParaTask;

#define TASK_MAGIC	0x2a7b5c3f

/**
* @brief ���ļ��������������
* @return 0�ɹ�, ����ʧ��
*/
int LoadParaTask(void)
{
	memset(&ParaTask, 0, sizeof(ParaTask));

	DebugPrint(0, "  load param task...(size=%d)", sizeof(ParaTask)+12);
	if(ReadBinFile(PARAM_SAVE_PATH "task.bin", TASK_MAGIC, (unsigned char *)&ParaTask, sizeof(ParaTask)) < 0) {
		DebugPrint(0, "no main, ");
		if(ReadBinFile(PARAM_BAK_PATH "task.bin", TASK_MAGIC, (unsigned char *)&ParaTask, sizeof(ParaTask)) < 0)
			DebugPrint(0, "no bak\n");
			return 1;
	}

	DebugPrint(0, "ok\n");
	return 0;
}

/**
* @brief �����������
* @return 0�ɹ�, ����ʧ��
*/
int SaveParaTask(void)
{
	SaveBinFile(PARAM_SAVE_PATH "task.bin", TASK_MAGIC, (unsigned char *)&ParaTask, sizeof(ParaTask));
	SaveBinFile(PARAM_BAK_PATH "task.bin", TASK_MAGIC, (unsigned char *)&ParaTask, sizeof(ParaTask));

	return 0;
}

static int ParaOpertionTask(int flag, para_task_t *ptask, unsigned char *buf, int len, int *actlen)
{
	int num;

	if(0 == flag) {
		num = (int)ptask->num & 0xff;
		*actlen = num*4 + 9;
		if(*actlen > len) return POERR_FATAL;

		*buf++ = ((ptask->mod_snd&0x03)<<6) + (ptask->dev_snd&0x3f);
		buf[0] = ptask->base_second;
		buf[1] = ptask->base_minute;
		buf[2] = ptask->base_hour;
		buf[3] = ptask->base_day;
		buf[4] = ptask->base_month;
		buf[5] = ptask->base_year;
		HexToBcd(buf, 6);
		buf[4] += ptask->base_week << 5;
		buf += 6;
		*buf++ = ptask->freq;
		*buf++ = ptask->num;
		if(num) memcpy(buf, ptask->duid, num*4);
	}
	else {
		num = (int)buf[8]&0xff;
		*actlen = num*4 + 9;
		if(*actlen > len) return POERR_FATAL;
		if(num == 0 || num > MAX_TASK_DUID) return POERR_INVALID;

		ptask->mod_snd = (buf[0]>>6)&0x03;
		ptask->dev_snd = buf[0]&0x3f; buf++;
		ptask->base_second = *buf++;
		ptask->base_minute = *buf++;
		ptask->base_hour = *buf++;
		ptask->base_day = *buf++;
		ptask->base_month = *buf & 0x1f;
		ptask->base_week = (*buf++)>>5;
		ptask->base_year = *buf++;
		BcdToHex(&ptask->base_year, 6);
		ptask->freq = *buf++;
		ptask->num = *buf++;
		memcpy(ptask->duid, buf, num*4);
	}

	return 0;
}

/**
* @brief �ն˲���F65����
* @param flag ������ʽ, 0-��, 1-д
* @param metpid �������
* @param buf ������ָ��
* @param len ����������
* @param actlen ��Ч���ݳ���(�ɺ�������)
* @return 0�ɹ�, ����ʧ��(�ο�POERR_XXX�궨��)
* @note ����ͬ������ͷ���ֵ��ͬ, �����ظ�ע��
*/
int ParaOperationF65(int flag, unsigned short metpid, unsigned char *buf, int len, int *actlen)
{
	para_task_t *ptask = &ParaTask.cls1[metpid-1];
	int rtn;

	rtn = ParaOpertionTask(flag, ptask, buf, len, actlen);

	if(flag && 0 == rtn) {
		if(ptask->valid) DataTaskCls1ReInit(metpid-1);
	}

	return rtn;
}

/**
* @brief �ն˲���F66����
*/
int ParaOperationF66(int flag, unsigned short metpid, unsigned char *buf, int len, int *actlen)
{
	para_task_t *ptask = &ParaTask.cls2[metpid-1];
	int rtn;

	rtn = ParaOpertionTask(flag, ptask, buf, len, actlen);

	if(flag && 0 == rtn) {
		if(ptask->valid) {
			//CheckSndTime();
			InitDataTaskTime(metpid-1);
			DataTaskCls2ReInit(metpid-1);
		}
	}

	return rtn;
}

/**
* @brief �ն˲���F67����
*/
int ParaOperationF67(int flag, unsigned short metpid, unsigned char *buf, int len, int *actlen)
{
	para_task_t *ptask = &ParaTask.cls1[metpid-1];
	unsigned char old;

	if(0 == flag) {
		if(ptask->valid) buf[0] = 0x55;
		else buf[0] = 0xaa;
	}
	else {
		old = ptask->valid;
		if(buf[0] == 0x55) ptask->valid = 1;
		else if(buf[0] == 0xaa) ptask->valid = 0;
		else return POERR_INVALID;

		if(old != ptask->valid) DataTaskCls1ReInit(metpid-1);
	}

	return 0;
}

/**
* @brief �ն˲���F68����
*/
int ParaOperationF68(int flag, unsigned short metpid, unsigned char *buf, int len, int *actlen)
{
	para_task_t *ptask = &ParaTask.cls2[metpid-1];
	unsigned char old;

	if(0 == flag) {
		if(ptask->valid) buf[0] = 0x55;
		else buf[0] = 0xaa;
	}
	else {
		old = ptask->valid;

		if(buf[0] == 0x55) ptask->valid = 1;
		else if(buf[0] == 0xaa) ptask->valid = 0;
		else return POERR_INVALID;

		if(old != ptask->valid) {
			if(ptask->valid) InitDataTaskTime(metpid-1);
			else RemoveDataTaskTime(metpid-1);
			DataTaskCls2ReInit(metpid-1);
		}
	}

	return 0;
}

