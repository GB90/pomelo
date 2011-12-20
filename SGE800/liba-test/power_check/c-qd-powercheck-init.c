/*****************************************************************************/
/*��̵����ɷ����޹�˾                                     ��Ȩ��2008-2015   */
/*****************************************************************************/
/* ��Դ���뼰������ĵ�Ϊ���������̵����ɷ����޹�˾�������У�δ��������    */
/* �ɲ��������޸Ļ򷢲�������׷����صķ������Ρ�                          */
/*                                                                           */
/*                      ���������̹ɷ����޹�˾                             */
/*                      www.xjgc.com                                         */
/*                      (0374) 321 2924                                      */
/*                                                                           */
/*****************************************************************************/


/******************************************************************************
    ��Ŀ����    ��  SGE800���������ն�ƽ̨
    �ļ���      ��  c-qd-powercheck-init.c
    ����        ��  ���ļ�����ƽ̨��powercheck_init�����Ĳ���
    �汾        ��  0.1
    ����        ��  ��ΰ��
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>		//exit
#include <unistd.h>		//sleep
#include <db.h>

//ƽ̨��ͷ�ļ�
#include "../include/dbs.h"
#include "../include/xj_assert.h"
#include "../include/error.h"

//���Գ���ͷ�ļ�
#include "../include/powercheck.h"

int main()
{
	int ret1 = -1, ret2 = -1, ret3 = -1, ret4 = -1;
	u8 mode;
	//������ʼ��
	inittest();

	//��������1
	mode = POWERCHECK_MODE_NOBLOCK;
	ret1 = powercheck_init(mode);
	assert(ret1 == 0,"powercheck_init 1 error");
	powercheck_close();

	//��������2
	mode = POWERCHECK_MODE_BLOCK_UP;
	ret2 = powercheck_init(mode);
	assert(ret2 == 0,"powercheck_init 2 error");
	powercheck_close();

	//��������3
	mode = POWERCHECK_MODE_BLOCK_DOWN;
	ret3 = powercheck_init(mode);
	assert(ret3 == 0,"powercheck_init 3 error");
	powercheck_close();

	//��������4
	mode = POWERCHECK_MODE_BLOCK_UPDOWN;
	ret4 = powercheck_init(mode);
	assert(ret4 == 0,"powercheck_init 4 error");
	powercheck_close();

	// ��������5
	mode = POWERCHECK_MODE_NOBLOCK;
	ret4 = powercheck_init(mode);
	assert(ret4 == -ERR_NOFILE,"powercheck_init 5 error");
	powercheck_close();

	// ��������6
	mode = POWERCHECK_MODE_NOBLOCK;
	ret4 = powercheck_init(mode);
	ret4 = powercheck_init(mode);
	assert(ret4 == -ERR_BUSY,"powercheck_init 6 error");
	powercheck_close();

	// ��������7
	mode = 13;
	ret4 = powercheck_init(mode);
	assert(ret4 == -ERR_INVAL,"powercheck_init 7 error");
	powercheck_close();

	finaltest();
	return 0;
}

