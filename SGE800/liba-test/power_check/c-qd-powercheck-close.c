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
    �ļ���      ��  c-qd-powercheck-close.c
    ����        ��  ���ļ�����ƽ̨��powercheck_close�����Ĳ���
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
	int ret = -1;
	u8 mode = POWERCHECK_MODE_BLOCK_UPDOWN;
	//������ʼ��
	inittest();
	powercheck_init(mode);
	//��������1
	ret = powercheck_close();
	assert(ret == 0,"powercheck_close error");
	//��������2
	ret = powercheck_close();
	assert(ret == -ERR_NOINIT,"powercheck_close error");
	//��������3

	//��������4

	// ��������5
	finaltest();
	return 0;
}

