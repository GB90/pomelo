/******************************************************************************
	��Ŀ����	��  SGE800���������ն�ƽ̨
	�ļ�		��  mcu.c
	����		��  ���ļ������˵�Ƭ����չ������������
	�汾		��  0.1
	����		��  ·ȽȽ
	��������	��  2010.12
******************************************************************************/
#include "mcu.h"

//ģ����Կ���
//#undef	DEBUG
#define DEBUG
#ifdef DEBUG
#define	DPRINTK( x... )		printk("mcu: " x)
#else
#define DPRINTK( x... )
#endif

/*************************************************
  ��̬ȫ�ֱ������궨��
*************************************************/

static struct cdev mcu_cdev;
static dev_t mcu_dev_m = MKDEV(MAJOR_DEVICE,0);
static struct class *drv_class;

static struct mcu_dev_t mcu_dev = {
		.name = "mcu",
		.opened = ATOMIC_INIT(1),
		.addr	= 0,
		.data_size	= MCU_BUF_SIZE,
		.baud = SPI_DEFAULT_BAUD,
		.irq = AT91SAM9260_ID_IRQ2,  //??????????
		.rw_flag = MCU_FRAME_IDEAL,
};

/*************************************************
  API
*************************************************/
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
		sr = __raw_readl(mcu_dev.base + AT91_SPI_SR);
	} while (!(sr & bit) && (--loop_cntr > 0));

	return (loop_cntr > 0);
}
/******************************************************************************
*	����:	mcu_spi_write
*	����:	spi д����
*	����:
*	����:
 ******************************************************************************/
static void inline mcu_spi_write(u32 data)
{
	__raw_writel(data, mcu_dev.base + AT91_SPI_TDR);
}
/******************************************************************************
*	����:	mcu_spi_read
*	����:	spi д����
*	����:
*	����:
 ******************************************************************************/
static int inline mcu_spi_reade(void)
{
	int retval = -1;
	__raw_writel(0xff, mcu_dev.base + AT91_SPI_TDR);
	retval = at91_poll_status(AT91_SPI_RDRF);
	if(retval < 0)
		return retval;
	return __raw_readl(mcu_dev.base + AT91_SPI_RDR);
}
/******************************************************************************
*	����:	atmel_spi_hardinit
*	����:	spi Ӳ����ʼ��
*	����:
*	����:
 ******************************************************************************/
static void inline atmel_spi_hardinit(void)
{
	u32 cdiv;
	//set pin
	at91_set_A_periph(AT91_PIN_PB0, 0);	//set SPI1 MISO
	at91_set_A_periph(AT91_PIN_PB1, 0);	//set SPI1 MOSI
	at91_set_A_periph(AT91_PIN_PB2, 0);	//set SPCK
	//at91_set_A_periph(AT91_PIN_PB3, 0);	//set NPCS0		//Ϊ�˽�ʡio�ڣ�Ƭѡû���õ�
	//enable spi clock
	mcu_dev.clk = clk_get(NULL, "spi1_clk");
	clk_enable(mcu_dev.clk);

	//ioremap
	mcu_dev.base = ioremap(AT91SAM9260_BASE_SPI1, 0x40);
	//spi hardware init
	__raw_readl(mcu_dev.base + AT91_SPI_SR); // clear irq flag
	__raw_writel(0xffffffff, mcu_dev.base + AT91_SPI_IDR);		//disable all spi interrupt
	__raw_writel(AT91_SPI_SWRST, mcu_dev.base + AT91_SPI_CR);	//reset spi
	//set mode
	__raw_writel(AT91_SPI_MSTR | AT91_SPI_PS_VARIABLE , mcu_dev.base + AT91_SPI_MR);
	//set baudrate
	cdiv = (clk_get_rate(mcu_dev.clk) >> 6) / mcu_dev.baud + 1;	// /(64 * drv_info.baud)
	if (cdiv > 255) {
		cdiv = 255;
	}
	//very important!! ����spi��ƬѡоƬ��ʱ������λ��
	__raw_writel((cdiv << 8) | AT91_SPI_BITS_8 | AT91_SPI_NCPHA, mcu_dev.base + AT91_SPI_CSR(mcu_dev.addr));
	__raw_writel(AT91_SPI_SPIEN, mcu_dev.base + AT91_SPI_CR);	//enable spi

}
/******************************************************************************
*	����:	mcu_interrupt
*	����:	��ʱ���жϺ���,ÿ���趨�ĺ������ж�һ�Σ�����¼��
*	����:
*	����:	IRQ_HANDLED		-	�ɹ�
 ******************************************************************************/
