/*****************************************************************************
	��̵����ɷ����޹�˾			��Ȩ��2008-2015

	��Դ���뼰������ĵ�Ϊ���������̵����ɷ����޹�˾�������У�δ��������
	�ɲ��������޸Ļ򷢲�������׷����صķ������Ρ�

						���������̹ɷ����޹�˾
						www.xjgc.com
						(0374) 321 2924
*****************************************************************************/


/******************************************************************************
	��Ŀ����	��SGE800���������ն�ƽ̨
	�ļ�		��syn.c
	����		�����ļ�ʵ����ͬ��ģ���е�API����
	�汾		��0.1
	����		������
	��������	��2009.12
******************************************************************************/


//������ͷ�ļ�
#include "private/config.h"

//ģ�����ÿ���
#ifdef CFG_SYN_MODULE

//����ͷ�ļ�
#include "private/debug.h"

//��������ͷ�ļ�

//C��ͷ�ļ�
#include <stdio.h>						//printf
#include <string.h>						//bzero
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>

//�ṩ���û���ͷ�ļ�
#include "include/syn.h"
#include "include/error.h"


/*************************************************
  ��̬ȫ�ֱ�������
*************************************************/
static struct{
	u8 state;                   //0��ʾδ��ʹ�ã�1��ʾ��ʹ��
	pthread_mutex_t mutex;		
}mutex_info[CFG_MUTEX_MAX];

static struct {
	u8 state;                   //0��ʾδ��ʹ�ã�1��ʾ��ʹ��
	sem_t sem;					
} sem_info[CFG_SEM_MAX];

static struct {
	u8 state;                   //0��ʾδ��ʹ�ã�1��ʾ��ʹ��
	pthread_rwlock_t rwlp;		       
}lock_info[CFG_RWLOCK_MAX];


/*************************************************
  API����ʵ��
*************************************************/
/******************************************************************************
*	����:	syn_init
*	����:	��ʼ��ͬ��ģ��
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����

*	˵��:	��ʼ�����ݿ⻷�������ݿ������
******************************************************************************/
int syn_init(void)
{
	int i;
	for (i=0; i<CFG_MUTEX_MAX; i++)    
	{
		mutex_info[i].state = 0;               //��ʼ��������ʹ��״̬
	}
	
	for (i=0; i<CFG_SEM_MAX; i++)    
	{
		sem_info[i].state = 0;                //��ʼ���ź�ʹ��״̬
	}
	
	for (i=0; i<CFG_RWLOCK_MAX; i++)    
	{
		lock_info[i].state = 0;               //��ʼ���ź�ʹ��״̬
	}
	return(0);

}

/******************************************************************************
*	����:	syn_mutex_init
*	����:	����������
*	����:	id				-	���������
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_BUSY		-	�˻������ѳ�ʼ��

*	˵��:	��
******************************************************************************/
int syn_mutex_init(u8 id)
{
	int ref;
	int ret = 0;
	
	const pthread_mutex_t mutex_zero = PTHREAD_MUTEX_INITIALIZER;
	if ((id < 0) || (id >= CFG_MUTEX_MAX))     //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	
	if (mutex_info[id].state == 1)
	{
		ret = -ERR_BUSY;
		goto error;
	}
	
	memcpy(&mutex_info[id].mutex, &mutex_zero, sizeof(pthread_mutex_t));
	ref = pthread_mutex_init(&mutex_info[id].mutex, NULL);
	if (ref == EBUSY)
	{
		ret = -ERR_BUSY;
		goto error;
	}
	else if (ref)
	{
		ret = -ERR_SYS;
		goto error;
	}
	else
	{
		ret = 0;
		mutex_info[id].state = 1;
	}

error:
	return(ret);
}

