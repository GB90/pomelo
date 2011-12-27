/*
 * flatsave.c
 * ��NOR FLASH��ʵ��ƽ���洢������
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "flash.h"

#define FLATSAVE_DRVNAME  "sdco_flatsave"//�豸����

static int flatsave_major = 0;  //���豸��

//#define FLATSAVE_BASEADDR		(FLASH_BASE+0x600000)
#define FLATSAVE_BASEADDR		(flash_virt_base+0x600000)   //flash����ַ
#define FLATSAVE_RECSIZE		128
#define FLASH_BLOCKSIZE			0x10000   //���С
#define FLATSAVE_FILENUM		10  //�ļ���Ŀ
#define FLATSAVE_FILEBLOCKS		3 //�ļ�����
#define FLATSAVE_FILESIZE		(FLASH_BLOCKSIZE*FLATSAVE_FILEBLOCKS)//0x10000*3
#define FLATSAVE_RECPERBLOCK    (FLASH_BLOCKSIZE/FLATSAVE_RECSIZE)  //0x10000/128

//#define FLATSAVE_MAGIC			0x3e7a
static unsigned short flatsave_magic = 0x3e7a; //�ӿڵ�ַ

struct flatsave_record {
	unsigned short magic;  //�ӿڵ�ַ
	unsigned short crc;    //crc����
	unsigned long sn;      //����
	unsigned char data[FLATSAVE_RECSIZE-8]; //���ݿ�
};//flash����

#define CRC16_POLY    0x1021
//crcУ���
static const unsigned short crc_tab[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0,
};
//���ܺ���:����crc
//��ڲ���:Ҫ����������׵�ַpbuf�����鳤��count
//����ֵ:����õ���crcֵ
static unsigned short cal_crc(volatile unsigned char*pbuf, unsigned long count)
{
	unsigned short temp = 0;

	if(0 == count) return 0;

	while ( count-- != 0 ) {
		temp = ( temp<<8 ) ^ crc_tab[ ( temp>>8 ) ^ *pbuf++ ];
	}
    
	return temp;
}

/*return 1-valid, 0-invalid*/
//����crc����flash�Ƿ���Ч
//����ֵΪ1˵����Ч
static inline int valid_record(volatile struct flatsave_record *prec)
{
	unsigned short crc;

	if(prec->magic != flatsave_magic) return 0;//���ȼ��flatsave_magic�Ƿ�ƥ��

	crc = cal_crc((volatile unsigned char *)&prec->sn, sizeof(*prec)-4);
	if(crc != prec->crc) return 0; //���crc�Ƿ����

	return 1;
}
//snɨ��
//pbase�Ƿ�������?
//����ֵΪ1˵��ɨ��ɹ�
static inline int scan_block_for_sn(volatile unsigned char *pbase, unsigned long *psn)
{
	int i;
	volatile struct flatsave_record *prec = (volatile struct flatsave_record *)pbase;

	for(i=0; i<FLATSAVE_RECPERBLOCK; i++,prec++) {
		if(valid_record(prec)) {
			*psn = prec->sn;   //�õ�ÿһ��snֵ
			return 0;
		}
	}

	*psn = 0;
	return 1;
}

static inline int scan_block_for_last(volatile unsigned char *pbase, volatile struct flatsave_record **pprec)
{
	int i, offset;
	volatile struct flatsave_record *prec = (volatile struct flatsave_record *)pbase;
	volatile struct flatsave_record *prev = NULL;

	offset = -1;

	for(i=0; i<FLATSAVE_RECPERBLOCK; i++,prec++) {
		if(valid_record(prec)) {
			prev = prec; //����
			offset = i;  //�õ�ƫ��
		}
	}

	*pprec = prev;
	return offset;
}
//������ȷ�Ŀ�
//����ֵΪ0˵��������ȷ
static inline int find_correct_block(unsigned long *sns, int *empties)
{
	int i;
	unsigned long biggest = 0;

	for(i=0; i<FLATSAVE_FILEBLOCKS; i++) {
		if(sns[i] > biggest) biggest = sns[i];
	}

	for(i=0; i<FLATSAVE_FILEBLOCKS; i++) {
		if((sns[i] == biggest) && (0 == empties[i])) return i;
	}

	return 0;
}