static irqreturn_t  atmel_mcu_spi_interrupt(int irq,void *dev_id)
{
	if( mcu_dev.rw_flag == MCU_FRAME_IDEAL){
		if(down_trylock(&(mcu_dev.rwsem)) == 0){
			mcu_dev.rw_flag = MCU_FRAME_READ;
			wake_up_interruptible(&(mcu_dev.wq));
		}
	}
	if(mcu_dev.rw_flag == MCU_FRAME_READ ){
		wake_up_interruptible(&(mcu_dev.wq));
	}
	return IRQ_HANDLED;
}


/******************************************************************************
*	����:	mcu_open
*	����:	��Ƭ����չ�����򿪺���,����mcu
*	����:
*	����:
 ******************************************************************************/
int mcu_open(struct inode*inode,struct file *filp)
{

	int retval = 0;
	void *buf;

//	minor = iminor(inode);
	if(!atomic_dec_and_test(&mcu_dev.opened)) {
		retval = -EBUSY;
		goto out;
	}
	//Ϊadת���������ռ�
	buf = kmalloc(mcu_dev.data_size, GFP_KERNEL);
	if (!buf) {
		retval = -ENOMEM;
		goto out;
	}
	mcu_dev.buf = buf;
	mcu_dev.cur = 0;
	filp->private_data = &mcu_dev;
	//��ʼ���ȴ�����
	init_waitqueue_head( &(mcu_dev.wq) );
	//pc14 irq2
	retval = request_irq(mcu_dev.irq,
							atmel_mcu_spi_interrupt,
							IRQF_DISABLED,
							mcu_dev.name,
							NULL);

	init_MUTEX(&(mcu_dev.sem));
	init_MUTEX(&(mcu_dev.rwsem));
	DPRINTK("mcu: Open!! \n");
	return retval;
out:
	atomic_inc(&mcu_dev.opened);
	return retval;


}
/******************************************************************************
*	����:	mcu_release
*	����:	adת�������ͷź���,�ͷ���Դ
*	����:
*	����:
 ******************************************************************************/
int mcu_release(struct inode *inode,struct file *filp)
{
	struct mcu_dev_t *dev = filp->private_data;
	kfree(dev->buf);
	atomic_inc(&dev->opened);
	free_irq(mcu_dev.irq, NULL);
	DPRINTK("mcu_adc: Release!!\n");
	return 0;
}


static int inline mcu_data_analays(void)
{
	int ret = -1;
	int i = 0;

#define MCU_FRAME_NULL 0xffff
	static u16 head = MCU_FRAME_NULL,tail = MCU_FRAME_NULL;		//֡ͷ��֡β
	u16 tail_t;
	u16 cur_pos = mcu_dev.cur;
	u8 sum_check;

	if(cur_pos > MCU_BUF_SIZE/2){
		head = MCU_FRAME_NULL;			//��ʼ��������
		tail = MCU_FRAME_NULL;
		memset(mcu_dev.buf, 0 , MCU_BUF_SIZE);
		mcu_dev.cur = 0;
		ret = -2;		//�������ռ䲻��
		return ret;
	}
	//�ҵ�����ͷ
	if(head == MCU_FRAME_NULL){
		for(i = 0; i < cur_pos; i ++){
			if(mcu_dev.buf[i] == MCU_FRAME_HEAD){
				head = i;
				break;
			}
		}
	}

	//�ҵ�����β
	if((tail == MCU_FRAME_NULL) && (head != MCU_FRAME_NULL)){
		tail_t = head + 2 + mcu_dev.buf[head + 2] + 2;
		if(mcu_dev.buf[tail_t] == MCU_FRAME_TAIL)
			tail = tail_t;
	}

	//֡У��
	if((tail != MCU_FRAME_NULL) && (head != MCU_FRAME_NULL)){
		sum_check = 0;
		for(i = head; i < tail - 1; i ++){
			sum_check += mcu_dev.buf[i];
		}
		if(sum_check == mcu_dev.buf[tail - 1]){
			ret = head;
		}else{
			memset(mcu_dev.buf, 0 , MCU_BUF_SIZE);
			mcu_dev.cur = 0;
			ret = -3; 		//У�����
		}
		head = MCU_FRAME_NULL;			//��ʼ��������
		tail = MCU_FRAME_NULL;
	}

	return ret;
}
/******************************************************************************
*	����:	mcu_read
*	����:	adת��������ѯ��ȡad���������������������
*	����:
*	����:
 ******************************************************************************/
