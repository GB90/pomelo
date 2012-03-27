/*****************************************************************************
	���̵����ɷ����޹�˾			��Ȩ��2008-2015

	��Դ���뼰������ĵ�Ϊ�����������̵����ɷ����޹�˾�������У�δ��������
	�ɲ��������޸Ļ򷢲�������׷����صķ������Ρ�

						�����������̹ɷ����޹�˾
						www.xjgc.com
						(0374) 321 2924
*****************************************************************************/


/******************************************************************************
	��Ŀ����	��SGE800���������ն�ƽ̨
	�ļ�		��thread.c
	����		�����ļ�ʵ�����̲߳���ģ���е�API����
	�汾		��0.1
	����		����迡
	��������	��2009.12
******************************************************************************/
//������ͷ�ļ�
#include "private/config.h"

//ģ�����ÿ���
#ifdef CFG_THREAD_MODULE

//����ͷ�ļ�
#include "private/debug.h"

//��������ͷ�ļ�

//C��ͷ�ļ�
#include <stdio.h>                      //printf
#include <pthread.h>                    //pthread


//�ṩ���û���ͷ�ļ�
#include "include/thread.h"
#include "include/error.h"


/*************************************************
  ��̬ȫ�ֱ�������
*************************************************/
static struct {
    u8 prio;                    //0��ʾ��ͨ�̣߳�1-99��ʾʵʱ�߳����ȼ���
    pthread_t tid;              //0��ʾδ������[0]=0��ʾ�߳�ģ��δ��ʼ��
} thread_info[CFG_THREAD_MAX];

/*************************************************
  API����ʵ��
*************************************************/
/******************************************************************************
*	����:	thread_create
*	����:	�����߳�
*	����:	id				-	�̺߳�
 			function		-	�̺߳���
 			arg				-	�̺߳�������
 			mode			-	�߳�ģʽ����ͨ��ʵʱ��
 			prio			-	���ȼ���1-99��
*	����:	0				-	�ɹ�
 			-ERR_NOINIT		-	ģ��δ��ʼ��
 			-ERR_INVAL		-	��������ȷ
 			-ERR_BUSY		-	�߳��Ѵ���
 			-ERR_NOMEM		-	�ڴ治��
*	˵��:	ֻ��ʵʱģʽ֧�����ȼ���99Ϊ������ȼ�����ͨģʽ���ȼ��̶�Ϊ0��
 ******************************************************************************/
int thread_create(u8 id, void *(*function)(void *), void *arg, u8 mode, u8 prio)
{
    int ret = 0;
    pthread_t tid;
    pthread_attr_t attr;
    struct sched_param param;
    if (0 == thread_info[0].tid) {
        ret = -ERR_NOINIT;
        goto error;
    }
    if (id > CFG_THREAD_MAX - 1) {
        ret = -ERR_INVAL;
        goto error;
    }
    if (0 != thread_info[id].tid) {
        ret = -ERR_BUSY;
        goto error;
    }
//��ʼ���߳�����
    ret = pthread_attr_init(&attr);
    if (ret) {
        ret = -ERR_NOMEM;
        goto error;
    }
//�����߳�Ϊ����״̬
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (ret) {
        ret = -ERR_INVAL;
        goto error;
    }
//�������ȼ�
    if (THREAD_MODE_NORMAL == mode) {
        if (0 != prio) {
            ret = -ERR_INVAL;
            goto error;
        }
    }
    else if (THREAD_MODE_REALTIME == mode) {
        if ((prio < 1) || (prio > 99)) {			//1��ʵʱ�߳�������ȼ���99��������ȼ�
            ret = -ERR_INVAL;
            goto error;
        }
        ret = pthread_attr_setschedpolicy(&attr, SCHED_RR);
        if (ret) {
            ret = -ERR_INVAL;
            goto error;
        }
        param.sched_priority = prio;
        ret = pthread_attr_setschedparam(&attr, &param);
        if (ret) {
            ret = -ERR_INVAL;
            goto error;
        }
    }
    else {
        ret = -ERR_INVAL;
        goto error;
    }
//�����߳�
    ret = pthread_create (&tid, &attr, function, arg);
    if (ret) {
        ret = -ERR_NOMEM;
        goto error;
    }
//�����߳���Ϣ
    thread_info[id].prio = prio;
    thread_info[id].tid = tid;

error:
    return ret;
}