typedef struct {
	unsigned long sn;
	short block;
	short offset;
} flatsave_index_t;  //��������
static flatsave_index_t flatsave_index[FLATSAVE_FILENUM];
//����flash����
static inline void add_index(int id)
{
	flatsave_index[id].offset++;
	if(flatsave_index[id].offset >= FLATSAVE_RECPERBLOCK) {
		flatsave_index[id].offset = 0;
		flatsave_index[id].block++;
		flatsave_index[id].sn++;
		if(flatsave_index[id].block >= FLATSAVE_FILEBLOCKS) 
			flatsave_index[id].block = 0;
	}
}

/*#define o_readc(a)		(*(volatile unsigned char __force *)(a))
static void print_hexbuf(unsigned char *buf, int len)
{
	char str[128], *pstr;
	int i;

	pstr = str;
	*pstr = 0;
	for(i=1; i<=len; i++) {
		sprintf(pstr, "%02X ", o_readc(buf++));
		pstr += 3;
		if(0 == (i&0x0f)) {
			printk(KERN_ALERT "%s\n", str);
			pstr = str;
			*pstr = 0;
		}
		else if(0 == (i&0x07)) {
			*pstr++ = ':';
			*pstr++ = ' ';
			*pstr = 0;
		}
	}

	if(*str) {
		printk(KERN_ALERT "%s\n", str);
	}
}*/
//��ȡflash���ļ�
//�β�Ϊid��flatsave_recordͷ�ڵ�
static int load_file(int id, struct flatsave_record *buf)
{
	volatile unsigned char *pflash;
	int blockid, offset;
	volatile struct flatsave_record *prec;//precΪͷ�ڵ�

	pflash = (volatile unsigned char *)FLATSAVE_BASEADDR;//�õ�����ַ
	pflash += id*FLATSAVE_FILESIZE;//�õ�ӳ���ַ

	{
		unsigned long sns[FLATSAVE_FILEBLOCKS];
		int empties[FLATSAVE_FILEBLOCKS], i;

		for(i=0; i<FLATSAVE_FILEBLOCKS; i++)
			empties[i] = scan_block_for_sn(pflash+i*FLASH_BLOCKSIZE, &sns[i]);

		blockid = find_correct_block(sns, empties);//�õ�blockid
		flatsave_index[id].sn = sns[blockid];
	}

	flatsave_index[id].block = blockid;
	pflash += (int)blockid*FLASH_BLOCKSIZE;
	offset = scan_block_for_last(pflash, &prec);

	if(offset < 0) {
		flatsave_index[id].offset = 0;
		return 1;
	}

	*buf = *prec; //���������ݱ��浽bufָ��
	add_index(id);

	return 0;
}

static void write_file(int id, struct flatsave_record *buf)
{
	unsigned char *pflash;

	if(flatsave_index[id].block < 0) {
		char *kbuf;
        //�����ں˿ռ�
		kbuf = kmalloc(sizeof(*buf), GFP_KERNEL);
		if(NULL == kbuf) {
			printk("kmalloc failed\n");
			return;
		}
        //��ȡflash���ں˿ռ�
		load_file(id, (struct flatsave_record *)kbuf);
		kfree(kbuf);
	}

	pflash = (unsigned char *)FLATSAVE_BASEADDR;
	pflash += id*FLATSAVE_FILESIZE;

	if(flatsave_index[id].offset < 0) flatsave_index[id].offset = 0;

	pflash += (int)flatsave_index[id].block * FLASH_BLOCKSIZE;
	pflash += (int)flatsave_index[id].offset * FLATSAVE_RECSIZE;
    //�Ȳ�����д             
	if(flatsave_index[id].offset == 0) { /*empty*/
		flash_eraseblk((unsigned short *)pflash);   //����ģ��
	}

	buf->magic = flatsave_magic;   //�ṹ���Ա��ֵ
	buf->sn = flatsave_index[id].sn;
	buf->crc = cal_crc((volatile unsigned char *)&buf->sn, sizeof(*buf)-4); //����crc
    //дflash
	flash_writeblock((unsigned short *)pflash, (unsigned short *)buf, sizeof(*buf));

	add_index(id);  //�ӵ��ļ���ҳ��
}