/******************************************************************************
*	����:	syn_mutex_lock
*	����:	����������
*	����:	id				-	���������
			timeout      	-	0Ϊ���õȴ���0xffffΪ���ȴ�������Ϊ�ȴ�����
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_NOINIT  	-	 �˻�����δ��ʼ����δʹ�ã�
 			-ERR_BUSY   	-	�ѱ������߳�����
 			-ERR_TIMEOUT 	-	��ʱ
*	˵��:	��
******************************************************************************/
int syn_mutex_lock(u8 id, u16 timeout)
{
	int ref;
	int ret = 0;
	struct timespec to;
	
	if ((id < 0) || (id >= CFG_MUTEX_MAX))      //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	if (mutex_info[id].state == 0)
	{
		ret = -ERR_NOINIT;
		goto error;
	}
	
	if (timeout == SYN_RECV_BLOCK){           //���õȴ�ģʽ
		ref = pthread_mutex_lock(&mutex_info[id].mutex);
		if (ref)
    	{
    		ret = -ERR_SYS;
    		goto error;
    	}
	}
	else if (timeout == SYN_RECV_NONBLOCK)  //���ȴ�ģʽ
	{
		ref = pthread_mutex_trylock(&mutex_info[id].mutex);
		if (ref== EBUSY)
		{
			ret = -ERR_BUSY;
			goto error;
		}
		else if (ref)
		{
			ret = -ERR_SYS;
			goto error;
		}
	}
	else                                      //��ʱ����
	{
    	to.tv_sec = time(NULL) + timeout;     //��
    	to.tv_nsec = 0;                       //����
    	ref = pthread_mutex_timedlock(&(mutex_info[id].mutex), &to);
		if (ref == ETIMEDOUT)
		{
			ret = -ERR_TIMEOUT;
			goto error;
		}
		else if(ref)
		{
			ret = -ERR_SYS;
			goto error;
		}
    }
error:
	return(ret);
	
}

/******************************************************************************
*	����:	syn_mutex_unlock
*	����:	���������
*	����:	id				-	���������
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_NOINIT  	-	 �˻�����δ��ʼ����δʹ�ã�
*	˵��:
******************************************************************************/
int syn_mutex_unlock(u8 id)
{
	int ref;
	int ret = 0;
	
	if ((id < 0) || (id >= CFG_MUTEX_MAX))     //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	if (mutex_info[id].state == 0)
	{
		ret = -ERR_NOINIT;
		goto error;
	}
	//���������
	ref = pthread_mutex_unlock(&mutex_info[id].mutex);
	if (ref)
	{
		ret = -ERR_SYS;
		goto error;
	}

error:
	return(ret);
}

/******************************************************************************
*	����:	syn_mutex_destroy
*	����:	���ٻ�����
*	����:	id				-	���������
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_NOINIT  	-	 �˻�����δ��ʼ����δʹ�ã�
 			-ERR_BUSY  		-	��������������
*	˵��:	��
******************************************************************************/
int syn_mutex_destroy(u8 id)
{
	int ref;
	int ret = 0;
	
	if ((id < 0) || (id >= CFG_MUTEX_MAX))     //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	
	if (mutex_info[id].state == 0)
	{
		ret = -ERR_NOINIT;
		goto error;
	}
	//���ٻ�����
	ref = pthread_mutex_destroy(&mutex_info[id].mutex);
	if (ref == EBUSY )
	{
		ret = -ERR_BUSY;
		goto error;
	}
	else if (ref)
	{
		ret = -ERR_SYS;
		goto error;
	}
	else
	{
		ret = 0;
		mutex_info[id].state = 0;
	}

error:
	return(ret);
}


/***************************�ź���************************************************/

/******************************************************************************
*	����:	syn_sem_init
*	����:	��ʼ���ź���
*	����:	id				-	�ź������
			value			-	�ź�����ֵ
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_BUSY  		-	 ���ź����ѳ�ʼ��
*	˵��:	��
******************************************************************************/
int syn_sem_init(u8 id, int value)
{
	int ref;
	int ret = 0;
	
	if ((id < 0) || (id >= CFG_SEM_MAX))     //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	
	if (sem_info[id].state == 1)
	{
		ret = -ERR_BUSY;
		goto error;
	}
	
	ref = sem_init(&sem_info[id].sem, 0, value);
	if (ref == EINVAL)
	{
		ret = -ERR_INVAL;
		goto error;
	}
	else if (ref)
	{
		ret = -ERR_SYS;
		goto error;
	}
	else
	{
		ret = 0;
		sem_info[id].state = 1;
	}
error:
	return (ret);	
}

/******************************************************************************
*	����:	syn_sem_wait
*	����:	�ź�����һ
*	����:	id				-	�ź������
			timeout    		-	0Ϊ���õȴ���0xffffΪ���ȴ�������Ϊ�ȴ�����
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_BUSY  		-	 �ź�����Ϊ����״̬��������������
*	˵��:	��
******************************************************************************/
int syn_sem_wait(u8 id, u16 timeout)
{
	int ref;
	int ret = 0;
	struct timespec to;
	int errnum;

	if ((id < 0) || (id >= CFG_SEM_MAX))        //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	if (sem_info[id].state == 0)
	{
		ret = -ERR_NOINIT;
		goto error;
	}
	
	if (timeout == SYN_RECV_BLOCK){           //���õȴ�ģʽ
		ref = sem_wait(&sem_info[id].sem);
		if (ref)
    	{
    		ret = -ERR_SYS;
    		goto error;
    	}
	}
	else if (timeout == SYN_RECV_NONBLOCK)  //���ȴ�ģʽ
	{
		ref = sem_trywait(&sem_info[id].sem);
		if (ref < 0)
		{
			errnum = errno;
			if (errnum == EAGAIN)                  //���ش���
			{
				ret = -ERR_BUSY;
				goto error;
			}
			else
			{
				ret = -ERR_SYS;
				goto error;
			}
		}
	}
	else                                      //��ʱ����
	{
    	to.tv_sec = time(NULL) + timeout;     //��
    	to.tv_nsec = 0;                       //����
    	ref = sem_timedwait(&(sem_info[id].sem), &to);
    	if (ref < 0)
		{
			errnum = errno;
			if (errnum == ETIMEDOUT)
			{
				ret = -ERR_TIMEOUT;
				goto error;
			}
			else
			{
				ret = -ERR_SYS;
				goto error;
			}
		}
}
error:
	return(ret);
}

