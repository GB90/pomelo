/**
* qrydata.h -- ��ѯ����
* 
* 
* ����ʱ��: 2010-6-2
* ����޸�ʱ��: 2010-6-2
*/

#ifndef _QRY_DATA_H
#define _QRY_DATA_H

int ReadMdb(const unsigned char *pnfn,unsigned char *buf,int len,int *pactlen);
int ReadPlMdb(const unsigned char *pnfn,unsigned char *buf,int len,int *pactlen);

#endif /*_QRY_DATA_H*/

