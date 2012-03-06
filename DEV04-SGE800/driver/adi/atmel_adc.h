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
	�ļ�		��  atmel_adc.h
	����		��  ���ļ�������at91sam9260�ڲ�adת��ģ��ĵײ���������ӿڵײ�
				�궨�壬�ṹ�塢�Ĵ�����
	�汾		��  0.1
	����		��  ·ȽȽ
	��������	��  2010.04
******************************************************************************/

#ifndef _ATMEL_ADC_H
#define _ATMEL_ADC_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>		//dev_t
#include <linux/cdev.h>		//char device register
#include <linux/kernel.h>		//container of macro
#include <linux/slab.h>		//memory manage
#include <linux/errno.h>		//error code
#include <linux/ioctl.h>		//ioctl system call
#include <linux/device.h>		//class_create

#include <linux/wait.h>		//waitqueue
#include <linux/clk.h>			//clock func
#include <linux/delay.h>		//udelay

#include <asm/io.h>			//ioremap
#include <asm/uaccess.h>		//copy_to_user
#include <asm/atomic.h>		//atomic

#include <mach/hardware.h>	//�Ĵ�������
#include <mach/gpio.h>		//gpio
#include <mach/at91_adc.h>	//TC
#include <mach/at91_pmc.h>	//PMC

#include <mach/at91sam9260.h>
#include <mach/at91_aic.h> //aic aboat SPI interupt

#define SOFTWARE_VERSION	"XJ atmel_adc adc 1.0"

#define MAJOR_DEVICE	109

struct atmel_adc_dev_t {
	char *name;
	void *buf;				// �豸��������
	atomic_t opened;		// �豸�򿪱�־
	u8 ch_nr;				// ͨ������
	u8 ch_en;				//����ͨ��ʹ��
	void __iomem *base;
	struct clk *clk;

};
#define CHANNEL0	4		//��ȡ�ڲ�ģ��ͨ��0
#define CHANNEL1	5		//��ȡ�ڲ�ģ��ͨ��1
#define CHANNEL2	6		//��ȡ�ڲ�ģ��ͨ��2
#define CHANNEL3	7		//��ȡ�ڲ�ģ��ͨ��3

#endif //_ATMEL_ADC_H
