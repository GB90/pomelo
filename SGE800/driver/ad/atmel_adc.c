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
	�ļ�		��  adclib.c
	����		��  ���ļ�������adת��ģ��ĵײ���������ӿڵ�ʵ��
	�汾		��  0.2
	����		��  ·ȽȽ
	��������	��  2010.04
******************************************************************************/

#include "adclib.h"
#include "atmel_adc.h"

//ģ����Կ���
#undef	DEBUG
//#define DEBUG
#ifdef DEBUG
#define	DPRINTK( x... )		printk("tlv1504: " x)
#else
#define DPRINTK( x... )
#endif

/*************************************************
  ��̬ȫ�ֱ������궨��
*************************************************/

static struct cdev tlv1504_cdev;
static dev_t tlv1504_dev_t = MKDEV(MAJOR_DEVICE,0);
static struct class *drv_class;

static struct tlv1504_dev_t tlv1504_dev = {
		.name = "tlv1504",
		.opened = ATOMIC_INIT(1),
		.addr	= 0x0,
		.data_max	= 4,

		.baud = SPI_DEFAULT_BAUD,
};

/*************************************************
  API
*************************************************/

/******************************************************************************
*	����:	tlv1504_config
*	����:	tlv1504оƬ�Ĺ���ģʽ���ú���,дcfr
*	����:
*	����:
 ******************************************************************************/
void tlv1504_config(void)
{
	//1 1 0 00 00 00 1 00=3076
	__raw_writel( CFR_WRITE	|			//дcfr��������12λ����
					INTER_REF	|		//�ο���ѹ�ڲ��ο�
					INT_REF_V2	|		//�ڲ�2V�ο���ѹ
					SAM_PERIOD12	|	//ѡ��̲���ʱ��12SCLK
					SELECT_CLK_INT	|	//ADת��ʱ��Դѡ���ڲ�����
					SELECT_MODE_S	|	//ADת��ģʽѡ���ȡ��·���
					SWEEP_SEQ	|		//��ѭ����ȡ��·ת��
					INT_EN	|			//EOC/INT��������EOC
					FIFO_TRIG_LEVEL0,	//FIFO�жϴ�������Ϊ������
					tlv1504_dev.base + AT91_SPI_TDR);
}

/******************************************************************************
*	����:	tlv1504_open
*	����:	adת�������򿪺���,����tlv1504
*	����:
*	����:
 ******************************************************************************/
int tlv1504_open(struct inode*inode,struct file *filp)
{	

	int retval = 0;
	void *buf;

//	minor = iminor(inode);
	if(!atomic_dec_and_test(&tlv1504_dev.opened)) {
		retval = -EBUSY;
		goto out;
	}
	//Ϊadת���������ռ�
	buf = kmalloc(tlv1504_dev.data_max, GFP_KERNEL);
	if (!buf) {
		retval = -ENOMEM;
		goto out;
	}
	tlv1504_dev.buf = buf;

	filp->private_data = &tlv1504_dev;

	tlv1504_config();

	DPRINTK("tlv1504 adc: Open!! \n");
	return retval;
out:
	atomic_inc(&tlv1504_dev.opened);
	return retval;
	
	
}
/******************************************************************************
*	����:	tlv1504_release
*	����:	adת�������ͷź���,�ͷ���Դ
*	����:
*	����:
 ******************************************************************************/
int tlv1504_release(struct inode *inode,struct file *filp)
{ 
	struct tlv1504_dev_t *dev = filp->private_data;
	kfree(dev->buf);
	atomic_inc(&dev->opened);

	DPRINTK("tlv1504_adc: Release!!\n");
	return 0;
}
/******************************************************************************
*	����:	tlv1504_write
*	����:	adת������д�������ݲ�ʵ��
*	����:
*	����:
 ******************************************************************************/
static ssize_t tlv1504_write(struct file *file,const char *bur,size_t count,loff_t *offset)
{	
	return -EBUSY;

}
/******************************************************************************
*	����:	tlv1504_set_readch
*	����:	adת���������ö�ȡ���ܺ������˺������ú�����Ϸ��ؽ����spi���ռĴ���
*	����:	channel Ҫ��ȡ��ͨ����0~3������ 4��5,6-���Ե�ƽ,7-��ȡCFR��8-��ȡFIFO
*	����:
 ******************************************************************************/
