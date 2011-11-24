/**
* sndtime.c -- ����������ʱ�䴦��
* 
* 
* ����ʱ��: 2010-5-27
* ����޸�ʱ��: 2010-5-27
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/environment.h"
#include "include/debug.h"
#include "include/debug/shellcmd.h"
#include "include/sys/timeal.h"
#include "include/sys/bin.h"
#include "include/sys/cycsave.h"
#include "include/lib/dbtime.h"
#include "include/param/datatask.h"

#define SNDTIME_FILE		DATA_PATH "sndtime.dat"
#define SNDTIME_MAGIC		0xacf15de0
#define MAX_SNDTIME_NUM	4096
#define INDEX_NULL			0xffff

typedef union {
	unsigned char uc[4];
	unsigned short us[2];
	unsigned int ul;
} pnfn_t;

typedef struct {
	pnfn_t pnfn;
	dbtime_t sndtime;
} sndtime_t;

#define SNDTIME_TMPLEN	(sizeof(sndtime_t)*MAX_SNDTIME_NUM+MAX_SNDTIME_NUM)

struct sndtime_cache{
	struct sndtime_cache *next;
	sndtime_t dtime;
};
static struct sndtime_cache SndTimeCache[MAX_SNDTIME_NUM];
static unsigned short SndTimeNum = 0;
static struct sndtime_cache *SndTimeIndex[256];

static unsigned char SndTimeTemp[SNDTIME_TMPLEN];
static unsigned short SndTimeTempNum = 0;
static unsigned short SndTimeTempStep = 0;

#define NODE_OFFSET(uc)		((uc)[0]^(uc)[1]^(uc)[2]^(uc)[3])

/**
* @brief ������ʱ�������в���һ���ڵ�
* @param pnode �ڵ�ָ��
*/
static void InsertNode(struct sndtime_cache *pnode)
{
	unsigned char offset;
	struct sndtime_cache *prev, *pscan, *ptmp;

	offset = NODE_OFFSET(pnode->dtime.pnfn.uc);

	prev = NULL;
	pscan = SndTimeIndex[offset];
	while(NULL != pscan) {  //delete same
		if(pnode->dtime.pnfn.ul == pscan->dtime.pnfn.ul) {
			pscan->dtime.pnfn.ul = 0;
			if(NULL == prev) SndTimeIndex[offset] = pscan->next;
			else prev->next = pscan->next;
			ptmp = pscan;
			pscan = pscan->next;
			ptmp->next = NULL;
			continue;
		}

		prev = pscan;
		pscan = pscan->next;
	}

	pnode->next = SndTimeIndex[offset];
	SndTimeIndex[offset] = pnode;
}

/**
* @brief ��������ʱ��(���û�о������������)
* @param ptime ����ʱ�����ָ��
* @return �ɹ�0, ����ʧ��
*/
static int UpdateNode(const sndtime_t *ptime)
{
	unsigned char offset;
	struct sndtime_cache *pnode;
	unsigned short index;

	offset = NODE_OFFSET(ptime->pnfn.uc);

	pnode = SndTimeIndex[offset];
	for(; NULL!=pnode; pnode=pnode->next) {
		if(pnode->dtime.pnfn.ul == ptime->pnfn.ul) {
			pnode->dtime.sndtime = ptime->sndtime;
			return 0;
		}
	}

	if(SndTimeNum >= MAX_SNDTIME_NUM) {
		for(index=0; index<MAX_SNDTIME_NUM; index++) {
			if(SndTimeCache[index].dtime.pnfn.ul == 0) break;
		}
		if(index >= MAX_SNDTIME_NUM) return 1;
	}
	else {
		index = SndTimeNum;
		SndTimeNum++;
	}

	pnode = &SndTimeCache[index];
	pnode->dtime.pnfn.ul = ptime->pnfn.ul;
	pnode->dtime.sndtime = ptime->sndtime;
	pnode->next = SndTimeIndex[offset];
	SndTimeIndex[offset] = pnode;

	return 0;
}

