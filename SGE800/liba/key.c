/*****************************************************************************
	��̵����ɷ����޹�˾			��Ȩ��2008-2015

	��Դ���뼰������ĵ�Ϊ���������̵����ɷ����޹�˾�������У�δ��������
	�ɲ��������޸Ļ򷢲�������׷����صķ������Ρ�

						���������̹ɷ����޹�˾
						www.xjgc.com
						(0374) 321 2924
*****************************************************************************/


/******************************************************************************
	��Ŀ����	��  SGE800���������ն�ƽ̨
	�ļ�		��  key.c
	����		��  ���ļ�������״̬��ģ��Ľӿ�
	�汾		��  0.1
	����		��  ·ȽȽ
	��������	��  2010.04
******************************************************************************/
//������ͷ�ļ�
#include "private/config.h"
	
//ģ�����ÿ���
#ifdef CFG_KEY_MODULE
//C��ͷ�ļ�
#include <stdio.h>
#include <fcntl.h> 		//open ��־	
#include <sys/ioctl.h>		//ioctl
#include <string.h> 		//memset
#include <unistd.h>		//close

//�ṩ���û���ͷ�ļ�
#include "include/key.h"
#include "include/gpio.h"
#include "include/error.h"
	
//��������ͷ�ļ�
#include "private/debug.h"
	
/*************************************************
  ��̬ȫ�ֱ������궨��
*************************************************/
typedef struct {
	u8	status;				//keyֵ״̬
	u8	level;				//��ǰio�ڵ�ƽ
	u8	chn;				//keyͨ����Ӧ��io������ͨ��
	u8	type;				//IO�ߵ��밴���Ƿ���֮�����ϵ��1-�߰��£�0-���ͷ�
}key_info_t;

#define KEYSTAT_UP		0				//��ǰ�����ͷ�
#define KEYSTAT_DOWN	1				//��ǰ��������
#define KEYSTAT_LONG	2				//��ǰ��������
#define PINBASE 		32				//���뵽������io�ڵ�ַ��ַ

#define MAX_KEY_CHN 16					//���key����

static u8 	key_count = 0;				//ģ��򿪼���
static key_info_t	key[CFG_KEY_NUM];

/*************************************************
  API
*************************************************/

/******************************************************************************
*	����:	key_init
*	����:	����ģ���ʼ��
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_BUSY		-	�Ѿ���
	˵��:�����������󲿷ַ���gpio.h��gpio_init��gpio_set�����Ĵ���
 ******************************************************************************/
int key_init(void)
{
	int ret = -1;
	int i;

	if(key_count == 1){
		ret = -ERR_BUSY;		//�Ѿ���
		goto err;
	}

	//����ͨ��
#ifdef CFG_KEY_1
	key[KEY1].chn = CFG_KEY_1 ;
	key[KEY1].type = CFG_KEY_TYPE1;
#endif
#ifdef CFG_KEY_2
	key[KEY2].chn = CFG_KEY_2 ;
	key[KEY2].type = CFG_KEY_TYPE2;
#endif
#ifdef CFG_KEY_3
	key[KEY3].chn = CFG_KEY_3  ;
	key[KEY3].type = CFG_KEY_TYPE3;
#endif
#ifdef CFG_KEY_4
	key[KEY4].chn = CFG_KEY_4 ;
	key[KEY4].type = CFG_KEY_TYPE4;
#endif
#ifdef CFG_KEY_5
	key[KEY5].chn = CFG_KEY_5  ;
	key[KEY5].type = CFG_KEY_TYPE5;
#endif
#ifdef CFG_KEY_6
	key[KEY6].chn = CFG_KEY_6  ;
	key[KEY6].type = CFG_KEY_TYPE6;
#endif
#ifdef CFG_KEY_7
	key[KEY7].chn = CFG_KEY_7  ;
	key[KEY7].type = CFG_KEY_TYPE7;
#endif
#ifdef CFG_KEY_8
	key[KEY8].chn = CFG_KEY_8  ;
	key[KEY8].type = CFG_KEY_TYPE8;
#endif
	//��gpio��
	ret = gpio_init();
	if((ret < 0) && (ret != -ERR_BUSY)){
		goto err;
	}
	for(i = 1; i <= CFG_KEY_NUM; i++){
		ret = gpio_set(key[i].chn,GPIO_IN,0,0);
		if(ret < 0){
			goto err;
		}
		key[i].status = KEYSTAT_UP;
	}

	key_count = 1;
	ret = 0;
err:
	return ret;
}

