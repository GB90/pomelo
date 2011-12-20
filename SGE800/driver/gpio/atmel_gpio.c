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
	�ļ�		��  atmel_gpio.c
	����		��  ���ļ�������adת��ģ��ĵײ���������ӿ�
	�汾		��  0.2
	����		��  ·ȽȽ
	��������	��  2010.04
******************************************************************************/
#include "atmel_gpio.h"
#include "gpiolib.h"

//ģ����Կ���
#undef	DEBUG
//#define DEBUG
#ifdef DEBUG
#define	DPRINTK( x... )		printk("atmel_gpio: " x)
#else
#define DPRINTK( x... )
#endif

/*************************************************
  ��̬ȫ�ֱ������궨��
*************************************************/

struct cdev atmel_gpio_cdev;		//�ַ��豸
struct class *atmel_gpio_class;		//��

/*************************************************
  API
*************************************************/

/******************************************************************************
*	����:	gpio_open
*	����:	gpio�豸�򿪺���
*	����:
*	����:	0		-	�ɹ�
 ******************************************************************************/
int gpio_open(struct inode*inode,struct file *file)
{	
	//unsigned int temp = 0;
	DPRINTK( "atmel_gpio: Open!!  \n");
	return 0;
}

/******************************************************************************
*	����:	gpio_release
*	����:	gpio�豸�ͷź���
*	����:
*	����:	0		-	�ɹ�
 ******************************************************************************/
int gpio_release(struct inode *inode,struct file *file)
{ 
	DPRINTK( "atmel_gpio: Release!!\n");
	return 0;
}

/******************************************************************************
*	����:	io�ڿ��ƺ���
*	����:	atmel ��ȡio�ڵ��ڴ�ӳ�����
*	����:
*	����:
 ******************************************************************************/
static inline void __iomem *pin_to_controller_ry(unsigned pin)
{
	void __iomem *sys_base = (void __iomem *) AT91_VA_BASE_SYS;	
	pin -= PIN_BASE;
	pin /= 32;
	
	if (likely(pin < MAX_GPIO_BANKS))
		return sys_base + atmel_gpio[pin];
	return NULL;
}
static inline unsigned pin_to_mask_ry(unsigned pin)
{
	pin -= PIN_BASE;
	return 1 << (pin % 32);
}

/******************************************************************************
*	����:	gpio_ctl_ioctl
*	����:	gpio�豸���ƺ���������io�ڵ����Լ�״̬
*	����:
*	����:	0				-	�ɹ�
 ******************************************************************************/
