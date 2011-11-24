/**
* cenmet_task.c -- ���ͨ������
* 
* 
* ����ʱ��: 2010-5-17
* ����޸�ʱ��: 2010-5-17
*/

#include <stdio.h>
#include <string.h>

#include "include/debug.h"
#include "include/sys/schedule.h"
#include "include/sys/event.h"
#include "include/sys/timeal.h"
#include "include/sys/timer.h"
#include "include/sys/task.h"
#include "include/param/capconf.h"
#include "include/param/commport.h"
#include "include/param/meter.h"
#include "include/param/metp.h"
#include "include/monitor/runstate.h"
#include "mdb/mdbconf.h"
#include "mdb/mdbcur.h"
#include "mdb/mdbfrez.h"
#include "mdb/mdbuene.h"
#include "mdb/mdbstic.h"
#include "mdb/mdbtgrp.h"
#include "mdb/cuvupdate.h"
#include "cenmet_proto.h"
#include "cenmet_comm.h"
#include "cenmet_alm.h"
#include "include/sys/gpio.h"

#define METEV_MON				0x00000001   //��Ƽ��(δ��)
#define METEV_15MIN			0x00000002   // 15�������߶���
#define METEV_DAY				0x00000004   // �ն���
#define METEV_RDMETDAY		0x00000008   //�����ն���
#define METEV_RDMET			0x00000010   //����
#define METEV_1MIN				0x00000020   // 1���Ӽ���


#define METEV_WAIT    (METEV_MON|METEV_15MIN|METEV_DAY|METEV_RDMETDAY|METEV_RDMET|METEV_1MIN)

#define CMSEV_1MIN		0x00001
static sys_event_t CenMetEvent;
//static sys_event_t CMetSticEvent;

static int TimerIdRmd = -1;
static int TimerIdReadMet = -1;

/**
* @brief ���ɱ�������¼�
* @param event �����ɵ��¼�
*/
static void MakeCenMetEvent(unsigned long event)
{
	SysSendEvent(&CenMetEvent, event);
}

/**
* @brief �����ն�ʱ��
* @param arg ��ʱ������
* @param utime ��ǰʱ��
*/
static void RTimerRmd(unsigned long arg, utime_t utime)
{
	unsigned char day;
	unsigned int mask;
	sysclock_t clk;
	const para_commport_t *pcfg = GetParaCommPort(COMMPORT_RS485_2);

	SysClockReadCurrent(&clk);
	day = clk.day;

	if(day) day -= 1;
	mask = 1<<day;
	if(mask & pcfg->dateflag) MakeCenMetEvent(METEV_RDMETDAY);
}

/**
* @brief �����ն����ʼ��
*/
void CenMetRmdInit(void)
{
	//@change later: ���ĳ�����ʱ�����³�ʼ��
	rtimer_conf_t conf;
	const para_commport_t *pcfg = GetParaCommPort(COMMPORT_RS485_2);

	if(TimerIdRmd >= 0) SysStopRTimer(TimerIdRmd);

	conf.curtime = UTimeReadCurrent();
	conf.bonce = 0;
	conf.tdev = 1;
	conf.tmod = UTIMEDEV_DAY;  // 1 day

	SysClockReadCurrent(&conf.basetime);
	conf.basetime.hour = pcfg->time_hour;
	conf.basetime.minute = pcfg->time_minute;

	TimerIdRmd = SysAddRTimer(&conf, RTimerRmd, 0);
}

/**
* @brief ����ʱ��
*/
static void RTimerReadMet(unsigned long arg, utime_t utime)
{
	MakeCenMetEvent(METEV_RDMET);
}

/**
* @brief �Զ������ʼ��
*/
void AutoReadCenMetInit(void)
{
	//@change later: ���ĳ�����ʱ�����³�ʼ��
	rtimer_conf_t conf;
	const para_commport_t *pcfg = GetParaCommPort(COMMPORT_RS485_2);

	conf.curtime = UTimeReadCurrent();;
	conf.bonce = 0;
	memset(&conf.basetime, 0, sizeof(sysclock_t));

	if(TimerIdReadMet >= 0) SysStopRTimer(TimerIdReadMet);

	if(pcfg->cycle >= 60) {
		conf.tdev = pcfg->cycle/60;
		conf.tmod = UTIMEDEV_HOUR;  // 1 hour
	}
	else if(pcfg->cycle < 1) {
		conf.tdev = 15;
		conf.tmod = UTIMEDEV_MINUTE;  // 15 minute
	}
	else {
		conf.tdev = pcfg->cycle;//F33���õĳ�����ʱ��
		conf.tmod = UTIMEDEV_MINUTE; //minute
	}

	TimerIdReadMet = SysAddRTimer(&conf, RTimerReadMet, 0);
}

