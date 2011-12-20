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
    �ļ���      ��  c-qd-powercheck-setwaittime-getwaittime.c
    ����        ��  ���ļ�����ƽ̨��powercheck_setwaittime,powercheck_getwaittime�����Ĳ���
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

//���Գ���ͷ�ļ�
#include "../include/powercheck.h"

int main()
{
	int ret1 = -1, ret2 = -1;
	u8 mode = POWERCHECK_MODE_BLOCK_UPDOWN;
	u16 set_timeout, get_timeout;

	//������ʼ��
	inittest();
	powercheck_init(mode);

	//��������1
	set_timeout = 10;
	ret1 = powercheck_setwaittime(set_timeout);
	ret2 = powercheck_getwaittime(&get_timeout);
	assert(((ret1 == 0) && (ret2 == 0) && (get_timeout == 100)),\
			"powercheck_setwaittime,powercheck_getwaittime 1 error");


	//��������2
	set_timeout = 0xffff;
	ret1 = powercheck_setwaittime(set_timeout);
	ret2 = powercheck_getwaittime(&get_timeout);
	assert(((ret1 == 0) && (ret2 == 0) && (get_timeout == 0xffff)),\
			"powercheck_setwaittime,powercheck_getwaittime 2 error");
	printf("ret1 = %d\r\n",ret1);
	printf("ret2 = %d\r\n",ret2);
	printf("get_timeout = 0x%x\r\n",get_timeout);

	//��������3
	set_timeout = 0;
	ret1 = powercheck_setwaittime(set_timeout);
	ret2 = powercheck_getwaittime(&get_timeout);
	assert(((ret1 == 0) && (ret2 == 0) && (get_timeout == 0)),\
			"powercheck_setwaittime,powercheck_getwaittime 3 error");
	//��������4

	// ��������5
	finaltest();
	return 0;
}