ssize_t mcu_read(struct file *filp,char  *buf,size_t count,loff_t *offset)
{
	int retval = 0;
	u8 read_count = 0;
	u8 next_frame;

	struct mcu_dev_t *dev = filp->private_data;
	//DPRINTK("%s:enter FUNC = %d\n",__FUNCTION__,count);
	//������������
read:
	retval = mcu_data_analays();
	if(retval >= 0){
		read_count = 2 + mcu_dev.buf[retval + 2] ;
		next_frame = dev->buf[retval + 1] & MCU_FRAME_NEXT;
		if (copy_to_user(buf, &dev->buf[retval + 1], read_count)) {
			retval = -EFAULT;
			return retval;
		}
		//��ʼ��
		memset(mcu_dev.buf, 0 , MCU_BUF_SIZE);
		mcu_dev.cur = 0;
		if(next_frame == 0){
			mcu_dev.rw_flag = MCU_FRAME_IDEAL;
			up(&(mcu_dev.rwsem));
		}
		retval = read_count;
		return retval;
	}else if(retval == -3){//У�����
		mcu_dev.rw_flag = MCU_FRAME_IDEAL;
		up(&(mcu_dev.rwsem));
	}
	interruptible_sleep_on(&(dev->wq));	//˯�ߵȴ���ʱʱ�䵽
	if(signal_pending(current))
		return -ERESTARTSYS;

	mcu_dev.buf[mcu_dev.cur ++] = (u8) mcu_spi_reade();
	printk("%x ",mcu_dev.buf[mcu_dev.cur -1]);


	goto read;
}

/******************************************************************************
*	����:	mcu_write
*	����:	��Ƭ����չ����д֡����
*	����:
*	����:
 ******************************************************************************/
static void inline mcu_make_frame(u16 size)
{
	int i;
	u8 sum_check = 0;
	u16 write_size;

	mcu_dev.buf[0] = MCU_FRAME_HEAD;
	for(i = 0; i < size + 1; i++ ){
		sum_check += mcu_dev.buf[i];
	}

	mcu_dev.buf[i++] = sum_check;

	mcu_dev.buf[i++] = MCU_FRAME_TAIL;
	write_size = i ;

	for(i = 0; i < write_size; i ++){
		at91_poll_status(AT91_SPI_TXBUFE);
		mcu_spi_write(mcu_dev.buf[i]);
	}

}
/******************************************************************************
*	����:	mcu_write
*	����:	��Ƭ����չ����д֡����
*	����:
*	����:
 ******************************************************************************/
