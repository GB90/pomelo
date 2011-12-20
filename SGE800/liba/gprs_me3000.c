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
    �� �� ��    ��  gprs_me3000.c
    ��    ��    ��  ���ļ�����ҵ��ƽ̨��gprs���ܵ�ʵ��
    ��    ��    ��  0.1
    ��    ��    ��  ����
    ��������    ��  2010.04
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>		    //exit
#include <unistd.h>		    //sleep
//#include <db.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

//ƽ̨��ͷ�ļ�
#include "include/sdev.h"
#include "include/gprs_me3000.h"
#include "include/comport.h"
#include "include/gpio.h"
#include "private/config.h"
#include "include/error.h"

/*************************************************
  �ṹ���Ͷ���
*************************************************/
//GPRSģ�������
#define DEV_STREAM_SET_COMPORT      0           		//���ô��ڵ�����
#define	DEV_STREAM_SET_DEV			1					//�����豸������
#define	check_echo(q,p) (memcmp(q, p, sizeof(p)-1))     //�жϻ����Ƿ���ȷ


#define DEV_GPRS_TCP				0					//TCP���ӷ�ʽ
#define DEV_GPRS_UDP				1					//UDP���ӷ�ʽ

//����ͻظ����п��ܵĽ���״̬
#define DEV_STREAM_DEMAND			0					//��������
#define DEV_STREAM_DEFINE			1					//�յ���������ȷ�ϣ�>
#define DEV_STREAM_RECV_OK	        2            		//gprsģ���봮�ڽ�����ʾ��"ok"
#define DEV_STREAM_RECV_VERIFY      3             		//gprsģ���봮�ڽ�����ʾ��ȷ�Ͼ�����Ϣ

//��������״̬
#define DEV_GPRS_ATE0				0					//gprsģ�����ӹرջ���״̬
#define DEV_GPRS_CHECK_ATE0			1					//gprsģ�����Ӳ鿴�����Ƿ�ر�
#define DEV_GPRS_CNMI				2					//gprsģ���������ö��Ÿ�ʽ
#define DEV_GPRS_CHECK_CNMI			3					//gprsģ�����Ӳ鿴���Ÿ�ʽ
#define DEV_GPRS_CMGF				4					//gprsģ����������PDU��ʽ
#define DEV_GPRS_CHECK_CMGF			5					//gprsģ�����Ӳ�ѯ�Ƿ�ΪPDU��ʽ
#define DEV_GPRS_CSMS				6					//gprsģ���������ö��ŷ���
#define DEV_GPRS_CHECK_CSMS			7					//gprsģ�����Ӳ�ѯ���ŷ���
#define DEV_GPRS_CSCS				8					//gprsģ���������ñ�����ʽ
#define DEV_GPRS_CHECK_CSCS			9					//gprsģ�����Ӳ�ѯ������ʽ
#define DEV_GPRS_CHECK_CREG			10					//gprsģ�����Ӳ�ѯ����
#define DEV_GPRS_XISP				11					//gprsģ�����������ڲ�Э��
#define DEV_GPRS_CHECK_XISP			12					//gprsģ�����Ӳ�ѯЭ��
#define DEV_GPRS_CODE				13					//gprsģ�����������û�������
#define DEV_GPRS_INITPPP			14					//gprsģ�����ӵ�¼����
#define DEV_GPRS_CHECK_PPPOK		15					//gprsģ�����Ӳ�ѯ�Ƿ��¼����
#define DEV_GPRS_CHECK_PPPNET		16					//gprsģ������IP����
#define DEV_GPRS_CHECK_PPPLINK		17					//gprsģ������IP�����Ƿ�ɹ�

/*******************************����***********************************/
#define ERR_GPRS_MODEL      		34                  //gprsģ�����ù���ʱ�ظ�����
#define ERR_GPRS_CLOSE				35					//gprsģ�鱻�Ͽ�


//GPRSģ���״̬
static struct{
	u8 init;              		//��ʼ����־
	u8 timeout;					//���ô��ڽ��ճ�ʱ
	u8 busy;    				//0�����У�1��æµ�����͵ȴ�״̬����
	u8 connect_state;      		//0:������״̬��1������״̬
	u8 sms_flag;				//����״̬
	u8 ip[12];					//��վIP
	u8 connect_index;      		//0:TCP; 1:UDP
	pthread_mutex_t mutex;		//����������ֹͬʱ���ͽ��գ�
	pthread_mutex_t mutex_data;	//������������������
	u16 data_len;				//������Ч���ݳ���
	u8 data_buf[CFG_DATA_MAX];	//��󻺴�����--���ܷ������ݵ���󳤶����������ܶ�֡
	pthread_mutex_t mutex_sms;  //������������������
	u8 sms_tel[11];				//���ͷ��绰����
	u16 sms_len;				//������Ч���ų���
	u8 sms_buf[CFG_SMS_MAX];    //��󻺳����
}gprsstate;

/******************************************************************************
*	����:	accesssmsnum
*	����:	���ַ��͵绰���루11λ��,ת����6���ֽڵ�����(�ڲ�ʹ�ú������ⲿ�ӿ�)
*	����:	num				-	�����ֽڵĵ绰����
			num_char		-	�ַ��͵绰�������ʼ��ַ������Ϊ11���ַ��ĵ绰�ַ����������̨���㣩
*	����:	0				-	�ɹ�(�յ�ȷ������)

*	˵��:	��
******************************************************************************/
int accesssmsnum(char *num_char, u8 *num)
{
	int i;
	u8 number[6];
	char *point;

	point = num_char;
	for(i=0; i<5; i++){
		number[i] = atoi(point);
		point ++;
		number[i] += atoi(point) << 4;
		point ++;
	}
	number[5] = atoi(point) | 0xF0;
	memcpy(num, number, sizeof(number));
	exit(0);
}

