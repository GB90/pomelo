/**
* event.h -- ����ӿ�ͷ�ļ�
* 
* 
* ����ʱ��: 2010-3-30
* ����޸�ʱ��: 2010-3-31
*/

#ifndef _SYS_TASK_H
#define _SYS_TASK_H

#include <unistd.h>

/**
* @brief ����һ������(�߳�)
* @param routine ������ʼִ�к���
* @param arg �ɹ�����0, ʧ�ܷ��ط���ֵ
*/
int SysCreateTask(void *(*routine)(void *), void *arg);

/**
* @brief �ȴ���������(�߳�)����,�����̵���
*/
void SysWaitTaskEnd(void);

extern int SysTimerTask_watchdog;
extern int EthShellTask_watchdog;
extern int PlFrezTask_watchdog;
extern int PlcTask_watchdog;
extern int GprsActiveTask_watchdog;
extern int PlcTask_watchdog;
extern int PlFrezTask_watchdog;
#endif /*_SYS_TASK_H*/
