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
    �ļ���         ��  c-tx-comport-send.c
    ����	       ��  ���ļ�����ƽ̨��comport_send�����Ĳ���
    �汾              ��  0.1
    ����              ��  ����
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>			//exit
#include <unistd.h>			//sleep
#include <db.h>
#include <string.h>
#include <sys/stat.h>					//stat
#include <sys/types.h>

//ƽ̨��ͷ�ļ�
#include "../include/comport.h"
#include "../include/xj_assert.h"
#include "../include/error.h"

int main()
{
	int ret,ret1;
	u8 flag;
	//�����ĳ�ʼ��
	inittest();

	u8 port1 = 2;
	u8 port2 = 5;
	u8 buf_rev[20];
	u8 buf[6] = "685016";

	comport_config_t fig = {COMPORT_VERIFY_NO, 8,  1, 0, 9600, COMPORT_RTSCTS_DISABLE};      //������ʽ
	comport_config_t fig1 = {COMPORT_VERIFY_NO, 8,  1, 0xff, 9600, COMPORT_RTSCTS_DISABLE};  //��������ʽ
	comport_config_t fig2 = {COMPORT_VERIFY_NO, 8,  1, 20, 9600, COMPORT_RTSCTS_DISABLE};    //��ʱ��ʽ

	/**********��������9***************/
	ret = comport_send(port1, buf, 6);
	memset(buf_rev,0,20);
	ret1 = comport_recv(port2, buf_rev, 6);
	assert(((ret==-ERR_NOINIT)&&(ret1==-ERR_NOINIT)),"test9:send-recv error");
//#if 0
	u8 mode1;
	mode1 = COMPORT_MODE_NORMAL;
	ret = comport_init(port1, mode1);    //��ͨ����
	if(ret < 0){
		printf("init error!\n");
		goto error;
	}

	ret = comport_init(port2, mode1);
	if(ret < 0){
		printf("init error!\n");
		goto error;
	}
/********��������1-������ʽ************************/
	ret = comport_setconfig (port1, &fig);
	if(ret<0){
		printf("test1:port1 setconfig error!\n");
		goto error;
	}
	ret = comport_setconfig (port2, &fig);
	if(ret<0){
		printf("test1:port2 setconfig error!\n");
		goto error;
	}

	ret = comport_send(port1, buf, 6);
	if (ret<=0){
		assert(ret==6,"test1:send error");
		goto error;
	}
	memset(buf_rev,0,20);
	ret = comport_recv(port2, buf_rev, 6);
	assert((ret==6) && (strcmp((char *)buf_rev, "685016")==0),"test1:recv error");

	/**********��������2--��������ʽ******************/
	ret = comport_setconfig (port1, &fig1);
	if(ret<0){
		printf("test2:port1 setconfig error!\n");
		goto error;
	}
	ret = comport_setconfig (port2, &fig1);
	if(ret<0){
		printf("test2:port2 setconfig error!\n");
		goto error;
	}

	ret = comport_send(port1, buf, 6);
	if (ret<=0){
		assert(ret==6,"test2:send error");
		goto error;
	}

	memset(buf_rev,0,20);
	flag = 1;
	while(flag){
		ret = comport_recv(port2, buf_rev, 6);
		if (ret == -ERR_TIMEOUT){
			flag = 1;
		}else if(ret <= 0){
			flag = 0;
			assert(ret > 0,"test2:recv error");
		}else if(ret > 0){
			assert((ret==6) && (strcmp((char *)buf_rev, "685016")==0),"test2:recv2 error");
			flag = 0;
		}
	}
	/*****��������3-��ʱ��ʽ*******************************/
	ret = comport_setconfig (port1, &fig2);
	if(ret<0){
		printf("test3:port1 setconfig error!\n");
		goto error;
	}
	ret = comport_setconfig (port2, &fig2);
	if(ret<0){
		printf("test3:port2 setconfig error!\n");
		goto error;
	}

	ret = comport_send(port1, buf, 6);
	if (ret<=0){
		assert(ret==6,"test3:send error");
		goto error;
	}

	system("date");
	memset(buf_rev,0,20);
	ret = comport_recv(port2, buf_rev, 6);
	if(ret > 0){
		assert(ret==6 && (strcmp((char *)buf_rev, "685016")==0),"test3:recv error");
	}else if(ret == -ERR_TIMEOUT){
		assert(ret > 0,"test3:recv timeout");
	}else{
		assert(ret > 0,"test3:recv error");
	}
	system("date");

	//�رմ���
	ret = comport_close(port1);
	if(ret < 0){
		printf("close error!\n");
		goto error;
	}
	ret = comport_close(port2);
	if(ret < 0){
		printf("close error!\n");
		goto error;
	}
	/**********��������7***************/
	port1 = 9;
	port2 = 9;
	ret = comport_send(port1, buf, 6);
	memset(buf_rev,0,20);
	ret1 = comport_recv(port2, buf_rev, 6);
	assert(((ret==-ERR_INVAL)&&(ret1==-ERR_INVAL)),"test7:send-recv error");

	/**********��������8***************/
	port1 = 2;
	ret = comport_send(port1, buf, 0);
	assert(ret==-ERR_INVAL,"test8:send-recv error");

//#endif
/***********************485��ʽ�������ܲ���************************/
#if 0
printf("485\n");
	port1 = 1;
	u8 mode2 = COMPORT_MODE_485;
	ret = comport_init(port1, mode2);    //485����
	if(ret < 0){
		printf("init error!\n");
		goto error;
	}

	port2 = 2;
	ret = comport_init(port2, mode2);
	if(ret < 0){
		printf("init error!\n");
		goto error;
	}
	/********��������4-������ʽ************************/
	ret = comport_setconfig (port1, &fig);  //fig ������ʽ
	if(ret<0){
		printf("test4:port1 setconfig error!\n");
		goto error;
	}
	ret = comport_setconfig (port2, &fig);
	if(ret<0){
		printf("test4:port2 setconfig error!\n");
		goto error;
	}
	ret = comport_send(port1, buf, 6);
	if (ret<=0){
		assert(ret==6,"test4:send error");
		goto error;
	}
	memset(buf_rev,0,20);
	ret = comport_recv(port2, buf_rev, 6);
	assert((ret==6) && (strcmp((char *)buf_rev, "685016")==0),"test4:recv error");

	/**********��������5--��������ʽ******************/
	ret = comport_setconfig (port1, &fig1);
	if(ret<0){
		printf("test5:port1 setconfig error!\n");
		goto error;
	}
	ret = comport_setconfig (port2, &fig1);
	if(ret<0){
		printf("test5:port2 setconfig error!\n");
		goto error;
	}

	ret = comport_send(port1, buf, 6);
	if (ret<=0){
		assert(ret==6,"test5:send error");
	}

	memset(buf_rev,0,20);
	flag = 1;
	while(flag){
		ret = comport_recv(port2, buf_rev, 6);
		if (ret == -ERR_TIMEOUT){
			flag = 1;
			sleep(1);
		}else if(ret <= 0){
			flag = 0;
			assert(ret > 0,"test5:recv error");
		}else if(ret > 0){
			printf("rev ok!\n");
			assert((ret==6) && (strcmp((char *)buf_rev, "685016")==0),"test5:recv1 error");
			flag = 0;
		}
	}

	/*****��������6-��ʱ��ʽ*******************************/
	ret = comport_setconfig (port1, &fig2);
	if(ret<0){
		printf("test6:port1 setconfig error!\n");
		goto error;
	}
	ret = comport_setconfig (port2, &fig2);
	if(ret<0){
		printf("test6:port2 setconfig error!\n");
		goto error;
	}

	ret = comport_send(port1, buf, 6);
	if (ret<=0){
		assert(ret==6,"test6:send error");
	}

	system("date");
	memset(buf_rev,0,20);
	ret = comport_recv(port2, buf_rev, 6);
	if(ret > 0){
		assert(ret==6 && (strcmp((char *)buf_rev, "685016")==0),"test6:recv error");
		printf("rev ok!\n");
	}else if(ret == -ERR_TIMEOUT){
		assert(ret > 0,"test6:recv timeout");
	}else if(ret == 0){
		assert(ret > 0,"test6:recv error");
	}
	system("date");

	ret = comport_close(port1);
	if(ret < 0){
		printf("close error!\n");
		goto error;
	}
	ret = comport_close(port2);
	if(ret < 0){
		printf("close error!\n");
		goto error;
	}
#endif
	finaltest();
error:
	exit(0);
}