/******************************************************************************
*	����:	deal_recv
*	����:	������յ��ö���Ϣ������,�ж����Ƿ���Ч����Ч�����뻺������(�ڲ�ʹ�ú������ⲿ�ӿ�)
*	����:	buf				-	���յ������ݵ���ʼ��ַ
			len				-	���յ����ݵĳ���
			char			-	Ŀ�Ľ����������ʼ��ַ
*	����:	0				-	�ɹ�(�յ�ȷ������)
			-ERR_NOFUN		-	û���յ�ȷ������
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
 		    -ERR_DISCONNECT	-	������δ��
 		    -ERR_GPRS_CLOSE	-	���ӱ����Ͽ�

*	˵��:	��
******************************************************************************/
int deal_recv(u8 *buf, u32 len, char *cmd)
{
	int ret,ref;
	int i, j, k;
	u8 num;
	u16 multiflag;   //���Ž���֡��־
	u8 *point;
	u32 point_len = 0;
	u32 data_len;
	struct timespec to;
	u8 tel_num[11]; //���ͷ��绰����
	u8 mid[512];   	//�м���
	memset(mid, 0, sizeof(mid));

	memcpy(mid, buf, len);
	point_len += len;

	//�������մ�������
	ret = comport_recv(CFG_GPRS_ME3000_COMPORT, &mid[point_len-1], sizeof(mid)-len);
	if(ret > 0){
		point_len += ret;
	}else if (ret < 0){
		goto error;
	}
	//�ж��ڶ�ȡȷ�Ϲ������յ����Ƿ�����Ч�Ľ��յ����ݻ��߶��ŵ���Ϣ
	if (check_echo((char *)mid, "\r\n+CMT") == 0){    		  //���յ�����
		//���Ž��յ�һ������Ϊ�ӵ�14λ��ʼ:\r\n+CMT : <oa>, <����>,�������ASSIIC��
		u8 buff[512];
		memset(buff, 0, 512);
		memcpy(buff, &mid[13], strlen((char *)mid));  //strlen������ȿ��ܲ�׼ȷ,һ�㳬��
		k = strlen((char *)mid)/2;
		//ASCIIתΪHEX
		for(j=0; j<k; j++){
			if((buff[2*j]>0x40) && (buff[2*j]<0x47)){
				buff[2*j] -= 0x37;
			}else{
				buff[2*j] -= 0x30;
			}
			if((buff[2*j+1]>0x40) && (buff[2*j+1]<0x47)){
				buff[2*j+1] -= 0x37;
			}else{
				buff[2*j+1] -= 0x30;
			}
			mid[j]=(buff[2*j] << 4) + buff[2*j+1];
		}
		//memcpy(RecSMSNum, &ReturnStr[1], ReturnStr[0]);  //�������ĺ���
		num = 1 + mid[0] + 1 ;  //10
		if(mid[num] % 2){ //����λ��Ϊ����
			memset(tel_num, 0, sizeof(tel_num));
			memcpy(tel_num, &mid[num + 1], mid[num] / 2 + 1 + 1);  //����Դ����
			num += (mid[num] / 2 + 1) + 3 ;  //20
		}else{
			memset(tel_num, 0, 10);
			memcpy(tel_num, &mid[num + 1], mid[num] / 2 + 1);
			num += (mid[num] / 2 ) + 3;
		}
		//�����
		to.tv_sec = time(NULL) + CFG_SEND_TIMEOUT;      //��
		to.tv_nsec = 0;                       			//����
		ref = pthread_mutex_timedlock(&(gprsstate.mutex_sms), &to);
		if (ref == ETIMEDOUT){
			ret = -ERR_BUSY;               //�豸æ�������У�
			goto error;
		}else{
			ret = -ERR_SYS;
			goto error;
		}
		if ((gprsstate.sms_len > 0) && (memcmp(gprsstate.sms_tel, tel_num, sizeof(tel_num)-1) != 0) && (gprsstate.sms_flag == 0)){  //����δ��ȫ�����Ƿ��ͺ��벻һ��
			//��ն��Ż��棿����������
			memset(gprsstate.sms_buf, 0, sizeof(gprsstate.sms_buf));
			gprsstate.sms_len = 0;
			gprsstate.sms_flag = 0;
		}
		if(mid[num] != 0x04){ //�������HEX���룬����
			ret = 0;
			goto error;
		}
		memcpy(&(gprsstate.sms_buf[gprsstate.sms_len]), &mid[num + 11], mid[num + 8] - 2);    //�����û����ݣ�ǰ�����ֽ������ж��Ƿ��֡
		gprsstate.sms_len += (mid[num + 8] - 2);
		multiflag = mid[num + 9] + mid[num + 10] * 256 ;  //��λ��ǰ����λ�ں�
		if(multiflag & 0x8000){     //smfin Ϊ1 ����֡
			//�յ������Ķ���
			gprsstate.sms_flag = 1;
		}else{
			//�������ն���
			gprsstate.sms_flag = 0;
		}
		//���������
		ret = pthread_mutex_unlock(&(gprsstate.mutex_sms));
		if (ret){
			ret = -ERR_SYS;
			goto error;
		}
		
		//�Ƿ��յ�Ŀ��ظ�
		if (point_len >= mid[num+8]+num+9){
			//û���յ�ȷ������
			ret = -ERR_NOFUN;
			goto error;
		}else{
			//�ж��Ƿ���ȷ������
			num = mid[num+8]+num+9;
			if (check_echo((char *)&mid[num], cmd) == 0){
				ret = 0;
				return(ret);
			}else{
				ret = -ERR_NOFUN;
				goto error;
			}
		}
	}else if (check_echo((char *)mid, "\r\n+TCPCLOSE") == 0){ //GPRSģ�鱻���Ͽ�
		gprsstate.connect_state = 0;   
		ret = -ERR_GPRS_CLOSE;     //ģ���쳣�Ͽ�
		goto error;
	
	}else if ((check_echo((char *)mid, "\r\n+TCPRECV") == 0) || (check_echo((char *)mid, "\r\n+UDPRECV") == 0)){  //���յ�TCP����
		point = &mid[13];       			//���յ����ݳ����ַ�����ʼָ��
		i =	atoi((char *)point);
		//�жϽ������ݳ�������
		if ((point_len > (i + 15)) && (i <= 1024)){
			if (i <	10){
				data_len = i + 16;      //pointΪ������Ч����  ???���峤�ȴ���֤    ������17
			}else if(i < 100){
				data_len = i + 17;
			}else if(i < 1000){
				data_len = i + 18;
			}else{
				data_len = i + 19;
			}
			//����Ч���շ������ݻ�������
			to.tv_sec = time(NULL) + 2;           //��
			to.tv_nsec = 0;                       //����
			ret = pthread_mutex_timedlock(&gprsstate.mutex_data, &to);    	//�����
			if(ret == 0){
				memset(gprsstate.data_buf, 0, sizeof(gprsstate.data_buf));  //��գ������һ��Ч֡��2S��δ�����п��ܱ����
				memcpy(gprsstate.data_buf, mid, data_len);
				gprsstate.data_len = data_len;
				//�ͷ���
				ret = pthread_mutex_unlock(&gprsstate.mutex_data);
				if(ret){
					goto error;
				}
			}
			//�ж��յ��������Ƿ����ȷ��֡
			if (point_len >= data_len + strlen(cmd)){    //�п����յ���ȷ��
				for(i = data_len; i< point_len-strlen(cmd); i++){
					if (check_echo((char *)&mid[i], cmd) == 0){
						return(0);
					}else{
						ret = -ERR_NOFUN;
						goto error;
					}
				}
			}else{
				ret = -ERR_NOFUN;
				goto error;
			}

		}else{    //���Ȳ�����
			data_len = strlen(cmd);
			if (point_len >= data_len){    //ֱ���ж��Ƿ������ȷ�ϻظ�
				for(i = 0; i< point_len-strlen(cmd); i++){
					if (check_echo((char *)&mid[i], cmd) == 0){
						return(0);
					}else{
						ret = -ERR_NOFUN;
						goto error;
					}
				}
			}else{
				ret = -ERR_NOFUN;
				goto error;
			}
		}
	//�����յ���Ч����
	}else{
		//�ж��յ��������Ƿ����ȷ��֡
		if (point_len >= strlen(cmd)){    //�п����յ���ȷ��
			for(i = 0; i< point_len-strlen(cmd); i++){
				if (check_echo((char *)&mid[i], cmd) == 0){
					return(0);
				}else{
					ret = -ERR_NOFUN;
					goto error;
				}
			}
		}else{
			ret = -ERR_NOFUN;
			goto error;
		}
	}
error:
	return(ret);
}
/******************************************************************************
*	����:	gprs_me3000_init
*	����:	��ʼ��gprs(�ڲ�ʹ�ú������ⲿ�ӿ�)
*	����:
*	����:	0				-	�ɹ�(�յ�ȷ������)
			-ERR_SYS		-	ϵͳ����

*	˵��:	��
******************************************************************************/
int gprs_me3000_init()
{
	int ret,ref;
	if (gprsstate.init == 1){
		ret = -ERR_BUSY;			//�ѳ�ʼ��
		goto error;
	}
	gprsstate.busy = 0;    			//����
	gprsstate.connect_index = 0;    //δ����
	gprsstate.sms_flag = 0;         //�Ƕ���
	gprsstate.init = 0 ;
	memset(gprsstate.data_buf, 0, CFG_DATA_MAX);    //������ݻ���
	gprsstate.data_len = 0;
	
	//��ʼ��������
	const pthread_mutex_t mutex_zero = PTHREAD_MUTEX_INITIALIZER;
	memcpy(&gprsstate.mutex, &mutex_zero, sizeof(pthread_mutex_t));
	ref = pthread_mutex_init(&gprsstate.mutex, NULL);
	if (ref){
		ret = -ERR_SYS;
		goto error;
	}
	//��ʼ��������
	memcpy(&gprsstate.mutex_data, &mutex_zero, sizeof(pthread_mutex_t));
	ref = pthread_mutex_init(&gprsstate.mutex_data, NULL);
	if (ref){
		ret = -ERR_SYS;
		goto error;
	}
	//��ʼ��������
	memcpy(&gprsstate.mutex_sms, &mutex_zero, sizeof(pthread_mutex_t));
	ref = pthread_mutex_init(&gprsstate.mutex_sms, NULL);
	if (ref){
		ret = -ERR_SYS;
		goto error;
	}
	
	//��ʼ��gpioģ��
	ret = gpio_init();  
	if ((ret != 0) && (ret != -ERR_BUSY)){
		goto error;
	}
	//����gprsģ��power��IO����
	ret = gpio_set(CFG_GPRS_POWER_IO, GPIO_OUT, CFG_GPRS_POWER_IO_OD, CFG_GPRS_POWER_IO_PU);
	if (ret){
		goto error;
	}
	gprsstate.init = 1;
error:
	return(ret);
}


/******************************************************************************
*	����:	gprs_me3000_setconfig
*	����:	���ô��ڻ�gprsģ��
*	����:	cmd				-	0�����ô��ڣ�1������gprsģ��
			cfg				-	���ýӿ�
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���

*	˵��:	��
******************************************************************************/
int gprs_me3000_setconfig(u8 cmd, void *cfg)
{
	int ret;
	comport_config_t cfg_set;
	if (cmd == DEV_STREAM_SET_COMPORT){      //������GPRSģ�������Ĵ��ڵ�����
		ret = comport_init(CFG_GPRS_ME3000_COMPORT, COMPORT_MODE_NORMAL);
		if ((ret != 0) && (ret != -ERR_BUSY)){
			goto error;
		}
		cfg_set.baud = ((comport_config_t *)cfg) -> baud;
		cfg_set.ndata = ((comport_config_t *)cfg) -> ndata;
		cfg_set.nstop = ((comport_config_t *)cfg) -> nstop;
		cfg_set.rtscts = ((comport_config_t *)cfg) -> rtscts;
		cfg_set.verify = ((comport_config_t *)cfg) -> verify;

		//���ó�ʱ��
		cfg_set.timeout = CFG_RECV_TIMEOUT;     //���ô����ǳ�ʱ��ʽ
		((comport_config_t *)cfg) -> timeout = gprsstate.timeout;

		//���ô�������
		ret = comport_setconfig (CFG_GPRS_ME3000_COMPORT, &cfg_set);
		if (ret){
			goto error;
		}
	}else if (cmd == DEV_STREAM_SET_DEV){	//����ME3000����

	}else{
		ret = -ERR_INVAL;
		goto error;
	}
	ret = 0;
error:
	return(ret);
}