/**
* @brief ɾ��һ���ڵ�
* @param ptime �ڵ��ʶ����ָ��
*/
static void RemoveNode(const sndtime_t *ptime)
{
	unsigned char offset;
	struct sndtime_cache *pnode, *prev;

	offset = NODE_OFFSET(ptime->pnfn.uc);

	pnode = SndTimeIndex[offset];
	prev = NULL;
	while(NULL != pnode) {
		if(pnode->dtime.pnfn.ul == ptime->pnfn.ul) {
			pnode->dtime.pnfn.ul = 0;
			if(NULL == prev) SndTimeIndex[offset] = pnode->next;
			else prev->next = pnode->next;
			pnode->next = NULL;
			return;
		}

		prev = pnode;
		pnode = pnode->next;
	}
}

/**
* @brief Ѱ�ҽڵ�
* @param ptime �ڵ��ʶ����ָ��
* @return �ɹ����ؽڵ�ָ��, ʧ�ܷ���NULL
*/
static struct sndtime_cache *FindNode(const sndtime_t *ptime)
{
	unsigned char offset;
	struct sndtime_cache *pnode;

	offset = NODE_OFFSET(ptime->pnfn.uc);

	pnode = SndTimeIndex[offset];
	for(; NULL != pnode; pnode=pnode->next) {
		if(pnode->dtime.pnfn.ul == ptime->pnfn.ul) return pnode;
	}

	return NULL;
}

/**
* @brief ���ڵ���û������������а���, û������ɾ��
*/
static void CheckSndTime(void)
{
	const para_task_t *ptask;
	unsigned int i, num, id;
	sndtime_t dtime;
	struct sndtime_cache *pnode, *prev, *ptmp;

	ptask = &ParaTaskCls2[0];

	for(id=0; id<MAX_DTASK_CLS2; id++,ptask++) {
		if(0 == ptask->valid || 0 == ptask->num || ptask->num > MAX_TASK_DUID) continue;

		num = ptask->num;

		for(i=0; i<num; i++) {
			if(ptask->duid[i].da[1] > 1) {
				dtime.pnfn.uc[0] = ptask->duid[i].da[0];
				dtime.pnfn.uc[1] = ptask->duid[i].da[1];
				dtime.pnfn.uc[2] = ptask->duid[i].dt[0];
				dtime.pnfn.uc[3] = ptask->duid[i].dt[1];

				pnode = FindNode(&dtime);
				if(NULL != pnode) pnode->dtime.sndtime.s.year &= 0x7f;
			}
			else {
				unsigned char pnmask, fnmask;

				for(pnmask=1; 0!=pnmask; pnmask<<=1) {
					if(0 == (pnmask&ptask->duid[i].da[0])) continue;

					for(fnmask=1; 0!=fnmask; fnmask<<=1) {
						if(0 == (fnmask&ptask->duid[i].dt[0])) continue;

						dtime.pnfn.uc[0] = pnmask;
						dtime.pnfn.uc[1] = ptask->duid[i].da[1];
						dtime.pnfn.uc[2] = fnmask;
						dtime.pnfn.uc[3] = ptask->duid[i].dt[1];

						pnode = FindNode(&dtime);
						if(NULL != pnode) pnode->dtime.sndtime.s.year &= 0x7f;
					}
				}
			}
		}
	}

	for(i=0; i<256; i++) {
		if(NULL == SndTimeIndex[i]) continue;

		pnode = SndTimeIndex[i];
		prev = NULL;

		while(NULL != pnode) {
			if(pnode->dtime.sndtime.s.year & 0x80) {
				pnode->dtime.pnfn.ul = 0;
				if(NULL == prev) SndTimeIndex[i] = pnode->next;
				else prev->next = pnode->next;

				ptmp = pnode->next;
				pnode->next = NULL;
				pnode = ptmp;
				continue;
			}

			prev = pnode;
			pnode = pnode->next;
		}
	}
}

