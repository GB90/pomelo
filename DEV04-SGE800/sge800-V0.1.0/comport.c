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
	�ļ�		��  comport.c
	����		��  ���ļ�ʵ���˴���ģ���е�API����
	�汾		��  0.1
	����		��  ·ȽȽ
	��������	��  2010.01
******************************************************************************/
//������ͷ�ļ�
#include "private/config.h"

#ifdef CFG_COMPORT_MODULE
//��������ͷ�ļ�

//C��ͷ�ļ�

#include <stdio.h>						//printf
#include <fcntl.h>						//open
#include <unistd.h>						//read,write
#include <termios.h>					//�������ü��궨��
#include <string.h>						//bzero,memcyp
#include <sys/ioctl.h>					//ioctl
#include <errno.h>
#include <sys/mman.h>

//�ṩ���û���ͷ�ļ�
#include "include/comport.h"
#include "include/error.h"
#include "include/pinio.h"

/*************************************************
  ��̬ȫ�ֱ������궨��
*************************************************/
#ifdef  CFG_GSERIAL_MODULE
#define MAX_COMNUM 8		//�������usbת���ڣ�����󴮿���Ϊ8
#else
#define MAX_COMNUM 7		//���û�ж���usbת���ڣ�����󴮿���Ϊ7
#endif
//#define RS485CMD 0x545f	//����485ģʽ�Ŀ�������
static struct{
	comport_config_t com;	//����ѡ��
	int fd;
	struct termios opt;
	u8 count;				//�򿪼���
	u8 mode;
//	pthread_mutex_t mutex;	
}comport[MAX_COMNUM];

static void * sys_base;
static void * usart_base;
static int mem_fd;
/*************************************************
  API����ʵ��
*************************************************/

/*************************************************
  �����ײ����õ��ĺ���
*************************************************/
#define AT91_SYS			(0xffffc000 )
#define AT91_GPIO_BASE		(0xFFFFF400 - AT91_SYS)
#define AT91_USART_BASE		(0xfffb0000)
#define AT91_USART0			(0xfffb0000 - AT91_USART_BASE)
#define AT91_USART1			(0xfffb4000 - AT91_USART_BASE)
#define AT91_USART2			(0xfffb8000 - AT91_USART_BASE)
#define AT91_USART3			(0xfffd0000 - AT91_USART_BASE)

#define AT91_USART_MR		(0x0004)
//GPIO
#define PIO_PDR		0x04	/* Disable Register */
#define PIO_IDR		0x44	/* Interrupt Disable Register */
#define PIO_PUDR	0x60	/* Pull-up Disable Register */
#define PIO_PUER	0x64	/* Pull-up Enable Register */
#define PIO_ASR		0x70	/* Peripheral A Select Register */
#define PIO_BSR		0x74	/* Peripheral B Select Register */

//static inline void at91_sys_write(unsigned int reg_offset, unsigned long value)
//{
//	//void __iomem *addr = (void __iomem *)AT91_VA_BASE_SYS;
//	*(volatile unsigned long*)(usart_base + reg_offset) = value;
//	//__raw_writel(value, addr + reg_offset);
//}

#define __raw_writel(a,b) (*(volatile unsigned int*)(b) = (a) )
static inline void  *pin_to_controller(unsigned pin)
{
	pin /= 32;
	return sys_base + pin*512 + AT91_GPIO_BASE;

}
static inline unsigned pin_to_mask(unsigned pin)
{
	return 1 << (pin % 32);
}