int gprs_me3000_getconfig(u8 cmd, void *cfg)
{
	int ret;
	comport_config_t cfg_get;


	if (cmd == DEV_STREAM_SET_COMPORT){      //�����GPRSģ�������Ĵ��ڵ�����
		ret = comport_init(CFG_GPRS_ME3000_COMPORT, COMPORT_MODE_NORMAL);
		if ((ret != 0) && (ret != -ERR_BUSY)){
			goto error;
		}
		ret = comport_getconfig (CFG_GPRS_ME3000_COMPORT, &cfg_get);
		if (ret != 0){
			goto error;
		}
		((comport_config_t *)cfg) -> baud = cfg_get.baud;
		((comport_config_t *)cfg) -> ndata = cfg_get.ndata;
		((comport_config_t *)cfg) -> nstop = cfg_get.nstop;
		((comport_config_t *)cfg) -> rtscts = cfg_get.rtscts;
		((comport_config_t *)cfg) -> verify = cfg_get.verify;

		((comport_config_t *)cfg) -> timeout = gprsstate.timeout;

	}else if (cmd == DEV_STREAM_SET_DEV){	//���M580����

	}else{
		ret = -ERR_INVAL;
		goto error;
	}
	ret = 0;
error:
	return(ret);
}

/******************************************************************************
*	����:	gprs_me3000_send
*	����:	GPRS��������
*	����:	buf				-	����������ʼָ��
			count			-	�������ݳ���
*	����:	>0				-	�ɹ�(����ʵ�ʷ��ͳɹ����ֽ���)
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
 		    -ERR_DISCONNECT	-	δ����
 		    -ERR_BUSY		-	�豸æ

*	˵��:	��ʱʱ��ͨ�����÷��ͳ�ʱ��
******************************************************************************/
int gprs_me3000_send(u8 *buf, u32 count)
{
	int ret,ref;
	u8 recv_len,timeout,back;
	u8 step,flag,i;                 //����״̬
	char send_cmd[CFG_DATA_MAX];  	//������������
	u8 recv_cmd[CFG_RECV_BACKMAX];  				//��������
	struct timespec to;

	if (count <= 0){
		ret = -ERR_INVAL;
		return ret;
	}
	//�ж��Ƿ�Ϊ����״̬
	if(gprsstate.connect_state == 0){
		ret = -ERR_DISCONNECT;
		return ret;
	}
	//�����
	to.tv_sec = time(NULL) + CFG_SEND_TIMEOUT;      //��
	to.tv_nsec = 0;                       			//����
	ref = pthread_mutex_timedlock(&(gprsstate.mutex), &to);
	if (ref == ETIMEDOUT){
		ret = -ERR_BUSY;               //�豸æ�������У�
		return ret;
	}else if(ref){
		ret = -ERR_SYS;
		return ret;
	}
	//�ж�ģ���Ƿ����㹻�ķ��ͻ�������δд��
	//֪ͨME3000����Ҫ��������
	memset(send_cmd, 0, sizeof(send_cmd));
	if (gprsstate.connect_index == 0){					//TCPģʽ
		sprintf(send_cmd, "%s%d\r", "AT+TCPSEND=0,",count);
	}else{												//UDPģʽ
		sprintf(send_cmd, "%s%d\r", "AT+UDPSEND=0,",count);
	}
	//��շ��ͻ���
	comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_WR);
	//������������
	ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
	if(ret != strlen(send_cmd)){
		goto error;
	}
	timeout = 200;    //���ظ�ʱ��-----�ֲ�
	sleep(1);
	//200S�ڷ���---�ʱ�䣿��������
	back = 1;     //ȷ�ϱ�ʶ
	step = DEV_STREAM_DEFINE;
	while(back){
		switch(step){
		case DEV_STREAM_DEFINE:     //������������ʾ
			memset(recv_cmd, 0, sizeof(recv_cmd));
			ret = comport_recv (CFG_GPRS_ME3000_COMPORT, recv_cmd, 10);
			flag = 0;
			if(ret > 0){     //���յ�������ʾ��
				for (i = 0; i < ret; i++){
					if (recv_cmd[i] == '>'){
						flag = 1;
						break;
					}
				}
				if (flag == 1){
					//��շ��ͻ���
					comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_WR);
					//��������
					memset(send_cmd, 0, sizeof(send_cmd));
					memcpy(send_cmd, buf, count);
					send_cmd[count] = '\r';
					ret = comport_send (CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, (count+1));
					if (ret < 0){
						goto error;
					}
					timeout = 0;
					step = DEV_STREAM_RECV_OK;
				}else{
					ret = deal_recv(recv_cmd, ref, (char *)'>');    //�����ڣ������ڻ��嶼���������Ѷ��������ݷ������ݻ���֮�в��ж��Ƿ�����Ҫ�ķ��أ�
					if(ret == 0){
						timeout = 0;
						step = DEV_STREAM_RECV_OK;
					}else if(ret == -ERR_GPRS_CLOSE){  //�����������Ͽ�
						goto error;
					}else{
						step = DEV_STREAM_DEFINE;  //��������>��
					}
				}
			}else if(ret == 0){
				timeout = timeout - 1 - CFG_RECV_TIMEOUT;
				if(timeout > 0){
					step = DEV_STREAM_DEFINE;
					sleep(1);
				}else{
					back = 0;
					ret = -ERR_TIMEOUT;
					goto error;
				}
			}else if(ret < 0){
				goto error;
			}
			break;
		case DEV_STREAM_RECV_OK:   //���շ���ȷ�ϡ�OK��
			//�ж��Ƿ��ͳɹ���Ϣ
			memset(recv_cmd, 0, sizeof(recv_cmd));
			recv_len = strlen("\r\nOK\r\n");
			ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, recv_len);
			if (ret < 0){
				goto error;      //���մ���
			}else if((ret > 0) && (check_echo((char *)recv_cmd, "\r\nOK\r\n") == 0)){
				timeout = 0;
				step = DEV_STREAM_RECV_VERIFY;
			}else if((ret > 0) && (check_echo((char *)recv_cmd, "\r\nOK\r\n") != 0)){
				ret = deal_recv(recv_cmd, ref, "\r\nOK\r\n");    //�����ڣ������ڻ��嶼���������Ѷ��������ݷ������ݻ���֮�в��ж��Ƿ�����Ҫ�ķ��أ�
				if(ret == 0){
					timeout = 0;
					step = DEV_STREAM_RECV_VERIFY;
				}else if(ret == -ERR_GPRS_CLOSE){  //�����������Ͽ�
					goto error;
				}else{
					step = DEV_STREAM_RECV_OK;  //��������\r\nOK\r\n��
				}
			}else if (ret==0){
				timeout = timeout - 1 - CFG_RECV_TIMEOUT;
				if(timeout > 0){
					step = DEV_STREAM_RECV_OK;
					sleep(1);
				}else{
					back = 0;
					ret = -ERR_TIMEOUT;
					goto error;
				}
			}
			break;
		case DEV_STREAM_RECV_VERIFY:
			ref = comport_recv (CFG_GPRS_ME3000_COMPORT, recv_cmd, 13);   //���Է���λ����ʮλ������λ�������
			if((ret > 0) && (check_echo((char *)recv_cmd, "\r\n+TCPSEND") == 0)){
				ret = atoi((char *)&recv_cmd[13]);
				back = 0;
				timeout = 0;
			}else if((ret > 0) && (check_echo((char *)recv_cmd, "\r\n+TCPSEND") != 0)){
				ret = deal_recv(recv_cmd, ref, "\r\n+TCPSEND");    //�����ڣ������ڻ��嶼���������Ѷ��������ݷ������ݻ���֮�в��ж��Ƿ�����Ҫ�ķ��أ�
				if(ret == 0){
					timeout = 0;
					back = 0;
				}else if(ret == -ERR_GPRS_CLOSE){  //�����������Ͽ�
					back = 0;
					goto error;
				}else{
					step = DEV_STREAM_RECV_VERIFY;  //��������+TCPSEND��
				}
			}else if(ret == 0){
				timeout = timeout - 1 - CFG_RECV_TIMEOUT;
				if(timeout > 0){
					step = DEV_STREAM_RECV_VERIFY;
					sleep(1);
				}else{
					back = 0;
					ret = -ERR_TIMEOUT;
					goto error;
				}
			}else if(ret < 0){
				goto error;
			}
			break;
		default:
			back = 0;
			ret = -ERR_SYS;
			goto error;
		break;
		}
	}
	
error:
	//���������
	ret = pthread_mutex_unlock(&(gprsstate.mutex));
	if (ret){
		ret = -ERR_SYS;
		goto error;
	}
	return(ret);
}

