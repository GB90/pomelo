/**
* flat.h -- ƽ������
* 
* 
* ����ʱ��: 2010-5-15
* ����޸�ʱ��: 2010-5-15
*/

#ifndef _SYS_FLAT_H
#define _SYS_FLAT_H

/**
* @brief ��ȡFLAT�ļ�����
* @param sector �ļ�������(0~3)
* @param buf ������ָ��
* @param len ����������
* @return �ɹ�����ʵ�ʶ�ȡ����, ʧ�ܷ���-1
*/
int ReadFlatFile(unsigned int sector, unsigned char *buf, int len);

/**
* @brief д��FLAT�ļ�����
* @param sector �ļ�������(0~3)
* @param buf ������ָ��
* @param len ����������
* @return �ɹ�����ʵ��д�볤��, ʧ�ܷ���-1
*/
int WriteFlatFile(unsigned int sector, const unsigned char *buf, int len);

#endif /*_SYS_FLAT_H*/

