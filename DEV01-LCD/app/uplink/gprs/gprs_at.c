/**
* gprs_at.c -- GPRSģ��AT�����
* 
* 
* ����ʱ��: 2010-5-19
* ����޸�ʱ��: 2010-5-19
*/

#include <stdio.h>
#include <string.h>

#include "include/debug.h"
#include "include/sys/schedule.h"
#include "gprs_hardware.h"
#include "gprs_at.h"

unsigned long gprs_atclr;

/**
* @brief ����AT��������Ӧ
* @param list AT�����Ӧ�б�
* @param timeout ��ʱʱ��(��)
* @return �ɹ�������Ӧ������ֵ, ���򷵻�-1
*/
int GprsSendAt(const atlist_t *list, int timeout)
{
	int count, i;
	char c;
	int pos[4] = {0, 0, 0, 0};

	timeout *= 10;

	GPRSAT_CLRRCV;

	GprsLineSend((unsigned char *)(list->cmd), strlen(list->cmd));

	Sleep(10);

	for(count=0; count<timeout; count++) {
		while(GprsLineRecv((unsigned char *)&c, 1) > 0) {
			//DebugPrint(0, "atr:%c\r\n", c);
			for(i=0; i<list->keynum; i++) {
				if(c == list->key[i][pos[i]]) {
					pos[i]++;
					if(0 == list->key[i][pos[i]]) {
						Sleep(10);
						GPRSAT_CLRRCV;
						return i;
					}
				}
				else pos[i] = 0;
			}
		}

		Sleep(10);
	}

	return -1;
}