/******************************************************************************
*	����:	gprs_me3000_recv
*	����:	GPRS�������ݣ�N��δ��ȡ���ܱ���գ�
*	����:	buf				-	����������ʼָ��
			count			-	���ջ���������
*	����:	>0				-	�ɹ�(����ʵ�ʷ��ͳɹ����ֽ���)
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
 		    -ERR_DISCONNECT	-	δ����
 		    -ERR_BUSY		-	�豸æ
 		    -ERR_NOMEM		-	�ӿڴ洢�ռ�С---��֡

*	˵��:	typeΪ0�յ����ţ�type = 1�յ�tcp���ݣ�type=2�յ�udp���ݣ�type=3���յ���Ϣ��ʾ
			ѭ�����øĺ����������ݣ�2s��δ�����ѽ��յ����ݿ��ܱ��µ������滻�������ؽ��յ����ݺ�Ӧ�ý����ݿ�����������
			�����µ��øú���������������ֱ������Ϊ0,����һ��ʱ�䣨2S�ڣ����µ��á�
******************************************************************************/
int gprs_me3000_recv(u8 *buf, u32 count, u8 *type, u8 *tel)
{
	int ret;
	struct timespec to;
	
	int j,k;
	u8 buffer[CFG_DATA_MAX];
	u16 buffer_len = 0;
	u8 *data_point;    //��������ַ
	u16	num;
	u16 multiflag;
	u16 data_len;     //���ݳ���
	u8 remainder = gprsstate.timeout;     //��ʱ

	memset(buffer, 0, CFG_DATA_MAX);
	//�ж��ڻ������������Ƿ�������
	if(gprsstate.data_len){     	  //������
		to.tv_sec = time(NULL) + CFG_SEND_TIMEOUT;           	//��
		to.tv_nsec = 0;                       					//����
		ret = pthread_mutex_timedlock(&gprsstate.mutex_data, &to);    //�����
		if(ret == 0){
			//�����ݿ�����
			memcpy(buffer, gprsstate.data_buf, gprsstate.data_len);
			buffer_len = gprsstate.data_len;
			//���
			memset(gprsstate.data_buf, 0, CFG_DATA_MAX);
			gprsstate.data_len = 0;
			//����
			ret = pthread_mutex_unlock(&gprsstate.mutex_data);
			if(ret){
				goto error;
			}
		}else{
			goto error;
		}
	}else if (gprsstate.sms_flag){
		if(count >= gprsstate.sms_len ){
			to.tv_sec = time(NULL) + CFG_SEND_TIMEOUT;           	//��
			to.tv_nsec = 0;                       					//����
			ret = pthread_mutex_timedlock(&gprsstate.mutex_sms, &to);    //�����
			if(ret == 0){
				//�����ݿ�����
				memcpy(buf, gprsstate.sms_buf, gprsstate.sms_len);
				buffer_len = gprsstate.sms_len;
				*type = 0;
				memcpy(tel, gprsstate.sms_tel,strlen((char *)gprsstate.sms_tel));
				//���
				memset(gprsstate.sms_buf, 0, CFG_DATA_MAX);
				memset(gprsstate.sms_tel, 0, sizeof(gprsstate.sms_tel));
				gprsstate.sms_len = 0;
				//����
				ret = pthread_mutex_unlock(&gprsstate.mutex_sms);
				if(ret){
					goto error;
				}
				//���ض��ų���
				return(buffer_len);
			}else{
				goto error;
			}
		}else{
			ret = -ERR_NOMEM;
			goto error;
		}
	}else{
		remainder = gprsstate.timeout;
		//ֱ�ӴӴ��ڽ�������
		while(remainder > 0){
			//�����
			to.tv_sec = time(NULL) + CFG_SEND_TIMEOUT;      //��
			to.tv_nsec = 0;                       			//����
			ret = pthread_mutex_timedlock(&(gprsstate.mutex), &to);
			if ((ret != ETIMEDOUT) && (ret != 0)){
				ret = -ERR_SYS;
				goto error;
			}
			ret = comport_recv(CFG_GPRS_ME3000_COMPORT, buffer, CFG_DATA_MAX);
			//����
			if(pthread_mutex_unlock(&gprsstate.mutex) != 0){    
				ret = -ERR_SYS;
				goto error;
			}
			if (ret == 0){
				sleep(1);
				remainder = remainder - 1 - CFG_RECV_TIMEOUT;
			}else if (ret < 0){
				goto error;
			}else if (ret > 0){
				buffer_len = ret;
				remainder = 0;
			}
		}
	}

	if (buffer_len){
		//�жϽ����Ƿ���ȷ
		if (check_echo((char *)buffer, "\r\n+CMT") == 0){    		 //���յ�����
			*type = 0;  //�յ�����
			//���Ž��յ�һ������Ϊ�ӵ�14λ��ʼ:\r\n+CMT : <oa>, <����>,����ת������
			u8 buff[512];
			memset(buff, 0, 512);
			memcpy(buff, &buffer[13], strlen((char *)buffer));  //strlen������ȿ��ܲ�׼ȷ,һ�㳬��
			k = strlen((char *)buffer)/2;
			//ASCIIתΪHEX
			for(j=0; j<k; j++){
				if((buff[2*j]>0x40) && (buff[2*j]<0x47)){
					buff[2*j] -= 0x37;
				}else{
					buff[2*j] -= 0x30;
				}
				if((buff[2*j+1]>0x40) && (buff[2*j+1]<0x47)){
					buff[2*j+1] -= 0x37;
				}else{
					buff[2*j+1] -= 0x30;
				}
				buffer[j]=(buff[2*j] << 4) + buff[2*j+1];
			}
			//memcpy(RecSMSNum, &ReturnStr[1], ReturnStr[0]);  //�������ĺ���
			num = 1 + buffer[0] + 1 ;  			//10
			if(buffer[num] % 2){ 				//����λ��Ϊ����
				memset(tel, 0, 10);
				memcpy(tel, &buffer[num + 1], buffer[num] / 2 + 1 + 1);  //����Դ����
				num += (buffer[num] / 2 + 1) + 3 ;  //20
			}else{
				memset(tel, 0, 10);
				memcpy(tel, &buffer[num + 1], buffer[num] / 2 + 1);
				num += (buffer[num] / 2 ) + 3;
			}
			if(buffer[num] != 0x04){ //�������HEX���룬����
				ret = 0;
				goto error;
			}
			memcpy(&(gprsstate.sms_buf[gprsstate.sms_len]), &buffer[num + 11], buffer[num + 8] - 2);    //�����û����ݣ�ǰ�����ֽ������ж��Ƿ��֡
			gprsstate.sms_len += (buffer[num + 8] - 2);
			multiflag = buffer[num + 9] + buffer[num + 10] * 256 ;  //��λ��ǰ����λ�ں�
			if(multiflag & 0x8000){     //smfin Ϊ1 ����֡
				//�յ������Ķ���
				remainder = 0;
				if (count >= gprsstate.sms_len){
					memcpy(buf, gprsstate.sms_buf, gprsstate.sms_len);
					memcpy(tel, gprsstate.sms_tel, strlen((char *)gprsstate.sms_tel));
					//�������
					memset(gprsstate.sms_buf, 0, sizeof(gprsstate.sms_buf));
					memset(gprsstate.sms_tel, 0, sizeof(gprsstate.sms_tel));
					gprsstate.sms_flag = 0;
					ret = gprsstate.sms_len;
					return(ret);
				}else{
					gprsstate.sms_flag = 1;
					ret = -ERR_NOMEM;
					goto error;
				}
			}else{
				//�������ն���
				remainder = gprsstate.timeout;
				gprsstate.sms_flag = 0;
			}
		}else if (check_echo((char *)buffer, "\r\n+TCPRECV") == 0){  //���յ�TCP����
			data_point = &buffer[13];       			//���յ����ݳ����ַ�����ʼָ��
			data_len =	atoi((char *)data_point);
			//�жϽ������ݳ�������
			if ((buffer_len > (data_len + 15)) && (buffer_len <= 1024)){
				if (data_len <	10){      //���ݳ��ȸ�λ��
					data_point += 2;      //ָ�����ݵ�һ���ֽڵĵ�ַ
				}else if(data_len < 100){ //���ݳ�����λ��
					data_point += 3;
				}else if(data_len < 1000){ //���ݳ�����λ��
					data_point += 4;
				}else{
					data_point += 5;
				}
				if (count >= data_len){
					memcpy(buf, data_point, data_len);
					ret = data_len;
					*type = 1;
				}else{
					ret = -ERR_NOMEM;
					goto error;
				}
			}
		}else if (check_echo((char *)buffer, "\r\n+UDPRECV") == 0){  	 //���յ�UDP����
			data_point = &buffer[13];       			//���յ����ݳ����ַ�����ʼָ��
			data_len =	atoi((char *)data_point);

			//�жϽ������ݳ�������
			if ((buffer_len > (data_len + 15)) && (data_len <= 1024)){
				if (data_len <	10){      //���ݳ��ȸ�λ��
					data_point += 2;      //ָ�����ݵ�һ���ֽڵĵ�ַ
				}else if(data_len < 100){ //���ݳ�����λ��
					data_point += 3;
				}else if(data_len < 1000){ //���ݳ�����λ��
					data_point += 4;
				}else{
					data_point += 5;
				}

				if (count >= data_len){      //�ӿڽ��տռ�С
					memcpy(buf, data_point, data_len);
					ret = data_len;
					*type = 2;
				}else{
					ret = -ERR_NOMEM;
					goto error;
				}
			}
		}else{
			ret = 0;
		}
	}else{
		ret = 0;
	}
error:
	return(ret);
}

