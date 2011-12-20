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
	�ļ�		��  powercheck.c
	����		��  ���ļ������˵����ϵ���ӿ�
	�汾		��  0.1
	����		��  ·ȽȽ
	��������	��  2010.01
******************************************************************************/
//������ͷ�ļ�
#include "private/config.h"

//ģ�����ÿ���
#ifdef CFG_POWERCHECK_MODULE

//C��ͷ�ļ�
#include <stdio.h>
#include <fcntl.h> 		//ioctl
#include <string.h> 	//memset
#include <unistd.h>		//close
#include <signal.h>		//
#include <sys/ioctl.h>

//�ṩ���û���ͷ�ļ�
#include "include/powercheck.h"
#include "include/error.h"

//��������ͷ�ļ�
#include "private/drvlib/powerchecklib.h"

/*************************************************
  ��̬ȫ�ֱ������궨��
*************************************************/
#define PINBASE 32			//���뵽������io�ڵ�ַ��ַ
static int fd;				//����pwrd�����ļ�������
static u8  pwrd_count = 0;		//ģ��򿪼���
static u8  pwrd_mode;			//ģ���ģʽ
static u16 pwrd_timeout;		//�ȴ���ʱʱ��

/*************************************************
  API����ʵ��
*************************************************/
/******************************************************************************
*	����:	powercheck_init
*	����:	������ģ���ʼ��
*	����:	mode			-	������ģʽ
*	����:	0				-	�ɹ�
			-ERR_NOFILE		-	û�д�·��
			-ERR_BUSY		-	�豸æ���Ѿ���
			-ERR_SYS		-	��ʼ����ʧ�ܣ����ڴ治�㣻�Ѿ���ʼ��
*	˵��:	��
 ******************************************************************************/
int powercheck_init (u8 mode)
{
	int ret = -1;
	if(pwrd_count == 1){
		ret = -ERR_BUSY;		//�Ѿ���
		goto err;
	}
	if(mode == POWERCHECK_MODE_NOBLOCK){
		//����Ϊ��������ʽ��ȡ
		fd = open("/dev/power_detect",O_RDWR | O_NOCTTY | O_NONBLOCK);		
		if (fd < 0){
			ret = -ERR_NOFILE;		//û�д�·��
			goto err;
		} 
	}else if(mode == POWERCHECK_MODE_BLOCK_UP || mode == POWERCHECK_MODE_BLOCK_DOWN ||
		mode == POWERCHECK_MODE_BLOCK_UPDOWN){
		pwrd_mode = mode;
		fd = open("/dev/power_detect",O_RDWR | O_NOCTTY);		
		if (fd < 0){
			ret = -ERR_NOFILE;		//û�д�·��
			goto err;
		} 
	}else{
		ret = -ERR_INVAL;		//������Ч
		goto err;
	}		
	
	//���õ����ϵ����Ӳ���ܽ�
	ret = ioctl(fd, PWRD_SET_IO, CFG_POWERCHECK_IO + PINBASE);
	if (ret < 0){
		ret = -ERR_SYS; 	
		goto err;
	} 
	//���õ��紦��ģʽ
	ret = ioctl(fd, PWRD_SET_MODE, pwrd_mode );
	if (ret < 0){
		ret = -ERR_SYS; 	
		goto err;
	} 
	
	pwrd_count = 1;
	ret = 0;
err:
	return ret;
}

/******************************************************************************
 * ����:	powercheck_setfasync
 * ����:	�����첽֪ͨ
 * ����:	p ����ָ��,�ص�����
 * ����:	0���ɹ�
 			-ESYS�� ����
 * ˵��:	��
 ******************************************************************************/
int powercheck_setfasync (void (*p)())
{
	int ret = -1;
	int oflags;
	//�����źźʹ�����
	signal(SIGIO, p);

	//�����豸�ļ���ӵ����Ϊ������
	ret = fcntl(fd, F_SETOWN, getpid());
	if(ret < 0){
		perror("setown\n");		
	} 
	//�����豸֧���첽֪ͨģʽ
	oflags = fcntl(fd, F_GETFL);
	ret = fcntl(fd, F_SETFL, oflags | FASYNC);
	if(ret < 0){
		perror("setfl\n");		
	} 
	return ret;
}

/******************************************************************************
*	����:	powercheck_setwaittime
*	����:	����ģʽ����������ʱ��
*	����:	timeout������ʱ�䣨��λ��100ms��Ϊ0��ʾ����������
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_NOINIT  	-	ģ��δ��ʼ��
*	˵��:	��
 ******************************************************************************/
int powercheck_setwaittime (u16 timeout)
{
	int ret = -1;
	pwrd_timeout = 10*timeout;	//�ڲ��ⲿ��ʱ��λת��
	ret = ioctl(fd, PWRD_TIMEOUT, pwrd_timeout );
	if (ret < 0){
		return -ERR_SYS;		
	} 
	return 0;
}

/******************************************************************************
*	����:	powercheck_getwaittime
*	����:	��ȡ��ǰ���õ�����ʱ��
*	����:	timeout������ʱ�䣨��λ��100ms��Ϊ0��ʾ���������������ݴ�����
*	����:	0				-	�ɹ�
			-ERR_NOINIT		-	ģ��δ��ʼ��
*	˵��:	��
 ******************************************************************************/
int powercheck_getwaittime (u16 *timeout)
{
	*timeout = pwrd_timeout;	
	return 0;
}

/******************************************************************************
*	����:	powercheck_check
*	����:	��⵱ǰ��Դ״̬
*	����:	��
*	����:	0				-	����״̬
			1				-	�ϵ�״̬
			-ESYS			-	ϵͳ����
			-ERR_NOINIT		-	ģ��δ��ʼ��
*	˵��:	��
 ******************************************************************************/
int powercheck_check ()
{
	int ret = -1;
	u8 buf;
	if(pwrd_count == 0){
		ret = -ERR_NOINIT;	
		goto err;
	}
	ret = read(fd, &buf, 0); 
	if(ret < 0){
		ret = -ERR_SYS;
		goto err;
	}
	ret = buf;
err:		
	return ret;
}

/******************************************************************************
*	����:	powercheck_close
*	����:	�رյ�����ģ��
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_NOINIT		-	ģ��δ��ʼ��
*	˵��:	��
 ******************************************************************************/
int powercheck_close ()
{
	int ret = -1;
	if(pwrd_count == 0)
		return -ERR_NOINIT;
	ret = close(fd);
	if(ret < 0)
		return -ERR_SYS;
	pwrd_count = 0;
	
	return ret;

}
#endif