int gpio_ctl_ioctl(struct inode *inode, struct file *file,
	unsigned int cmd, unsigned long arg)
{
	static unsigned int temp = 0 ;
	static int retval = 0;
	u32	pdsr = 0, odsr = 0;		//���io��ȡֵ
	u32 pin=0;

	void __iomem *pio ;
	unsigned mask ;
	//io��ַӳ��
	if((cmd == IOGETI) || (cmd == IOGETO)){
		pin = *((u32*)arg);
	}else{
		pin = arg;
	}

	pio = pin_to_controller_ry(pin);
	mask = pin_to_mask_ry(pin);
	if (!pio)
		return -EINVAL;


	DPRINTK( "%s:enter,%d,%d,%d\n",__FUNCTION__,IOGETI,cmd,pin);
	switch(cmd){
		
		case SETI:	//��������
			__raw_writel(mask, pio + PIO_IDR);
			__raw_writel(mask, pio + PIO_ODR);
			__raw_writel(mask, pio + PIO_PER);
			//DPRINTK( "atmel_gpio: Ioctl set !! arg=%d\n",arg);
			break;

		case SETO:	//�������
			__raw_writel(mask, pio + PIO_IDR);
			__raw_writel(mask, pio + PIO_OER);
			__raw_writel(mask, pio + PIO_PER);
			//DPRINTK( " Ioctl output mode !! arg=%ld\n",arg);
			break;

		case OCLR:	//�������Ϊ0
			__raw_writel(mask, pio + PIO_CODR);
			//DPRINTK( " Ioctl output clr !! arg=%ld\n",arg);
			break;

		case OSET:	//�������Ϊ1
			__raw_writel(mask, pio + PIO_SODR);
			//DPRINTK( " Ioctl output set !! arg=%ld\n",arg);
			break;

		case ODE:	//����OD��ʹ��
			__raw_writel(mask, pio + PIO_MDER);
			break;

		case ODD:	//����OD�Ž���
			__raw_writel(mask, pio +PIO_MDDR);
			break;

		case PUE:	//��������ʹ��
			__raw_writel(mask, pio + PIO_PUER);
			break;

		case PUD:	//������������
			__raw_writel(mask, pio + PIO_PUDR);
			break;

		case IOGETI:	//��ȡ����ֵ
			DPRINTK( "%s:IOGETI IO = %d\n",__FUNCTION__, pin);
			pdsr = __raw_readl(pio + PIO_PDSR);
			temp = ((pdsr & mask) != 0);
			retval = put_user(temp, ((int *)arg));
			break;

		case IOGETO:	//��ȡ���ֵ
			odsr = __raw_readl(pio + PIO_ODSR);
			DPRINTK( "%s:IOGETO IO = %d\n",__FUNCTION__, pin);
			DPRINTK( "%s:read osr = %d\n",__FUNCTION__,odsr);
			temp = ((odsr & mask) != 0);
			DPRINTK( "%s:read output IO val = %d\n",__FUNCTION__,temp);
			retval = put_user(temp, ((int *)arg));
			break;
		case IFE:	//�����˲�ʹ��
			__raw_writel(mask, pio + PIO_IFER);
			break;
		case IFD:	//�����˲�����
			__raw_writel(mask, pio + PIO_IFDR);
			break;
		default:
			__raw_writel(mask, pio + PIO_IDR);
			__raw_writel(mask, pio + PIO_ODR);
	}

	retval = 0;
	return retval;
}

struct file_operations atmel_gpio_fops= {
	.owner 		= THIS_MODULE,
	.ioctl 		= gpio_ctl_ioctl,
	.open  		= gpio_open,
	.release 	= gpio_release,
};

/******************************************************************************
*	����:	atmel_gpio_exit
*	����:	gpio�豸�رպ���
*	����:
*	����:	0	-	�ɹ�
 ******************************************************************************/
void atmel_gpio_exit(void)
{
	dev_t dev = 0;
	dev = MKDEV(IOPORT_MAJOR, 0);
	cdev_del(&atmel_gpio_cdev);
	unregister_chrdev_region(dev, 1);
	
    device_destroy(atmel_gpio_class, dev);
    class_destroy(atmel_gpio_class);
	
    DPRINTK(KERN_WARNING "atmel_gpio: exit!!\n");
}

/******************************************************************************
*	����:	atmel_gpio_init
*	����:	gpio�豸��ʼ������
*	����:
*	����:	0	-	�ɹ�
 ******************************************************************************/
int __init atmel_gpio_init(void)
{

	int ret;

	dev_t dev = 0;
	dev = MKDEV(IOPORT_MAJOR, 0);
	ret = register_chrdev_region(dev, 1, "atmel_gpio");
	if(ret < 0 )	{
		DPRINTK( "init: can't get major %d!!\n", IOPORT_MAJOR);
		return ret;
	}
	//�ַ��豸��ʼ������ӡ�
	cdev_init(&atmel_gpio_cdev, &atmel_gpio_fops);
	atmel_gpio_cdev.owner = THIS_MODULE;
	atmel_gpio_cdev.ops = &atmel_gpio_fops;
	ret = cdev_add(&atmel_gpio_cdev,dev,1);
	if(ret)
		DPRINTK( "init: Error %d adding atmel_gpio!!\n", ret);

	//�����࣬��sysfs��ע���豸���Ա�udev�����豸�ڵ�
    atmel_gpio_class = class_create(THIS_MODULE, "atmel_gpio");
    if(IS_ERR(atmel_gpio_class)) {
    	DPRINTK("Err: failed in creating class.\n");
        return -1;
    }
    device_create(atmel_gpio_class, NULL, dev, NULL, "atmel_gpio");

	DPRINTK("init: init done!!\n");
	return 0;
}
module_init(atmel_gpio_init);
module_exit(atmel_gpio_exit);

MODULE_AUTHOR("roy <luranran@gmail.com>");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("GPIO driver For AT91SAM9260");