/******************************************************************************
*	����:	gprs_me3000_sendsmg
*	����:	���Ͷ�Ϣ
*	����:	buf				-	����������ʼָ��
			count			-	�������ݳ���
			telnum			-	���ͷ��绰������ʼָ�루������11���ֽڵĵ绰����Ҳ������6���ֽڵ�udp�����ʽ��
			tellen			-	�绰���볤��
*	����:	>0				-	�ɹ�(����ʵ�ʷ��ͳɹ����ֽ���)�ۼ�������
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	�ӿڲ������ô���
 		    -ERR_DISCONNECT	-	δ����
 		    -ERR_BUSY		-	�豸æ
 		    -ERR_NOMEM		-	�ӿڴ洢�ռ�С---��֡

*	˵��:	
******************************************************************************/
int gprs_me3000_sendsmg(u8 *buf, u32 count, u8 *telnum, u8 tellen)
{
	int ret = 0;
	int ref;
	int i,j;
	u8 point;
	u8 recv_len,timeout,back;
	u8 step,flag;
	u8 lenbak = 0;            //����״̬
	char send_cmd[CFG_DATA_MAX];  	//������������
	u8 recv_cmd[CFG_RECV_BACKMAX];  //��������
	
	//�������ĺ���
	u8 tel_center[6];
	//���͵��ֻ�����
	u8 tel[6];   					
	u8 buff[512];
	u8 tel_x[11];

	struct timespec to;
	s16 smseq  = 0;  //��֡��ʶ
	u8 sms_total;    //��֡��
	u8 smsdata[CFG_SMS_MAX][CFG_SMS_LEN_MAX];
	u8 smsdata_len[CFG_SMS_MAX];

	if (count <= 0){
		ret = -ERR_INVAL;
		return ret;
	}
	//�ж��Ƿ�Ϊ����״̬
	if(gprsstate.connect_state == 0){
		ret = -ERR_DISCONNECT;
		return ret;
	}

	//���ŷ������ĺ���ת����ʽ
	accesssmsnum(CFG_SMS_CENTER, tel_center);   //ת��������
	
	//Ҫ���͵ĺ���ת����ʽ
	memset(tel_x, 0, sizeof(tel_x));
	if (tellen == 6){
		memcpy(tel_x, telnum, sizeof(tel));
	}else if(tellen == 11){
		memset(tel_x, 0, sizeof(tel_x));
		memcpy(tel_x, telnum, sizeof(tel_x));
		for(i=0; i<5; i++){
			tel[i] = tel_x[2*i] + (tel_x[2*i+1] << 4);
		}
		tel[5] = tel_x[10] + 0xF0;
	}else{
		ret = -ERR_INVAL;
		return ret;
	}

	//�����
	to.tv_sec = time(NULL) + CFG_SEND_TIMEOUT;      //��
	to.tv_nsec = 0;                       			//����
	ref = pthread_mutex_timedlock(&(gprsstate.mutex), &to);
	if (ref == ETIMEDOUT){
		ret = -ERR_BUSY;               //�豸æ�������У�
		return ret;
	}else if(ref){
		ret = -ERR_SYS;
		return ret;
	}
	//�ж�ģ���Ƿ����㹻�ķ��ͻ�������δд��

	//�鷢�Ͷ�������
	if (count > 138){   //���֡����
		if(count%138){
			sms_total = count / 138 + 1;  //��֡��
		}else{
			sms_total = count / 138;
		}

		if(sms_total > CFG_SMS_MAX){      //����̫��Խ��
			ret = -ERR_INVAL;
			goto error;
		}
		for(i=0; i< sms_total; i++){
			point = 0;
			smsdata[i][point++] = 0x08;	    //�̶�8λ
			smsdata[i][point++] = 0x91;
			smsdata[i][point++] = 0x68;

			//�������ĺ���
			memcpy(&smsdata[i][point], tel_center, strlen((char *)tel_center));
			point += 6 ;
			smsdata[i][point++] = 0x11;            // 11	��������(TP-MTI/VFP)	���ͣ�TP-VP����Ը�ʽ
			smsdata[i][point++] = 0x00;            // 00	��Ϣ��׼ֵ(TP-MR)	0

			//Ҫ���͵ĺ��볤��-----������������
			if((tel[strlen((char *)tel)-1] & 0xF0) == 0xF0){              	//Ҫ���͵ĺ������һλ�ĸ�4�ֽ��Ƿ�ΪF
				smsdata[i][point++] = (strlen((char *)tel) - 1) * 2 -1; 	//���ں���Ϊ�������ܳ��ȼ�ȥ91����A1��1�ֽڣ�������2 �ټ�ȥF
			}else{
				smsdata[i][point++] = (strlen((char *)tel) - 1) * 2;
			}
			memcpy(&smsdata[i][point], tel, strlen((char *)tel));         	//Ҫ���͵ĺ���
			point += strlen((char *)tel) ;
			smsdata[i][point++] = 0x00; //Э���ʶ������0x00��0x21
			smsdata[i][point++] = 0x04; //�����־��ΪHEX
			smsdata[i][point++] = 0x00;                     //��Ч�� 5����
			if(i == (sms_total - 1)){
				smsdata[i][point++] = count - 138 * i + 2;	//���һ֡���ͳ���
			}
			else{
				smsdata[i][point++] = 138 + 2;          //���������ֽڵķ�֡��־,��֡ʱǰ(SMS_MulFrameCnt-2)֡��140�ֽ�
			}

			if(i == 0){
				lenbak = point;  						//����ǰ��Ĺ̶����� �������Ϊ����ı�������
			}

			if(i == (sms_total - 1)){    //���һ֡����������smseq
				smsdata[i][point++] = (smseq | 0x8000 | (((0x8000 >>12) | i)<<12));
				smsdata[i][point++] = (smseq | 0x8000 | (((0x8000 >>12) | i)<<12)); //�ȵ��ֽ��ٸ��ֽ�
				memcpy(&smsdata[i][point], (buf + 138 * i), count - 138 * i);
				point += count - 138 * i;

				/* ����ת��Ϊ ASCII */
				memset(buff, 0, 512);
				memcpy(buff, smsdata[i], point);
				for(j=0; j<point; j++){
					if(((buff[j] & 0xF0)>= 0xA0) && ((buff[j] & 0xF0) <= 0xF0)){
						smsdata[i][2*j]=(buff[j] >> 4) + 0x37;
					}else{
						smsdata[i][2*j]=(buff[j] >> 4) + 0x30;
					}
					if(((buff[j] & 0x0F)>=0x0A) && ((buff[j] & 0x0F)<=0x0F)){
						smsdata[i][2*j+1]=(buff[j] & 0x0F) + 0x37;
					}else{
						smsdata[i][2*j+1]=(buff[j] & 0x0F) + 0x30;
					}
				}

				smsdata[i][2*j+2] = 0x1A;   //ctrl+z
				smsdata_len[i] = lenbak + (count - 138 * i + 2) + 1;  //smsdata_lenΪҪ���͵����ݵĳ��� ��smsdata_len-9��Ϊ��AT+CMGSʱ��������ݳ���
			}else{
				smsdata[i][point++] = (((smseq >> 12) | i) << 12);
				smsdata[i][point++] = (((smseq >> 12) | i) << 12); 	  //�ȵ��ֽ��ٸ��ֽ�
				memcpy(&smsdata[i][point], (buf + 138 * i), 138);
				point += 138;

				/* ����ת��Ϊ ASCII */
				memset(buff, 0, 512);
				memcpy(buff, smsdata[i], point);
				for(j=0; j<point; j++){
					if(((buff[j] & 0xF0) >= 0xA0) && ((buff[j] & 0xF0) <= 0xF0)){
						smsdata[i][2*j] = (buff[j] >> 4) + 0x37;
					}else{
						smsdata[i][2*j] = (buff[j] >> 4) + 0x30;
					}
					if(((buff[j] & 0x0F) >= 0x0A) && ((buff[j] & 0x0F) <= 0x0F)){
						smsdata[i][2*j+1] = (buff[j] & 0x0F) + 0x37;
					}else{
						smsdata[i][2*j+1] = (buff[j] & 0x0F) + 0x30;
					}
				}

				smsdata[i][2*j+2] = 0x1A;   //ctrl+z
				smsdata_len[i] = lenbak + 140 + 1;
			}
			point = 0 ;  //ÿһ֡��������
			}
			smseq++;   	//ÿһ��֡��ֵ����
			if(smseq > 0x0FFF){
				smseq = 0;
			}

	}else{
	    i = 0;
	    point = 0;
	    sms_total = 1;  //��֡
	    smsdata[i][point++] = 0x08;
	    smsdata[i][point++] = 0x91;
	    smsdata[i][point++] = 0x68;
	    //����������ĺ���
	    memcpy(&smsdata[i][point], tel_center, 6);
	    point += 6;
	    smsdata[i][point++] = 0x11;             // 11	��������(TP-MTI/VFP)	���ͣ�TP-VP����Ը�ʽ
        smsdata[i][point++] = 0x00;             // 00	��Ϣ��׼ֵ(TP-MR)	0

        if((tel[strlen((char *)tel)-1] & 0xF0) == 0xF0){  //�������һλ�ĸ�4�ֽ��Ƿ�ΪF
	    	smsdata[i][point++] = (strlen((char *)tel) - 1) * 2 -1; //���ں���Ϊ�������ܳ��ȼ�ȥ91����A1��1�ֽڣ�������2 �ټ�ȥF
	    }else{
	    	smsdata[i][point++] = (strlen((char *)tel) - 1) * 2;
	    }
	    memcpy(&smsdata[i][point], tel, strlen((char *)tel)); //Ҫ���͵ĺ���
	    point += strlen((char *)tel) ;
	    smsdata[i][point++] = 0x00; //Э���ʶ������0x00��0x21
	    smsdata[i][point++] = 0x04; //�����־��ΪHEX
	    smsdata[i][point++] = 0x00;                     //��Ч�� 5����

	    smsdata[i][point++] = count + 2;	 //���������ֽڵķ�֡��־
	    smsdata[i][point++] = (smseq | 0x8000);
	    smsdata[i][point++] = (smseq | 0x8000) >> 8; //�ȵ��ֽ��ٸ��ֽ�
	    
	    memcpy(&smsdata[i][point], buf, count);
	    point += count;

	    /* ����ת��Ϊ ASCII */
    	memset(buff, 0, 512);
    	memcpy(buff, smsdata[i], point);
    	for(j=0; j<point; j++){
    		if(((buff[j] & 0xF0) >= 0xA0) && ((buff[j] & 0xF0) <= 0xF0)){
    			smsdata[i][2*j]=(buff[j] >> 4) + 0x37;
    		}else{
    			smsdata[i][2*j]=(buff[j] >> 4) + 0x30;
    		}
    		if(((buff[j] & 0x0F)>=0x0A) && ((buff[j] & 0x0F)<=0x0F)){
    			smsdata[i][2*j+1]=(buff[j] & 0x0F) + 0x37;
    		}else{
    			smsdata[i][2*j+1]=(buff[j] & 0x0F) + 0x30;
    		}
    		if(j > 138){
    			j--;
    		}
    	}
    	smsdata[i][2*j] = 0x1A;   //ctrl+z
    	smsdata_len[i] = point + 1;
	    point = 0 ;  //��������
   }

	//200S�ڷ���---�ʱ�䣿��������
	timeout = 0;  //ȷ�ϳ�ʱ
	back = 1;     //ȷ�ϱ�ʶ
	step = DEV_STREAM_DEMAND;
	i = 0;
	while(back){
		switch(step){
		case DEV_STREAM_DEMAND:
			//��շ��ͻ���
			memset(send_cmd, 0, sizeof(send_cmd));
			comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_WR);
			sprintf(send_cmd, "%s%d\r", "AT+CMGS=", (smsdata_len[i] - 10));	 //��Ҫ��ȥǰ���9���ֽ�(SMSC)������0x1A
			//������������
			ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
			if(ret != strlen(send_cmd)){
				goto error;
			}
			step = DEV_STREAM_DEFINE;
			sleep(1);
			break;
		case DEV_STREAM_DEFINE:     //������������ʾ
			memset(recv_cmd, 0, sizeof(recv_cmd));
			ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, 10);
			flag = 0;
			if(ret > 0){     //���յ�������ʾ��
				for (j = 0; j < ret; j++)
				{
					if (recv_cmd[j] == '>'){
						flag = 1;
						break;
					}
				}
				if (flag == 1){
					//��շ��ͻ���
					comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_WR);
					//���Ͷ���
					ret = comport_send (CFG_GPRS_ME3000_COMPORT, smsdata[i], smsdata_len[i]);
					if (ret < 0){
						goto error;
					}
					timeout = 0;
					step = DEV_STREAM_RECV_VERIFY;
				}else{
					ret = deal_recv(recv_cmd, ref, (char *)'>');    //�����ڣ������ڻ��嶼���������Ѷ��������ݷ������ݻ���֮�в��ж��Ƿ�����Ҫ�ķ��أ�
					if(ret == 0){
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_WR);
						//���Ͷ���
						ret = comport_send (CFG_GPRS_ME3000_COMPORT, smsdata[i], smsdata_len[i]);
						if (ret < 0){
							goto error;
						}
						timeout = 0;
						step = DEV_STREAM_RECV_VERIFY;
					}else if(ret == -ERR_GPRS_CLOSE){  //�����������Ͽ�
						goto error;
					}else{
						step = DEV_STREAM_DEFINE;  //��������>��
					}
				}
			}else if(ret == 0){
				timeout = timeout - 1 - CFG_RECV_TIMEOUT;
				if(timeout > 0){
					step = DEV_STREAM_DEFINE;
					sleep(1);
				}else{
					back = 0;
					ret = -ERR_TIMEOUT;
					goto error;
				}
			}else if(ret < 0){
				goto error;
			}
			break;
		case DEV_STREAM_RECV_VERIFY:
			ref = comport_recv (CFG_GPRS_ME3000_COMPORT, recv_cmd, 11);   //���Է���λ����ʮλ������λ�������
			if((ret > 0) && (check_echo((char *)recv_cmd, "\r\n+CMGS:") == 0)){
				ret = atoi((char *)&recv_cmd[8]);
				back = 0;
				timeout = 0;    //�п��ܰ�0K�������������Ļ�OK���״̬�Ͳ�Ҫ�ˣ�ֱ����Ϊ���ͳɹ�
				i++;
			}else if((ret > 0) && (check_echo((char *)recv_cmd, "\r\nCMS ERROR:") == 0)){
				back = 0;
				timeout = 0;
				ret = -ERR_GPRS_MODEL;
				goto error;
			}else if(ret > 0){
				ret = deal_recv(recv_cmd, ref, "\r\n+CMGS:");    //�����ڣ������ڻ��嶼���������Ѷ��������ݷ������ݻ���֮�в��ж��Ƿ�����Ҫ�ķ��أ�
				if(ret == 0){
					timeout = 0;
					step = DEV_STREAM_RECV_OK;     //���ok���ܺ�+cmgsһ���յ�����ô���OK���״̬�Ͳ�Ҫ��??????��ȷ��
					i++;
				}else if(ret == -ERR_GPRS_CLOSE){  //�����������Ͽ�
					goto error;
				}else{
					step = DEV_STREAM_RECV_VERIFY;  //��������+TCPSEND��
				}
			}else if(ret == 0){
				timeout = timeout - 1 - CFG_RECV_TIMEOUT;
				if(timeout > 0){
					step = DEV_STREAM_RECV_VERIFY;
					sleep(1);
				}else{
					back = 0;
					ret = -ERR_TIMEOUT;
					goto error;
				}
			}else if(ret < 0){
				goto error;
			}
			break;
		case DEV_STREAM_RECV_OK:   //���շ���ȷ�ϡ�OK��????????????����һ��һ��״̬
			//�ж��Ƿ��ͳɹ���Ϣ
			memset(recv_cmd, 0, sizeof(recv_cmd));
			recv_len = strlen("\r\nOK\r\n");
			ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, recv_len);
			if (ret < 0){
				goto error;      //���մ���
			}else if((ret > 0) && (check_echo((char *)recv_cmd, "\r\nOK\r\n") == 0)){
				timeout = 0;
				if(i == sms_total){
					back = 0;
					ret = count; //???????????����
				}else{
					step = DEV_STREAM_DEMAND;   //��������
				}
			}else if((ret > 0) && (check_echo((char *)recv_cmd, "\r\nOK\r\n") != 0)){
				ret = deal_recv(recv_cmd, ref, "\r\nOK\r\n");    //�����ڣ������ڻ��嶼���������Ѷ��������ݷ������ݻ���֮�в��ж��Ƿ�����Ҫ�ķ��أ�
				if(ret == 0){
					timeout = 0;
					back = 0;
				}else if(ret == -ERR_GPRS_CLOSE){  //�����������Ͽ�
					back = 0;
					goto error;
				}else{
					step = DEV_STREAM_RECV_OK;  //��������\r\nOK\r\n��
				}
			}else if (ret==0){
				timeout = timeout - 1 - CFG_RECV_TIMEOUT;
				if(timeout > 0){
					step = DEV_STREAM_RECV_OK;
					sleep(1);
				}else{
					back = 0;
					ret = -ERR_TIMEOUT;
					goto error;
				}
			}
			break;
		default:
			back = 0;
			ret = -ERR_SYS;
			goto error;
		break;
		}
	}
	