void inline tlv1504_set_readch(int channel)
{		
	int ch_s=0;	
	switch(channel){
		case 0:
			ch_s = SELECT_CH0;
			break;
		case 1:
			ch_s = SELECT_CH1;
			break;
		case 2:
			ch_s = SELECT_CH2;
			break;
		case 3:
			ch_s = SELECT_CH3;
			break;
		case 4:				/*for test voltage=refm */
			ch_s = SELECT_TEST0;
			break;
		case 5:				/*for test voltage=(refm+refp)/2 */
			ch_s = SELECT_TEST1;
			break;
		case 6:				/*for test voltage=refp */
			ch_s = SELECT_TEST2;
			break;
		case 7:				/*read cfr */
			ch_s = CFR_READ;
			break;
		case 8:			/*read FIFO */
			ch_s = FIFO_READ;
		break;
		default:
			break;
	}			
	__raw_writel(ch_s, tlv1504_dev.base + AT91_SPI_TDR);
	
}

/******************************************************************************
*	����:	at91_poll_status
*	����:	��ѯspi״̬�Ĵ������ȴ���ȡspi���ݡ�ÿ10us��ѯһ�Σ�����1000�κ����������򱨴�
*	����:
*	����:	>0 - �ɹ���������
*			0  - ��ʱ
 ******************************************************************************/
static int at91_poll_status(unsigned long bit)
{
	int loop_cntr = 100;	//���10ms
	int sr;
	do {
		udelay(100);	//ÿ����ʱ100us������<10us����������
		sr = __raw_readl(tlv1504_dev.base + AT91_SPI_SR);
		//DPRINTK("sr=%x--",sr);
		/*
		if(sr & 0x01c0) {
			return -1;
		}
		*/
	} while (!(sr & bit) && (--loop_cntr > 0));

	return (loop_cntr > 0);
}

/******************************************************************************
*	����:	tlv1504_read
*	����:	adת��������ѯ��ȡad���������������������
*	����:
*	����:
 ******************************************************************************/
static ssize_t tlv1504_read(struct file *filp,char  *buf,size_t count,loff_t *offset)
{
	int retval = 0;	
	u32 *pbuf;
	int fun = count;	
	u8 read_count = 0;
	
	struct tlv1504_dev_t *dev = filp->private_data;
	//DPRINTK("%s:enter FUNC = %d\n",__FUNCTION__,count);		
	pbuf = dev->buf;
	if ((count < 0) | (count > 8)) {	//������Ч���ж�
		retval = -EINVAL;
		goto out;
	}

//��ѯ��ȡת�����ݣ�ѭ��3��
read:
	tlv1504_set_readch(fun);
	retval = at91_poll_status(AT91_SPI_RDRF);
	DPRINTK("\n%s:retval=%d\n",__FUNCTION__,retval);
	if (retval > 0) {
		*pbuf  = __raw_readl(tlv1504_dev.base + AT91_SPI_RDR);
		DPRINTK("%s:poll:pbuf=%x\n",__FUNCTION__,*pbuf);
		if(read_count++ < 2)		//��ȡ3��
			goto read;
		read_count = 0;
	}else if (retval == 0) {
		retval = -ETIMEDOUT;
	}else {
		retval = -EFAULT;
	}

//ת����ȡ��������
	if(fun == 7)					/*��CFR,����λD11-D0��Ч */
		*pbuf &= 0xfff;
	else
		*pbuf = ((*pbuf)>>6)&0x3ff;	/*��ȡͨ������, ����λ D15-D6 ��Ч����СֵΪ0�����ֵΪ0x3ff*/
	
	DPRINTK("%s:pbuf=%x\n",__FUNCTION__,*pbuf);

	retval = sizeof(pbuf);
	if (copy_to_user(buf, pbuf, sizeof(pbuf))) {
			retval = -EFAULT;			
	}

out:
	return retval;
}
/******************************************************************************
*	����:	tlv1504_ioctl
*	����:	adת���������ƺ���������ʵ��
*	����:
*	����:
 ******************************************************************************/
int tlv1504_ioctl(struct inode *inode, struct file *file,
	unsigned int cmd, unsigned long arg)
{
	int retval=0;
	
	DPRINTK("%s: enter!!\n",__FUNCTION__);
	switch(cmd){		
//	case DS_ADC_CHS:
	case DS_ADC_IRQ_QUERY:
		break;		
//	case DS_ADC_START:		
	case 1:	
		break;		
//	case DS_ADC_STOP:
	case 2:			
		break;
	default:
		break;
	}
	return retval;
}

static struct file_operations tlv1504_fops = { 
	.owner	= THIS_MODULE, 
	.open	= tlv1504_open, 
	.write	= tlv1504_write, 
	.read	= tlv1504_read, 
	.ioctl	= tlv1504_ioctl,
	.release = tlv1504_release, 
}; 