/**
* @brief ����ʱ�䴦��ģ���ʼ��
*/
DECLARE_INIT_FUNC(SndTimeInit);
int SndTimeInit(void)
{
	unsigned char *pmem = SndTimeTemp;
	int len;
	unsigned short num, i;
	sndtime_t *pdtime;

	for(i=0; i<256; i++) SndTimeIndex[i] = NULL;

	DebugPrint(0, "  load sndtime(maxsize=%d)...", SNDTIME_TMPLEN);

	len = ReadBinFileCache(SNDTIME_FILE, SNDTIME_MAGIC, pmem, SNDTIME_TMPLEN);
	if(len < 0) {
		DebugPrint(0, "fail\n");
		goto mark_end;
	}
	else {
		DebugPrint(0, "ok\n");
	}

	{
		unsigned short *pus, num2;
		int len2;

		pus = (unsigned short *)pmem;
		num = *pus++;
		num2 = *pus;
		if(num != num2 || num > MAX_SNDTIME_NUM) {
			ErrorLog("invalid num(%d,%d)\n", num, num2);
			goto mark_end;
		}

		len2 = (int)num&0xffff;
		len2 = len2*sizeof(sndtime_t) + 4;
		if(len2 != len) {
			ErrorLog("invalid len(%d,%d)\n", len, num);
			goto mark_end;
		}
	}

	SndTimeNum = 0;
	pdtime = (sndtime_t *)(pmem+4);
	for(i=0; i<num; i++,pdtime++) {
		if(pdtime->pnfn.ul == 0) continue;

		SndTimeCache[SndTimeNum].dtime.pnfn.ul = pdtime->pnfn.ul;
		SndTimeCache[SndTimeNum].dtime.sndtime.u = pdtime->sndtime.u;
		SndTimeCache[SndTimeNum].dtime.sndtime.s.year |= 0x80;
		InsertNode(&SndTimeCache[SndTimeNum]);
		SndTimeNum++;
	}

	CheckSndTime();

mark_end:
	SET_INIT_FLAG(SndTimeInit);
	return 0;
}

/**
* @brief ��������ʱ�䵽�ļ���
*/
void SaveSndTimeFile(void)
{
	unsigned char *pmem;
	int len;
	unsigned short i, num, *pus;
	sndtime_t *pdtime;

	//DebugPrint(0, "save snd time file(%s), sndnum=%d...\n", SNDTIME_FILE, SndTimeNum);

	AssertLogReturnVoid(SndTimeNum>MAX_SNDTIME_NUM, "invalid num(%d)\n", SndTimeNum);
	if(0 == SndTimeNum) {
		remove(SNDTIME_FILE);
		return;
	}

	pmem = malloc(SNDTIME_TMPLEN);
	if(NULL == pmem) {
		ErrorLog("malloc %d bytes fail\n", SNDTIME_TMPLEN);
		return;
	}

	len = (int)SndTimeNum&0xffff;
	len = len*sizeof(sndtime_t)+4;

	pdtime = (sndtime_t *)(pmem+4);
	num = 0;
	for(i=0; i<SndTimeNum; i++) {
		/*DebugPrint(0, "pnfn=%04X, sndtime=%04X\n", SndTimeCache[i].dtime.pnfn.ul,
			SndTimeCache[i].dtime.sndtime.u);*/
		if(SndTimeCache[i].dtime.pnfn.ul == 0 || SndTimeCache[i].dtime.sndtime.u == 0) continue;

		pdtime->pnfn.ul = SndTimeCache[i].dtime.pnfn.ul;
		pdtime->sndtime = SndTimeCache[i].dtime.sndtime;
		pdtime++;
		num++;
	}

	//DebugPrint(0, "num=%d\n", num);

	if(0 == num) {
		free(pmem);
		remove(SNDTIME_FILE);
		return;
	}

	len = (int)num&0xffff;
	len = len*sizeof(sndtime_t)+4;
	pus = (unsigned short *)pmem;
	pus[0] = num;
	pus[1] = num;
	SaveBinFile(SNDTIME_FILE, SNDTIME_MAGIC, pmem, len);

	free(pmem);
	return;
}
//DECLARE_CYCLE_SAVE(SaveSndTimeFile, 0);