error:
	//���������
	ref = pthread_mutex_unlock(&(gprsstate.mutex));
	if (ref){
		ret = -ERR_SYS;
		goto error;
	}
	return(ret);
}


int gprs_me3000_recvsmg(u8 *buf, u32 count)
{
	return(0);
}

/******************************************************************************
*	����:	gprs_me3000_connect
*	����:	gprsģ�齨������
*	����:	mode			-	���ӷ�ʽ��0���������ӣ�1��������
*	����:	0				-	�ɹ�(�յ�ȷ������)
			-ERR_TIMEOUT	-	���ӳ�ʱ

*	˵��:	��
******************************************************************************/
int gprs_me3000_connect(u8 mode)
{
	int ret,ref;
	u8 step, connect_flag;
	u8 timeout;
	char send_cmd[CFG_DATA_MAX];  	//������������
	u8 recv_cmd[CFG_RECV_BACKMAX];  //��������
	
	u8 ip_port[6];
	memcpy(ip_port, gprsstate.ip, sizeof(ip_port));
	struct timespec to;
	//��ô�����
	to.tv_sec = time(NULL) + CFG_SEND_TIMEOUT;      //��
	to.tv_nsec = 0;                       			//����
	ref = pthread_mutex_timedlock(&(gprsstate.mutex), &to);
	if (ref == ETIMEDOUT){
		ret = -ERR_BUSY;               //�豸æ�������У�
		return ret;
	}else{
		ret = -ERR_SYS;
		return ret;
	}
	if (mode == 0){
		step = DEV_GPRS_ATE0;
	}else if(mode == 1){
		step = DEV_GPRS_CHECK_PPPNET;
	}
	connect_flag = 1;
	while(connect_flag){     //connect_flag = 0 �����Ѿ�����
		switch(step){
			case DEV_GPRS_ATE0:    //�رջ���
				memset(send_cmd, 0, sizeof(send_cmd));
				memcpy(send_cmd, "ATE0\r\n", sizeof("ATE0\r\n")-1);   
				//��շ��ͻ���
				comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
				//������������
				ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
				if(ret != strlen(send_cmd)){
					goto error;
				}
				timeout = 10;   //���Իظ������ʱ�� ----- �ֲ�
				step = DEV_GPRS_CHECK_ATE0;
				sleep(1);
				break;
			case DEV_GPRS_CHECK_ATE0:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if ((check_echo((char *)recv_cmd, "\r\nOK\r\n") == 0) || (check_echo((char *)recv_cmd, "ATE0") == 0)){    //���Ա��ض�
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+CNMI=2,2,0,0,0\r\n", sizeof("AT+CNMI=2,2,0,0,0\r\n")-1);   
						//��ջ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						timeout = 5;   //CNMI���ûظ������ʱ�� ----- �ֲ�
						step = DEV_GPRS_CNMI;
						sleep(1);
					}else{
						step = DEV_GPRS_CHECK_ATE0;
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout > 0){
						//�ط�
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_CHECK_ATE0;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CNMI:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\nOK\r\n") == 0){
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+CNMI?\r\n", sizeof("AT+CNMI?\r\n")-1);   
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						timeout = 10;   //CNMI��ѯ�ظ������ʱ�� ----- �ֲ�
						step = DEV_GPRS_CHECK_CNMI;
						sleep(1);	
					}else if (check_echo((char *)recv_cmd, "\r\nCME ERROR:") == 0){
						//���Դ�ӡһ�´����ʶ
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
					}else{
						step = DEV_GPRS_CNMI;
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout > 0){
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_CNMI;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CHECK_CNMI:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\n+CNMI: 2,2,0,0,0\r\nOK\r\n") == 0){       //������������֤
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+CMGF=0\r\n", sizeof("AT+CMGF=0\r\n")-1);  //����ΪPDUģʽ 
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						timeout = 10;   //CMGF�ظ������ʱ�� ----- �ֲ�
						step = DEV_GPRS_CMGF;
						sleep(1);	
					}else{
						step = DEV_GPRS_CHECK_CNMI;
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout){
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_CHECK_CNMI;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CMGF:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\nOK\r\n") == 0){
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+CMGF?\r\n", sizeof("AT+CMGF?\r\n")-1);   //��ѯ�Ƿ�ΪPDU��ʽ
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						timeout = 10;   //CMGF��ѯ�ظ������ʱ�� ----- �ֲ�
						step = DEV_GPRS_CHECK_CMGF;
						sleep(1);	
					}else if (check_echo((char *)recv_cmd, "\r\nCME ERROR:") == 0){
						//���Դ�ӡһ�´����ʶ
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
					}else{
						step = DEV_GPRS_CMGF;
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout > 0){
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_CMGF;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CHECK_CMGF:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\n+CMGF: 0\r\nOK\r\n") == 0){    //????�����
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+CSMS=0\r\n", sizeof("AT+CSMS=0\r\n")-1);  //���ö��ŷ���SMS���ATָ��֧�� GSM07.05 Phase 2��
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						timeout = 10;   //CNMI��ѯ�ظ������ʱ�� ----- �ֲ�
						step = DEV_GPRS_CSMS;
						sleep(1);	
					}else{
						step = DEV_GPRS_CHECK_CMGF;
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout){
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_CHECK_CMGF;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CSMS:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\nOK\r\n") == 0){     //�ظ�����֤��������
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+CSMS?\r\n", sizeof("AT+CSMS?\r\n")-1);   //��ѯCSMS
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						timeout = 10;   //CNMI��ѯ�ظ������ʱ�� ----- �ֲ�
						step = DEV_GPRS_CHECK_CSMS;
						sleep(1);	
					}else if (check_echo((char *)recv_cmd, "\r\nCME ERROR:") == 0){
						//���Դ�ӡһ�´����ʶ
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
					}else{
						step = DEV_GPRS_CSMS;
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout > 0){
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_CSMS;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CHECK_CSMS:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\n+CSMS: 0") == 0){
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+CSCS=\"HEX\"\r\n", sizeof("AT+CSCS=\"HEX\"\r\n")-1);  //���ñ�����ʽ
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						timeout = 10;   //CSCS��ѯ�ظ������ʱ�� ----- �ֲ�
						step = DEV_GPRS_CSCS;
						sleep(1);	
					}else{
						step = DEV_GPRS_CHECK_CSMS;
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout){
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_CHECK_CSMS;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CSCS:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\nOK\r\n") == 0){
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+CSCS?\r\n", sizeof("AT+CSCS?\r\n")-1);   //��ѯCSMS
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						timeout = 10;   //CSCS��ѯ�ظ������ʱ�� ----- �ֲ�
						step = DEV_GPRS_CHECK_CSCS;
						sleep(1);	
					}else if (check_echo((char *)recv_cmd, "\r\nCME ERROR:") == 0){
						//���Դ�ӡһ�´����ʶ
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
					}else{
						step = DEV_GPRS_CSCS;
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout > 0){
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_CSCS;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CHECK_CSCS:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\n+CSCS: \"HEX\"") == 0){
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+CREG?\r\n", sizeof("AT+CREG?\r\n")-1);  //��ѯ����	
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						timeout = 200;   //��ѯ����ظ������ʱ�� ----- �ֲ�
						step = DEV_GPRS_CHECK_CREG;
						sleep(1);	
					}else{
						step = DEV_GPRS_CHECK_CSCS;
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout){
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_CHECK_CSCS;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CHECK_CREG:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\n+CREG:") == 0){     //����������OK������
						//�����ɹ�
						if ((recv_cmd[11] == '1') || (recv_cmd[11] == '5')){   //1��ʾ��ע�᱾�����磬5��ʾ��ע������״̬
		            		memset(send_cmd, 0, sizeof(send_cmd));
							memcpy(send_cmd, "AT+XISP=0\r\n", sizeof("AT+XISP=0\r\n")-1);  //�����ڲ�Э��ջ
							//��շ��ͻ���
							comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
							//������������
							ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
							if(ret != strlen(send_cmd)){
								goto error;
							}
							//����ʱ�䲻����80s
							step = DEV_GPRS_XISP;
							timeout = 80;    //�����ڲ�Э��ʱ��----�ֲ�
							sleep(1);	
						}else{
							timeout = 0;
						}
					}else{
						step = DEV_GPRS_CHECK_CREG;
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout > 0){
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_CHECK_CREG;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_XISP:     //�����ڲ�Э��
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\nOK\r\n") == 0){
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+XISP?\r\n", sizeof("AT+XISP?\r\n")-1);  //����Э������0�ڲ�Э��
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						timeout = 30;   //��ѯXISP�ظ������ʱ�� ----- �ֲ�
						step = DEV_GPRS_CHECK_XISP;
						sleep(1);	
					}else{
						step = DEV_GPRS_XISP;
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout){
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_XISP;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CHECK_XISP:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\n+XISP:0") == 0){
						//Э��������ȷ
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", sizeof("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n")-1);   //����PDP��ʽ
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_INITPPP;
						timeout = 80;        //����PDP��ʽ�ظ�ʱ�� -----�ֲ�                      
						sleep(1);	
					}else{
						step = DEV_GPRS_CHECK_XISP;
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout > 0){
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_CHECK_XISP;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CODE:
				/************���õ�¼�û��������롢��¼�����˳�򣿣���******************/
				//���APN��USER,PWD������������������
				memset(send_cmd, 0, sizeof(send_cmd));
				snprintf(send_cmd, 100, "AT+ZPNUM=\"%s\",\"%s\",\"%s\"\r\n", "APN", "USER", "PWD");
				//��շ��ͻ���
				comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
				//������������
				ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
				if(ret != strlen(send_cmd)){
					goto error;
				}
				timeout = 80;   //����ظ������ʱ�� ----- �ֲ�
				step = DEV_GPRS_INITPPP;
				sleep(1);	
				break;
			case DEV_GPRS_INITPPP:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\nOK\r\n") == 0){
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+XIIC=1\r\n", sizeof("AT+XIIC=1\r\n")-1);            //��¼����
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						timeout = 200;   //��¼����ظ������ʱ�� ----- �ֲ�
						step = DEV_GPRS_CHECK_PPPOK;
						sleep(1);	
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout > 0){
						//���·�������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						step = DEV_GPRS_INITPPP;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
					
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CHECK_PPPOK:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\nOK\r\n") == 0){
						memset(send_cmd, 0, sizeof(send_cmd));
						memcpy(send_cmd, "AT+XIIC?\r\n", sizeof("AT+XIIC?\r\n")-1);   //��ѯ�Ƿ��¼����
						//��շ��ͻ���
						comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
						//������������
						ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
						if(ret != strlen(send_cmd)){
							goto error;
						}
						timeout = 30;   //��¼����ظ������ʱ�� ----- �ֲ�
						step = DEV_GPRS_CHECK_PPPNET;
						sleep(1);	
					}else{
						step = DEV_GPRS_CODE;   //û�е�¼�ɹ�ȥ�����û���������
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout > 0){
						step = DEV_GPRS_CHECK_PPPOK;
						sleep(1);
					}else{
						step = DEV_GPRS_CODE;   //û�е�¼�ɹ�ȥ�����û���������
					}
				}else{
					goto error;
				}
				break;
			case DEV_GPRS_CHECK_PPPNET:
				if (mode == 0){
					memset(recv_cmd, 0, sizeof(recv_cmd));
					ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
					if (ret > 0){
						if (check_echo((char *)recv_cmd, "\r\n+XIIC:1") == 0){
							//PPP������
							//???????????????????����״̬
							//IP����
	    					memset(send_cmd, 0, sizeof(send_cmd));
							if(gprsstate.connect_index == DEV_GPRS_TCP){
	        					sprintf(send_cmd, "AT+TCPSETUP=0,%d.%d.%d.%d,%d\r\n",ip_port[0],ip_port[1],ip_port[2],ip_port[3],ip_port[4]|(ip_port[5]<<8));    //TCP����,bit7Ϊ0
	    
	    					}else{
	    						sprintf(send_cmd, "AT+UDPSETUP=0,%d.%d.%d.%d,%d\r\n",ip_port[0],ip_port[1],ip_port[2],ip_port[3],ip_port[4]|(ip_port[5]<<8));    //UDP����,bit7Ϊ1
	    					}
							//��շ��ͻ���
							comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
							//������������
							ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
							if(ret != strlen(send_cmd)){
								goto error;
							}
							timeout = 60;   //IP���ӻظ������ʱ�� ----- �ֲ�
							step = DEV_GPRS_CHECK_PPPLINK;
							sleep(1);	
							
						}else{
							step = DEV_GPRS_CHECK_CREG;
						}
					}else if (ret == 0){
						timeout = timeout - 1 - CFG_RECV_TIMEOUT;
						if(timeout > 0){
							step = DEV_GPRS_CHECK_CREG;
							sleep(1);
						}else{
							ret = -ERR_TIMEOUT;
							goto error;
						}
					}else{
						goto error;
					}
				}else if (mode == 1){      //���¼�������
					memset(send_cmd, 0, sizeof(send_cmd));
					if(gprsstate.connect_index == DEV_GPRS_TCP){
    					sprintf(send_cmd, "AT+TCPSETUP=0,%d.%d.%d.%d,%d\r\n",ip_port[0],ip_port[1],ip_port[2],ip_port[3],ip_port[4]|(ip_port[5]<<8));    //TCP����,bit7Ϊ0

					}else{
						sprintf(send_cmd, "AT+UDPSETUP=0,%d.%d.%d.%d,%d\r\n",ip_port[0],ip_port[1],ip_port[2],ip_port[3],ip_port[4]|(ip_port[5]<<8));    //UDP����,bit7Ϊ1
					}
					//��շ��ͻ���
					comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
					//������������
					ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
					if(ret != strlen(send_cmd)){
						goto error;
					}
					timeout = 60;   //IP���ӻظ������ʱ�� ----- �ֲ�
					step = DEV_GPRS_CHECK_PPPLINK;
					sleep(1);	
				}
				break;
			case DEV_GPRS_CHECK_PPPLINK:
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret > 0){
					if (check_echo((char *)recv_cmd, "\r\nOK\r\n\r\n+TCPSETUP:0,OK") == 0){    //���ӳɹ�
						//״̬λ��������
					}
				}else if (ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout > 0){
						step = DEV_GPRS_CHECK_CREG;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}else{
					goto error;
				}
				break;
			default:
				break;
		}
	}
	