static int at91_set_A_periph(unsigned pin, int use_pullup)
{
	void *pio = pin_to_controller(pin);
	unsigned	mask = pin_to_mask(pin);

	if (!pio)
		return -1;

	__raw_writel(mask, pio + PIO_IDR);
	__raw_writel(mask, pio + (use_pullup ? PIO_PUER : PIO_PUDR));
	__raw_writel(mask, pio + PIO_ASR);
	__raw_writel(mask, pio + PIO_PDR);
	return 0;
}
static int at91_set_B_periph(unsigned pin, int use_pullup)
{
	void *pio = pin_to_controller(pin);
	unsigned	mask = pin_to_mask(pin);

	if (!pio)
		return -1;

	__raw_writel(mask, pio + PIO_IDR);
	__raw_writel(mask, pio + (use_pullup ? PIO_PUER : PIO_PUDR));
	__raw_writel(mask, pio + PIO_BSR);
	__raw_writel(mask, pio + PIO_PDR);
	return 0;
}
static void  comport_set_serial(u8 comport, u8 is_rtscts)
{

	//COMPORT0��������̨���Դ��ڣ��Ѿ��ڰ漶���������á�
	switch(comport){
	case COMPORT1:
		at91_set_A_periph(PIN_PB4, 1);		/* TXD0 */
		at91_set_A_periph(PIN_PB5, 0);		/* RXD0 */
		if(is_rtscts){
			at91_set_A_periph(PIN_PB26, 0);	/* RTS0 */
			at91_set_A_periph(PIN_PB27, 0);	/* CTS0 */
		}
		break;
	case COMPORT2:
		at91_set_A_periph(PIN_PB6, 1);		/* TXD1 */
		at91_set_A_periph(PIN_PB7, 0);		/* RXD1 */
		if(is_rtscts){
			at91_set_A_periph(PIN_PB28, 0);	/* RTS1 */
			at91_set_A_periph(PIN_PB29, 0);	/* CTS1 */
		}
	case COMPORT3:
		at91_set_A_periph(PIN_PB8, 1);		/* TXD2 */
		at91_set_A_periph(PIN_PB9, 0);		/* RXD2 */
		if(is_rtscts){
			at91_set_A_periph(PIN_PA4, 0);	/* RTS2 */
			at91_set_A_periph(PIN_PA5, 0);	/* CTS2 */
		}
	case COMPORT4:
		at91_set_A_periph(PIN_PB10, 1);		/* TXD3 */
		at91_set_A_periph(PIN_PB11, 0);		/* RXD3 */
		if(is_rtscts){
			at91_set_B_periph(PIN_PC8, 0);		/* RTS3 */
			at91_set_B_periph(PIN_PC10, 0);	/* CTS3 */
		}
	case COMPORT5:
		at91_set_B_periph(PIN_PA31, 1);		/* TXD4 */
		at91_set_B_periph(PIN_PA30, 0);		/* RXD4 */

	case COMPORT6:
		at91_set_A_periph(PIN_PB12, 1);		/* TXD5 */
		at91_set_A_periph(PIN_PB13, 0);		/* RXD5 */
		break;
	default:
		break;
	}
}
static inline void at91_usmr_write(unsigned int reg_offset)
{
#define		ATMEL_US_USMODE		(0xf <<  0)		/* Mode of the USART */
	//void __iomem *addr = (void __iomem *)AT91_VA_BASE_SYS;
	*(volatile unsigned long*)(usart_base + reg_offset + AT91_USART_MR) &= ~ATMEL_US_USMODE;
	*(volatile unsigned long*)(usart_base + reg_offset + AT91_USART_MR) |= 1;
	//__raw_writel(value, addr + reg_offset);
}
static void  comport_set_485(u8 comport)
{

	switch(comport){
	case COMPORT1:
		at91_set_A_periph(PIN_PB26, 0);	/* RTS0 */
		at91_usmr_write(AT91_USART0);
		break;
	case COMPORT2:
		at91_set_A_periph(PIN_PB28, 0);	/* RTS1 */
		at91_usmr_write(AT91_USART1);
		break;
	case COMPORT3:
		at91_set_A_periph(PIN_PA4, 0);	/* RTS2 */
		at91_usmr_write(AT91_USART2);
		break;
	case COMPORT4:
		at91_set_B_periph(PIN_PC8, 0);	/* RTS3 */
		at91_usmr_write(AT91_USART3);
		break;
	default:
		break;
	}

}
/******************************************************************************
*	����:	comport_init
*	����:	�򿪲���ʼ������
*	����:	port			-	���ڶ˿ں�
			mode			-	���ڲ���ģʽ
*	����:	0				-	�ɹ�
			-ERR_SYS			-	����
			-ERR_INVAL		-	��������
			-ERR_BUSY		-	�Ѿ���
*	˵��:	��
 ******************************************************************************/
