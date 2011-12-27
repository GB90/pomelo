/*
 * flash.h
 * NOR FLASH k8d6*16utm (Samsung) оƬ����
 */

#include <asm/arch/gpio.h>

#define FLASH_PHY_BASE    0x10000000    //flash�����ַ
static unsigned long flash_virt_base;

//READY=1, BUSY=0
#define FLASHIO_RYBY		AT91_PIN_PA19
#define FLASH_DELAYTIME		500
#define FLASH_POLLTIME		1000000

#define flash_writew(v, a)	(*(volatile unsigned short __force *)(a) = (v))
#define flash_readw(a)		(*(volatile unsigned short __force *)(a))

static void inline flash_program_delay(void)
{
	int i;

	for(i=0; i<FLASH_DELAYTIME; i++);
}

static int flash_program_wait(void)
{
	int i;

	flash_program_delay();  //�����ʱ

	for(i=0; i<FLASH_POLLTIME; i++) {
		if(at91_get_gpio_value(FLASHIO_RYBY)) break;//�жϱ�־λ���Ƿ�������
	}

	if(i >= FLASH_POLLTIME) return 1;  //��ʱ������1
	else return 0;
}
//����flash�ȴ�
static int flash_erase_wait(void)
{
	int i;

	flash_program_delay();//flash��̵ȴ�

	for(i=0; i<FLASH_POLLTIME; i++) {
		if(!flash_program_wait()) break;
	}

	if(i >= FLASH_POLLTIME) return 1;
	else return 0;
}
//flashд��һ���֣��β�Ϊ�׵�ַ��Ҫд�������
static int flash_writeword(unsigned short *addr, unsigned short data)
{
	unsigned short *pop;
    //���Ƚ��̶�����0xaa��0x55��0xa0д���̶���ַ
	pop = (unsigned short *)flash_virt_base + 0x555;
	flash_writew(0xaa, pop);

	pop = (unsigned short *)flash_virt_base + 0x2aa;
	flash_writew(0x55, pop);

	pop = (unsigned short *)flash_virt_base + 0x555;
	flash_writew(0xa0, pop);

	flash_writew(data, addr);  //д��Ҫд�ĵ�ַ������

	return(flash_program_wait());
}
//��ram����д�뵽flash
//��ڲ���:flash�׵�ַ��ram�׵�ַ,���ݳ���len
static int flash_writeblock(unsigned short *flash_addr, unsigned short *ram_addr, unsigned long len)
{
	if(len&0x01) len = (len>>1)+1;
	else len >>= 1;
    //һ����һ���ֵ�д
	while(len) {
		flash_writeword(flash_addr++, *ram_addr++);  
		len--;
	}

	return 0;
}
//����flash�飬�β�Ϊ�׵�ַ
static int flash_eraseblk(unsigned short *addr)
{
	unsigned short *pop;

	pop = (unsigned short *)flash_virt_base + 0x555;
	flash_writew(0xaa, pop);

	flash_program_delay();

	pop = (unsigned short *)flash_virt_base + 0x2aa;
	flash_writew(0x55, pop);

	flash_program_delay();

	pop = (unsigned short *)flash_virt_base + 0x555;
	flash_writew(0x80, pop);

	flash_program_delay();

	pop = (unsigned short *)flash_virt_base + 0x555;
	flash_writew(0xaa, pop);

	flash_program_delay();

	pop = (unsigned short *)flash_virt_base + 0x2aa;
	flash_writew(0x55, pop);

	flash_program_delay();

	flash_writew(0x30, addr); //д��0x30��Ҫ������flash��ַ

	flash_erase_wait();

	return 0;
}

extern unsigned long at91_norflash_virtaddrbase(void);
static void flash_init(void)
{
	at91_set_gpio_input(FLASHIO_RYBY, 1);
	flash_virt_base = at91_norflash_virtaddrbase();
}