error:
	//���������
	ret = pthread_mutex_unlock(&(gprsstate.mutex));
	if (ret){
		ret = -ERR_SYS;
		goto error;
	}
	return(ret);
}

/******************************************************************************
*	����:	gprs_me3000_disconnect
*	����:	gprsģ��Ͽ�����
*	����:	
*	����:	0				-	�ɹ�(�յ�ȷ������)
			-ERR_TIMEOUT	-	���ӳ�ʱ

*	˵��:	��
******************************************************************************/
int gprs_me3000_disconnect(void)
{
	int ret,ref;
	u8 timeout,back;
	u8 step;            			//����״̬
	char send_cmd[CFG_DATA_MAX];  	//������������
	u8 recv_cmd[CFG_RECV_BACKMAX];  				//��������
	
	struct timespec to;
	
	//�ж��Ƿ�Ϊ����״̬
	if(gprsstate.connect_state == 0){
		ret = -ERR_DISCONNECT;
		return ret;
	}

	//�����
	to.tv_sec = time(NULL) + CFG_SEND_TIMEOUT;      //��
	to.tv_nsec = 0;                       			//����
	ref = pthread_mutex_timedlock(&(gprsstate.mutex), &to);
	if (ref == ETIMEDOUT){
		ret = -ERR_BUSY;               //�豸æ�������У�
		return ret;
	}else if(ref){
		ret = -ERR_SYS;
		return ret;
	}
	
	timeout = 0;  //ȷ�ϳ�ʱ
	back = 1;     //ȷ�ϱ�ʶ
	step = DEV_STREAM_DEMAND;
	while(back){
		switch(step){
			case DEV_STREAM_DEMAND:
				memset(send_cmd, 0, sizeof(send_cmd));
				if(gprsstate.connect_index == 0){    //tcp��ʽ
					memset(send_cmd, 0, sizeof(send_cmd));
					memcpy(send_cmd, "AT+TCPCLOSE=0\r\n", sizeof("AT+TCPCLOSE=0\r\n")-1);

				}else{                               //UDP��ʽ
					memset(send_cmd, 0, sizeof(send_cmd));
					memcpy(send_cmd, "AT+UDPCLOSE=0\r\n", sizeof("AT+UDPCLOSE=0\r\n")-1);
				}
				//��շ��ͻ���
				comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
				//������������
				ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
				if(ret != strlen(send_cmd)){
					goto error;
				}
				timeout = 60;   //IP���ӻظ������ʱ�� ----- �ֲ�
				step = DEV_GPRS_CHECK_PPPLINK;
				sleep(1);
				break;
			case DEV_STREAM_RECV_VERIFY:
				//�ж��Ƿ��յ��ɹ��Ͽ���Ϣ
				memset(recv_cmd, 0, sizeof(recv_cmd));
				ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
				if (ret < 0){
					goto error;      //���մ���
				}else if(ret > 0){
					if ((check_echo((char *)recv_cmd, "\r\n+TCPCLOSE") == 0) && (check_echo((char *)recv_cmd, "\r\n+UDPCLOSE") == 0)){
					//??״̬��
						back = 0;
						ret = 0;
					}
				}else if(ret == 0){
					timeout = timeout - 1 - CFG_RECV_TIMEOUT;
					if(timeout > 0){
						step = DEV_GPRS_CHECK_CREG;
						sleep(1);
					}else{
						ret = -ERR_TIMEOUT;
						goto error;
					}
				}
				break;
			default:
				break;
		}
	}
	
error:
	//���������
	ret = pthread_mutex_unlock(&(gprsstate.mutex));
	if (ret){
		ret = -ERR_SYS;
		goto error;
	}
	return(ret);
}