int comport_init (u8 port, u8 mode)
{
	int ret = -1;	

	if((mem_fd = open("/dev/mem",O_RDWR|O_SYNC))==-1){
		ret = -1;
		return ret;
	}
	sys_base = mmap(0,0x3fff, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd,AT91_SYS);
	usart_base = mmap(0,0x24fff, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd,AT91_USART_BASE);

	//ttygs0Ϊusb���⴮���豸
	char *dev[]={"/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3",
				"/dev/ttyS4","/dev/ttyS5","/dev/ttyS6","/dev/ttyGS0"};

	if((port == (MAX_COMNUM - 1)) && (mode == COMPORT_MODE_485)){
		ret = -ERR_INVAL;
		goto err;
	}
	if(port < 0 || port >= MAX_COMNUM){
		ret = -ERR_INVAL;
		goto err;
	}
	if(comport[port].count == 1){
		ret = -ERR_BUSY;		//�Ѿ���
		goto err;
	}
	if((mode == COMPORT_MODE_485) || (mode == COMPORT_MODE_NORMAL)){
		;
	}else{
		ret = -ERR_INVAL;
		goto err;
	}
	// com5 com6 ��֧��485ģʽ
	if((mode == COMPORT_MODE_485) && ((port > COMPORT4)||(port == 0))){
		ret = -ERR_NOFUN;
		goto err;
	}


	//�򿪴���
	comport[port].fd = open(dev[port], O_RDWR|O_NOCTTY|O_NDELAY);
	if (comport[port].fd < 0){
		//perror("Can't Open Serial Port");
		ret = -ERR_SYS;
		goto err;
	}


	fcntl(comport[port].fd, F_SETFL,0);	//�ָ���������ģʽ

	//�����Ƿ�Ϊ�ն��豸
	if(isatty(STDIN_FILENO)==0){
		ret = -ERR_SYS;
		goto err;
	}

	//����Ĭ�ϴ�������	
	comport[port].com.baud = 9600;	//���ò�����-9600
	comport[port].com.verify = COMPORT_VERIFY_NO;
	comport[port].com.ndata = 8;	//��������λ-8λ
	comport[port].com.nstop = 1;
	comport[port].com.timeout = 0;	//Ĭ������
	comport[port].com.rtscts = COMPORT_RTSCTS_DISABLE;
	
	memset( &comport[port].opt, 0,sizeof( comport[port].opt ) );		
	comport[port].opt.c_cflag  |=  CLOCAL | CREAD; //�����ַ���С	
	comport[port].opt.c_cflag &= ~CSIZE; 	
	
	comport[port].opt.c_cflag |= CS8;			//��������λ-8λ
	
	comport[port].opt.c_cflag &= ~PARENB;		//������żУ��-��	

	cfsetispeed(&comport[port].opt, B9600);	//���ò�����-9600
	cfsetospeed(&comport[port].opt, B9600);	
	
	comport[port].opt.c_cflag &=  ~CSTOPB;		//����ֹͣλ-1λ
	
	comport[port].opt.c_cc[VTIME]  = 0;		//Ĭ����������
	comport[port].opt.c_cc[VMIN] = 1;
	
	comport[port].opt.c_cflag &= ~CRTSCTS;		//����Ӳ������-��Ӳ���������
	comport[port].opt.c_iflag &= ~(IXON|IXOFF);

	if((tcsetattr(comport[port].fd, TCSANOW, &comport[port].opt)) != 0)//��������
	{
		ret = -ERR_SYS;
		goto err;
	}	
	tcflush(comport[port].fd,TCIFLUSH);//����δ�����ַ�

	//����io�����Ź���
	comport_set_serial(port, COMPORT_RTSCTS_DISABLE);

	comport[port].mode = mode;
	comport[port].count = 1;
	if(comport[port].mode == COMPORT_MODE_485){
		comport_set_485(port);
//		ret = ioctl(comport[port].fd, RS485CMD, 0);
//		if(ret < 0){
//			perror("com set error");
//			ret = -ERR_SYS;
//			goto err;
//		}
	}
	ret = 0;
err:
	return ret;
}

/******************************************************************************
*	����:	comport_setconfig
*	����:	���ô���
*	����:	port			-	���ڶ˿ں�
			cfg				-	��������ݴ��룩
*	����:	0				-	�ɹ�
			-ESYS			-	ϵͳ����
			-ERR_INVAL		-	������Ч
			-ERR_NOINIT		-	û�г�ʼ��
*	˵��:	������֧�ֵĲ�����Ϊ 50��110��150��300��600��1200��2400��4800��9600
 			115200��460800��4000000
 ******************************************************************************/
