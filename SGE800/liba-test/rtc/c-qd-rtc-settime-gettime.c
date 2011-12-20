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
    �ļ���      ��  c-qd-rtc-setwaittime-getwaittime.c
    ����        ��  ���ļ�����ƽ̨��rtc_setwaittime,rtc_getwaittime�����Ĳ���
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
#include "../include/rtc.h"

int main()
{
	int ret1 = -1, ret2 = -1;
	rtc_time_t set_time, get_time;

	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 13;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 6;

	//������ʼ��
	inittest();
	rtc_init();

	//��������1
	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 5) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 6) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 6)) \
			,"rtc_setwaittime,rtc_getwaittime 1 error");

	//��������2
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 30;
	set_time.mon = 9;
	set_time.year = 123;
	set_time.wday = 2;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 5) \
			&& (get_time.day == 30) \
			&& (get_time.mon == 9) \
			&& (get_time.year == 123) \
			&& (get_time.wday == 2)) \
			,"rtc_setwaittime,rtc_getwaittime 2 error");

	//��������3
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 29;
	set_time.mon = 2;
	set_time.year = 4;
	set_time.wday = 2;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 5) \
			&& (get_time.day == 29) \
			&& (get_time.mon == 2) \
			&& (get_time.year == 4) \
			&& (get_time.wday == 2)) \
			,"rtc_setwaittime,rtc_getwaittime 3 error");
	printf("ret1 = %d\r\n",ret1);
	printf("ret2 = %d\r\n",ret2);
	printf("get_time.sec = %d\r\n",get_time.sec);
	printf("get_time.min = %d\r\n",get_time.min);
	printf("get_time.hour = %d\r\n",get_time.hour);
	printf("get_time.day = %d\r\n",get_time.day);
	printf("get_time.mon = %d\r\n",get_time.mon);
	printf("get_time.year = %d\r\n",get_time.year);
	printf("get_time.wday = %d\r\n",get_time.wday);

	//��������4
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 31;
	set_time.mon = 12;
	set_time.year = 122;
	set_time.wday = 6;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 5) \
			&& (get_time.day == 31) \
			&& (get_time.mon == 12) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 6)) \
			,"rtc_setwaittime,rtc_getwaittime 4 error");

	//��������5
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 28;
	set_time.mon = 2;
	set_time.year = 101;
	set_time.wday = 2;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 5) \
			&& (get_time.day == 28) \
			&& (get_time.mon == 2) \
			&& (get_time.year == 101) \
			&& (get_time.wday == 2)) \
			,"rtc_setwaittime,rtc_getwaittime 5 error");

	//��������6
	set_time.sec = 0;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 13;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 6;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 0) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 5) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 6) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 6)) \
			,"rtc_setwaittime,rtc_getwaittime 6 error");

	//��������7
	set_time.sec = 59;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 13;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 6;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 59) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 5) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 6) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 6)) \
			,"rtc_setwaittime,rtc_getwaittime 7 error");

	//��������8
	set_time.sec = 2;
	set_time.min = 0;
	set_time.hour = 5;
	set_time.day = 13;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 6;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 0) \
			&& (get_time.hour == 5) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 6) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 6)) \
			,"rtc_setwaittime,rtc_getwaittime 8 error");

	//��������9
	set_time.sec = 2;
	set_time.min = 59;
	set_time.hour = 5;
	set_time.day = 13;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 6;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 59) \
			&& (get_time.hour == 5) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 6) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 6)) \
			,"rtc_setwaittime,rtc_getwaittime 9 error");

	//��������10
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 0;
	set_time.day = 13;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 4;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 0) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 6) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 4)) \
			,"rtc_setwaittime,rtc_getwaittime 10 error");

	//��������11
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 23;
	set_time.day = 13;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 6;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 23) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 6) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 6)) \
			,"rtc_setwaittime,rtc_getwaittime 11 error");

	//��������12
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 1;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 3;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 5) \
			&& (get_time.day == 1) \
			&& (get_time.mon == 6) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 3)) \
			,"rtc_setwaittime,rtc_getwaittime 12 error");

	//��������13
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 30;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 5;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 5) \
			&& (get_time.day == 30) \
			&& (get_time.mon == 6) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 5)) \
			,"rtc_setwaittime,rtc_getwaittime 13 error");

	//��������14
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 12;
	set_time.day = 13;
	set_time.mon = 12;
	set_time.year = 122;
	set_time.wday = 6;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 12) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 12) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 6)) \
			,"rtc_setwaittime,rtc_getwaittime 14 error");

	//��������15
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 12;
	set_time.day = 13;
	set_time.mon = 1;
	set_time.year = 122;
	set_time.wday = 2;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 12) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 1) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 2)) \
			,"rtc_setwaittime,rtc_getwaittime 15 error");

	//��������16
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 12;
	set_time.day = 13;
	set_time.mon = 2;
	set_time.year = 0;
	set_time.wday = 2;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 12) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 2) \
			&& (get_time.year == 0) \
			&& (get_time.wday == 2)) \
			,"rtc_setwaittime,rtc_getwaittime 16 error");

	//��������17
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 12;
	set_time.day = 13;
	set_time.mon = 2;
	set_time.year = 255;
	set_time.wday = 6;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 12) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 2) \
			&& (get_time.year == 255) \
			&& (get_time.wday == 6)) \
			,"rtc_setwaittime,rtc_getwaittime 17 error");

	//��������18
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 12;
	set_time.day = 13;
	set_time.mon = 1;
	set_time.year = 122;
	set_time.wday = 0;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 12) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 1) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 0)) \
			,"rtc_setwaittime,rtc_getwaittime 18 error");

	//��������19
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 12;
	set_time.day = 13;
	set_time.mon = 1;
	set_time.year = 122;
	set_time.wday = 6;

	ret1 = rtc_settime(&set_time);
	ret2 = rtc_gettime(&get_time);
	assert(((ret1 == 0) \
			&& (ret2 == 0) \
			&& (get_time.sec == 2) \
			&& (get_time.min == 10) \
			&& (get_time.hour == 12) \
			&& (get_time.day == 13) \
			&& (get_time.mon == 1) \
			&& (get_time.year == 122) \
			&& (get_time.wday == 6)) \
			,"rtc_setwaittime,rtc_getwaittime 19 error");


	//��������20
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 31;
	set_time.mon = 9;
	set_time.year = 123;
	set_time.wday = 2;
	ret1 = rtc_settime(&set_time);
	assert(ret1 == -ERR_INVAL,"rtc_setwaittime 20 error");

	//��������21
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 29;
	set_time.mon = 2;
	set_time.year = 101;
	set_time.wday = 2;
	ret1 = rtc_settime(&set_time);
	assert(ret1 == -ERR_INVAL,"rtc_setwaittime 21 error");

	//��������22
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 30;
	set_time.mon = 2;
	set_time.year = 100;
	set_time.wday = 5;
	ret1 = rtc_settime(&set_time);
	assert(ret1 == -ERR_INVAL,"rtc_setwaittime 22 error");

	//��������23
	set_time.sec = 60;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 13;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 6;
	ret1 = rtc_settime(&set_time);
	assert(ret1 == -ERR_INVAL,"rtc_setwaittime 23 error");

	//��������24
	set_time.sec = 2;
	set_time.min = 60;
	set_time.hour = 5;
	set_time.day = 13;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 6;
	ret1 = rtc_settime(&set_time);
	assert(ret1 == -ERR_INVAL,"rtc_setwaittime 24 error");

	//��������25
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 24;
	set_time.day = 13;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 4;
	ret1 = rtc_settime(&set_time);
	assert(ret1 == -ERR_INVAL,"rtc_setwaittime 25 error");

	//��������26
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 0;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 3;
	ret1 = rtc_settime(&set_time);
	assert(ret1 == -ERR_INVAL,"rtc_setwaittime 26 error");

	//��������27
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 5;
	set_time.day = 32;
	set_time.mon = 6;
	set_time.year = 122;
	set_time.wday = 3;
	ret1 = rtc_settime(&set_time);
	assert(ret1 == -ERR_INVAL,"rtc_setwaittime 27 error");

	//��������28
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 12;
	set_time.day = 13;
	set_time.mon = 0;
	set_time.year = 122;
	set_time.wday = 6;
	ret1 = rtc_settime(&set_time);
	assert(ret1 == -ERR_INVAL,"rtc_setwaittime 28 error");

	//��������29
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 12;
	set_time.day = 13;
	set_time.mon = 13;
	set_time.year = 122;
	set_time.wday = 6;
	ret1 = rtc_settime(&set_time);
	assert(ret1 == -ERR_INVAL,"rtc_setwaittime 29 error");

	//��������30
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 12;
	set_time.day = 13;
	set_time.mon = 1;
	set_time.year = 122;
	set_time.wday = 8;
	ret1 = rtc_settime(&set_time);
	assert(ret1 == -ERR_INVAL,"rtc_setwaittime 30 error");

	//��������31
	rtc_close();
	set_time.sec = 2;
	set_time.min = 10;
	set_time.hour = 12;
	set_time.day = 13;
	set_time.mon = 1;
	set_time.year = 122;
	set_time.wday = 6;
	ret1 = rtc_settime(&set_time);
	assert(ret1 == -ERR_NOINIT,"rtc_setwaittime 31 error");

	finaltest();
	return 0;
}