ssize_t mcu_write(struct file *filp,const char *buf,size_t count,loff_t *offset)
{
	int retval = -1;
	int i;
//	struct mcu_dev_t *dev = filp->private_data;

	if(count > MCU_FRAME_DATA_MAX){
		retval = -EINVAL;
		return retval;
	}

	retval = down_interruptible(&(mcu_dev.sem));
	if (retval) {
		retval = -EBUSY;
		return retval;
	}

	retval = down_interruptible(&(mcu_dev.rwsem));
	if (retval) {
		retval = -EBUSY;
		goto err2;
	}
	mcu_dev.rw_flag = MCU_FRAME_WRITE;

	//��ȡ�û����ݲ���֡
	if (copy_from_user(&mcu_dev.buf[1], buf, count)) {
		retval = -EFAULT;
		mcu_dev.rw_flag = MCU_FRAME_IDEAL;
		goto err1;
	}
	mcu_make_frame(count);
	DPRINTK("%s: count = %d,write:",__FUNCTION__,count + 3);
	for(i = 0; i < count + 3; i++){
		printk("%x ",mcu_dev.buf[i]);
	}
	printk("\n");
	mcu_dev.rw_flag = MCU_FRAME_READ;
	retval = count;
	up(&(mcu_dev.sem));
	return retval;
err1:
	up(&(mcu_dev.rwsem));
err2:
	up(&(mcu_dev.sem));
	return retval;
}
/******************************************************************************
*	����:	mcu_ioctl
*	����:	���ƺ���
*	����:
*	����:
 ******************************************************************************/
int mcu_ioctl(struct inode *inode, struct file *file,
	unsigned int cmd, unsigned long arg)
{
	int retval=0;

	DPRINTK("%s: enter!!\n",__FUNCTION__);
	switch(cmd){
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
	}
	return retval;
}

static struct file_operations mcu_fops = {
	.owner	= THIS_MODULE,
	.open	= mcu_open,
	.write	= mcu_write,
	.read	= mcu_read,
	.ioctl	= mcu_ioctl,
	.release = mcu_release,
};

/******************************************************************************
*	����:	mcu_init
*	����:	adת��������ʼ���������ַ��豸��ʼ����spi���߳�ʼ��
*	����:
*	����:
 ******************************************************************************/
static int __init mcu_init(void)
{
	int retval;

	DPRINTK(SOFTWARE_VERSION" build time:" __TIME__ "\n");
	//DPRINTK("%s:enter,major = (%d)...\n",__FUNCTION__, MAJOR(mcu_dev_t));

//ע���ַ��豸������������udev���õ��豸��
	retval = register_chrdev_region (mcu_dev_m, 1, mcu_dev.name);
	if (retval)
		return retval;
	cdev_init(&mcu_cdev, &mcu_fops);

	mcu_cdev.owner = THIS_MODULE;
	retval = cdev_add(&mcu_cdev, mcu_dev_m, 1);
	if (retval)
		return retval;

	drv_class = class_create(THIS_MODULE, mcu_dev.name);
	if (drv_class == NULL)
		return -ENOMEM;

	device_create(drv_class, NULL,mcu_dev_m, NULL, mcu_dev.name);

//Ӳ���豸��ʼ����spi���߳�ʼ��
	atmel_spi_hardinit();
	DPRINTK(SOFTWARE_VERSION " init\n\n");
	return 0;
}
/******************************************************************************
*	����:	mcu_exit
*	����:	adת������ע������
*	����:
*	����:
 ******************************************************************************/
static void __exit mcu_exit(void)
{

	//disable spi master
	__raw_writel(AT91_SPI_SPIDIS, mcu_dev.base + AT91_SPI_CR);
	//iounmap
	iounmap(mcu_dev.base);

	//disable clock
	clk_disable(mcu_dev.clk);
	clk_put(mcu_dev.clk);
	//set pin input
	at91_set_gpio_input(AT91_PIN_PB0, 1);
	at91_set_gpio_input(AT91_PIN_PB1, 1);
	at91_set_gpio_input(AT91_PIN_PB2, 1);
	at91_set_gpio_input(AT91_PIN_PB3, 1);
	//delete device file and unregister char device
	device_destroy(drv_class, MKDEV(MAJOR_DEVICE,0));
	class_destroy(drv_class);
	cdev_del(&mcu_cdev);
	unregister_chrdev_region(mcu_dev_m, 1);
	DPRINTK(SOFTWARE_VERSION " exit\n");

}

MODULE_AUTHOR("Roy <luranran@gmail.com>");
MODULE_DESCRIPTION("mcu adc driver");
MODULE_LICENSE("GPL");
module_init(mcu_init);
module_exit(mcu_exit);