/******************************************************************************
*	����:	syn_sem_post
*	����:	�ź�����һ��ͬʱ�����źŻ��ѵȴ�
*	����:	id				-	�ź������
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_NOINIT		-	���ź���δ��ʼ��
*	˵��:	��
******************************************************************************/

int syn_sem_post(u8 id)
{
	int ref;
	int ret = 0;
	
	if ((id < 0) || (id >= CFG_SEM_MAX))     //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	
	if (sem_info[id].state == 0)
	{
		ret = -ERR_NOINIT;
		goto error;
	}
	//�����ź�
	ref = sem_post(&sem_info[id].sem);
	if (ref)
	{
		ret = -ERR_SYS;
		goto error;
	}
	
error:
	return(ret);
}

/******************************************************************************
*	����:	syn_sem_getvalue
*	����:	����ź�����ֵ
*	����:	id				-	�ź������
			value      		-	�ź���ֵ�����أ�
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_NOINIT		-	���ź���δ��ʼ��
*	˵��:	��
******************************************************************************/
int syn_sem_getvalue(u8 id, int *value)
{
	int ref;
	int ret = 0;
	
	if ((id < 0) || (id >= CFG_SEM_MAX))     //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	if (sem_info[id].state == 0)
	{
		ret = -ERR_NOINIT;
		goto error;
	}
	ref = sem_getvalue(&sem_info[id].sem, value);
	if (ref)
	{
		ret = -ERR_SYS;
		goto error;
	}
	
error:
	return (ret);
	
}

/******************************************************************************
*	����:	syn_sem_destroy
*	����:	�����ź���
*	����:	id				-	�ź������
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_NOINIT		-	���ź���δ��ʼ��
*	˵��:	��
******************************************************************************/
int syn_sem_destroy(u8 id)
{
	int ref;
	int ret = 0;
	int errnum;
	
	if ((id < 0) || (id >= CFG_SEM_MAX))     //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	
	if (sem_info[id].state == 0)
	{
		ret = -ERR_NOINIT;
		goto error;
	}
	//���ٻ�����
	ref = sem_destroy(&sem_info[id].sem);
	if (ref < 0)
	{
		errnum = errno;
		if (errnum == EINVAL )
		{
			ret = -ERR_NOINIT;
			goto error;
		}
		else
		{
			ret = -ERR_SYS;
			goto error;
		}
	}
	else
	{
		mutex_info[id].state = 0;
	}

error:
	return(ret);
}


/***********************��д��************************************/
/******************************************************************************
*	����:	syn_rwlock_init
*	����:	��д����ʼ��
*	����:	id				-	��д�����
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_BUSY		-	�˶�д���ѳ�ʼ��
*	˵��:	��
******************************************************************************/
int syn_rwlock_init(u8 id)
{
	int ref;
	int ret = 0;
	
	if ((id < 0) || (id >= CFG_RWLOCK_MAX))     //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	
	if (lock_info[id].state == 1)
	{
		ret = -ERR_BUSY;
		goto error;
	}
	ref = pthread_rwlock_init(&lock_info[id].rwlp, NULL);
	if (ref == EBUSY)
	{
		ret = -ERR_BUSY;
		goto error;
	}
	else if (ref)
	{
		ret = -ERR_SYS;
		goto error;
	}
	else
	{
		ret = 0;
		lock_info[id].state = 1;
	}

error:
	return(ret);
}

