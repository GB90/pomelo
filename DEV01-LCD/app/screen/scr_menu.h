/**
* scr_menu.h -- �˵���ʾ����ͷ�ļ�
* 
* 
* ����ʱ��: 2010-5-20
* ����޸�ʱ��: 2010-7-10
*/

#ifndef _SCR_MENU_H_
#define _SCR_MENU_H_
//------------------------------------------------------
#define SCHAR     char
#define UCHAR     unsigned char
#define VCHAR     volatile unsigned char
#define SSHORT    short
#define USHORT    unsigned short
#define VSHORT    volatile unsigned short
#define SINT      int
#define UINT      unsigned int
#define VINT      volatile unsigned int
#define SLONG     long
#define ULONG     unsigned long
#define VLONG     volatile unsigned long
#define INT64     __int64
//------------------------------------------------------
#define DO_FUNC			0x100
#define DO_SHOW		    0x200
#define DO_SHOW_CHG	    0x300

#define SHOW_ATTR		0x00
#define DO_ATTR			0x01

#define MENU_NULL	    0x00
//--------------------------------------------------------
#define START_LINE_POS	0
#define START_COL_POS	0
#define ARR_COL_POS	    104

#define PG_MNU_MAX	    7
#define MENU_LINE_POS	9
//---------------------------------------------------------


//-------------------------define key value-----------------------------
///�ڽ�����ײ�˵�֮ǰֻ��UPKEY��DOWNKEY��OKKEY��ESCKEY��Ч
#define UPKEY		0x08
#define DOWNKEY	0x02
#define LEFTKEY		0x05
#define RIGHTKEY	0x07
#define OKKEY		0x01
#define ESCKEY		0x04

/*
#define OKKEY1		0x18
#define OKKEY2		0x19
#define OKKEY3		0x20
#define OKKEY4		0x21
#define OKKEY5		0x22
#define OKKEY6		0x22
*/

#define OKKEY1		0x18
#define OKKEY2		0x19
#define OKKEY3		0x2A
#define OKKEY4		0x2B
#define OKKEY5		0x2C
#define OKKEY6		0x2D
#define OKKEY7		0x2E

/*
#define CHGFKEY		0x07
#define EXITKEY		0x08
#define ADJKEY		0x09
#define FUNCKEY		0x0a
#define OUTKEY		0x0b
#define LIGHTKEY		0x0c
#define CANCELKEY	0x0d


#define LOOPKEY		0x0e
#define PAGE_UPKEY	0x0f
#define PAGE_DOWNKEY	0x10
#define OUTKEY2		0x11
#define NULLKEY	0xff
*/

#define CHGFKEY		0x09
#define EXITKEY		0x0a
#define ADJKEY		0x0b
#define FUNCKEY		0x0c
#define OUTKEY		0x0d
#define LIGHTKEY	0x0e
#define CANCELKEY	0x0f


#define LOOPKEY		0x10
#define PAGE_UPKEY	0x11
#define PAGE_DOWNKEY	0x12
#define OUTKEY2		0x13
#define NULLKEY	0xff
//---------------------------------------------------------
#define LCD_ROW_SIZE	7
#define LCD_COL_SIZE	20  

#define PARA_SET 1
///#define PARA_SET 0
#define MET_NUM_SEL 2

///��ʾ�������ɺ���
typedef  void (*pmenu_do_func)(unsigned short id,unsigned long para,unsigned char key);


///#define MAX_SHOW_LINE	32   //������
///#define MAX_SHOW_COL	20   //������

#define MAX_SHOW_LINE	63   //������
#define MAX_SHOW_COL	21   //������


typedef struct{
	int line;      //��
	char buf[MAX_SHOW_LINE][MAX_SHOW_COL];  //��ʾ������
	}scrpage_t,*pscrpage_t;

typedef struct{
	unsigned short  id; //�������
	unsigned long int para; //����
	unsigned char key;    
	}MNU_FUC_PARA,*PMNU_FUC_PARA;


typedef struct{
	char *  title;   //�����ַ���
	///unsigned char *  title;   //�����ַ���
	pmenu_do_func mpmenu_do_func;  //�����¼��˵��ĺ�������mpmenu_do_func��û��son
	MNU_FUC_PARA para;   //��ʾ�����Ĳ���
	///son�ں���ʹ��ʱ������һ��ָ������ת����ָ����MNU_ITEMP
	unsigned char * son;/*MNU_ITEMP*/   ///��son��û��mpmenu_do_func
	unsigned char attr;
	}MNU_SUB_ATTR,*PMNU_SUB_ATTR;


typedef struct{
	pmenu_do_func mpmenu_do_func;
	MNU_FUC_PARA para;
	}MNU_LOOP_FUNC,*PMNU_LOOP_FUNC;

///���Զ���һ��ȫ�ֵĽṹ�壬���β�������������д���
typedef struct XMNU_ITEM{
    ///PMNU_SUB_ATTRΪָ�����ͣ�sub_attr��Ϊ����ʹ��
	PMNU_SUB_ATTR sub_attr;  ///ӵ����һ�����ֲ˵����Ӳ˵����ڳ�ʼ����ʱ�򱻸���һ�����������
	struct XMNU_ITEM * father;  ///���˵�Ϊ��һ�����ֲ˵����γ�һ������
	char item_num;    ///�˽�����Ŀ�ĸ���,��sub_attr�����Ա�ĸ���
    ///ȷ��ѡ��˵��ڽ����е��У�������Ϊsub_attr�������±�
	unsigned char item_cur;    ///��ǰ������ѡ����У�ͨ��������ݼ����ı�sub_attr[]���±꣬�Ӷ�ѡ��ͬ�Ĳ˵�
	unsigned int  item_action;    ///�ǻ��ֽ��滹�Ƿ�ҳ����
	}MNU_ITEM,* MNU_ITEMP;  ///�˽ṹ���������һ������˵����е���Ϣ


///ʵ�ֻ��ֲ˵�������ʽ����
typedef struct{
	MNU_ITEMP mnu;          
	MNU_ITEMP root_mnu;   
	MNU_SUB_ATTR *  mnu_item;  ///Ϊһ��������
	}MNU_ATTR,*PMNU_ATTR;


void ReturnRoot(void);

///unsigned int GetMenu(MNU_ITEMP item);
///void Make_menu(PMNU_ATTR m_menux);
void SetSysMenu(MNU_ITEMP xmenu,MNU_LOOP_FUNC * lpf, MNU_LOOP_FUNC * almf);
///unsigned char Run_Menu(void);
void DefMenuFuc(unsigned char id,unsigned long int x,unsigned char key);

void ClrShowBuf(pscrpage_t pshowp);
void ShowPageBuf(unsigned char key);
void ShowDspBufLoop(void);
void scr_alarm(UCHAR flag, pmenu_do_func pfunc);
void scr_note(UCHAR flag, char *info);
void LoadAllMenu(PMNU_ATTR m_menux);
void InitSysMenu(MNU_ITEMP xmenu,MNU_LOOP_FUNC * lpf);
unsigned char MenuProcess(void);
//------------------------------------------------------------------------------
void DrawMenuLine(void);
#endif

