/**
* rs485bus.h -- RS485���߳���ӿ�ͷ�ļ�
* 
* ����: dongqi
* ����ʱ��: 2010-1-19
* ����޸�ʱ��: 2010-1-19
*/

#ifndef _RS485BUS_H
#define _RS485BUS_H

#define RS485BUS_BUF_LEN		272
int Rs485BusRead(const plc_dest_t *dest, unsigned short itemid, unsigned char *buf, int len);

#endif/*_RS485BUS_H*/

