/******************************************************************************
	��Ŀ����	��  SGE800���������ն�ƽ̨
	�ļ�		��  mcu.h
	����		��  ���ļ������˵�Ƭ����չ������������
	�汾		��  0.1
	����		��  ·ȽȽ
	��������	��  2010.12
******************************************************************************/
#ifndef _MCU_H
#define _MCU_H

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
#include <linux/interrupt.h> 	//irqaction
#include <linux/wait.h>		//waitqueue
#include <linux/clk.h>			//clock func
#include <linux/delay.h>		//udelay
#include <linux/sched.h>

#include <asm/io.h>			//ioremap
#include <asm/uaccess.h>		//copy_to_user
#include <asm/atomic.h>		//atomic

#include <mach/hardware.h>	//�Ĵ�������
#include <mach/gpio.h>		//gpio
#include <mach/at91_tc.h>	//TC
#include <mach/at91_pmc.h>	//PMC
#include <mach/at91_spi.h>	//SPI
#include <mach/at91sam9260.h>
#include <mach/at91_aic.h> //aic aboat SPI interupt

#define SOFTWARE_VERSION	"XJ mcu serial and adc 1.0"

#define MAJOR_DEVICE	104
#define SPI_DEFAULT_BAUD 10000

#define SPITEMP 0
#define SPIOTHER 1<<12

#define SPI_READ_STAT 0
#define SPI_WRITE_STAT 1
#define MCU_BUF_SIZE 1024			//��������С

#define MCU_FRAME_IDEAL	 0
#define MCU_FRAME_READ	 1
#define MCU_FRAME_WRITE 2

#define MCU_FRAME_DATA_MAX 240		//֡��������ֽ���
#define MCU_FRAME_HEAD 0x68
#define MCU_FRAME_TAIL 0x16
#define MCU_FRAME_NEXT (1<<5)
struct mcu_dev_t {
	//spi���
	u8 *name;
	u8 *buf;				// �豸��������
	u16 cur;				//���ݵ�ǰָ�룬��ʼ��Ϊ0,�ж����ӣ�read����
	atomic_t opened;		// �豸�򿪱�־
	u8 addr;				// �豸Ƭѡ��ַ
	u16 data_size;			// һ�δ�������ֽ���
	u32 baud;
	int irq;
	void __iomem *base;
	struct clk *clk;
	wait_queue_head_t wq;

	u8 rw_flag;				//��ǰspi���ݴ���״̬,����֡��д״̬
	struct semaphore sem;
	struct semaphore rwsem;
};

#endif
