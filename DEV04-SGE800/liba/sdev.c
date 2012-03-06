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
    ��Ŀ����    ��  SGE800�����ն�ҵ��ƽ̨
    �� �� ��       ��  gprs_me3000.c
    ��          ��    ��  ���ļ�����ҵ��ƽ̨��gprs���ܵ�ʵ��
    ��         ��     ��  0.1
    ��          ��    ��  ����
    ��������    ��  2010.04
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>		    //exit
#include <unistd.h>		    //sleep
#include <string.h>
#include <pthread.h>
#include <errno.h>

//ƽ̨��ͷ�ļ�
#include "include/comport.h"
#include "include/gprs_me3000.h"
#include "include/sdev_485.h"
#include "include/sdev_net.h"
#include "private/config.h"
#include "include/error.h"
#include "include/sdev.h"

/*************************************************
  �궨��
*************************************************/

/*************************************************
 * �ṹ�嶨��
*************************************************/

static struct {
	int (*setconfig)(u8 cmd, void *cfg);
	int (*getconfig)(u8 cmd, void *cfg);
	int (*send)(u8 *buf, u32 count);
	int (*recv)(u8 *buf, u32 count, u8 *type, u8 *tel);
	int (*sendsmg)(u8 *buf, u32 count,u8 *telnum, u8 tellen);
	int (*recvsmg)(u8 *buf, u32 count);
	int (*connect)(u8 mode);
	int (*disconnect)(void);
	int (*getstat)(u8 cmd, void *stat);
	int (*turnon)(void);
	int (*turnoff)(void);
	int (*poweron)(void);
	int (*poweroff)(void);
	int (*reset)(void);
}sdevhandle[CFG_SDEV_MAX];


/******************************************************************************
*	����:	streamdev_open
*	����:	���������豸
*	����:
*	����:	0				-	�ɹ�


*	˵��:	��
******************************************************************************/
struct sdevhandle * streamdev_open(u8 id)
{
	int ret;
	struct sdevhandle *p;
	if((id < 0) || (id >= CFG_SDEV_MAX)){
		goto error;
	}

	switch(id){
	case CFG_GPRS_ME3000:
		sdevhandle[CFG_GPRS_ME3000].setconfig = gprs_me3000_setconfig;
		sdevhandle[CFG_GPRS_ME3000].getconfig = gprs_me3000_getconfig;
		sdevhandle[CFG_GPRS_ME3000].send = gprs_me3000_send;
		sdevhandle[CFG_GPRS_ME3000].recv = gprs_me3000_recv;
		sdevhandle[CFG_GPRS_ME3000].sendsmg = gprs_me3000_sendsmg;
		sdevhandle[CFG_GPRS_ME3000].recvsmg = gprs_me3000_recvsmg;
		sdevhandle[CFG_GPRS_ME3000].connect = gprs_me3000_connect;
		sdevhandle[CFG_GPRS_ME3000].disconnect = gprs_me3000_disconnect;
		sdevhandle[CFG_GPRS_ME3000].getstat = gprs_me3000_getstat;
		sdevhandle[CFG_GPRS_ME3000].turnon = gprs_me3000_turnon;
		sdevhandle[CFG_GPRS_ME3000].turnoff = gprs_me3000_turnoff;
		sdevhandle[CFG_GPRS_ME3000].poweron = gprs_me3000_poweron;
		sdevhandle[CFG_GPRS_ME3000].poweroff = gprs_me3000_poweroff;
		sdevhandle[CFG_GPRS_ME3000].reset = gprs_me3000_reset;
		ret = gprs_me3000_init();
		if (ret){
			goto error;
		}
		p = (struct sdevhandle *)&sdevhandle[CFG_GPRS_ME3000];
		break;
	case CFG_SDEV_485:
		sdevhandle[CFG_SDEV_485].setconfig = sdev_485_setconfig;
		sdevhandle[CFG_SDEV_485].getconfig = sdev_485_getconfig;
		sdevhandle[CFG_SDEV_485].send = sdev_485_send;
		sdevhandle[CFG_SDEV_485].recv = sdev_485_recv;
		sdevhandle[CFG_SDEV_485].sendsmg = sdev_485_sendsmg;
		sdevhandle[CFG_SDEV_485].recvsmg = sdev_485_recvsmg;
		sdevhandle[CFG_SDEV_485].connect = sdev_485_connect;
		sdevhandle[CFG_SDEV_485].disconnect = sdev_485_disconnect;
		sdevhandle[CFG_SDEV_485].getstat = sdev_485_getstat;
		sdevhandle[CFG_SDEV_485].turnon = sdev_485_turnon;
		sdevhandle[CFG_SDEV_485].turnoff = sdev_485_turnoff;
		sdevhandle[CFG_SDEV_485].poweron = sdev_485_poweron;
		sdevhandle[CFG_SDEV_485].poweroff = sdev_485_poweroff;
		sdevhandle[CFG_SDEV_485].reset = sdev_485_reset;
		ret = comport_init(CFG_SDEV_485_COMPORT, COMPORT_MODE_485);
		if (ret){
			goto error;
		}
		p = (struct sdevhandle *)&sdevhandle[CFG_SDEV_485];
		break;
	case CFG_SDEV_NET:
		sdevhandle[CFG_SDEV_NET].setconfig = sdev_net_setconfig;
		sdevhandle[CFG_SDEV_NET].getconfig = sdev_net_getconfig;
		sdevhandle[CFG_SDEV_NET].send = sdev_net_send;
		sdevhandle[CFG_SDEV_NET].recv = sdev_net_recv;
		sdevhandle[CFG_SDEV_NET].sendsmg = sdev_net_sendsmg;
		sdevhandle[CFG_SDEV_NET].recvsmg = sdev_net_recvsmg;
		sdevhandle[CFG_SDEV_NET].connect = sdev_net_connect;
		sdevhandle[CFG_SDEV_NET].disconnect = sdev_net_disconnect;
		sdevhandle[CFG_SDEV_NET].getstat = sdev_net_getstat;
		sdevhandle[CFG_SDEV_NET].turnon = sdev_net_turnon;
		sdevhandle[CFG_SDEV_NET].turnoff = sdev_net_turnoff;
		sdevhandle[CFG_SDEV_NET].poweron = sdev_net_poweron;
		sdevhandle[CFG_SDEV_NET].poweroff = sdev_net_poweroff;
		sdevhandle[CFG_SDEV_NET].reset = sdev_net_reset;
		p = (struct sdevhandle *)&sdevhandle[CFG_SDEV_NET];
		break;
	default:
		goto error;
		break;
	}
	return(p);
error:
	return NULL;
}


