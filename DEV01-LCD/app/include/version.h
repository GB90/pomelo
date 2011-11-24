/**
* version.h -- �汾��ͷ�ļ�
* 
* 
* ����ʱ��: 2010-3-31
* ����޸�ʱ��: 2010-5-6
*/

#ifndef _VERSION_H
#define _VERSION_H

//���̰汾��
#define VERPROJ_MAIN		0
#define VERPROJ_TEST		1

#define VERSION_MAJOR		0   //���汾��
#define VERSION_MINOR		10   //�ΰ汾��

#define VERSION_PROJECT		VERPROJ_TEST   //���̰汾��

//�汾��������
#if 0
#define RELEASE_DATE_YEAR	0x09
#define RELEASE_DATE_MONTH	0x05
#define RELEASE_DATE_DAY	0x05
#else
extern const unsigned char _mk_year;
extern const unsigned char _mk_month;
extern const unsigned char _mk_day;
extern const unsigned char _mk_hour;
extern const unsigned char _mk_minute;
#endif

extern const char *VerStringProj[];
#define STRING_PROJECT		VerStringProj[VERSION_PROJECT]

#endif /*_VERSION_H*/

