/**
* svrnote.h -- ������ͨ��������������
* 
* 
* ����ʱ��: 2010-5-19
* ����޸�ʱ��: 2010-5-19
*/

#ifndef _SVRCOMM_NOTE_H
#define _SVRCOMM_NOTE_H

#define SVREV_NOTE				0x00000001

#define SVRNOTEMASK_ALARM		0x80000000
#define SVRNOTEMASK_TASKCLS2	0x00000001
#define SVRNOTEMASK_TASKCLS1	0x00000001

#define SVRNOTEID_ALARM		32
#define SVRNOTEID_TASKCLS2(id)		(id+33)
#define SVRNOTEID_TASKCLS1(id)		(id+1)

void SvrCommNote(int id);


#endif /*_SVRCOMM_NOTE_H*/