int comport_setconfig (u8 port, comport_config_t *cfg)
{
	int ret = -1;

	if(port < 0 || port >= MAX_COMNUM){
		ret = -ERR_INVAL;
		goto err;
	}
	if(comport[port].count == 0){
		ret = -ERR_NOINIT;		//û�г�ʼ��
		goto err;
	}
	memset( &comport[port].opt, 0,sizeof( comport[port].opt ) );		

	//��������λ
	switch( cfg->ndata ){
		case 6:		//��������λ6λ
			comport[port].opt.c_cflag |= CS6;
			break;
		case 7:		//��������λ
			comport[port].opt.c_cflag |= CS7;
			break;
		case 8:		//��������λ
			comport[port].opt.c_cflag |= CS8;
			break;
		default:
			return -ERR_INVAL;
			break;
	}
	
	//������żУ��λ
	switch( cfg->verify){
	case COMPORT_VERIFY_EVEN://����
		comport[port].opt.c_cflag |= PARENB;
		comport[port].opt.c_cflag |= PARODD;
		comport[port].opt.c_iflag |= (INPCK | ISTRIP);
		break;
	case COMPORT_VERIFY_ODD: //ż��
		comport[port].opt.c_iflag |= (INPCK | ISTRIP);
		comport[port].opt.c_cflag |= PARENB;
		comport[port].opt.c_cflag &= ~PARODD;
		break;
	case COMPORT_VERIFY_NO:  //����żУ��λ
		comport[port].opt.c_cflag &= ~PARENB;
		break;
	default:
		return -ERR_INVAL;
		break;
	}
	
	//���ò�����
	switch( cfg->baud){
		case 50:	//������
			cfsetispeed(&comport[port].opt, B50);
			cfsetospeed(&comport[port].opt, B50);
			break;
		case 110:	//������
			cfsetispeed(&comport[port].opt, B110);
			cfsetospeed(&comport[port].opt, B110);
			break;
		case 150:	//������
			cfsetispeed(&comport[port].opt, B150);
			cfsetospeed(&comport[port].opt, B150);
			break;
		case 300:	//������
			cfsetispeed(&comport[port].opt, B300);
			cfsetospeed(&comport[port].opt, B300);
			break;
		case 600:	//������
			cfsetispeed(&comport[port].opt, B600);
			cfsetospeed(&comport[port].opt, B600);
			break;
		case 1200:	//������
			cfsetispeed(&comport[port].opt, B1200);
			cfsetospeed(&comport[port].opt, B1200);
			break;
		case 2400:	//������
			cfsetispeed(&comport[port].opt, B2400);
			cfsetospeed(&comport[port].opt, B2400);
			break;
		case 4800:	//������
			cfsetispeed(&comport[port].opt, B4800);
			cfsetospeed(&comport[port].opt, B4800);
			break;
		case 9600:	//������
			cfsetispeed(&comport[port].opt, B9600);
			cfsetospeed(&comport[port].opt, B9600);
			break;
		case 19200:	//������
			cfsetispeed(&comport[port].opt, B19200);
			cfsetospeed(&comport[port].opt, B19200);
			break;
		case 38400:	//������
			cfsetispeed(&comport[port].opt, B38400);
			cfsetospeed(&comport[port].opt, B38400);
			break;
		case 57600:	//������
			cfsetispeed(&comport[port].opt, B57600);
			cfsetospeed(&comport[port].opt, B57600);
			break;
		case 115200:	//������
			cfsetispeed(&comport[port].opt, B115200);
			cfsetospeed(&comport[port].opt, B115200);
			break;
		case 230400:	//������
			cfsetispeed(&comport[port].opt, B230400);
			cfsetospeed(&comport[port].opt, B230400);
			break;
		case 460800:	//������
			cfsetispeed(&comport[port].opt, B460800);
			cfsetospeed(&comport[port].opt, B460800);
			break;
		case 500000:	//������
			cfsetispeed(&comport[port].opt, B500000);
			cfsetospeed(&comport[port].opt, B500000);
			break;
		case 576000:	//������
			cfsetispeed(&comport[port].opt, B576000);
			cfsetospeed(&comport[port].opt, B576000);
			break;
		case 921600:	//������
			cfsetispeed(&comport[port].opt, B921600);
			cfsetospeed(&comport[port].opt, B921600);
			break;
		case 1000000:	//������
			cfsetispeed(&comport[port].opt, B1000000);
			cfsetospeed(&comport[port].opt, B1000000);
			break;
		case 1152000:	//������
			cfsetispeed(&comport[port].opt, B1152000);
			cfsetospeed(&comport[port].opt, B1152000);
			break;
	
		case 1500000:	//������
			cfsetispeed(&comport[port].opt, B1500000);
			cfsetospeed(&comport[port].opt, B1500000);
			break;
		case 2000000:	//������
			cfsetispeed(&comport[port].opt, B2000000);
			cfsetospeed(&comport[port].opt, B2000000);
			break;
		case 2500000:	//������
			cfsetispeed(&comport[port].opt, B2500000);
			cfsetospeed(&comport[port].opt, B2500000);
			break;
		case 3000000:	//������
			cfsetispeed(&comport[port].opt, B3000000);
			cfsetospeed(&comport[port].opt, B3000000);
			break;		
		case 3500000:	//������
			cfsetispeed(&comport[port].opt, B3500000);
			cfsetospeed(&comport[port].opt, B3500000);
			break;
		case 4000000:	//������
			cfsetispeed(&comport[port].opt, B4000000);
			cfsetospeed(&comport[port].opt, B4000000);
			break;
		default:
			ret = -ERR_INVAL;
			goto err;
			break;
	}

	
	//����ֹͣλ
	if( cfg->nstop == 1 )
		comport[port].opt.c_cflag &=  ~CSTOPB;
	else if ( cfg->nstop == 2 )				//����2λֹͣλ
		comport[port].opt.c_cflag |=  CSTOPB;
	else{
		ret = -ERR_INVAL;
		goto err;
	}
	//�趨��ʱʱ��
	if( (cfg->timeout < 0) || (cfg->timeout > 255) ){
		ret = -ERR_INVAL;
		goto err;
	}
	if(cfg->timeout == 0) {
		comport[port].opt.c_cc[VTIME] 	= 0;
		comport[port].opt.c_cc[VMIN] 	= 1;
	}else if( cfg->timeout == 0xff) {		//������
		comport[port].opt.c_cc[VTIME]	= 0;
		comport[port].opt.c_cc[VMIN]	= 0;
	}else{
		comport[port].opt.c_cc[VTIME]	= cfg->timeout;
		comport[port].opt.c_cc[VMIN] 	= 0;
	}
	
	//����Ӳ������
	if( cfg->rtscts == COMPORT_RTSCTS_ENABLE ){
		comport[port].opt.c_cflag |= CRTSCTS;
		comport_set_serial(port, 0);
	}else if ( cfg->rtscts == COMPORT_RTSCTS_DISABLE )
		comport[port].opt.c_cflag &= ~CRTSCTS;
	else{
		ret = -ERR_INVAL;
		goto err;
	}
	//��������
	if((tcsetattr(comport[port].fd, TCSANOW, &comport[port].opt)) != 0)
	{
		ret = -ERR_SYS;
		goto err;
	}
	//����δ�����ַ�
	tcflush(comport[port].fd,TCIFLUSH);
	if(comport[port].mode == COMPORT_MODE_485){
		comport_set_485(port);
	}

	memcpy(&comport[port].com,cfg,sizeof(comport_config_t));
	ret = 0;
err:
	return ret;
}

