/**
* rs485.c -- RS485�����ӿ�ͷ�ļ�
* 
* 
* ����ʱ��: 2010-4-24
* ����޸�ʱ��: 2010-4-27
*/

#ifndef _RS485_H
#define _RS485_H

//RS485 �˿���
#define RS485_PORTNUM	3

/**
* @brief ��λRS485�˿�
* @param port RS485�˿ں�
* @return 0�ɹ�, ����ʧ��
*/
int Rs485Reset(unsigned int port);
/**
* @brief ����RS485�˿ڲ����ʺ�����
* @param port �˿ں�
* @param baud ������
* @param databits ����λ, 5~8
* @param stopbits ֹͣλ, 1~2
* @param parity У��λ
*/
void Rs485Set(unsigned int port, int baud, int databits, int stopbits, char parity);
/**
* @brief ��RS485�˿ڷ�������
* @param port �˿ں�
* @param buf ���ͻ�����
* @param len ����������
* @param 0�ɹ�, ����ʧ��
*/
int Rs485Send(unsigned int port, const unsigned char *buf, int len);
/**
* @brief ��RS485�˿ڽ�������
* @param port �˿ں�
* @param buf ���ջ�����
* @param len ����������
* @return ʧ�ܷ���-1, �ɹ����ؽ��յ����ֽ���, ����0��ʾδ���յ�����
*/
int Rs485Recv(unsigned int port, unsigned char *buf, int len);

/**
* @brief ��סRs485�˿�
* @param port �˿ں�
*/
void Rs485Lock(unsigned int port);
/**
* @brief ����Rs485�˿�
* @param port �˿ں�
*/
void Rs485Unlock(unsigned int port);

#endif /*_RS485_H*/