/******************************************************************************
*	����:	syn_rwlock_rd
*	����:	��ȡ����
*	����:	id				-	��д�����
			timeout     	-	0:���õȴ���0xffff��ʾ���ȴ�����֧�ֳ�ʱʱ��
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_BUSY		-	�ѱ������߳�����
			-ERR_NOINIT  	-	�˶�д��δ��ʼ����δʹ�ã�
			-ERR_TIMEOUT	-	��ʱ
*	˵��:	��
******************************************************************************/
int syn_rwlock_rd(u8 id, u8 timeout)
{
	int ref;
	int ret = 0;
	
	if ((id < 0) || (id >= CFG_RWLOCK_MAX))     //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	if (lock_info[id].state == 0)
	{
		ret = -ERR_NOINIT;
		goto error;
	}
	
	if (timeout == SYN_RECV_BLOCK){           //���õȴ�ģʽ
		ref = pthread_rwlock_rdlock(&lock_info[id].rwlp);
		if (ref)
    	{
    		ret = -ERR_SYS;
    		goto error;
    	}
	}
	else if (timeout == SYN_RECV_NONBLOCK)  //���ȴ�ģʽ
	{
		ref = pthread_rwlock_tryrdlock(&lock_info[id].rwlp);
		if (ref == EBUSY)          //���ش���
		{
			ret = -ERR_BUSY;
			goto error;
		}
		else if (ref)
		{
			ret = -ERR_SYS;
			goto error;
		}
	}
	else                                      //��ʱ����
	{
    	ret = -ERR_INVAL;
    	goto error;
    }
error:
	return(ret);
	
}

/******************************************************************************
*	����:	syn_rwlock_wr
*	����:	��ȡд��
*	����:	id				-	��д�����
			timeout     	-	0:���õȴ���0xffff��ʾ���ȴ�����֧�ֳ�ʱʱ��
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_BUSY		-	�ѱ������߳�����
			-ERR_NOINIT  	-	�˶�д��δ��ʼ����δʹ�ã�
			-ERR_TIMEOUT	-	��ʱ
*	˵��:	��
******************************************************************************/
int syn_rwlock_wr(u8 id, u8 timeout)
{
	int ref;
	int ret = 0;
	
	if ((id < 0) || (id >= CFG_RWLOCK_MAX))     //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	if (lock_info[id].state == 0)
	{
		ret = -ERR_NOINIT;
		goto error;
	}
	
	if (timeout == SYN_RECV_BLOCK){           //���õȴ�ģʽ
		ref = pthread_rwlock_wrlock(&lock_info[id].rwlp);
		if (ref)
    	{
    		ret = -ERR_SYS;
    		goto error;
    	}
	}
	else if (timeout == SYN_RECV_NONBLOCK)  //���ȴ�ģʽ
	{
		ref = pthread_rwlock_trywrlock(&lock_info[id].rwlp);
		if (ref == EBUSY)                  //���ش���
		{
			ret = -ERR_BUSY;
			goto error;
		}
		else if (ref)
		{
			ret = -ERR_SYS;
			goto error;
		}
	}
	else                                      //��ʱ����
	{
    	ret = -ERR_INVAL;
    	goto error;
    }
error:
	return(ret);
}

/******************************************************************************
*	����:	syn_rwlock_unlock
*	����:	�����
*	����:	id				-	��д�����
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_NOINIT  	-	�˶�д��δ��ʼ����δʹ�ã�
*	˵��:	��
******************************************************************************/
int syn_rwlock_unlock(u8 id)
{
	int ref;
	int ret = 0;
	
	if ((id < 0) || (id >= CFG_RWLOCK_MAX))     //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	if (lock_info[id].state == 0)
	{
		ret = -ERR_NOINIT;
		goto error;
	}
	//���������
	ref = pthread_rwlock_unlock(&lock_info[id].rwlp);
	if (ref)
	{
		ret = -ERR_SYS;
		goto error;
	}
error:
	return(ret);
}

/******************************************************************************
*	����:	syn_rwlock_destroy
*	����:	���ٻ�����
*	����:	id				-	��д�����
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
			-ERR_NOINIT  	-	�˶�д��δ��ʼ����δʹ�ã�
			-ERR_BUSY 		-	��������������
*	˵��:	��
******************************************************************************/
int syn_rwlock_destroy(u8 id)
{
	int ref;
	int ret = 0;
	
	if ((id < 0) || (id >= CFG_RWLOCK_MAX))     //�жϽӿ�
	{
		ret = -ERR_INVAL;
		goto error;
	}
	
	if (lock_info[id].state == 0)
	{
		ret = -ERR_NOINIT;
		goto error;
	}
	//���ٻ�����
	ref = pthread_rwlock_destroy(&lock_info[id].rwlp);
	if (ref == EBUSY )
	{
		ret = -ERR_BUSY;
		goto error;
	}
	else if (ref)
	{
		ret = -ERR_SYS;
		goto error;
	}
	else
	{
		ret = 0;
		lock_info[id].state = 0;
	}

error:
	return(ret);
}

#endif      /* CFG_SYN_MODULE */