/******************************************************************************
*	����:	comport_getconfig
*	����:	��ȡ��������
*	����:	port			-	���ڶ˿ں�
			cfg				-	��������ݴ�����
*	����:	0				-	�ɹ�
			-ERR_INVAL		-	������Ч
			-ERR_NOINIT		-	û�г�ʼ��
*	˵��:	��
 ******************************************************************************/
int comport_getconfig (u8 port, comport_config_t *cfg)
{
	int ret = -1;
	if(port < 0 || port >= MAX_COMNUM){
		ret = -ERR_INVAL;
		goto err;
	}
	if(comport[port].count == 0){
		ret = -ERR_NOINIT;		//û�д�
		goto err;
	}
	memcpy(cfg,&comport[port].com,sizeof(comport_config_t));
	ret = 0;
err:
	return ret;
}

/******************************************************************************
*	����:	comport_send
*	����:	���ڷ���
*	����:	port			-	���ڶ˿ں�
			buf				-	���ݻ����������ݴ��룩
			count			-	�����ֽ���
*	����:	>=0				-	���ͳɹ����ֽ���
			-ESYS			-	ϵͳ����
			-ERR_INVAL		-	������Ч
			-ERR_NOINIT		-	û�г�ʼ��
*	˵��:	��
 ******************************************************************************/