/**
* @brief �������ʱ��
* @param �������ʶָ��
* @return ����ʱ��(û���򷵻ص�ǰʱ��)
*/
unsigned int GetSndTime(const unsigned char *pnfn)
{
	unsigned char offset = NODE_OFFSET(pnfn);
	pnfn_t p;
	struct sndtime_cache *pnode;

	p.uc[0] = pnfn[0];
	p.uc[1] = pnfn[1];
	p.uc[2] = pnfn[2];
	p.uc[3] = pnfn[3];

	pnode = SndTimeIndex[offset];
	for(; NULL!=pnode; pnode=pnode->next) {
		if(pnode->dtime.pnfn.ul == p.ul) {
			return pnode->dtime.sndtime.u;
		}
	}

	{
		sysclock_t clock;
		//dbtime_t dbtime;
		sndtime_t node;

		SysClockReadCurrent(&clock);
		SYSCLOCK_DBTIME(&clock, node.sndtime);
		node.sndtime.s.tick = 0;
		node.pnfn.ul = p.ul;

		UpdateNode(&node);

		return node.sndtime.u;
	}
}

/**
* @brief �������ʱ�仺��
*/
void ClearSndTime(void)
{
	SndTimeTempNum = 0;
	SndTimeTempStep = 0;
}

/**
* @brief ��������ʱ�䵽������
* @param pnfn �������ʶָ��
* @param utime ʱ��
* @param step ����(��������仯ʱ+1)
*/
void SetSndTime(const unsigned char *pnfn, unsigned int sndtime, unsigned char step)
{
	sndtime_t *pdtime;
	unsigned char *pstep;

	if(SndTimeTempNum >= MAX_SNDTIME_NUM) {
		ErrorLog("too mor tmp num(%d)\n", SndTimeTempNum);
	}

	pstep = SndTimeTemp;
	pdtime = (sndtime_t *)(pstep + MAX_SNDTIME_NUM);
	pstep += SndTimeTempNum;
	pdtime += SndTimeTempNum;

	pdtime->pnfn.uc[0] = pnfn[0];
	pdtime->pnfn.uc[1] = pnfn[1];
	pdtime->pnfn.uc[2] = pnfn[2];
	pdtime->pnfn.uc[3] = pnfn[3];
	pdtime->sndtime.u = sndtime;
	*pstep = step;

	SndTimeTempNum++;
}

/**
* @brief �������е�����ʱ����µ�������
*/
void SaveSndTime(void)
{
	unsigned short i;
	sndtime_t *pdtime;
	unsigned char *pstep, step;

	pstep = SndTimeTemp;
	pdtime = (sndtime_t *)(pstep + MAX_SNDTIME_NUM);
	pstep += SndTimeTempStep;
	pdtime += SndTimeTempStep;
	step = *pstep;

	for(i=SndTimeTempStep; i<SndTimeTempNum; i++,pstep++,pdtime++) {
		if(step != *pstep) break;

		UpdateNode(pdtime);
		SndTimeTempStep++;
	}
}

/**
* @brief ����������������������ʱ��
* @param taskid ��������
*/
void RemoveDataTaskTime(int taskid)
{
	const para_task_t *ptask;
	unsigned char i, num;
	sndtime_t dtime;

	AssertLog(taskid < 0 || taskid >= MAX_DTASK_CLS2, "invalid taskid(%d)\n", taskid);

	ptask = &ParaTaskCls2[taskid];

	num = ptask->num;
	if(num > MAX_TASK_DUID) {
		ErrorLog("invalid task num(%d)\n", num);
		num = MAX_TASK_DUID;
	}

	for(i=0; i<num; i++) {
		if(ptask->duid[i].da[1] > 1) {
			dtime.pnfn.uc[0] = ptask->duid[i].da[0];
			dtime.pnfn.uc[1] = ptask->duid[i].da[1];
			dtime.pnfn.uc[2] = ptask->duid[i].dt[0];
			dtime.pnfn.uc[3] = ptask->duid[i].dt[1];

			RemoveNode(&dtime);
		}
		else {
			unsigned char pnmask, fnmask;

			for(pnmask=1; 0!=pnmask; pnmask<<=1) {
				if(0 == (pnmask&ptask->duid[i].da[0])) continue;

				for(fnmask=1; 0!=fnmask; fnmask<<=1) {
					if(0 == (fnmask&ptask->duid[i].dt[0])) continue;

					dtime.pnfn.uc[0] = pnmask;
					dtime.pnfn.uc[1] = ptask->duid[i].da[1];
					dtime.pnfn.uc[2] = fnmask;
					dtime.pnfn.uc[3] = ptask->duid[i].dt[1];

					RemoveNode(&dtime);
				}
			}
		}
	}

	SaveSndTimeFile();
}