int gprs_me3000_getstat(u8 cmd, void *stat)
{
	return(0);
}

/******************************************************************************
*	����:	gprs_me3000_turnon
*	����:	����
*	����:	
*	����:	0				-	�ɹ�
			

*	˵��:	��
******************************************************************************/
int gprs_me3000_turnon(void)
{
	return(0);
}

/******************************************************************************
*	����:	gprs_me3000_turnoff
*	����:	�ػ�
*	����:	
*	����:	0				-	�ɹ�
			-ERR_TIMEOUT	-	��ʱ

*	˵��:	Ӳ���ϵ�ǰӦ����ػ�������GPRSģ��
******************************************************************************/
int gprs_me3000_turnoff(void)
{
	int ret,ref;
	u8 timeout;
	char send_cmd[CFG_DATA_MAX];  	//������������
	u8 recv_cmd[CFG_RECV_BACKMAX];  //��������
	struct timespec to;
	
	//�����
	to.tv_sec = time(NULL) + CFG_SEND_TIMEOUT;      //��
	to.tv_nsec = 0;                       			//����
	ref = pthread_mutex_timedlock(&(gprsstate.mutex), &to);
	if (ref == ETIMEDOUT){
		ret = -ERR_BUSY;               //�豸æ�������У�
		return ret;
	}else if(ref){
		ret = -ERR_SYS;
		return ret;
	}
	
	timeout = CFG_SEND_TIMEOUT;
	memset(send_cmd, 0, sizeof(send_cmd));
	//��շ��ͻ���
	comport_flush(CFG_GPRS_ME3000_COMPORT, COMPORT_FLUSH_ALL);
	//���͹ػ�����
	memcpy(send_cmd, "AT+CPWROFF\r\n", sizeof("AT+CPWROFF\r\n")-1);
	ret = comport_send(CFG_GPRS_ME3000_COMPORT, (u8 *)send_cmd, strlen(send_cmd));
	if(ret != strlen(send_cmd)){
		goto error;
	}
	timeout = 10;   //�ػ��ظ������ʱ�� ----- �ֲ�
	while(timeout){
		//�ж��Ƿ��յ��ɹ��Ͽ���Ϣ
		memset(recv_cmd, 0, sizeof(recv_cmd));
		ret = comport_recv(CFG_GPRS_ME3000_COMPORT, recv_cmd, sizeof(recv_cmd));
		if (ret < 0){
			goto error;      //���մ���
		}else if(ret > 0){
			if (check_echo((char *)recv_cmd, "\r\nOK\r\n") == 0){
				ret = 0;
				timeout = 0;
			}
		}else if(ret == 0){
			timeout = timeout - 1 - CFG_RECV_TIMEOUT;
			ret = -ERR_TIMEOUT;
		}
	}
	
error:
	//���������
	ret = pthread_mutex_unlock(&(gprsstate.mutex));
	if (ret){
		ret = -ERR_SYS;
		goto error;
	}
	return(ret);
}

/******************************************************************************
*	����:	gprs_me3000_poweron
*	����:	�ϵ�
*	����:	
*	����:	0				-	�ɹ�
			

*	˵��:	��
******************************************************************************/
int gprs_me3000_poweron(void)
{
	return(0);
}

/******************************************************************************
*	����:	gprs_me3000_poweroff
*	����:	����
*	����:	
*	����:	0				-	�ɹ�
			

*	˵��:	��
******************************************************************************/
int gprs_me3000_poweroff(void)
{
	return(0);
}

/******************************************************************************
*	����:	gprs_me3000_reset
*	����:	��������
*	����:	
*	����:	0				-	�ɹ�
			

*	˵��:	��
******************************************************************************/
int gprs_me3000_reset(void)
{
	return(0);
}