#if 0
static void RTimer15Minute(unsigned long arg, utime_t utime)
{
	//DebugPrint(0, "rtimer 15min...\n");
	MakeCenMetEvent(METEV_15MIN);

	SetUTimeFrez(utime);
}
#endif
static void RTimerDay(unsigned long arg, utime_t utime)
{
	MakeCenMetEvent(METEV_DAY);
}

static void RTimer1Minute(unsigned long arg, utime_t utime)
{
	MakeCenMetEvent(METEV_1MIN);
}

/**
* @brief �๦�ܱ��ͨ������
*/
static void *CenMetTask(void * arg)
{
	unsigned long ev;
	sysclock_t clock;

	Sleep(500);
	SysClockReadCurrent(&clock);
	while(1) 
	{
		SysWaitEvent(&CenMetEvent, 1, METEV_WAIT, &ev);

		if(ev&METEV_RDMET) //���ݳ������Զ������ܱ�����
		{
			CenMetProc();//�����ܱ�����
		}

		if(ev&METEV_1MIN) // 1���Ӽ���
		{
			UpdateMdbUseEne();//�����õ������ݿ�
		}

		if(ev&METEV_DAY) // �ն���
		{ 
			UpdateCurveDay();
			SysClockReadCurrent(&clock);
		}
	}

	return 0;
}


#if 0
#define CMSEV_1MIN		0x00001

static int CTimerCMetStic(unsigned long arg)
{
	SysSendEvent(&CMetSticEvent, CMSEV_1MIN);

	return 0;
}

/**
* @brief �๦�ܱ��ͳ������
*/
static void *CenMetSticTask(void * arg)
{
	unsigned long ev;

	SysAddCTimer(60, CTimerCMetStic, 0);

	while(1) {
		SysWaitEvent(&CMetSticEvent, 1, CMSEV_1MIN, &ev);

		if(ev&CMSEV_1MIN) {
			UpdateMdbTGrpPower();// ����(����)�ܼ��鹦��
			UpdateMdbStic();
		}
	}
}
#endif

extern int MdbInit(void);
extern int IMetInit(void);
extern int CenMetAlarmInit(void);

/**
* @brief �๦�ܱ�ģ���ʼ��
* @return �ɹ�0, ����ʧ��
*/
DECLARE_INIT_FUNC(CenMetInit);

int CenMetInit(void)
{
	rtimer_conf_t conf;

	//Sleep(2000*6);
	MdbInit();
	//LoadCenMetProto();

	CMetCommInit();

	CenMetRmdInit();
	AutoReadCenMetInit();

	IMetInit();

	CenMetAlarmInit();

	conf.curtime = UTimeReadCurrent();
	conf.bonce = 0;
	memset(&conf.basetime, 0, sizeof(sysclock_t));


	conf.tdev = 1;
	conf.tmod = UTIMEDEV_MINUTE;  // 1 minute
	SysAddRTimer(&conf, RTimer1Minute, 0);

	//conf.tdev = 15;
	//conf.tmod = UTIMEDEV_MINUTE;  // 15 minute
	//SysAddRTimer(&conf, RTimer15Minute, 0);


	memset(&conf, 0, sizeof(conf));
	conf.basetime.year = 11;
	conf.basetime.month = 1;
	conf.basetime.day = 3;
	conf.basetime.hour = 23;
	conf.basetime.minute = 58;
	conf.basetime.second = 00;
	conf.curtime = UTimeReadCurrent();
	conf.bonce = 0;
	conf.tdev = 1;
	conf.tmod = UTIMEDEV_DAY;  // 1 day
	SysAddRTimer(&conf, RTimerDay, 0);

	if(SysCreateTask(CenMetTask, 0)) printf("create cenmet failed\n");
	//if(SysCreateTask(CenMetSticTask, 0)) printf("create cenmetstic failed\n");

	SET_INIT_FLAG(CenMetInit);

	return 0;
}