/**
* @brief ��ʼ���������������������ʱ��
* @param taskid ��������
*/
void InitDataTaskTime(int taskid)
{
	const para_task_t *ptask;
	unsigned char i, num;
	sndtime_t dtime;
	sysclock_t clock;
	dbtime_t dbtime;

	SysClockReadCurrent(&clock);
	SYSCLOCK_DBTIME(&clock, dbtime);

	AssertLog(taskid < 0 || taskid >= MAX_DTASK_CLS2, "invalid taskid(%d)\n", taskid);

	ptask = &ParaTaskCls2[taskid];

	num = ptask->num;
	if(num > MAX_TASK_DUID) {
		ErrorLog("invalid task num(%d)\n", num);
		num = MAX_TASK_DUID;
	}

	for(i=0; i<num; i++) {
		if(ptask->duid[i].da[1] > 1) {
			dtime.pnfn.uc[0] = ptask->duid[i].da[0];
			dtime.pnfn.uc[1] = ptask->duid[i].da[1];
			dtime.pnfn.uc[2] = ptask->duid[i].dt[0];
			dtime.pnfn.uc[3] = ptask->duid[i].dt[1];
			dtime.sndtime.u = dbtime.u;

			UpdateNode(&dtime);
		}
		else {
			unsigned char pnmask, fnmask;

			for(pnmask=1; 0!=pnmask; pnmask<<=1) {
				if(0 == (pnmask&ptask->duid[i].da[0])) continue;

				for(fnmask=1; 0!=fnmask; fnmask<<=1) {
					if(0 == (fnmask&ptask->duid[i].dt[0])) continue;

					dtime.pnfn.uc[0] = pnmask;
					dtime.pnfn.uc[1] = ptask->duid[i].da[1];
					dtime.pnfn.uc[2] = fnmask;
					dtime.pnfn.uc[3] = ptask->duid[i].dt[1];
					dtime.sndtime.u = dbtime.u;

					UpdateNode(&dtime);
				}
			}
		}
	}

	SaveSndTimeFile();
}

#ifdef OPEN_ASSERTLOG
/**
* @brief ��ӡ����ʱ������������(������)
*/
static void PrintSndTimeList(int flag)
{
	int i, count;
	struct sndtime_cache *pnode;
	sysclock_t clock;

	for(i=0; i<256; i++) {
		if(NULL == SndTimeIndex[i]) continue;

		pnode = SndTimeIndex[i];
		for(count=0; NULL!=pnode; pnode=pnode->next) {
			if(flag) {
				DBTIME_SYSCLOCK(pnode->dtime.sndtime, &clock);
				PrintLog(0, "  %02X%02X:%02X%02X=20%02d-%d-%d %d:%d, idx=%d\n",
							pnode->dtime.pnfn.uc[0], pnode->dtime.pnfn.uc[1], 
							pnode->dtime.pnfn.uc[2], pnode->dtime.pnfn.uc[3], 
							clock.year, clock.month, clock.day,
							clock.hour, clock.minute,
							((unsigned int)pnode - (unsigned int)SndTimeCache)/sizeof(struct sndtime_cache));
			}
			count++;
		}
		PrintLog(0, "offset %d have %d node\n", i, count);
	}
}

/**
* @brief ��ӡ����ʱ������
*/
static int shell_printsndtime(int argc, char *argv[])
{
	char c;
	int flag;

	if(argc != 2) {
		PrintLog(0, "useage: sndtime s/a\n");
		return 1;
	}

	c = argv[1][0];
	if('s' == c) flag = 0;
	else if('a' == c) flag = 1;
	else {
		PrintLog(0, "unknown flag\n");
		return 1;
	}

	PrintLog(0, "sndtime num = %d\n", SndTimeNum);

	PrintSndTimeList(flag);
	return 0;
}
DECLARE_SHELL_CMD("sndtime", shell_printsndtime, "��ӡ������ʱ��״̬");

#endif