int comport_send (u8 port, u8 *buf, u32 count)
{
	int ret = -1;
	if(port < 0 || port >= MAX_COMNUM){
		ret = -ERR_INVAL;
		goto err;
	}
	if (count == 0){
		ret = -ERR_INVAL;
		goto err;
	}
	if(comport[port].count == 0){
		ret = -ERR_NOINIT;		//û�д�
		goto err;
	}
	ret = write(comport[port].fd, buf, count);
	if(ret < 0){
		ret = -ERR_SYS;
		goto err;
	}

err:
	return ret;
}

/******************************************************************************
*	����:	comport_recv
*	����:	���ڽ���
*	����:	port			-	���ڶ˿ں�
			buf				-	���ݻ����������ݴ�����
			count			-	�����ֽ���
*	����:	>=0				-	���ճɹ����ֽ���
			-ERR_SYS		-	����
			-ERR_INVAL		-	������Ч
			-ERR_NOINIT		-	û�г�ʼ��
*	˵��:	��
 ******************************************************************************/
int comport_recv (u8 port, u8 *buf, u32 count)
{	
	int ret = -1;
	if(port < 0 || port >= MAX_COMNUM){
		ret = -ERR_INVAL;
		goto err;
	}
	if(comport[port].count == 0){
		ret = -ERR_NOINIT;		//û�д�
		goto err;
	}
	ret = read(comport[port].fd, buf, count);
	if(ret == 0){		
		ret = -ERR_TIMEOUT;
		goto err;		
	}
	if(ret < 0){		
		ret = -ERR_SYS;		
		goto err;
	}
		
err:
	return ret;
}

/******************************************************************************
*	����:	comport_flush
*	����:	�崮�ڻ�����
*	����:	port			-	���ڶ˿ں�
			mode			-	�建����ģʽ
*	����:	0				-	�ɹ�
			-ERR_INVAL		-	������Ч
			-ERR_NOINIT		-	û�г�ʼ��
*	˵��:	��
 ******************************************************************************/
int comport_flush (u8 port, u8 mode)
{
	int ret = -1;
	if(port < 0 || port >= MAX_COMNUM){
		ret = -ERR_INVAL;
		goto err;
	}
	if(comport[port].count == 0){
		ret = -ERR_NOINIT;		//û�д�
		goto err;
	}
	if(mode == COMPORT_FLUSH_ALL)
		ret = tcflush(comport[port].fd,TCIOFLUSH);
	else if(mode == COMPORT_FLUSH_RD)
		ret = tcflush(comport[port].fd,TCIFLUSH);
	else if(mode == COMPORT_FLUSH_WR)
		ret = tcflush(comport[port].fd,TCOFLUSH);
	else{
		ret = -ERR_INVAL;
		goto err;
	}
	if(ret < 0){		
		ret = -ERR_SYS;		
		goto err;
	}
	ret = 0;
err:
	return ret;
}
	

/******************************************************************************
*	����:	comport_close
*	����:	�رմ���
*	����:	port			-	���ڶ˿ں�
*	����:	0				-	�ɹ�
			-ERR_SYS		-	����
			-ERR_INVAL		-	������Ч
			-ERR_NOINIT		-	û�г�ʼ��
*	˵��:	��
 ******************************************************************************/
int comport_close (u8 port)
{
	int ret = -1;
	if(port < 0 || port >= MAX_COMNUM){
		ret = -ERR_INVAL;
		goto err;
	}
	if(comport[port].count == 0){
		ret = -ERR_NOINIT;		//û�д�
		goto err;
	}
	ret = close(comport[port].fd);
	if(ret < 0){
		return -ERR_SYS;
	}
	comport[port].count = 0;	
	ret = 0;
err:
	return ret;
}
#endif		 //CFG_ADC_MODULE