/******************************************************************************
*	����:	tlv1504_init
*	����:	adת��������ʼ���������ַ��豸��ʼ����spi���߳�ʼ��
*	����:
*	����:
 ******************************************************************************/
static int __init tlv1504_init(void)
{	
	int retval;
	u32 cdiv;	
	DPRINTK(SOFTWARE_VERSION" build time:" __TIME__ "\n");	
	//DPRINTK("%s:enter,major = (%d)...\n",__FUNCTION__, MAJOR(tlv1504_dev_t));

//ע���ַ��豸������������udev���õ��豸��
	retval = register_chrdev_region (tlv1504_dev_t, 1, tlv1504_dev.name);
	if (retval)
		return retval;	
	cdev_init(&tlv1504_cdev, &tlv1504_fops);

	tlv1504_cdev.owner = THIS_MODULE;
	retval = cdev_add(&tlv1504_cdev, tlv1504_dev_t, 1);
	if (retval)
		return retval;

	drv_class = class_create(THIS_MODULE, tlv1504_dev.name);
	if (drv_class == NULL)
		return -ENOMEM;
	
	device_create(drv_class, NULL,tlv1504_dev_t, NULL, tlv1504_dev.name);

//Ӳ���豸��ʼ����spi���߳�ʼ��
	//set pin
	at91_set_A_periph(AT91_PIN_PB0, 0);	//set SPI1 MISO
	at91_set_A_periph(AT91_PIN_PB1, 0);	//set SPI1 MOSI
	at91_set_A_periph(AT91_PIN_PB2, 0);	//set SPCK
	at91_set_A_periph(AT91_PIN_PB3, 0);	//set NPCS0
	//enable spi clock
	tlv1504_dev.clk = clk_get(NULL, "spi1_clk");
	clk_enable(tlv1504_dev.clk);
	
	//ioremap
	tlv1504_dev.base = ioremap(AT91SAM9260_BASE_SPI1, 0x40);
	//spi hardware init
	__raw_readl(tlv1504_dev.base + AT91_SPI_SR); // clear irq flag
	__raw_writel(0xffffffff, tlv1504_dev.base + AT91_SPI_IDR);		//disable all spi interrupt
	__raw_writel(AT91_SPI_SWRST, tlv1504_dev.base + AT91_SPI_CR);	//reset spi
	//set mode
	__raw_writel(AT91_SPI_MSTR | AT91_SPI_PS_VARIABLE , tlv1504_dev.base + AT91_SPI_MR);
	//set baudrate
	cdiv = (clk_get_rate(tlv1504_dev.clk) >> 6) / tlv1504_dev.baud + 1;	// /(64 * drv_info.baud)
	if (cdiv > 255) {
		cdiv = 255;
	}
	/*very important!! ����spi��ƬѡоƬ��ʱ������λ��
	  */
	__raw_writel((cdiv << 8) | AT91_SPI_BITS_16 | AT91_SPI_NCPHA, tlv1504_dev.base + AT91_SPI_CSR(tlv1504_dev.addr));

	__raw_writel(AT91_SPI_SPIEN, tlv1504_dev.base + AT91_SPI_CR);	//enable spi
	
	DPRINTK(SOFTWARE_VERSION " init\n\n");
	return 0;
}
/******************************************************************************
*	����:	tlv1504_exit
*	����:	adת������ע������
*	����:
*	����:
 ******************************************************************************/
static void __exit tlv1504_exit(void)
{	

	//disable spi master
	__raw_writel(AT91_SPI_SPIDIS, tlv1504_dev.base + AT91_SPI_CR);
	//iounmap
	iounmap(tlv1504_dev.base);
	
	//disable clock
	clk_disable(tlv1504_dev.clk);
	clk_put(tlv1504_dev.clk);
	//set pin input
	at91_set_gpio_input(AT91_PIN_PB0, 1);
	at91_set_gpio_input(AT91_PIN_PB1, 1);
	at91_set_gpio_input(AT91_PIN_PB2, 1);
	at91_set_gpio_input(AT91_PIN_PB3, 1);
//delete device file and unregister char device
	device_destroy(drv_class, MKDEV(MAJOR_DEVICE,0));
	class_destroy(drv_class);		
	cdev_del(&tlv1504_cdev);
	unregister_chrdev_region(tlv1504_dev_t, 1);
	DPRINTK(SOFTWARE_VERSION " exit\n");

}

MODULE_AUTHOR("Roy <luranran@gmail.com>");
MODULE_DESCRIPTION("tlv1504 adc driver");
MODULE_LICENSE("GPL");
module_init(tlv1504_init);
module_exit(tlv1504_exit);