/******************************************************************************
*	����:	thread_cancel
*	����:	��ֹ�߳�
*	����:	id				-	�̺߳�
*	����:	0				-	�ɹ�
 			-ERR_NODEV		-	�޴��߳�
 			-ERR_INVAL		-	��������
*	˵��:
 ******************************************************************************/
int thread_cancel(u8 id)
{
    int ret = 0;

    if (id > CFG_THREAD_MAX - 1) {
        ret = -ERR_INVAL;
        goto error;
    }
    if (0 == thread_info[id].tid) {
        ret = -ERR_NODEV;
        goto error;
    }
//��ֹ�߳�
    ret = pthread_cancel (thread_info[id].tid);
    if (ret) {
        ret = -ERR_NODEV;
        goto error;
    }
//�����߳���Ϣ
    thread_info[id].prio = 0;
    thread_info[id].tid = 0;

error:
    return ret;
}

/******************************************************************************
*	����:	thread_setpriority
*	����:	�����߳����ȼ�
*	����:	id				-	�̺߳�
 			prio			-	���ȼ�
*	����:	0				-	�ɹ�
 			-ERR_NODEV		-	�޴��߳�
 			-ERR_INVAL		-	��������
*	˵��:	���ȼ�Ϊ1-99��99Ϊ������ȼ���������������ͨ�߳����ȼ������̲߳��ܴ���ֵ��
 ******************************************************************************/
int thread_setpriority(u8 id, u8 prio)
{
    int ret = 0;
    struct sched_param param;

    if (id > CFG_THREAD_MAX - 1) {
        ret = -ERR_INVAL;
        goto error;
    }
    if (0 == thread_info[id].tid) {
        ret = -ERR_NODEV;
        goto error;
    }
//��ͨ�̵߳���������̲߳��ܴ���ֵ��idΪ0��
    if ((0 == thread_info[id].prio) && (id != 0)) {
        if (0 != prio) {
            ret = -ERR_INVAL;
            goto error;
        }
    }
//ʵʱ�̵߳����
    else {
        if ((prio < 1) || (prio > 99)) {			//1��ʵʱ�߳�������ȼ���99��������ȼ�
            ret = -ERR_INVAL;
            goto error;
        }
        param.sched_priority = prio;
        ret = pthread_setschedparam(thread_info[id].tid, SCHED_RR, &param);
        if(ret){
            ret = -ERR_INVAL;
            goto error;
        }
    }
//�����߳���Ϣ
    thread_info[id].prio = prio;

error:
    return ret;
}

/******************************************************************************
*	����:	thread_getpriority
*	����:	��ȡ���ȼ�
*	����:	id				-	�̺߳�
 			prio			-	���ȼ������ݴ�����
*	����:	0				-	�ɹ�
 			-ERR_NODEV		-	�޴��߳�
 			-ERR_INVAL		-	��������
*	˵��:	ʵʱ�߳����ȼ�Ϊ1-99��99Ϊ������ȼ�����ͨ�̶̹߳�Ϊ0��
 ******************************************************************************/
int thread_getpriority(u8 id, u8 *prio)
{
    int ret = 0;

    if (id > CFG_THREAD_MAX - 1) {
        ret = -ERR_INVAL;
        goto error;
    }
    if (0 == thread_info[id].tid) {
        ret = -ERR_NODEV;
        goto error;
    }

    *prio = thread_info[id].prio;

error:
    return ret;
}

/******************************************************************************
*	����:	thread_init
*	����:	ģ���ʼ��
*	����:	��
*	����:	0				-	�ɹ�
*	˵��:	���������߳��е���
 ******************************************************************************/
int thread_init(void)
{
    int ret = 0;

    thread_info[0].tid = pthread_self();

    return ret;
}

#endif /* CFG_THREAD_MODULE */