static int flatsave_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int flatsave_close(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t flatsave_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char *kbuf;
	int id;
	struct flatsave_record *prec;
    
	if(count < (sizeof(*prec)+4)) return -EFAULT;
    //�����С
	count = sizeof(*prec)+4;
    //�����ں˻���������kbuf
	kbuf = kmalloc(sizeof(*prec)+4, GFP_KERNEL);
	if(NULL == kbuf) {
		printk("kmalloc failed.\n");
		return -ENOMEM;
	}
	//���û��ռ临�����ݵ��ں˿ռ�,
	if(copy_from_user(kbuf, buf, count)) {
		kfree(kbuf);
		return -EFAULT;
	}

	id = (int)kbuf[0];
	if((id < 0) || (id >= FLATSAVE_FILENUM)) {
		kfree(kbuf);
		return -EFAULT;
	}

	prec = (struct flatsave_record *)(kbuf+4);
	//�����Ѿ��������ں�
	write_file(id, prec);  //�ں�дflash

	kfree(kbuf);

	return count;
}
//pposΪ������ļ���ƫ�Ƶ�ַ����һ���õ�
static ssize_t flatsave_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	char *kbuf;
	int id;
	struct flatsave_record *prec;   //����flatsave_record�ṹ��

	if(count < (sizeof(*prec)+4)) return -EFAULT;//����ýṹ��Ĵ�С

	count = sizeof(*prec)+4;
    //���ں�����ռ�
	kbuf = kmalloc(sizeof(*prec)+4, GFP_KERNEL);
	if(NULL == kbuf) {
		printk("kmalloc failed.\n");
		return -ENOMEM;
	}
	//����û��βε��ں��βε�ӳ��
	if(copy_from_user(kbuf, buf, 4)) goto mark_error;
	
	id = (int)kbuf[0]; //����ĵ�һ��Ԫ��
	if((id < 0) || (id >= FLATSAVE_FILENUM)) goto mark_error;
    //�����ں˲���
	prec = (struct flatsave_record *)(kbuf+4);
	//��flash���ݵ��ں˿ռ�
	if(load_file(id, prec)) goto mark_error;
    //������������ͨ���ں˲�ת�����û���
	if(copy_to_user(buf, kbuf, count)) goto mark_error;

	kfree(kbuf);
	return count;

mark_error:
	kfree(kbuf); //�ͷ�������ں˿ռ�
	return -EFAULT;
}

static int flatsave_ioctl(struct inode *inode, struct file *file, u_int cmd, u_long arg)
{
	if(1 != cmd) return -EFAULT;///ֻ�����˿�������1

	flatsave_magic = (unsigned short)arg;
	//printk(KERN_ALERT "flatsave magic = %04X\n", flatsave_magic);

	return 0;
}
//�����豸ӳ��Ϊ�ļ������Ľӿ�
static struct file_operations flatsave_fops = {
	.owner = THIS_MODULE,
	.read = flatsave_read,
	.write = flatsave_write,
	.ioctl = flatsave_ioctl,
	.open = flatsave_open,
	.release = flatsave_close,
};
//ģ����غ���
int sdco_flatsave_init(void)
{
	int result;

	printk(KERN_ALERT "init flatsave driver...\n");
	///ע���豸
	result = register_chrdev(flatsave_major, FLATSAVE_DRVNAME, &flatsave_fops);
	if(result < 0) {
		printk(KERN_ALERT "flatsave: can not get major %d\n", flatsave_major);
		return result;
	}
	if(0 == flatsave_major) flatsave_major = result;

	/*set all valid io to input*/
	for(result=0; result<FLATSAVE_FILENUM; result++) {
		flatsave_index[result].block = -1;
		flatsave_index[result].offset = -1;
	}

	flash_init();//��ʼ��flash

	printk(KERN_ALERT "init flatsave driver OK, major=%d\n", flatsave_major);

	return 0;
}
//ģ��ж�غ���
void sdco_flatsave_exit(void)
{
	printk(KERN_ALERT "exit flatsave driver\n");
	unregister_chrdev(flatsave_major, FLATSAVE_DRVNAME);
}