/******************************************************************************
*	����:	key_get
*	����:	��ȡ������Ч״̬
*	����:	timeout			-	��ʱʱ�䣨��λΪ��,Ϊ0��ʾ������������
*	����:	0				-	�޼�����
*			0~15			-	��Ӧ���Ű���
*			100~115			-	��Ӧ���ų���
			-ERR_NOINIT		-	û�г�ʼ��
*	˵��:�����������󲿷ַ���gpio.h��gpio_input_get�����Ĵ���
 ******************************************************************************/
int key_get(u8 timeout)
{
	int ret = -1;
	int i;
	u32 delay = 0;		//��ʱʱ���ۻ�

	fd_set rfds;		//select��ʱ
	struct timeval tv;
	int fd = 1;

    FD_ZERO (&rfds);
    FD_SET (fd, &rfds);

	if(key_count == 0){
		return -ERR_NOINIT;
	}

scan:
	//ɨ�谴��
	for(i = 1; i <= CFG_KEY_NUM; i++){
		ret = gpio_input_get(key[i].chn,&key[i].level);
		if(ret < 0){
			goto err;
		}
	}

	//����ɨ������,���м�����ͬʱ���£��򷵻ؼ���С��ֵ
	for(i = 1; i <= CFG_KEY_NUM; i++){
		switch(key[i].status){

			case KEYSTAT_DOWN:
				if(key[i].level != key[i].type){
					key[i].status = KEYSTAT_UP;	//�ͷ�
					if(CFG_KEY_MODE == 0){
						return i;
					}

				}else{
					if(delay >= CFG_KEY_LONG){
						delay = 0;
						key[i].status = KEYSTAT_LONG;
						return (100+i);			//������ʼ
					}
				}
				break;

			case KEYSTAT_LONG:
				if(key[i].level != key[i].type){
					key[i].status = KEYSTAT_UP;
					delay = 0;				//��������
				}
				if(delay >= CFG_KEY_PER){
					delay = 0;
					return (100+i);			//�����ڼ�
				}
				break;

			case KEYSTAT_UP:
				if(key[i].level == key[i].type){
					key[i].status = KEYSTAT_DOWN;
					if(CFG_KEY_MODE == 1){
						return (i);					//��������,���·���
					}

				}
				break;
			default:
				key[i].status = KEYSTAT_UP;
				break;
		}
	}

	tv.tv_sec = 0;
	tv.tv_usec = CFG_KEY_SCAN_DELAY*1000;				//��ʱ
	select (0, NULL, NULL, NULL, &tv);
	delay += CFG_KEY_SCAN_DELAY;

	if((timeout <= 0) || (delay < timeout*1000)){		//����ģʽ
		goto scan;
	}else{
		ret = 0;		//��ʱ�޼�����
	}
err:
	return ret;
}


/******************************************************************************
*	����:	key_close
*	����:	����ģ��رպ���
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_NOINIT		-	û�г�ʼ��
*	˵��:�����������󲿷ַ���gpio.h��gpio_close�����Ĵ���
 ******************************************************************************/
int key_close(void)
{
	int ret = -1;

	if(key_count == 0){
		return -ERR_NOINIT;
	}

	ret = gpio_close();
	if(ret < 0){
		return ret;
	}
	key_count = 0;
	ret = 0;
	return ret;
}
#endif
