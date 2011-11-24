#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFINE_RUNSTATE

#include "include/debug.h"
#include "include/sys/syslock.h"
#include "include/sys/flat.h"
#include "include/sys/cycsave.h"
#include "include/monitor/runstate.h"
#include "include/debug/shellcmd.h"

#define FLATID_RUNSTATE		2

runstate_t RunState;

/**
* @brief ��ȡ����״̬�޸�ָ��
*/
runstate_t *RunStateModify(void)
{
	return &RunState;
}

/**
* @brief �������״̬
*/
void ClearRunState(void)
{
	memset(&RunState, 0, sizeof(RunState));
}

DECLARE_INIT_FUNC(RunStateInit);
int RunStateInit(void)
{
	memset(&RunState, 0, sizeof(RunState));
	DebugPrint(0, "  load runstate(size=%d)...", sizeof(RunState));
	if(ReadFlatFile(FLATID_RUNSTATE, (unsigned char *)&RunState, sizeof(RunState)) > 0) {
		DebugPrint(0, "ok\n");
	}
	else {
		DebugPrint(0, "fail\n");
	}

	SET_INIT_FLAG(RunStateInit);
	return 0;
}

/**
* @brief ��������״̬
*/
void SaveRunState(void)
{
	WriteFlatFile(FLATID_RUNSTATE, (unsigned char *)&RunState, sizeof(RunState));
}
DECLARE_CYCLE_SAVE(SaveRunState, 0);

int shell_runstate(int argc, char *argv[])
{
	PrintLog(0, "ʱ�ӵ��: %s\n", RunState.battery?"��":"����");
	PrintLog(0, "�е�״̬: %s\n", RunState.pwroff?"��":"��");
	PrintLog(0, "ң��״̬: %02X\n", RunState.isig_stat);
	PrintLog(0, "ң�ű�λ: %02X\n", RunState.isig_chg);

	PrintLog(0, "��س��: %s\n", RunState.batcharge?"�����":"ֹͣ");
	PrintLog(0, "�������: %s\n", RunState.batbad?"δ����":"����");
	PrintLog(0, "�ն��޳�: %s\n\n", RunState.outgrp?"Ͷ��":"���");

	PrintLog(0, "��Ҫ�¼�: head=%d, cur=%d, snd=%d\n", 
		RunState.alarm.head[0], RunState.alarm.cur[0], RunState.alarm.snd[0]);
	PrintLog(0, "һ���¼�: head=%d, cur=%d, snd=%d\n", 
		RunState.alarm.head[1], RunState.alarm.cur[1], RunState.alarm.snd[1]);
	PrintLog(0, "�¼���־: ");
	PrintHexLog(0, RunState.alarm.stat, 8);
	PrintLog(0, "\n");

	PrintLog(0, "softchg=");
	PrintHexLog(0, RunState.softchg.ver, 8);

	PrintLog(0, "cnt_snderr= %d, %d\n", RunState.cnt_snderr[0], RunState.cnt_snderr[1]);
	PrintLog(0, "flag_acd= %d\n", RunState.flag_acd);

	PrintLog(0, "timepoweroff= ");
	PrintHexLog(0, RunState.timepoweroff, 8);

	PrintLog(0, "malmflag= ");
	PrintHexLog(0, RunState.malmflag, LEN_MALM_STAT);

	PrintLog(0, "\n");
	return 0;
}
DECLARE_SHELL_CMD("runstate", shell_runstate, "��ʾ����״̬����");

