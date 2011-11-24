/**
* scr_menupage.c -- ��ʾ��ҳ�˵�
* 
* 
* ����ʱ��: 2010-5-20
* ����޸�ʱ��: 2010-7-10
*/

#include <string.h>
#include <stdio.h>   
#include <stdlib.h>

#define DEFINE_PARATERM
#define DEFINE_PARAUNI

#include "scr_menu.h"
#include "scr_input.h"
#include "scr_input_api.h"
#include "scr_lcd.h"
#include "scr_menupage.h"
#include "include/param/term.h"
#include "include/param/mix.h"
#include "include/param/hardware.h"
#include "include/param/commport.h"
#include "include/param/meter.h"
#include "include/param/metp.h"
#include "include/param/datatask.h"
#include "include/param/operation.h"
#include "include/param/capconf.h"
#include "include/param/route.h"
#include "include/param/unique.h"
#include "include/lib/dbtime.h"
#include "cenmet/mdb/mdbana.h"
#include "cenmet/mdb/mdbcur.h"
#include "cenmet/mdb/mdbconf.h"
#include "include/sys/timeal.h"  
#include "plcmet/plmdb.h"
#include "plcmet/plc_stat.h"
#include "include/version.h"
#include "include/debug.h"
#include "uplink/terminfo.h"
#include "include/monitor/runstate.h"
#include "uplink/ppp/ipcp.h"
#include "include/plcmet/pltask.h"
#include "include/sys/timeal.h"
#include "include/sys/reset.h"
#include "include/sys/schedule.h"
#include "screen/scr_lcd.h"
#include "param/operation_inner.h"
#include "include/lib/align.h"
#include "include/lib/bcd.h"

#define INTNUM_MASK			0x0c
#define DOTNUM_MASK		    0x03
#define PWRPHASE				0x0c
#define TASK_RUN				0x01
#define TASK_STOP				0x00
#define COMMFRAME_DATA		    0x03
#define METPRDNUM_MASK			0x1f

#define DISPLAY_MET_MUN		5

extern para_term_t ParaTerm;
extern para_uni_t ParaUni;
extern scrpage_t scrpbuf_main;
#define CHAR_FLAG           1
//�����л�ѡ��˵�
extern const char typ_charlist_a[];
static const char typ_charlist_a1[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',};
static const char typ_stringlist_a[11][MAX_LINECHAR] = {"  ��Ч   ","  ����   ", "GPRS/CDMA", "  DTMF   ", " ��̫��  ", "  ����   ", "  RS232  ", "   CSD   ", "  Radio  ", " ���Ż���","  ����   "};
static const char typ_stringlist_tcp[2][MAX_LINECHAR] = {"TCP","UDP"};
static const char typ_stringlist_c[2][MAX_LINECHAR] = {"��Ч","��Ч"};
static const char typ_stringlist_d[6][MAX_LINECHAR] = {"δ֪    ", "485��   ", "ģ����  ", "������  ", "����ֵ  ", "��������"};
static const char typ_stringlist_c1[2][MAX_LINECHAR] = {"ѡ��1","��Ч"};
static const char typ_stringlist_d1[6][MAX_LINECHAR] = {"ѡ��2    ", "485��   ", "ģ����  ", "������  ", "����ֵ  ", "��������"};
static const char typ_stringlist_e[5][MAX_LINECHAR] = {"��Ч","1   ","2   ","3   ","4   "};
static const char typ_stringlist_f[27][MAX_LINECHAR] ={"��Ч     ","����     ","IEC1107  ","��ʤ     ","����     ","������   ","��¡     ",
											"����     ","������D��","���     ","��½     ","����     ","������   ",
											"ABBa��   ","ABBԲ��  ","�����   ","����MK3��","����     ","������B��",
											"����     ","�������� ","��÷��   ","��˹��   ","����˹�� ","��ʤ     ",
											"���ֿ鳭 ","������   "};
static const char typ_stringlist_g[3][MAX_LINECHAR] ={"��У��", "żУ��", "��У��"};

//����ѡ���л��˵�
static const char typ_charlist_1[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
static const char typ_charlist_2[] = {'5', '6', '7','8'};
static const char typ_charlist_bcoms[2][MAX_LINECHAR]={"��","��"};
static const char typ_charlist_stopbit[2][MAX_LINECHAR]={"1","2"};
static const char typ_charlist_databit[4][MAX_LINECHAR]={"5","6","7","8"};
//static const char typ_charlist_3[] = {' ','A', 'B', 'C','D','E', 'F', 'G','H','I', 'J', 'K','L','M', 'N',
//							'O','P','Q', 'R', 'S','T','U', 'V', 'W','X','Y','Z','.'};
static const char typ_charlist_3[] = {'A', 'B', 'C','D','E', 'F', 'G','H','I', 'J', 'K','L','M', 'N',
							'O','P','Q', 'R', 'S','T','U', 'V', 'W','X','Y','Z','.'};
static const char typ_charlist_4[] = {'1', '2'};
static const char typ_charlist_5[] = {' ','0', '1', '2', '3', '4', '5', '6', '7', '8', '9','a','b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
							'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u','v', 'w', 'x', 'y', 'z','A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K','L','M', 'N',
							'O','P','Q', 'R', 'S','T','U', 'V', 'W','X','Y','Z'};
static const char typ_charlist_3_1[] = {'.'};
static const char typ_stringlist_fnum[27]={0,10,20,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,55,51,18,19};
static const char typ_baudrate[10][MAX_LINECHAR]={"    0","  300","  600"," 1200"," 2400"," 4800"," 7200"," 9600","19200","57600"};
static const char typ_charlist_mid[16][MAX_LINECHAR]={"0 ","1 ","2 ","3 ","4 ","5 ","6 ","7 ","8 ","9 ","10","11","12","13","14","15"};

unsigned int GetPortNum();
unsigned char GetTaskNum();
unsigned short GetSnNum();
unsigned int GetMajorMetNum();
void DefOkClick(void);
void DefEscClick(void);
void InitControlPage(void);
int AddButton(unsigned char x, unsigned char y, char *text, pf_button_click_t pf_click);
int ShowControlPage(void);
int AddEditBox(unsigned char x, unsigned char y, int numchars, char *charlist, int defsel, unsigned char *prtn,char flag);

#define SCRINFO_NULLMET  { \
	strcpy(scrpbuf_main.buf[0],"");\
	strcpy(scrpbuf_main.buf[1], "        ��ʾ"); \
	strcpy(scrpbuf_main.buf[2],"    ���ܻ�ȡ��Ӧ��");\
	strcpy(scrpbuf_main.buf[3], "���������,������");\
	strcpy(scrpbuf_main.buf[4],"Ӧ������������Ϣ");\
	scrpbuf_main.line = 5; }

extern const int SvrCommLineState;

    
/**
* @brief			�����׽���
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpHead(unsigned short mid, ULONG arg, UCHAR key)
{
	ClearTopBuffer();
	scrpbuf_main.line = 2;  
	strcpy(scrpbuf_main.buf[1], "    �人�ǳ̵���");
	strcpy(scrpbuf_main.buf[2], "    �Ƽ����޹�˾");
}

/**
* @brief			��ʾ�ܱ����������
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpMetDataEne(unsigned short mid, ULONG arg, UCHAR key)
{
	ScreenSetHeadFlag(DISPLAY_MET_MUN,mid);
    scrpbuf_main.line = 18;
	sprintf(scrpbuf_main.buf[0],"�����й��ܵ���:");
	sprintf(scrpbuf_main.buf[1],"  ��=%X%02X%02X.%02X%02XkWh",MdbCurrent[mid].enepa[4],MdbCurrent[mid].enepa[3],MdbCurrent[mid].enepa[2],MdbCurrent[mid].enepa[1],MdbCurrent[mid].enepa[0]);
	sprintf(scrpbuf_main.buf[2],"  P1=%X%02X%02X.%02X%02XkWh",MdbCurrent[mid].enepa[9],MdbCurrent[mid].enepa[8],MdbCurrent[mid].enepa[7],MdbCurrent[mid].enepa[6],MdbCurrent[mid].enepa[5]);
	sprintf(scrpbuf_main.buf[3],"  P2=%X%02X%02X.%02X%02XkWh",MdbCurrent[mid].enepa[14],MdbCurrent[mid].enepa[13],MdbCurrent[mid].enepa[12],MdbCurrent[mid].enepa[11],MdbCurrent[mid].enepa[10]);
	sprintf(scrpbuf_main.buf[4],"  P3=%X%02X%02X.%02X%02XkWh",MdbCurrent[mid].enepa[19],MdbCurrent[mid].enepa[18],MdbCurrent[mid].enepa[17],MdbCurrent[mid].enepa[16],MdbCurrent[mid].enepa[15]);
	sprintf(scrpbuf_main.buf[5],"  P4=%X%02X%02X.%02X%02XkWh",MdbCurrent[mid].enepa[24],MdbCurrent[mid].enepa[23],MdbCurrent[mid].enepa[22],MdbCurrent[mid].enepa[21],MdbCurrent[mid].enepa[20]);

	sprintf(scrpbuf_main.buf[7],"�����й��ܵ���:");
    sprintf(scrpbuf_main.buf[8],"  ��=%X%02X%02X.%02X%02XkWh",MdbCurrent[mid].enena[4],MdbCurrent[mid].enena[3],MdbCurrent[mid].enena[2],MdbCurrent[mid].enena[1],MdbCurrent[mid].enena[0]);
	sprintf(scrpbuf_main.buf[9],"�����޹��ܵ���:");
	sprintf(scrpbuf_main.buf[10],"  ��=%X%02X%02X.%02Xkvarh",MdbCurrent[mid].enepi[3],MdbCurrent[mid].enepi[2],MdbCurrent[mid].enepi[1],MdbCurrent[mid].enepi[0]);
	sprintf(scrpbuf_main.buf[11],"  һ=%X%02X%02X.%02Xkvarh",MdbCurrent[mid].enepi1[3],MdbCurrent[mid].enepi1[2],MdbCurrent[mid].enepi1[1],MdbCurrent[mid].enepi1[0]);
	sprintf(scrpbuf_main.buf[12],"  ��=%X%02X%02X.%02Xkvarh",MdbCurrent[mid].enepi4[3],MdbCurrent[mid].enepi4[2],MdbCurrent[mid].enepi4[1],MdbCurrent[mid].enepi4[0]);

	sprintf(scrpbuf_main.buf[14],"�����޹��ܵ���:");
	sprintf(scrpbuf_main.buf[15],"  ��=%X%02X%02X.%02Xkvarh",MdbCurrent[mid].eneni[3],MdbCurrent[mid].eneni[2],MdbCurrent[mid].eneni[1],MdbCurrent[mid].eneni[0]);
	sprintf(scrpbuf_main.buf[16],"  ��=%X%02X%02X.%02Xkvarh",MdbCurrent[mid].eneni2[3],MdbCurrent[mid].eneni2[2],MdbCurrent[mid].eneni2[1],MdbCurrent[mid].eneni2[0]);
	sprintf(scrpbuf_main.buf[17],"  ��=%X%02X%02X.%02Xkvarh",MdbCurrent[mid].eneni3[3],MdbCurrent[mid].eneni3[2],MdbCurrent[mid].eneni3[1],MdbCurrent[mid].eneni3[0]);


}
/**
* @brief			��ʾ�ܱ����������
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpMetDataDmn(unsigned short mid, ULONG arg, UCHAR key)
{
	ScreenSetHeadFlag(DISPLAY_MET_MUN,mid);
    scrpbuf_main.line = 14;
	sprintf(scrpbuf_main.buf[0],"�����й��������:");
	sprintf(scrpbuf_main.buf[1],"  %X.%02X%02XkW",MdbCurrent[mid].dmnpa[2],MdbCurrent[mid].dmnpa[1],MdbCurrent[mid].dmnpa[0]);
	sprintf(scrpbuf_main.buf[2],"  %02X-%02X  %02X:%02X",MdbCurrent[mid].dmntpa[3],MdbCurrent[mid].dmntpa[2],MdbCurrent[mid].dmntpa[1],MdbCurrent[mid].dmntpa[0]);
	sprintf(scrpbuf_main.buf[3],"�����޹��������:");
	sprintf(scrpbuf_main.buf[4],"  %X.%02X%02Xkvar",MdbCurrent[mid].dmnpi[2],MdbCurrent[mid].dmnpi[1],MdbCurrent[mid].dmnpi[0]);
	sprintf(scrpbuf_main.buf[5],"  %02X-%02X  %02X:%02X",MdbCurrent[mid].dmntpi[3],MdbCurrent[mid].dmntpi[2],MdbCurrent[mid].dmntpi[1],MdbCurrent[mid].dmntpi[0]);

	sprintf(scrpbuf_main.buf[7],"�����й��������:");
	sprintf(scrpbuf_main.buf[8],"  %X.%02X%02XkW",MdbCurrent[mid].dmnna[2],MdbCurrent[mid].dmnna[1],MdbCurrent[mid].dmnna[0]);
	sprintf(scrpbuf_main.buf[9],"  %02X-%02X  %02X:%02X",MdbCurrent[mid].dmntna[3],MdbCurrent[mid].dmntna[2],MdbCurrent[mid].dmntna[1],MdbCurrent[mid].dmntna[0]);
	sprintf(scrpbuf_main.buf[10],"�����޹��������:");
	sprintf(scrpbuf_main.buf[11],"  %X.%02X%02Xkvar",MdbCurrent[mid].dmnni[2],MdbCurrent[mid].dmnni[1],MdbCurrent[mid].dmnni[0]);
	sprintf(scrpbuf_main.buf[12],"  %02X-%02X  %02X:%02X",MdbCurrent[mid].dmntni[3],MdbCurrent[mid].dmntni[2],MdbCurrent[mid].dmntni[1],MdbCurrent[mid].dmntni[0]);
}
/**
* @brief			��ʾ�ܱ��˲ʱ������
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpMetDataImm(unsigned short mid, ULONG arg, UCHAR key)
{
	ScreenSetHeadFlag(DISPLAY_MET_MUN,mid);
	sprintf(scrpbuf_main.buf[0]," Ua=%d.%XV",(MdbCurrent[mid].vol[1]>>4)*100 + (MdbCurrent[mid].vol[1]&0x0F)*10 + (MdbCurrent[mid].vol[0]>>4),MdbCurrent[mid].vol[0]&0x0f);
	sprintf(scrpbuf_main.buf[1]," Ub=%d.%XV",(MdbCurrent[mid].vol[3]>>4)*100 + (MdbCurrent[mid].vol[3]&0x0F)*10 + (MdbCurrent[mid].vol[2]>>4),MdbCurrent[mid].vol[2]&0x0f);
	sprintf(scrpbuf_main.buf[2]," Uc=%d.%XV",(MdbCurrent[mid].vol[5]>>4)*100 + (MdbCurrent[mid].vol[5]&0x0F)*10 + (MdbCurrent[mid].vol[4]>>4),MdbCurrent[mid].vol[4]&0x0f);
	sprintf(scrpbuf_main.buf[3]," Ia=%X.%02XA",MdbCurrent[mid].amp[1],MdbCurrent[mid].amp[0]);
	sprintf(scrpbuf_main.buf[4]," Ib=%X.%02XA",MdbCurrent[mid].amp[3],MdbCurrent[mid].amp[2]);
	sprintf(scrpbuf_main.buf[5]," Ic=%X.%02XA",MdbCurrent[mid].amp[5],MdbCurrent[mid].amp[4]);

	sprintf(scrpbuf_main.buf[7]," In=%X.%02XA",MdbCurrent[mid].amp[7],MdbCurrent[mid].amp[6]);
	sprintf(scrpbuf_main.buf[8]," Ps=%X.%02X%02XkW",MdbCurrent[mid].pwra[2],MdbCurrent[mid].pwra[1],MdbCurrent[mid].pwra[0]);
	sprintf(scrpbuf_main.buf[9]," Pa=%X.%02X%02XkW",MdbCurrent[mid].pwra[5],MdbCurrent[mid].pwra[4],MdbCurrent[mid].pwra[3]);
	sprintf(scrpbuf_main.buf[10]," Pb=%X.%02X%02XkW",MdbCurrent[mid].pwra[8],MdbCurrent[mid].pwra[7],MdbCurrent[mid].pwra[6]);
	sprintf(scrpbuf_main.buf[11]," Pc=%X.%02X%02XkW",MdbCurrent[mid].pwra[11],MdbCurrent[mid].pwra[10],MdbCurrent[mid].pwra[9]);
	if(MdbCurrent[mid].pwri[2]&0x80)
	sprintf(scrpbuf_main.buf[12]," Qs=-%X.%02X%02Xkvar",MdbCurrent[mid].pwri[2]&0x7f,MdbCurrent[mid].pwri[1],MdbCurrent[mid].pwri[0]);
	else
	sprintf(scrpbuf_main.buf[12]," Qs=%X.%02X%02Xkvar",MdbCurrent[mid].pwri[2],MdbCurrent[mid].pwri[1],MdbCurrent[mid].pwri[0]);

	if(MdbCurrent[mid].pwri[5]&0x80)
	sprintf(scrpbuf_main.buf[14]," Qa=-%X.%02X%02Xkvar",MdbCurrent[mid].pwri[5]&0x7f,MdbCurrent[mid].pwri[4],MdbCurrent[mid].pwri[3]);
	else
	sprintf(scrpbuf_main.buf[14]," Qa=%X.%02X%02Xkvar",MdbCurrent[mid].pwri[5],MdbCurrent[mid].pwri[4],MdbCurrent[mid].pwri[3]);

	if(MdbCurrent[mid].pwri[8]&0x80)
	sprintf(scrpbuf_main.buf[15]," Qb=-%X.%02X%02Xkvar",MdbCurrent[mid].pwri[8]&0x7f,MdbCurrent[mid].pwri[7],MdbCurrent[mid].pwri[6]);
	else
	sprintf(scrpbuf_main.buf[15]," Qb=%X.%02X%02Xkvar",MdbCurrent[mid].pwri[8],MdbCurrent[mid].pwri[7],MdbCurrent[mid].pwri[6]);

	if(MdbCurrent[mid].pwri[11]&0x80)
	sprintf(scrpbuf_main.buf[16]," Qc=-%X.%02X%02Xkvar",MdbCurrent[mid].pwri[11]&0x7f,MdbCurrent[mid].pwri[10],MdbCurrent[mid].pwri[9]);
	else
	sprintf(scrpbuf_main.buf[16]," Qc=%X.%02X%02Xkvar",MdbCurrent[mid].pwri[11],MdbCurrent[mid].pwri[10],MdbCurrent[mid].pwri[9]);

	sprintf(scrpbuf_main.buf[17]," COS��=%d.%X%s",((MdbCurrent[mid].pwrf[1]&0x7F)>>4)*100 + ((MdbCurrent[mid].pwrf[1]&0x7F)&0x0F)*10 + (MdbCurrent[mid].pwrf[0]>>4),MdbCurrent[mid].pwrf[0]&0x0f,"%");
	sprintf(scrpbuf_main.buf[18], " S=%d.%dkVA", MdbAnalyze[mid].pwrv/10000,MdbAnalyze[mid].pwrv%10000);
	///sprintf(scrpbuf_main.buf[19], " Pavr=%d.%dkW", MdbAnalyze[mid].pwrav/10000,MdbAnalyze[mid].pwrav%10000);

	if(ParaMeter[mid].proto==METTYPE_ACSAMP)
    {
		sprintf(scrpbuf_main.buf[21]," ��ua=%d.%X��",((MdbCurrent[mid].phase_arc[1]&0x7F)>>4)*100 + ((MdbCurrent[mid].phase_arc[1]&0x7F)&0x0f)*10 + (MdbCurrent[mid].phase_arc[0]>>4),MdbCurrent[mid].phase_arc[0]&0x0f);
		sprintf(scrpbuf_main.buf[22]," ��ub=%d.%X��",((MdbCurrent[mid].phase_arc[3]&0x7F)>>4)*100 + ((MdbCurrent[mid].phase_arc[3]&0x7F)&0x0f)*10 + (MdbCurrent[mid].phase_arc[2]>>4),MdbCurrent[mid].phase_arc[2]&0x0f);
		sprintf(scrpbuf_main.buf[23]," ��ub=%d.%X��",((MdbCurrent[mid].phase_arc[5]&0x7F)>>4)*100 + ((MdbCurrent[mid].phase_arc[5]&0x7F)&0x0f)*10 + (MdbCurrent[mid].phase_arc[4]>>4),MdbCurrent[mid].phase_arc[4]&0x0f);	
		sprintf(scrpbuf_main.buf[24]," ��ia=%d.%X��",((MdbCurrent[mid].phase_arc[7]&0x7F)>>4)*100 + ((MdbCurrent[mid].phase_arc[7]&0x7F)&0x0f)*10 + (MdbCurrent[mid].phase_arc[6]>>4),MdbCurrent[mid].phase_arc[6]&0x0f);
		sprintf(scrpbuf_main.buf[25]," ��ib=%d.%X��",((MdbCurrent[mid].phase_arc[9]&0x7F)>>4)*100 + ((MdbCurrent[mid].phase_arc[9]&0x7F)&0x0f)*10 + (MdbCurrent[mid].phase_arc[8]>>4),MdbCurrent[mid].phase_arc[8]&0x0f);
		sprintf(scrpbuf_main.buf[26]," ��ic=%d.%X��",((MdbCurrent[mid].phase_arc[11]&0x7F)>>4)*100 + ((MdbCurrent[mid].phase_arc[11]&0x7F)&0x0f)*10 + (MdbCurrent[mid].phase_arc[10]>>4),MdbCurrent[mid].phase_arc[10]&0x0f);
		scrpbuf_main.line = 27;
	}
    else
		scrpbuf_main.line = 20;
}

typedef struct {
	unsigned char bak_head;
	unsigned char bak_end;
} bak_time_seg_t;
	
typedef struct {
    bak_time_seg_t  seg[24];
	unsigned char num;
} bak_seg_t;

static bak_seg_t bak_seg1;
/**
* @brief			�����ڵ�ʱ��κϲ���ʾ
* @param time_seg	�洢ʱ��εı���
*/
void GetTimeSeg(unsigned int time_seg)
{
	unsigned char i=0;
	unsigned char j=0;
	unsigned char k=0;
	unsigned char m=0;
	static unsigned char seg[24];

	for(i=0;i<24;i++)
	{
		bak_seg1.seg[i].bak_head=0;
		bak_seg1.seg[i].bak_end=0;
		seg[i]=0;
	}

	for(i=0;i<24;i++)
	{
		if(time_seg&(0x01<<i))
		seg[i]=0x24;
	}

	for(i=0,j=1,k=0;i<24;i++,j++)
	{
		if(seg[i]!=0)
		{
			bak_seg1.seg[k].bak_head=i;
			while(seg[i]==seg[j]&&i<24)
			{
				i++;
				j++;
			}
			bak_seg1.seg[k].bak_end=j;
			m=(bak_seg1.seg[k].bak_end-bak_seg1.seg[k].bak_head);
	
    		if(m>=1)
			k++;
		}
	
	}
	bak_seg1.num=k;
}

typedef struct {
	unsigned int time_seg_head;
	unsigned int time_seg_end;
} time_seg_1_t;

typedef struct {
    time_seg_1_t  seg[96];
	unsigned char num;
} time_seg_2_t;

static time_seg_2_t all_seg;
/**
* @brief			�����ڵ�ʱ��κϲ���ʾ
* @param period		�洢ʱ��ε�����
*/
void GetTime2(unsigned char *period)
{
	unsigned char i=0;
	unsigned char j=0;
	unsigned char k=0;
	unsigned int m=0;
	static unsigned int seg[96];
	unsigned char *str=period;
	
	for(i=0;i<96;i++)
		seg[i]=0;

	for(i=0;i<12;i++)
		for(j=0;j<8;j++)
		{
			if(str[i]&(0x01<<j))
				seg[i*8+j]=(i*8+j-1)*0x0f+0x0f;
    		else
				seg[i*8+j]=0x02;
		}

	for(i=0,j=1,k=0;i<96;i++,j++)
	{
		if(seg[i]!=0x02)
		{
			all_seg.seg[k].time_seg_head=seg[i];
			while((seg[j]-seg[i])==0x0f&&i<96)
			{
				i++;
				j++;
			}
			all_seg.seg[k].time_seg_end=seg[i]+0x0f;
			m=(all_seg.seg[k].time_seg_end-all_seg.seg[k].time_seg_head);
			if((m%15)==0)
				k++;
		}
	}
	all_seg.num=k;

	
}


/**
* @brief			�ն˲���֮ͨ�Ų���
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpTermParaCom(unsigned short mid, ULONG arg, UCHAR key)
{
	static const char *const_work_mode[3] = {"���ģʽ", "�ͻ���ģʽ", "������ģʽ"};
    static const char *const_client_mode[3] = {"  ��������ģʽ", "  ��������ģʽ", "  ʱ������ģʽ"};
	unsigned char i=0,j=0;
		unsigned long term_gprs_ipaddr = 0;
	unsigned char term_ipaddr[4];
	
	GetTimeSeg(ParaTerm.uplink.onlineflag);
	term_gprs_ipaddr = ipcp_get_addr();
	
	term_ipaddr[0] = (unsigned char)(term_gprs_ipaddr>>24)&0xff;
	term_ipaddr[1] = (unsigned char)(term_gprs_ipaddr>>16)&0xff;
	term_ipaddr[2] = (unsigned char)(term_gprs_ipaddr>>8)&0xff;
	term_ipaddr[3] = (unsigned char)term_gprs_ipaddr&0xff;
	strcpy(scrpbuf_main.buf[0], "��վIP��ַ:");
    sprintf(scrpbuf_main.buf[1], "  %d.%d.%d.%d", ParaTerm.svrip.ipmain[0], ParaTerm.svrip.ipmain[1], ParaTerm.svrip.ipmain[2], ParaTerm.svrip.ipmain[3]);
    sprintf(scrpbuf_main.buf[2], "��վ�˿ں�:%d", ParaTerm.svrip.portmain);
    strcpy(scrpbuf_main.buf[3], "��վ����IP��ַ��");
    sprintf(scrpbuf_main.buf[4], "  %d.%d.%d.%d", ParaTerm.svrip.ipbakup[0], ParaTerm.svrip.ipbakup[1], ParaTerm.svrip.ipbakup[2], ParaTerm.svrip.ipbakup[3]);
    sprintf(scrpbuf_main.buf[5], "��վ���ö˿ں�:%d", ParaTerm.svrip.portbakup);
	sprintf(scrpbuf_main.buf[6], "APN:%s", ParaTerm.svrip.apn);

    sprintf(scrpbuf_main.buf[7], "�ն������˿�:%d", ParaTerm.termip.portlisten);
    sprintf(scrpbuf_main.buf[8], "ͨ��Э��:%s", (ParaTerm.uplink.proto)?"UDP":"TCP");
    sprintf(scrpbuf_main.buf[9], "��ͨ����������:%d", ParaMixSave.mix.upflow_max);
	strcpy(scrpbuf_main.buf[10], "����ģʽ:");
	strcat(scrpbuf_main.buf[10], (char *)const_work_mode[ParaTerm.uplink.mode]);
    strcpy(scrpbuf_main.buf[11], "�ͻ�������ģʽ:");
	strcpy(scrpbuf_main.buf[12], (char *)const_client_mode[ParaTerm.uplink.clientmode]);
    sprintf(scrpbuf_main.buf[13], "�ز����:%d��", ParaTerm.uplink.timedail);
    sprintf(scrpbuf_main.buf[14], "�ز�����:%d��", ParaTerm.uplink.countdail);
    sprintf(scrpbuf_main.buf[15], "�Զ�����ʱ��:%d��", ParaTerm.uplink.timedown);
	if(bak_seg1.num != 0)
	{
		strcpy(scrpbuf_main.buf[16], "��������ʱ��:");
		for(i=0;i<bak_seg1.num;i++)
			sprintf(scrpbuf_main.buf[17+i], "  %02d:00-%02d:00",bak_seg1.seg[i].bak_head,bak_seg1.seg[i].bak_end);
	}
	else
	{
		strcpy(scrpbuf_main.buf[16], "��������ʱ��:δ����");
	}
	j=17+i;
	sprintf(scrpbuf_main.buf[j++], "��������ʱʱ��:%dms", ParaTerm.tcom.rts*20);
	sprintf(scrpbuf_main.buf[j++], "������ʱʱ��:%d��", ParaTerm.tcom.delay);
	sprintf(scrpbuf_main.buf[j++], "��ʱʱ��:%d��", ParaTerm.tcom.rsnd&0xFFF);
	sprintf(scrpbuf_main.buf[j++], "�ط�����:%d��", (ParaTerm.tcom.rsnd&(~0xCFFF))>>12);
    sprintf(scrpbuf_main.buf[j++], "�Ƿ���Ҫ��վȷ��:");
	sprintf(scrpbuf_main.buf[j++], "  1�������Զ��ϱ�:%s", (ParaTerm.tcom.flagcon&0x01)?"��":"��");
	sprintf(scrpbuf_main.buf[j++], "  2�������Զ��ϱ�:%s", (ParaTerm.tcom.flagcon&0x02)?"��":"��");
    sprintf(scrpbuf_main.buf[j++], "  3�������Զ��ϱ�:%s", (ParaTerm.tcom.flagcon&0x04)?"��":"��");
	sprintf(scrpbuf_main.buf[j++], "��������:%d��", ParaTerm.tcom.cycka);
    strcpy(scrpbuf_main.buf[j++], "��վ�绰����:");
    sprintf(scrpbuf_main.buf[j++],"  %d%d%d%d%d%d%d%d%d%d%d", 
		ParaTerm.smsc.phone[0]>>4,ParaTerm.smsc.phone[0]&0x0f,
		ParaTerm.smsc.phone[1]>>4,ParaTerm.smsc.phone[1]&0x0f,
		ParaTerm.smsc.phone[2]>>4,ParaTerm.smsc.phone[2]&0x0f,
		ParaTerm.smsc.phone[3]>>4,ParaTerm.smsc.phone[3]&0x0f,
		ParaTerm.smsc.phone[4]>>4,ParaTerm.smsc.phone[4]&0x0f,
		ParaTerm.smsc.phone[5]>>4
    );

    strcpy(scrpbuf_main.buf[j++], "�������ĺ���:");
    sprintf(scrpbuf_main.buf[j++],"  %d%d%d%d%d%d%d%d%d%d%d", 
		ParaTerm.smsc.sms[0]>>4,ParaTerm.smsc.sms[0]&0x0f,
		ParaTerm.smsc.sms[1]>>4,ParaTerm.smsc.sms[1]&0x0f,
		ParaTerm.smsc.sms[2]>>4,ParaTerm.smsc.sms[2]&0x0f,
		ParaTerm.smsc.sms[3]>>4,ParaTerm.smsc.sms[3]&0x0f,
		ParaTerm.smsc.sms[4]>>4,ParaTerm.smsc.sms[4]&0x0f,
		ParaTerm.smsc.sms[5]>>4
    );
	strcpy(scrpbuf_main.buf[j++], "�ն�IP��ַ:");
	sprintf(scrpbuf_main.buf[j++], "  %d.%d.%d.%d", ParaTerm.termip.ipterm[0], ParaTerm.termip.ipterm[1], ParaTerm.termip.ipterm[2], ParaTerm.termip.ipterm[3]);
	strcpy(scrpbuf_main.buf[j++], "���������ַ:");
	sprintf(scrpbuf_main.buf[j++], "  %d.%d.%d.%d", ParaTerm.termip.maskterm[0], ParaTerm.termip.maskterm[1], ParaTerm.termip.maskterm[2], ParaTerm.termip.maskterm[3]);
	strcpy(scrpbuf_main.buf[j++], "���ص�ַ:");
	sprintf(scrpbuf_main.buf[j++], "  %d.%d.%d.%d", ParaTerm.termip.ipgatew[0], ParaTerm.termip.ipgatew[1], ParaTerm.termip.ipgatew[2], ParaTerm.termip.ipgatew[3]);
	sprintf(scrpbuf_main.buf[j++],"�ն˵�ַ:%02X%02X-%02X%02X",ParaUni.addr_area[1],ParaUni.addr_area[0],ParaUni.addr_sn[1],ParaUni.addr_sn[0]);
	strcpy(scrpbuf_main.buf[j++], "�ն�GPRS��ȡIP��ַ:");
	sprintf(scrpbuf_main.buf[j++], "  %d.%d.%d.%d", term_ipaddr[0], term_ipaddr[1], term_ipaddr[2], term_ipaddr[3]);
	///sprintf(scrpbuf_main.buf[j++],"�ն����:%02X%02X",ParaUni.addr_sn[1],ParaUni.addr_sn[0]);
	scrpbuf_main.line = j;
}
/**
* @brief			�õ���������
* @param date		�������ڴ洢����
* @return           ���س�������
*/
unsigned int GetReadMetDate(unsigned int date)
{ 
	unsigned int i=0;
	while(date>>i)
		i++;
	return i;
}

/**
* @brief			�ն˲���֮�������
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
/*
void ScrpReadMetPara(unsigned short mid, ULONG arg, UCHAR key)
{
    unsigned char i=0,j=0;
	unsigned char *str;
	unsigned int  port_num=GetPortNum();
	const para_commport_t *port_para;
	port_para=GetParaCommPort(port_num);
	unsigned int date =GetReadMetDate(port_para->dateflag);
	
    str = (unsigned char*)port_para->period;
	GetTime2(str);
    sprintf(scrpbuf_main.buf[i++], "�ص㻧����:%d", ParaMixSave.mix.impuser.num);
    for(i=1;i<=ParaMixSave.mix.impuser.num;i++)
		sprintf(scrpbuf_main.buf[i], "�ص㻧%d������:%d",i,ParaMixSave.mix.impuser.metid[i-1]);
	sprintf(scrpbuf_main.buf[i++], "�������״̬��:%s", port_para->flag&RDMETFLAG_RDSTATUS?"��":"��");
	sprintf(scrpbuf_main.buf[i++], "�����Զ�����:%s", port_para->flag&RDMETFLAG_ENABLE?"��":"��");
    sprintf(scrpbuf_main.buf[i++], "ֻ���ص��:%s", port_para->flag&RDMETFLAG_ALL?"��":"��");
	sprintf(scrpbuf_main.buf[i++], "�㲥���᳭��:%s", port_para->flag&RDMETFLAG_FREZ?"��":"��");
	sprintf(scrpbuf_main.buf[i++], "��ʱ�㲥Уʱ:%s", port_para->flag&RDMETFLAG_CHECKTIME?"��":"��");
	sprintf(scrpbuf_main.buf[i++], "��Ѱ���:%s", port_para->flag&RDMETFLAG_FINDMET?"��":"��");
	if(all_seg.num != 0)
	{
		strcpy(scrpbuf_main.buf[i++], "������ʱ��:"); 
    	for(j=0;j<all_seg.num;j++)
			sprintf(scrpbuf_main.buf[i++], "  %02d:%02d-%02d:%02d",all_seg.seg[j].time_seg_head/60,all_seg.seg[j].time_seg_head%60,all_seg.seg[j].time_seg_end/60,all_seg.seg[j].time_seg_end%60); 
	}
	else
	{
		strcpy(scrpbuf_main.buf[i++], "������ʱ��:δ����"); 
	}
	sprintf(scrpbuf_main.buf[i++], "������:%02d��%02d:%02d", date,port_para->time_hour,port_para->time_minute);
	sprintf(scrpbuf_main.buf[i++], "����ʱ����:%d��", port_para->cycle);
	strcpy(scrpbuf_main.buf[i++], "�㲥Уʱ��ʱʱ��:");
	sprintf(scrpbuf_main.buf[i++], "  %02d��%02d:%02d", port_para->chktime_day,port_para->chktime_hour,port_para->chktime_minute);
	
	scrpbuf_main.line = i;
}
*/
/**
* @brief			1�������������
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpClass1DataTask(unsigned short mid, ULONG arg, UCHAR key)
{
    unsigned char i;
	unsigned char task_number;
	task_number=GetTaskNum()-1;
	static const char *dev_snd[4] = {"��","ʱ", "��", "��"};

    strcpy(scrpbuf_main.buf[0], "�ϱ�1������:");
	if(ParaTaskCls1[task_number].valid==TASK_RUN)
		strcat(scrpbuf_main.buf[0], "����");
	else if(ParaTaskCls1[task_number].valid==TASK_STOP)
		strcat(scrpbuf_main.buf[0], "ֹͣ");
	else 
		strcat(scrpbuf_main.buf[0], "δ����");
	sprintf(scrpbuf_main.buf[1], "��ʱ�ϱ�����:%d%s",ParaTaskCls1[task_number].dev_snd&0x3F,dev_snd[ParaTaskCls1[task_number].dev_snd>>6]);
    strcpy(scrpbuf_main.buf[2], "�ϱ���׼ʱ��:");
	sprintf(scrpbuf_main.buf[3], "  %02d��%02d��%02d��",ParaTaskCls1[task_number].base_year,ParaTaskCls1[task_number].base_month,ParaTaskCls1[task_number].base_day);	
	sprintf(scrpbuf_main.buf[4], "  %02d:%02d:%02d",ParaTaskCls1[task_number].base_hour,ParaTaskCls1[task_number].base_minute,ParaTaskCls1[task_number].base_second);
	sprintf(scrpbuf_main.buf[5], "�������ݳ�ȡ����:%d",ParaTaskCls1[task_number].freq);
	sprintf(scrpbuf_main.buf[6], "���ݵ�Ԫ��ʶ����:%d",ParaTaskCls1[task_number].num);
	
	for(i=0;i<ParaTaskCls1[task_number].num;i++)
    {
		sprintf(scrpbuf_main.buf[7+i], "���ݱ�ʶ%d:%02X%02X %02X%02X",i+1,ParaTaskCls1[task_number].duid[i].da[0],ParaTaskCls1[task_number].duid[i].da[1],
																		ParaTaskCls1[task_number].duid[i].dt[0],ParaTaskCls1[task_number].duid[i].dt[1]);
	}

	scrpbuf_main.line = 7 + i;
}
/**
* @brief			�ն˲���֮2�������������
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpClass2DataTask(unsigned short mid, ULONG arg, UCHAR key)
{
    unsigned char i;
	unsigned char task_number;
	task_number=GetTaskNum()-1;
	static const char *dev_snd[4] = {"��","ʱ", "��", "��"};
   
	strcpy(scrpbuf_main.buf[0], "�ϱ�2������:");
	if(ParaTaskCls2[task_number].valid==TASK_RUN) 
		strcat(scrpbuf_main.buf[0], "����");
    else if(ParaTaskCls2[task_number].valid==TASK_STOP)
		strcat(scrpbuf_main.buf[0], "ֹͣ");
	else 
		strcat(scrpbuf_main.buf[0], "δ����");
	sprintf(scrpbuf_main.buf[1], "��ʱ�ϱ�����:%d%s",ParaTaskCls2[task_number].dev_snd&0x3F,dev_snd[ParaTaskCls2[task_number].dev_snd>>6]);
    strcpy(scrpbuf_main.buf[2], "�ϱ���׼ʱ��:");
	sprintf(scrpbuf_main.buf[3], "  %02d��%02d��%02d��",ParaTaskCls2[task_number].base_year,ParaTaskCls2[task_number].base_month,ParaTaskCls2[task_number].base_day);	
	sprintf(scrpbuf_main.buf[4], "  %02d:%02d:%02d",ParaTaskCls2[task_number].base_hour,ParaTaskCls2[task_number].base_minute,ParaTaskCls2[task_number].base_second);
	sprintf(scrpbuf_main.buf[5], "�������ݳ�ȡ����:%d",ParaTaskCls2[task_number].freq);
	sprintf(scrpbuf_main.buf[6], "���ݵ�Ԫ��ʶ����:%d",ParaTaskCls2[task_number].num);
	for(i=0;i<ParaTaskCls2[task_number].num;i++)
    {
		sprintf(scrpbuf_main.buf[7+i], "���ݱ�ʶ%d:%02X%02X %02X%02X",i+1,ParaTaskCls2[task_number].duid[i].da[0],ParaTaskCls2[task_number].duid[i].da[1],
																		ParaTaskCls2[task_number].duid[i].dt[0],ParaTaskCls2[task_number].duid[i].dt[1]);
	}
	scrpbuf_main.line = 7+i;
}

/**
* @brief			�ն˲���֮�¼���¼����
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpEventReadCfg(unsigned short mid, ULONG arg, UCHAR key)
{
    scrpbuf_main.line = 35;
    sprintf(scrpbuf_main.buf[0], "1.�汾���%s %s", ((ParaTerm.almflag.valid[0]&0x01)?"Y":"N"),(ParaTerm.almflag.rank[0]&0x01)?"I":"");
    sprintf(scrpbuf_main.buf[1], "2.������ʧ%s %s", ((ParaTerm.almflag.valid[0]&(0x01<<1))?"Y":"N"),(ParaTerm.almflag.rank[0]&(0x01<<1))?"I":"");
    sprintf(scrpbuf_main.buf[2], "3.�������%s %s", ((ParaTerm.almflag.valid[0]&(0x01<<2))?"Y":"N"),(ParaTerm.almflag.rank[0]&(0x01<<2))?"I":"");
    sprintf(scrpbuf_main.buf[3], "4.״̬����λ%s %s", ((ParaTerm.almflag.valid[0]&(0x01<<3))?"Y":"N"),(ParaTerm.almflag.rank[0]&(0x01<<3))?"I":"");
    sprintf(scrpbuf_main.buf[4], "5.ң����բ%s %s", ((ParaTerm.almflag.valid[0]&(0x01<<4))?"Y":"N"),(ParaTerm.almflag.rank[0]&(0x01<<4))?"I":"");
    sprintf(scrpbuf_main.buf[5], "6.������բ%s %s", ((ParaTerm.almflag.valid[0]&(0x01<<5))?"Y":"N"),(ParaTerm.almflag.rank[0]&(0x01<<5))?"I":"");
    sprintf(scrpbuf_main.buf[6], "7.�����բ%s %s", ((ParaTerm.almflag.valid[0]&(0x01<<6))?"Y":"N"),(ParaTerm.almflag.rank[0]&(0x01<<6))?"I":"");
    sprintf(scrpbuf_main.buf[7], "8.���������%s %s", ((ParaTerm.almflag.valid[0]&(0x01<<7))?"Y":"N"),(ParaTerm.almflag.rank[0]&(0x01<<7))?"I":"");
    sprintf(scrpbuf_main.buf[8], "9.������·�쳣%s %s", ((ParaTerm.almflag.valid[1]&0x01)?"Y":"N"),(ParaTerm.almflag.rank[1]&0x01)?"I":"");
    sprintf(scrpbuf_main.buf[9], "10.��ѹ��·�쳣%s %s", ((ParaTerm.almflag.valid[1]&(0x01<<1))?"Y":"N"),(ParaTerm.almflag.rank[1]&(0x01<<1))?"I":"");
    sprintf(scrpbuf_main.buf[10], "11.�����쳣%s %s", ((ParaTerm.almflag.valid[1]&(0x01<<2))?"Y":"N"),(ParaTerm.almflag.rank[1]&(0x01<<2))?"I":"");
    sprintf(scrpbuf_main.buf[11], "12.���ʱ�䳬��%s %s", ((ParaTerm.almflag.valid[1]&(0x01<<3))?"Y":"N"),(ParaTerm.almflag.rank[1]&(0x01<<3))?"I":"");
    sprintf(scrpbuf_main.buf[12], "13.��������Ϣ%s %s", ((ParaTerm.almflag.valid[1]&(0x01<<4))?"Y":"N"),(ParaTerm.almflag.rank[1]&(0x01<<4))?"I":"");
    sprintf(scrpbuf_main.buf[13], "14.�ն�ͣ/�ϵ�%s %s", ((ParaTerm.almflag.valid[1]&(0x01<<5))?"Y":"N"),(ParaTerm.almflag.rank[1]&(0x01<<5))?"I":"");
    sprintf(scrpbuf_main.buf[14], "15.г��Խ�޸澯%s %s", ((ParaTerm.almflag.valid[1]&(0x01<<6))?"Y":"N"),(ParaTerm.almflag.rank[1]&(0x01<<6))?"I":"");
    sprintf(scrpbuf_main.buf[15], "16.ֱ��ģ����Խ��%s %s", ((ParaTerm.almflag.valid[1]&(0x01<<7))?"Y":"N"),(ParaTerm.almflag.rank[1]&(0x01<<7))?"I":"");
    sprintf(scrpbuf_main.buf[16], "17.ѹ����ƽ��Խ��%s %s", ((ParaTerm.almflag.valid[2]&0x01)?"Y":"N"),(ParaTerm.almflag.rank[2]&0x01)?"I":"");
    sprintf(scrpbuf_main.buf[17], "18.������Ͷ������%s %s", ((ParaTerm.almflag.valid[2]&(0x01<<1))?"Y":"N"),(ParaTerm.almflag.rank[2]&(0x01<<1))?"I":"");
    sprintf(scrpbuf_main.buf[18], "19.�����������%s %s", ((ParaTerm.almflag.valid[2]&(0x01<<2))?"Y":"N"),(ParaTerm.almflag.rank[2]&(0x01<<2))?"I":"");
    sprintf(scrpbuf_main.buf[19], "20.��Ϣ��֤����%s %s", ((ParaTerm.almflag.valid[2]&(0x01<<3))?"Y":"N"),(ParaTerm.almflag.rank[2]&(0x01<<3))?"I":"");
    sprintf(scrpbuf_main.buf[20], "21.�ն˹���%s %s", ((ParaTerm.almflag.valid[2]&(0x01<<4))?"Y":"N"),(ParaTerm.almflag.rank[2]&(0x01<<4))?"I":"");
    sprintf(scrpbuf_main.buf[21], "22.�й��ܲԽ��%s %s", ((ParaTerm.almflag.valid[2]&(0x01<<5))?"Y":"N"),(ParaTerm.almflag.rank[2]&(0x01<<5))?"I":"");
    sprintf(scrpbuf_main.buf[22], "23.��ظ澯�¼�%s %s", ((ParaTerm.almflag.valid[2]&(0x01<<6))?"Y":"N"),(ParaTerm.almflag.rank[2]&(0x01<<6))?"I":"");
    sprintf(scrpbuf_main.buf[23], "24.��ѹԽ��%s %s", ((ParaTerm.almflag.valid[2]&(0x01<<7))?"Y":"N"),(ParaTerm.almflag.rank[2]&(0x01<<7))?"I":"");
    sprintf(scrpbuf_main.buf[24], "25.����Խ��%s %s", ((ParaTerm.almflag.valid[3]&0x01)?"Y":"N"),(ParaTerm.almflag.rank[3]&0x01)?"I":"");
    sprintf(scrpbuf_main.buf[25], "26.���ڹ���Խ��%s %s", ((ParaTerm.almflag.valid[3]&(0x01<<1))?"Y":"N"),(ParaTerm.almflag.rank[3]&(0x01<<1))?"I":"");
    sprintf(scrpbuf_main.buf[26], "27.���ܱ�ʾ���½�%s %s", ((ParaTerm.almflag.valid[3]&(0x01<<2))?"Y":"N"),(ParaTerm.almflag.rank[3]&(0x01<<2))?"I":"");
    sprintf(scrpbuf_main.buf[27], "28.����������%s %s", ((ParaTerm.almflag.valid[3]&(0x01<<3))?"Y":"N"),(ParaTerm.almflag.rank[3]&(0x01<<3))?"I":"");
    sprintf(scrpbuf_main.buf[28], "29.���ܱ����%s %s", ((ParaTerm.almflag.valid[3]&(0x01<<4))?"Y":"N"),(ParaTerm.almflag.rank[3]&(0x01<<4))?"I":"");
    sprintf(scrpbuf_main.buf[29], "30.���ܱ�ͣ��%s %s", ((ParaTerm.almflag.valid[3]&(0x01<<5))?"Y":"N"),(ParaTerm.almflag.rank[3]&(0x01<<5))?"I":"");
    sprintf(scrpbuf_main.buf[30], "31.485����ʧ��%s %s", ((ParaTerm.almflag.valid[3]&(0x01<<6))?"Y":"N"),(ParaTerm.almflag.rank[3]&(0x01<<6))?"I":"");
    sprintf(scrpbuf_main.buf[31], "32.ͨ������������%s %s", ((ParaTerm.almflag.valid[3]&(0x01<<7))?"Y":"N"),(ParaTerm.almflag.rank[3]&(0x01<<7))?"I":"");
    sprintf(scrpbuf_main.buf[32], "33.����״̬�ֱ�λ%s %s", ((ParaTerm.almflag.valid[4]&0x01)?"Y":"N"),(ParaTerm.almflag.rank[4]&0x01)?"I":"");
    sprintf(scrpbuf_main.buf[33], "34.CT�쳣%s %s", ((ParaTerm.almflag.valid[4]&(0x01<<1))?"Y":"N"),(ParaTerm.almflag.rank[4]&(0x01<<1))?"I":"");
    sprintf(scrpbuf_main.buf[34], "35.����δ֪���%s %s", ((ParaTerm.almflag.valid[4]&(0x01<<2))?"Y":"N"),(ParaTerm.almflag.rank[4]&(0x01<<2))?"I":"");
}


/**
* @brief			�ն˲���֮�˿ڲ���
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpReadMetPara(unsigned short mid, ULONG arg, UCHAR key)
{
    unsigned char i=0,j=0;
	unsigned char *str;
	const para_commport_t *port_para;
	port_para=GetParaCommPort(arg);
	unsigned int date =GetReadMetDate(port_para->dateflag);
	
    str = (unsigned char*)port_para->period;
	GetTime2(str);

	PrintLog(0, "port_para->frame= %d.\r\n", port_para->frame);
    sprintf(scrpbuf_main.buf[i++], "������:%d",port_para->baudrate);
	sprintf(scrpbuf_main.buf[i++], "ֹͣλ:%s",port_para->frame&COMMFRAME_STOPBIT_2?"2":"1");
	sprintf(scrpbuf_main.buf[i++], "����У��:%s",port_para->frame&COMMFRAME_HAVECHECK?"��":"��");
	sprintf(scrpbuf_main.buf[i++], "��żУ��:%s",port_para->frame&COMMFRAME_EVENCHECK?"��":"ż");
	sprintf(scrpbuf_main.buf[i++], "����λ:%d",(port_para->frame&COMMFRAME_DATA)+5);
	sprintf(scrpbuf_main.buf[i++], "�������״̬��:%s", port_para->flag&RDMETFLAG_RDSTATUS?"��":"��");
	sprintf(scrpbuf_main.buf[i++], "�����Զ�����:%s", port_para->flag&RDMETFLAG_ENABLE?"��":"��");
    sprintf(scrpbuf_main.buf[i++], "ֻ���ص��:%s", port_para->flag&RDMETFLAG_ALL?"��":"��");
	sprintf(scrpbuf_main.buf[i++], "�㲥���᳭��:%s", port_para->flag&RDMETFLAG_FREZ?"��":"��");
	sprintf(scrpbuf_main.buf[i++], "��ʱ�㲥Уʱ:%s", port_para->flag&RDMETFLAG_CHECKTIME?"��":"��");
	sprintf(scrpbuf_main.buf[i++], "��Ѱ���:%s", port_para->flag&RDMETFLAG_FINDMET?"��":"��");
	if(all_seg.num != 0)
	{
		strcpy(scrpbuf_main.buf[i++], "������ʱ��:"); 
    	for(j=0;j<all_seg.num;j++)
			sprintf(scrpbuf_main.buf[i++], "  %02d:%02d-%02d:%02d",all_seg.seg[j].time_seg_head/60,all_seg.seg[j].time_seg_head%60,all_seg.seg[j].time_seg_end/60,all_seg.seg[j].time_seg_end%60); 
	}
	else
	{
		strcpy(scrpbuf_main.buf[i++], "������ʱ��:δ����"); 
	}
	sprintf(scrpbuf_main.buf[i++], "������:%02d��%02d:%02d", date,port_para->time_hour,port_para->time_minute);
	sprintf(scrpbuf_main.buf[i++], "����ʱ����:%d��", port_para->cycle);
	strcpy(scrpbuf_main.buf[i++], "�㲥Уʱ��ʱʱ��:");
	sprintf(scrpbuf_main.buf[i++], "  %02d��%02d:%02d", port_para->chktime_day,port_para->chktime_hour,port_para->chktime_minute);
    sprintf(scrpbuf_main.buf[i++], "�ص㻧����:%d", ParaMixSave.mix.impuser.num);
	for(j=1;j<=ParaMixSave.mix.impuser.num;j++)
		sprintf(scrpbuf_main.buf[i++], "�ص㻧%d������:%d",j,ParaMixSave.mix.impuser.metid[j-1]);

	scrpbuf_main.line = i;
}

/**
* @brief			�ն˲���֮��������
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpCascadePara(unsigned short mid, ULONG arg, UCHAR key)
{
	static const char *const_baud[8] = {"300","600", "1200", "2400","4800", "7200", "9600","19200"};
    unsigned char i = 0,j = 0,k = 0;

	PrintLog(0, "frame= %d.\r\n", ParaMixSave.mix.cascade.frame);
	sprintf(scrpbuf_main.buf[0], "�˿ں�:%d",ParaMixSave.mix.cascade.port);
    sprintf(scrpbuf_main.buf[1], "������:%s",const_baud[ParaMixSave.mix.cascade.frame>>5]);
	sprintf(scrpbuf_main.buf[2], "ֹͣλ:%s",ParaMixSave.mix.cascade.frame&COMMFRAME_STOPBIT_2?"2":"1");
	sprintf(scrpbuf_main.buf[3], "����У��:%s",ParaMixSave.mix.cascade.frame&COMMFRAME_HAVECHECK?"��":"��");
	sprintf(scrpbuf_main.buf[4], "��żУ��:%s",ParaMixSave.mix.cascade.frame&COMMFRAME_EVENCHECK?"��":"ż");
	sprintf(scrpbuf_main.buf[5], "����λ:%d",(ParaMixSave.mix.cascade.frame&COMMFRAME_DATA)+5);
	sprintf(scrpbuf_main.buf[6], "���ĳ�ʱʱ��:%dms",(ParaMixSave.mix.cascade.timeout*100));
	sprintf(scrpbuf_main.buf[7], "�ֽڳ�ʱʱ��:%dms",(ParaMixSave.mix.cascade.timeout_byte*10));
	sprintf(scrpbuf_main.buf[8], "�ط�����:%d��",ParaMixSave.mix.cascade.retry);
	sprintf(scrpbuf_main.buf[9], "����Ѳ������:%d��",ParaMixSave.mix.cascade.cycle);
	sprintf(scrpbuf_main.buf[10], "������־:%s",ParaMixSave.mix.cascade.flag?"��������":"����վ");
	sprintf(scrpbuf_main.buf[11], "�����ն˸���:%d��",ParaMixSave.mix.cascade.num);
	for(i = 0,j = 0,k = 0;j < ParaMixSave.mix.cascade.num;j++,i++)
	{
		sprintf(scrpbuf_main.buf[11 + (++k)], "�����ն�%d��ַ:",i + 1);
		sprintf(scrpbuf_main.buf[11 + (++k)], "  %02X%02X%02X%02X",ParaMixSave.mix.cascade.addr[j + 1],ParaMixSave.mix.cascade.addr[j],ParaMixSave.mix.cascade.addr[j + 3],ParaMixSave.mix.cascade.addr[j + 2]);
	}
	scrpbuf_main.line = 13 + j * 2;
}

/**
* @brief			�ܱ����֮��������
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpMajorMetBasePara(unsigned short mid, ULONG arg, UCHAR key)
{
#define PWRTYPE  0x03

    //unsigned int  major_met_number=GetMajorMetNum()-1;
	static const char *const_baud[8] = {"��������","600", "1200", "2400","4800", "7200", "9600","19200"};
	static const char *user_class[6] = {"  ����ר���û�","  ��С��ר���û�", "  ��ѹ���๤��ҵ�û�", "  ��ѹ���๤��ҵ�û�","  �����û�", "  ���俼�˼�����"};
    unsigned char i,j;
	unsigned char displayno[14];
	int scr_cen_meter_cnt;


	scr_cen_meter_cnt = get_cen_meter_total_cnt();
	if(!scr_cen_meter_cnt)
	{
		scrpbuf_main.line = 2;
		sprintf(scrpbuf_main.buf[1],"    δ�����ܱ�!");	
		sprintf(scrpbuf_main.buf[2],"    ��ESC������!");	
		return;
	}

    //if(major_met_number!=0)
    //{
	//	if(ParaMeter[major_met_number].metp_id == 0)
	//	{
	//		scrpbuf_main.line = 2;
	//		sprintf(scrpbuf_main.buf[1],"δ���ø���ŵı��!");	
	//		sprintf(scrpbuf_main.buf[2],"    ��ESC������!");	
	//		return;
	//	}
   // }



	scrpbuf_main.line = 26;
    ///sprintf(scrpbuf_main.buf[0], "װ�����:%d", CEN_METER_NUMBER + 1);
	sprintf(scrpbuf_main.buf[0], "�����������:%d",ParaMeter[CEN_METER_NUMBER].metp_id);
    sprintf(scrpbuf_main.buf[1], "ͨ������:%s", const_baud[ParaMeter[CEN_METER_NUMBER].portcfg>>5]);
	sprintf(scrpbuf_main.buf[2], "ͨ�Ŷ˿ں�:%d", ParaMeter[CEN_METER_NUMBER].portcfg&METPORT_MASK);
    strcpy(scrpbuf_main.buf[3], "ͨ�Ź�Լ����:");
	switch(ParaMeter[CEN_METER_NUMBER].proto)
    {
	case 0:
		strcpy(scrpbuf_main.buf[4], "  ������г���");
		break;
	case METTYPE_DL645:
		strcpy(scrpbuf_main.buf[4], "  DL/T645-1997��Լ");
		break;
	case METTYPE_ACSAMP:
		strcpy(scrpbuf_main.buf[4], "  ��������ͨ��Э��");
		break;
	case METTYPE_DL645_2007:
		strcpy(scrpbuf_main.buf[4], "  DL/T645-2007��Լ");
		break;
	case METTYPE_PLC:
		strcpy(scrpbuf_main.buf[4], "  ���нӿ�խ��Э��");
		break;
	default: break;
    }
	strcpy(scrpbuf_main.buf[5], "ͨ�ŵ�ַ:");
    sprintf(scrpbuf_main.buf[6], "  %02X%02X%02X%02X%02X%02X",
    ParaMeter[CEN_METER_NUMBER].addr[5],
    ParaMeter[CEN_METER_NUMBER].addr[4],
    ParaMeter[CEN_METER_NUMBER].addr[3],
    ParaMeter[CEN_METER_NUMBER].addr[2],
    ParaMeter[CEN_METER_NUMBER].addr[1],
    ParaMeter[CEN_METER_NUMBER].addr[0]
	);
	sprintf(scrpbuf_main.buf[7], "ͨ������:%d%d%d%d%d%d", ParaMeter[CEN_METER_NUMBER].pwd[0],ParaMeter[CEN_METER_NUMBER].pwd[1],
		ParaMeter[CEN_METER_NUMBER].pwd[2],ParaMeter[CEN_METER_NUMBER].pwd[3],ParaMeter[CEN_METER_NUMBER].pwd[4],ParaMeter[CEN_METER_NUMBER].pwd[5]);
	sprintf(scrpbuf_main.buf[8], "���ܷ��ʸ���:%d", ParaMeter[CEN_METER_NUMBER].prdnum&METPRDNUM_MASK);

	sprintf(scrpbuf_main.buf[9], "�й���������λ����:%d", (ParaMeter[CEN_METER_NUMBER].intdotnum&INTNUM_MASK)+4);
	sprintf(scrpbuf_main.buf[10], "�й�����С��λ����:%d", (ParaMeter[CEN_METER_NUMBER].intdotnum&DOTNUM_MASK)+1);
	strcpy(scrpbuf_main.buf[11], "�û������:");
	sprintf(scrpbuf_main.buf[12], "%s",user_class[(ParaMeter[CEN_METER_NUMBER].userclass>>4)]);
    sprintf(scrpbuf_main.buf[13], "�������:%d",ParaMeter[CEN_METER_NUMBER].userclass&0x0f);
    sprintf(scrpbuf_main.buf[14], "��ѹ����������:%d",ParaCenMetp[CEN_METER_NUMBER].base.pt);
    sprintf(scrpbuf_main.buf[15], "��������������:%d",ParaCenMetp[CEN_METER_NUMBER].base.ct);
	sprintf(scrpbuf_main.buf[16], "���ѹ:%d.%dV",ParaCenMetp[CEN_METER_NUMBER].base.vol_rating/10,(ParaCenMetp[CEN_METER_NUMBER].base.vol_rating%100)%10);
	sprintf(scrpbuf_main.buf[17], "�����:%d.%dA",ParaCenMetp[CEN_METER_NUMBER].base.amp_rating/100,ParaCenMetp[CEN_METER_NUMBER].base.amp_rating%100);
	sprintf(scrpbuf_main.buf[18], "�����:%d.%dkVA",ParaCenMetp[CEN_METER_NUMBER].base.pwr_rating/10000,ParaCenMetp[CEN_METER_NUMBER].base.pwr_rating%10000);
    strcpy(scrpbuf_main.buf[19], "��Դ���߷�ʽ:");
	switch(ParaCenMetp[CEN_METER_NUMBER].base.pwrtype&PWRTYPE)
	{
	case 1:
		strcpy(scrpbuf_main.buf[20], "  ��������");
		break;
	case 2:
		strcpy(scrpbuf_main.buf[20], "  ��������");
		break;
	case 3:
    	if(((ParaCenMetp[CEN_METER_NUMBER].base.pwrtype&PWRPHASE)>>2)==0)
			strcpy(scrpbuf_main.buf[20], "  ���಻ȷ��");
		else if(((ParaCenMetp[CEN_METER_NUMBER].base.pwrtype&PWRPHASE)>>2)==1)
			strcpy(scrpbuf_main.buf[20], "  ����A");
		else if(((ParaCenMetp[CEN_METER_NUMBER].base.pwrtype&PWRPHASE)>>2)==2)
			strcpy(scrpbuf_main.buf[20], "  ����B");
		else if(((ParaCenMetp[CEN_METER_NUMBER].base.pwrtype&PWRPHASE)>>2)==3)
			strcpy(scrpbuf_main.buf[20], "  ����C");
		break;
	default: break;
   	}
	
	sprintf(scrpbuf_main.buf[21], "����ͣͶ����:%s",ParaCenMetp[CEN_METER_NUMBER].stopped&0x01?"ͣ��":"Ͷ��");
	sprintf(scrpbuf_main.buf[22], "���ص��ܱ���ʾ��:");
	for(i = 0,j = 2;i < 12;i++,j++)
		displayno[j] = ParaCenMetp[CEN_METER_NUMBER].displayno[i];
	displayno[0] = displayno[1] = 32;         ///�γɿո�
	memcpy(scrpbuf_main.buf[23],displayno,14);
	sprintf(scrpbuf_main.buf[24], "����������ֵ1:%d.%d",(ParaCenMetp[CEN_METER_NUMBER].pwrf.limit1)/10,((ParaCenMetp[CEN_METER_NUMBER].pwrf.limit1)%100)%10);
	strcat(scrpbuf_main.buf[24], "%");
	sprintf(scrpbuf_main.buf[25], "����������ֵ2:%d.%d",(ParaCenMetp[CEN_METER_NUMBER].pwrf.limit2)/10,((ParaCenMetp[CEN_METER_NUMBER].pwrf.limit2)%100)%10);
	strcat(scrpbuf_main.buf[25], "%");
}

/**
* @brief			�ܱ����֮��ֵ����
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpMajorMetLmtPara(unsigned short mid, ULONG arg, UCHAR key)
{
	scrpbuf_main.line = 36;
    //unsigned int  major_met_number=GetMajorMetNum()-1;

	int scr_cen_meter_cnt;


	scr_cen_meter_cnt = get_cen_meter_total_cnt();
	if(!scr_cen_meter_cnt)
	{
		scrpbuf_main.line = 2;
		sprintf(scrpbuf_main.buf[1],"    δ�����ܱ�!");	
		sprintf(scrpbuf_main.buf[2],"    ��ESC������!");	
		return;
	}

	

	//if(ParaMeter[major_met_number].metp_id == 0)
	//{
	//	scrpbuf_main.line = 2;
	//	sprintf(scrpbuf_main.buf[1],"δ���ø���ŵı��!");	
	//	sprintf(scrpbuf_main.buf[2],"    ��ESC������!");	
	//	return;
	//}
		
	sprintf(scrpbuf_main.buf[0], "��ѹ�ϸ�����:%d.%dV", ParaCenMetp[CEN_METER_NUMBER].limit.volok_up/10,(ParaCenMetp[CEN_METER_NUMBER].limit.volok_up%100)%10);
	sprintf(scrpbuf_main.buf[1], "��ѹ�ϸ�����:%d.%dV", ParaCenMetp[CEN_METER_NUMBER].limit.volok_low/10,(ParaCenMetp[CEN_METER_NUMBER].limit.volok_low%100)%10);
	sprintf(scrpbuf_main.buf[2], "��ѹ��������:%d.%dV", ParaCenMetp[CEN_METER_NUMBER].limit.vol_lack/10,(ParaCenMetp[CEN_METER_NUMBER].limit.vol_lack%100)%10);

	sprintf(scrpbuf_main.buf[3], "��ѹ������:%d.%dV", ParaCenMetp[CEN_METER_NUMBER].limit.vol_over/10,(ParaCenMetp[CEN_METER_NUMBER].limit.vol_over%100)%10);
	sprintf(scrpbuf_main.buf[4], "Խ�޳���ʱ��:%d��", ParaCenMetp[CEN_METER_NUMBER].limit.time_volover);
	sprintf(scrpbuf_main.buf[5], "Խ�޻ָ���ѹ:%d.%dV", ParaCenMetp[CEN_METER_NUMBER].limit.restore_volover/10,(ParaCenMetp[CEN_METER_NUMBER].limit.restore_volover%100)%10);

	sprintf(scrpbuf_main.buf[6], "��ѹ������:%d.%dV", ParaCenMetp[CEN_METER_NUMBER].limit.vol_less/10,(ParaCenMetp[CEN_METER_NUMBER].limit.vol_less%100)%10);
	sprintf(scrpbuf_main.buf[7], "Խ�޳���ʱ��:%d��", ParaCenMetp[CEN_METER_NUMBER].limit.time_volless);
	sprintf(scrpbuf_main.buf[8], "Խ�޻ָ���ѹ:%d.%dV", ParaCenMetp[CEN_METER_NUMBER].limit.restore_volless/10,(ParaCenMetp[CEN_METER_NUMBER].limit.restore_volless%100)%10);

	sprintf(scrpbuf_main.buf[9], "��������:%d.%dA", ParaCenMetp[CEN_METER_NUMBER].limit.amp_over/100,(ParaCenMetp[CEN_METER_NUMBER].limit.amp_over%100));
	sprintf(scrpbuf_main.buf[10], "Խ�޳���ʱ��:%d��", ParaCenMetp[CEN_METER_NUMBER].limit.time_ampover);
	sprintf(scrpbuf_main.buf[11], "Խ�޻ָ�����:%d.%dA", ParaCenMetp[CEN_METER_NUMBER].limit.restore_ampover/100,(ParaCenMetp[CEN_METER_NUMBER].limit.restore_ampover%100));

	sprintf(scrpbuf_main.buf[12], "���������:%d.%dA", ParaCenMetp[CEN_METER_NUMBER].limit.amp_limit/100,(ParaCenMetp[CEN_METER_NUMBER].limit.amp_limit%100));
	sprintf(scrpbuf_main.buf[13], "Խ�޳���ʱ��:%d��", ParaCenMetp[CEN_METER_NUMBER].limit.time_amplimit);
	sprintf(scrpbuf_main.buf[14], "Խ�޻ָ�����:%d.%dA", ParaCenMetp[CEN_METER_NUMBER].limit.restore_amplimit/100,(ParaCenMetp[CEN_METER_NUMBER].limit.restore_amplimit%100));

	sprintf(scrpbuf_main.buf[15], "�����������:%d.%dA", ParaCenMetp[CEN_METER_NUMBER].limit.zamp_limit/100,(ParaCenMetp[CEN_METER_NUMBER].limit.zamp_limit%100));
	sprintf(scrpbuf_main.buf[16], "Խ�޳���ʱ��:%d��", ParaCenMetp[CEN_METER_NUMBER].limit.time_zamp);
	sprintf(scrpbuf_main.buf[17], "Խ�޻ָ�����:%d.%dA", ParaCenMetp[CEN_METER_NUMBER].limit.restore_zamp/100,(ParaCenMetp[CEN_METER_NUMBER].limit.restore_zamp%100));

	sprintf(scrpbuf_main.buf[18], "���ڹ�������:");
	sprintf(scrpbuf_main.buf[19], "  %d.%dkVA", ParaCenMetp[CEN_METER_NUMBER].limit.pwr_over/10000,(ParaCenMetp[CEN_METER_NUMBER].limit.pwr_over%10000));
	sprintf(scrpbuf_main.buf[20], "Խ�޳���ʱ��:%d��", ParaCenMetp[CEN_METER_NUMBER].limit.time_pwrover);
	sprintf(scrpbuf_main.buf[21], "Խ�޻ָ�����:");
	sprintf(scrpbuf_main.buf[22], "  %d.%dkVA", ParaCenMetp[CEN_METER_NUMBER].limit.restore_pwrover/10000,(ParaCenMetp[CEN_METER_NUMBER].limit.restore_pwrover%10000));

	sprintf(scrpbuf_main.buf[23], "���ڹ�������:");
	sprintf(scrpbuf_main.buf[24], "  %d.%dkVA", ParaCenMetp[CEN_METER_NUMBER].limit.pwr_limit/10000,(ParaCenMetp[CEN_METER_NUMBER].limit.pwr_limit%10000));
	sprintf(scrpbuf_main.buf[25], "Խ�޳���ʱ��:%d��", ParaCenMetp[CEN_METER_NUMBER].limit.time_pwrlimit);
	sprintf(scrpbuf_main.buf[26], "Խ�޻ָ�����:");
	sprintf(scrpbuf_main.buf[27], "  %d.%dkVA", ParaCenMetp[CEN_METER_NUMBER].limit.restore_pwrlimit/10000,(ParaCenMetp[CEN_METER_NUMBER].limit.restore_pwrlimit%10000));

	strcpy(scrpbuf_main.buf[28], "�����ѹ��ƽ����ֵ:");
	sprintf(scrpbuf_main.buf[29], "  %d.%dV", ParaCenMetp[CEN_METER_NUMBER].limit.vol_unb/10,(ParaCenMetp[CEN_METER_NUMBER].limit.vol_unb%100)%10);
	sprintf(scrpbuf_main.buf[30], "Խ�޳���ʱ��:%d��", ParaCenMetp[CEN_METER_NUMBER].limit.time_volunb);
	sprintf(scrpbuf_main.buf[31], "Խ�޻ָ���ѹ:%d.%dV", ParaCenMetp[CEN_METER_NUMBER].limit.restore_volunb/10,(ParaCenMetp[CEN_METER_NUMBER].limit.restore_volunb%100)%10);

	strcpy(scrpbuf_main.buf[32], "���������ƽ����ֵ:");
	sprintf(scrpbuf_main.buf[33], "  %d.%dV", ParaCenMetp[CEN_METER_NUMBER].limit.amp_unb/10,(ParaCenMetp[CEN_METER_NUMBER].limit.amp_unb%100)%10);
	sprintf(scrpbuf_main.buf[34], "Խ�޳���ʱ��:%d��", ParaCenMetp[CEN_METER_NUMBER].limit.time_ampunb);
	sprintf(scrpbuf_main.buf[35], "Խ�޻ָ�����:%d.%dV", ParaCenMetp[CEN_METER_NUMBER].limit.restore_ampunb/10,(ParaCenMetp[CEN_METER_NUMBER].limit.restore_ampunb%100)%10);
}

/**
* @brief			�ز������
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpUserMetPara (unsigned short mid, ULONG arg, UCHAR key)
{
	static const char *const_baud[8] = {"��������","600", "1200", "2400","4800", "7200", "9600","19200"};
	static const char *user_class[6] = {"����ר���û�","��С��ר���û�", "��ѹ���๤��ҵ�û�", "��ѹ���๤��ҵ�û�","�����û�", "���俼�˼�����"};
    	unsigned int  sn_number = GetSnNum() - 1;

	if(sn_number<2||sn_number>2039)
	{
		scrpbuf_main.line = 1;
		sprintf(scrpbuf_main.buf[0],"������ų�����Χ!");	
		return;
	}
	if(ParaMeter[sn_number].metp_id == 0)
	{
		scrpbuf_main.line = 2;
		sprintf(scrpbuf_main.buf[1],"δ���ø���ŵı��!");	
		sprintf(scrpbuf_main.buf[2],"    ��ESC������!");	
		return;
	}

    sprintf(scrpbuf_main.buf[0], "�����������:%d", ParaMeter[sn_number].metp_id);
    sprintf(scrpbuf_main.buf[1], "ͨ������:%s", const_baud[ParaMeter[sn_number].portcfg>>5]);
	sprintf(scrpbuf_main.buf[2], "ͨ�Ŷ˿ں�:%d", ParaMeter[sn_number].portcfg&METPORT_MASK);

	strcpy(scrpbuf_main.buf[3], "ͨ�Ź�Լ����:");
	switch(ParaMeter[sn_number].proto)
    {
	case 0:
		strcpy(scrpbuf_main.buf[4], "  ������г���");
		break;
	case METTYPE_DL645:
		strcpy(scrpbuf_main.buf[4], "  DL/T645-1997��Լ");
		break;
	case METTYPE_ACSAMP:
		strcpy(scrpbuf_main.buf[4], "  ��������װ��ͨ��Э��");
		break;
	case METTYPE_DL645_2007:
		strcpy(scrpbuf_main.buf[4], "  DL/T645-2007��Լ");
		break;
	case METTYPE_PLC:
		strcpy(scrpbuf_main.buf[4], "  �����ز�ͨ��Э��");
		break;
	default: break;
    }
	strcpy(scrpbuf_main.buf[5], "ͨ�ŵ�ַ:");
	sprintf(scrpbuf_main.buf[6], "  %02X%02X%02X%02X%02X%02X", 
		ParaMeter[sn_number].addr[5],
		ParaMeter[sn_number].addr[4],
		ParaMeter[sn_number].addr[3],
		ParaMeter[sn_number].addr[2],
		ParaMeter[sn_number].addr[1],
		ParaMeter[sn_number].addr[0]
	);

	strcpy(scrpbuf_main.buf[7], "�����ɼ�����ַ:");
	sprintf(scrpbuf_main.buf[8], "  %02X%02X%02X%02X%02X%02X", 
		ParaMeter[sn_number].owneraddr[5],
		ParaMeter[sn_number].owneraddr[4],
		ParaMeter[sn_number].owneraddr[3],
		ParaMeter[sn_number].owneraddr[2],
		ParaMeter[sn_number].owneraddr[1],
		ParaMeter[sn_number].owneraddr[0]
	);
	sprintf(scrpbuf_main.buf[9], "ͨ������:%d%d%d%d%d%d", ParaMeter[sn_number].pwd[0],ParaMeter[sn_number].pwd[1],ParaMeter[sn_number].pwd[2],ParaMeter[sn_number].pwd[3],ParaMeter[sn_number].pwd[4],ParaMeter[sn_number].pwd[5]);
    sprintf(scrpbuf_main.buf[10], "���ܷ��ʸ���:%d", ParaMeter[sn_number].prdnum&METPRDNUM_MASK);
	sprintf(scrpbuf_main.buf[11], "�й���������λ����:%d", ((ParaMeter[sn_number].intdotnum&INTNUM_MASK)>>2)+4);
	sprintf(scrpbuf_main.buf[12], "�й�����С��λ����:%d", (ParaMeter[sn_number].intdotnum&DOTNUM_MASK)+1);
	strcpy(scrpbuf_main.buf[14], "�û������:");
	if(ParaMeter[sn_number].userclass >= 1&&ParaMeter[sn_number].userclass <= 8)
	sprintf(scrpbuf_main.buf[15], "  %s",user_class[(ParaMeter[sn_number].userclass)-1]);
	else 
	strcpy(scrpbuf_main.buf[15], "  δ����");
	sprintf(scrpbuf_main.buf[16], "�������:%d",(ParaMeter[sn_number].metclass)&0x0f);
    sprintf(scrpbuf_main.buf[17], "����ͣͶ����:%s",ParaPlcMetp[sn_number].stopped&0x01?"ͣ��":"Ͷ��");
	//strcpy(scrpbuf_main.buf[18], "���ص��ܱ���ʾ��:");
	//for(i = 0,j = 2;i < 12;i++,j++)
		//displayno[j] = ParaPlcMetp[sn_number].displayno[i];
	//displayno[0] = displayno[1] = 32;         ///�γɿո�
	//memcpy(scrpbuf_main.buf[19],displayno,14);
		scrpbuf_main.line = 18;
}

/**
* @brief			�ն�������Ϣ֮�汾��Ϣ
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpVesionInfo(unsigned short mid, ULONG arg, UCHAR key)
{
	unsigned char VesionInfo[80];
	unsigned char Vesion_Info[4];
	char comply_code[4];
	char equipment_code[10];
	char soft_version[6];
	char soft_date[3];
	char capacity_info[13];
	char comm_agreement[6];
	char hardware_version[6];
	char hardware_date[3];
	
	Vesion_Info[0] = 0x00;
	Vesion_Info[1] = 0x00;
	Vesion_Info[2] = 0x01;
	Vesion_Info[3] = 0x00;

	ReadTermInfo(Vesion_Info, VesionInfo, 45);

	memcpy(comply_code, (char *)&VesionInfo[4], 4);
	memcpy(&equipment_code[2], (char *)&VesionInfo[8], 8);
	memcpy(&soft_version[2], (char *)&VesionInfo[16], 4);
	memcpy(soft_date, (char *)&VesionInfo[20], 3);
	memcpy(&capacity_info[2], (char *)&VesionInfo[23], 11);
	memcpy(&comm_agreement[2], (char *)&VesionInfo[34], 4);
	memcpy(&hardware_version[2], (char *)&VesionInfo[38], 4);
	memcpy(hardware_date, (char *)&VesionInfo[42], 3);

	scrpbuf_main.line = 10;
	//sprintf(scrpbuf_main.buf[0],"���̴���:%s",&comply_code[0]);
	sprintf(scrpbuf_main.buf[0],"�豸���:");
	equipment_code[0] = equipment_code[1] = 32;
	memcpy(scrpbuf_main.buf[1],equipment_code,10);
	sprintf(scrpbuf_main.buf[2],"����汾��:");
	soft_version[0] = soft_version[1] = 32;
	memcpy(scrpbuf_main.buf[3],soft_version,6);
	sprintf(scrpbuf_main.buf[4],"�����������:");
	sprintf(scrpbuf_main.buf[5],"  20%02X��%02X��%02X��",soft_date[2],soft_date[1],soft_date[0]);
	sprintf(scrpbuf_main.buf[6],"�ն�����������Ϣ��:");
	capacity_info[0] = capacity_info[1] = 32;
	memcpy(scrpbuf_main.buf[7],capacity_info,13);
	sprintf(scrpbuf_main.buf[8],"�ն�ͨ�Ź�Լ�汾��:");
	comm_agreement[0] = comm_agreement[1] = 32;
	//memcpy(scrpbuf_main.buf[10],comm_agreement,6);
	sprintf(scrpbuf_main.buf[9],"  Q��GDW_376.1-2009");
	
	//sprintf(scrpbuf_main.buf[11],"�ն�Ӳ���汾��:");
	//hardware_version[0] = hardware_version[1] = 32;
	//memcpy(scrpbuf_main.buf[12],hardware_version,6);
	//sprintf(scrpbuf_main.buf[13],"Ӳ����������:");
	//sprintf(scrpbuf_main.buf[14],"  20%02X��%02X��%02X��",hardware_date[2],hardware_date[1],hardware_date[0]);
}

/**
* @brief			�ն�������Ϣ֮�˿�����
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpPortCfg(unsigned short mid, ULONG arg, UCHAR key)
{
	unsigned char PortCfg[80];
	unsigned char Port_Cfg[4];
	static const char *channel_type[3] = {"ֱ��RS485�ӿ�","ֱ��RS232�ӿ�", "�����ز�ͨ��ģ��"};
	static const char *met_type[4] = {"ר��","���䳭��", "���վ����","̨����ѹ����"};
	Port_Cfg[0] = 0x00;
	Port_Cfg[1] = 0x00;
	Port_Cfg[2] = 0x02;
	Port_Cfg[3] = 0x00;
	
	ReadTermInfo(Port_Cfg, PortCfg, 80);
	
	scrpbuf_main.line = 27;
	sprintf(scrpbuf_main.buf[0],"����������·��:%d",PortCfg[4]);
	sprintf(scrpbuf_main.buf[1],"����������·��:%d",PortCfg[5]);
	sprintf(scrpbuf_main.buf[2],"ֱ��ģ��������·��:%d",PortCfg[6]);
	sprintf(scrpbuf_main.buf[3],"���������·��:%d",PortCfg[7]);
	sprintf(scrpbuf_main.buf[4],"װ��������:%d",(PortCfg[9]>>4)*4096 + (PortCfg[9]&0x0f)*256 + (PortCfg[8]>>4)*16 + (PortCfg[8]&0x0f));
	sprintf(scrpbuf_main.buf[5],"�����ջ�����:%d",(PortCfg[11]>>4)*4096 + (PortCfg[11]&0x0f)*256 + (PortCfg[10]>>4)*16 + (PortCfg[10]&0x0f));
	sprintf(scrpbuf_main.buf[6],"����ͻ�����:%d",(PortCfg[13]>>4)*4096 + (PortCfg[13]&0x0f)*256 + (PortCfg[12]>>4)*16 + (PortCfg[12]&0x0f));
	sprintf(scrpbuf_main.buf[7],"�ն�MAC��ַ:");
	sprintf(scrpbuf_main.buf[8],"  %02X%02X%02X%02X%02X%02X",PortCfg[19],PortCfg[18],PortCfg[17],PortCfg[16],PortCfg[15],PortCfg[14]);
	sprintf(scrpbuf_main.buf[9],"ͨ�Ŷ˿�����:%d",PortCfg[20]);
	sprintf(scrpbuf_main.buf[10],"�˿�%dͨ������:",PortCfg[21]&0x1F);
	sprintf(scrpbuf_main.buf[11],"  %s",channel_type[PortCfg[21]>>5]);
	sprintf(scrpbuf_main.buf[12],"�˿�%d��������:",PortCfg[21]&0x1F);
	sprintf(scrpbuf_main.buf[13],"  %s",met_type[PortCfg[22]>>5]);

	sprintf(scrpbuf_main.buf[14],"�˿�%dͨ������:",PortCfg[33]&0x1F);
	sprintf(scrpbuf_main.buf[15],"  %s",channel_type[PortCfg[33]>>5]);
	sprintf(scrpbuf_main.buf[16],"�˿�%d��������:",PortCfg[33]&0x1F);
	sprintf(scrpbuf_main.buf[17],"  %s",met_type[PortCfg[34]>>5]);

	sprintf(scrpbuf_main.buf[18],"�˿�%dͨ������:",PortCfg[45]&0x1F);
	sprintf(scrpbuf_main.buf[19],"  %s",channel_type[PortCfg[45]>>5]);
	sprintf(scrpbuf_main.buf[20],"�˿�%d��������:",PortCfg[45]&0x1F);
	sprintf(scrpbuf_main.buf[21],"  %s",met_type[PortCfg[46]>>5]);

	sprintf(scrpbuf_main.buf[22],"�˿�%dͨ������:",PortCfg[57]&0x1F);
	sprintf(scrpbuf_main.buf[23],"  %s",channel_type[PortCfg[57]>>5]);
	sprintf(scrpbuf_main.buf[24],"�˿�%d��������:",PortCfg[57]&0x1F);
	sprintf(scrpbuf_main.buf[25],"  %s",met_type[PortCfg[58]>>5]);
	
}

/**
* @brief			�ն�������Ϣ֮��������
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpOtherCfg(unsigned short mid, ULONG arg, UCHAR key)
{
	unsigned int max_meter = MAX_METER;
	unsigned int max_cenmrtp = MAX_CENMETP;
	unsigned int max_plc_basemetp = PLC_BASEMETP + 1;
	unsigned int max_important_user = MAX_IMPORTANT_USER;

	scrpbuf_main.line = 8;
	sprintf(scrpbuf_main.buf[0],"������������:%d",max_meter);
	sprintf(scrpbuf_main.buf[1],"����ܱ��������:%d",max_cenmrtp);
	sprintf(scrpbuf_main.buf[2],"�ز�����ʼ���:%d",max_plc_basemetp);
	sprintf(scrpbuf_main.buf[3],"����ص��û�����:%d",max_important_user);
	sprintf(scrpbuf_main.buf[4],"�ն��ⲿ�������:");
	sprintf(scrpbuf_main.buf[5],"  %s",ParaUni.manuno);
	sprintf(scrpbuf_main.buf[6],"�ն��ڲ��������:");
	sprintf(scrpbuf_main.buf[7],"  %s",ParaUni.manuno_inner);
}

int get_one_day_meter_data(unsigned char *ScrDay,plmdb_day_t *ScrPlMdbDay);
/**
* @brief			�û�����֮�ն�������
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpPlmMetDayData(unsigned short mid, ULONG arg, UCHAR key)
{
	#if 0
	plmdb_day_t PlMdbDayTmp[MAX_PLCMET];
	unsigned int  sn_number=GetSnNum()-1;
	
	//if(sn_number<7||sn_number>2039)
	if(sn_number<3||sn_number>2039)	
	{
		scrpbuf_main.line = 1;
		sprintf(scrpbuf_main.buf[0],"������ų�����Χ!");	
		return;
	}
	if(ParaMeter[sn_number].metp_id == 0)
	{
		scrpbuf_main.line = 2;
		sprintf(scrpbuf_main.buf[1],"δ���ø���ŵı��!");	
		sprintf(scrpbuf_main.buf[2],"    ��ESC������!");	
		return;
	}
	
    	scrpbuf_main.line = 5;
	//sprintf(scrpbuf_main.buf[0],"����ַ:");	
	//sprintf(scrpbuf_main.buf[1],"��ǰ�ܵ���:");
	//sprintf(scrpbuf_main.buf[2],"  %02X%02X%02X.%02XKWh",PlMdbDay[sn_number-MAX_CENMETP].meter_ene[3],PlMdbDay[sn_number-MAX_CENMETP-1].meter_ene[2],PlMdbDay[sn_number-MAX_CENMETP-1].meter_ene[1],PlMdbDay[sn_number-MAX_CENMETP-1].meter_ene[0]);
	//sprintf(scrpbuf_main.buf[3],"�ն���ʱ��:");
	//sprintf(scrpbuf_main.buf[4],"  %dʱ%d��",PlMdbDay[sn_number-MAX_CENMETP].readtime/60,PlMdbDay[sn_number-MAX_CENMETP-1].readtime%60);
	#endif
}


/**
* @brief			�û�����֮�ն�������
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpPlmMetDay(unsigned short mid, ULONG arg, UCHAR key)
{
	int meter_index = 2;
	
	scrpbuf_main.line = meter_total_cnt - cen_meter_cnt;
	for(meter_index=2;meter_index<(meter_total_cnt - cen_meter_cnt + 2);meter_index++)
	{
		sprintf(scrpbuf_main.buf[meter_index -2],"%04d:%02x%02x%02x%02x.%02xKWh",meter_index + 1,meter_ene_buffer[meter_index].meter_ene[4],
		meter_ene_buffer[meter_index].meter_ene[3],meter_ene_buffer[meter_index].meter_ene[2],meter_ene_buffer[meter_index].meter_ene[1],
		meter_ene_buffer[meter_index].meter_ene[0]);
		//strcat(scrpbuf_main.buf[meter_index -2],".");
		//strcat(scrpbuf_main.buf[meter_index -2],"%02x",meter_ene_buffer[meter_index].meter_ene[0]);
	}
}


/**
* @brief			�û�����֮�ص��û�����
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpImportantUserData(unsigned short mid, ULONG arg, UCHAR key)
{
    unsigned int i,j,k;
	int  sn_number = GetSnNum() - 1;
	unsigned char sn_number_1 = sn_number - MAX_CENMETP - 1;

	if(sn_number<=2||sn_number>2039)
	{
		scrpbuf_main.line = 1;
		sprintf(scrpbuf_main.buf[0],"������ų�����Χ!");	
		return;
	}
	if(ParaMeter[sn_number].metp_id == 0)
	{
		scrpbuf_main.line = 2;
		sprintf(scrpbuf_main.buf[1],"δ���ø���ŵı��!");	
		sprintf(scrpbuf_main.buf[2],"    ��ESC������!");	
		return;
	}
	
	for(i=0;i < ParaMix.impuser.num;i++)
	{
		if(sn_number == ParaMix.impuser.metid[i])
			break;
		else
		{
			scrpbuf_main.line = 2;
			sprintf(scrpbuf_main.buf[0],"��������Ų����ص�");	
			sprintf(scrpbuf_main.buf[1],"�û�!");	
			return;
		}
	}
    	scrpbuf_main.line = 48;
	for(i=0,j=0,k=0;i<24;j+=4)
	{
		sprintf(scrpbuf_main.buf[k++],"����%d�����:",i++);
		sprintf(scrpbuf_main.buf[k++],"  %02X%02X%02X.%02XKWh",PlMdbImp[sn_number_1].ene[j+3],PlMdbImp[sn_number_1].ene[j+2],PlMdbImp[sn_number_1].ene[j+1],PlMdbImp[sn_number_1].ene[j]);
	}
}



/**
* @brief			�û�����֮�ز�������Ϣ
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpPlmInfo(unsigned short mid, ULONG arg, UCHAR key)
{
#define   PHASE_A   (0x01<<4)
#define   PHASE_B   (0x01<<5)
#define   PHASE_C   (0x01<<6)

#define   PHASE_A_1   0x01
#define   PHASE_B_1   (0x01<<1)
#define   PHASE_C_1   (0x01<<2)

	int  sn_number = GetSnNum() - 1;

	if(sn_number < 7||sn_number > 2039)
	{
		scrpbuf_main.line = 1;
		sprintf(scrpbuf_main.buf[0],"������ų�����Χ!");	
		return;
	}
	if(ParaMeter[sn_number].metp_id == 0)
	{
		scrpbuf_main.line = 2;
		sprintf(scrpbuf_main.buf[1],"δ���ø���ŵı��!");	
		sprintf(scrpbuf_main.buf[2],"    ��ESC������!");	
		return;
	}
	
    scrpbuf_main.line = 13;
	sprintf(scrpbuf_main.buf[0],"�м�·�ɼ���:%d",PlcState[sn_number].routes);
	switch(PlcState[sn_number].phase&0X70)
	{
	case PHASE_A:
		sprintf(scrpbuf_main.buf[1],"�ز�������λ:A��");
		break;
	case PHASE_B:
		sprintf(scrpbuf_main.buf[1],"�ز�������λ:B��");
   		break;
	case PHASE_C:
		sprintf(scrpbuf_main.buf[1],"�ز�������λ:C��");
    	break;
	default: 
		sprintf(scrpbuf_main.buf[1],"�ز�������λ:δ����");
		break;
	}
	
	switch(PlcState[sn_number].phase&0X07)
	{
	case  PHASE_A_1:
		sprintf(scrpbuf_main.buf[2],"���ʵ����λ:A��");
    	break;
	case  PHASE_B_1:
		sprintf(scrpbuf_main.buf[2],"���ʵ����λ:B��");
    	break;
	case  PHASE_C_1:
		sprintf(scrpbuf_main.buf[2],"���ʵ����λ:C��");
    	break;
	default: 
		sprintf(scrpbuf_main.buf[2],"���ʵ����λ:δ֪");
		break;
	}
	
	sprintf(scrpbuf_main.buf[3],"���һ�γ���");
	sprintf(scrpbuf_main.buf[4],"�Ƿ�ɹ�:%s",(PlcState[sn_number].okflag&0x01)?"��":"��");
	sprintf(scrpbuf_main.buf[5],"�����������ʧ��");
	sprintf(scrpbuf_main.buf[6],"�ۼƴ���:%d",PlcState[sn_number].failcount);
	sprintf(scrpbuf_main.buf[7],"���һ�γ���ɹ�ʱ��");
	sprintf(scrpbuf_main.buf[8],"  20%02d��%d��%d��",PlcState[sn_number].oktime.year,PlcState[sn_number].oktime.month,PlcState[sn_number].oktime.day);
	sprintf(scrpbuf_main.buf[9],"  %d:%d:%d",PlcState[sn_number].oktime.hour,PlcState[sn_number].oktime.minute,PlcState[sn_number].oktime.second);
	sprintf(scrpbuf_main.buf[10],"���һ�γ���ʧ��ʱ��");
	sprintf(scrpbuf_main.buf[11],"  20%02d��%d��%d��",PlcState[sn_number].failtime.year,PlcState[sn_number].failtime.month,PlcState[sn_number].failtime.day);
	sprintf(scrpbuf_main.buf[12],"  %d:%d:%d",PlcState[sn_number].failtime.hour,PlcState[sn_number].failtime.minute,PlcState[sn_number].failtime.second);
}


/**
* @brief			״̬�˵�����
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void ScrpRunState(unsigned short mid, ULONG arg, UCHAR key)
{
	runstate_t   *runstate;
	runstate = RunStateModify();
	sysclock_t sys_clock;
	GetClockSysStart(&sys_clock);

    	scrpbuf_main.line = 5;
	//sprintf(scrpbuf_main.buf[0],"����״̬:%s",runstate->pwroff?"������":"����");
	//sprintf(scrpbuf_main.buf[1],"��س��״̬:%s",runstate->batcharge?"���":"�ǳ��");
	//sprintf(scrpbuf_main.buf[2],"ң����1״̬:%s",runstate->isig_stat&0x01?"��":"��");
	//sprintf(scrpbuf_main.buf[3],"ң����2״̬:%s",runstate->isig_stat&0x02?"��":"��");
	sprintf(scrpbuf_main.buf[0],"ϵͳ����ʱ��:");
	sprintf(scrpbuf_main.buf[1],"  20%02d��%02d��%02d��",sys_clock.year,sys_clock.month,sys_clock.day);
	sprintf(scrpbuf_main.buf[2],"  %02d:%02d:%02d",sys_clock.hour,sys_clock.minute,sys_clock.second);
	sprintf(scrpbuf_main.buf[3],"�ն�����վ����״̬:");
	sprintf(scrpbuf_main.buf[4],"  %s",SvrCommLineState?"���ӽ���":"���ӶϿ�");
}


void ScrpMeterCfg(unsigned short mid, ULONG arg, UCHAR key)
{
	int scr_meter_total_cnt;
	int scr_cen_meter_cnt;
	int scr_imp_meter_cnt;
	scr_meter_total_cnt = get_meter_total_cnt();
	scr_cen_meter_cnt = get_cen_meter_total_cnt();
	scr_imp_meter_cnt = get_imp_meter_total_cnt();

	
	sprintf(scrpbuf_main.buf[0],"�������:  %d��",scr_meter_total_cnt);
	sprintf(scrpbuf_main.buf[1],"�����������:  %d��",scr_meter_total_cnt - scr_cen_meter_cnt);
	sprintf(scrpbuf_main.buf[2],"�ܱ����:  %d��",scr_cen_meter_cnt);
	sprintf(scrpbuf_main.buf[3],"�ص�����:  %d��",scr_imp_meter_cnt);
	scrpbuf_main.line = 4;
}


extern sysclock_t read_meter_start_time;
extern sysclock_t read_meter_finish_time;

void ScrpReadMeterStat(unsigned short mid, ULONG arg, UCHAR key)
{
	unsigned char start_time[4];
	unsigned char finish_time[4];
	int i = 0,j = 0;

	memcpy(start_time,&read_meter_start_time.month,4);
	memcpy(finish_time,&read_meter_finish_time.month,4);
	HexToBcd(start_time,4);
	HexToBcd(finish_time,4);

	for(i=0;i<MAX_METER_CNT;i++)
	{
		if(!check_buf(meter_ene_buffer[i].meter_ene) 
			&& (meter_ene_buffer[i].met_id>=2) 
			&&(meter_ene_buffer[i].portcfg&0x1F) == PLC_PORT)
		{
			j++;
		}
	}
	sprintf(scrpbuf_main.buf[0],"�û���ɹ���:%d��",j);
	sprintf(scrpbuf_main.buf[1],"�ص��ɹ���:%d��",imp_meter_read_succ_cnt);
	sprintf(scrpbuf_main.buf[2],"������ʼʱ��: ");
	sprintf(scrpbuf_main.buf[3],"  %02x��%02x��%02x:%02x", start_time[0], start_time[1], start_time[2],start_time[3]);
	sprintf(scrpbuf_main.buf[4],"�������ʱ��:");
	if(!read_meter_finish)
	{
		sprintf(scrpbuf_main.buf[5],"  ���������.....");
	}
	else
	{
		sprintf(scrpbuf_main.buf[5],"  %02x��%02x��%02x:%02x", finish_time[0], finish_time[1], finish_time[2],finish_time[3]);
	}
	sprintf(scrpbuf_main.buf[6],"����״̬: ");
	if(!read_meter_finish)
	{
		strcat(scrpbuf_main.buf[6],"���ڳ���");
	}
	else
	{
		strcat(scrpbuf_main.buf[6],"�������");
	}
		
	scrpbuf_main.line = 7;
}


void ScrpRestartTerm(unsigned short mid, ULONG arg, UCHAR key)
{
	ClearLcdScreen();
	sprintf(scrpbuf_main.buf[0],"  ϵͳ��������.....");
	scrpbuf_main.line = 1;
	DisplayNormal((unsigned char*)scrpbuf_main.buf,strlen((char *)&scrpbuf_main.buf[0]),16 * 3,0);
	//DisplayNormal(const unsigned char *str,unsigned char len,unsigned char x,unsigned char y);
	DisplayLcdBuffer();
	Sleep(500);
	SysRestart();
}

static void PortTranStr(unsigned short port, unsigned char *pstr)
{
	pstr[0] = port/10000;
	pstr[1] = (port-pstr[0]*10000)/1000;
	pstr[2] = (port - pstr[0]*10000 - pstr[1]*1000)/100;
	pstr[3] = (port - pstr[0]*10000 - pstr[1]*1000 - pstr[2]*100)/10;
	pstr[4] = port%10;
}

void ScrShowTermPara(unsigned short mid, ULONG arg, UCHAR key)
{
  	int x, y;
	unsigned char svrip_rtn[12],svrbakip_rtn[12],  port_rtn[5],city_area_rtn[4],scr_term_addr_rtn[5];
	unsigned char apn_rtn[16];
	unsigned char port_ini[5],term_addr_ini[5];
	unsigned short term_addr;
	char apn[16];
	//unsigned char apn_len;
	int i;

	ClrShowBuf(&scrpbuf_main);
	InitControlPage();
	PortTranStr(ParaTerm.svrip.portmain, port_ini);
	memset(term_addr_ini,0x00,5);
	term_addr = MAKE_SHORT(ParaUni.addr_sn);
	PortTranStr(term_addr,term_addr_ini);
	
	DisplayNormal("    ͨ�Ų�������",16,2*16-MENU_LINE_POS,0);	
	
 	x = 4; 
	y = 5;
	DisplayNormal("�ն˵�ַ:",13,3*16-MENU_LINE_POS,0);

	AddEditBox(3, 10, 10, (char *)&typ_charlist_a[0], ParaUni.addr_area[1]>>4, &city_area_rtn[0], CHAR_FLAG);
	AddEditBox(3, 11, 10, (char *)&typ_charlist_a[0], ParaUni.addr_area[1]&0x0f, &city_area_rtn[1], CHAR_FLAG);	
	AddEditBox(3, 12, 10, (char *)&typ_charlist_a[0], ParaUni.addr_area[0]>>4, &city_area_rtn[2], CHAR_FLAG);
	AddEditBox(3, 13, 10, (char *)&typ_charlist_a[0], ParaUni.addr_area[0]&0x0f, &city_area_rtn[3], CHAR_FLAG);
	DisplayNormal("-",1,3*16-MENU_LINE_POS,14*8);
	AddEditBox(3, 15, 10, (char *)&typ_charlist_a[0], term_addr_ini[0], &scr_term_addr_rtn[0], CHAR_FLAG);
	AddEditBox(3, 16, 10, (char *)&typ_charlist_a[0], term_addr_ini[1], &scr_term_addr_rtn[1], CHAR_FLAG);	
	AddEditBox(3, 17, 10, (char *)&typ_charlist_a[0], term_addr_ini[2], &scr_term_addr_rtn[2], CHAR_FLAG);
	AddEditBox(3, 18, 10, (char *)&typ_charlist_a[0], term_addr_ini[3], &scr_term_addr_rtn[3], CHAR_FLAG);
	AddEditBox(3, 19, 10, (char *)&typ_charlist_a[0], term_addr_ini[4], &scr_term_addr_rtn[4], CHAR_FLAG);

	DisplayNormal("APN:",strlen("APN:"),x*16-MENU_LINE_POS,0);

/*
	for(i=0;i<16;i++)
	{
		apn_rtn[i] = 0;
	}

	for(i=0;i<16;i++)
	{
		if(ParaTerm.svrip.apn[i] != '.')
		{
			apn[i] = (ParaTerm.svrip.apn[i]-typ_charlist_3[0]);
		}
		else
		{
			apn[i] = (ParaTerm.svrip.apn[i]-'.'+26);
			PrintLog(0, "ParaTerm.svrip.apn i=%d\n",i);
		}
	}
	*/

	for(i=0;i<16;i++)
	{
		apn_rtn[i] = '\0';
	}
	

	for(i=0;i<strlen(ParaTerm.svrip.apn);i++)
	{
		if(ParaTerm.svrip.apn[i] != '.')
		{
			apn[i] = (ParaTerm.svrip.apn[i]-typ_charlist_3[0]);
		}
		else
		{
			apn[i] = (ParaTerm.svrip.apn[i]-'.'+26);
			PrintLog(0, "ParaTerm.svrip.apn i=%d\n",i);
		}
	}
	
	
    	AddEditBox(x, 4, 28, (char*)&typ_charlist_3[0],apn[0],&apn_rtn[0], CHAR_FLAG);
    	AddEditBox(x, 5, 28, (char*)&typ_charlist_3[0],apn[1],&apn_rtn[1], CHAR_FLAG);
    	AddEditBox(x, 6, 28, (char*)&typ_charlist_3[0],apn[2],&apn_rtn[2], CHAR_FLAG);
    	AddEditBox(x, 7, 28, (char*)&typ_charlist_3[0],apn[3],&apn_rtn[3], CHAR_FLAG);
    	AddEditBox(x, 8, 28, (char*)&typ_charlist_3[0],apn[4],&apn_rtn[4], CHAR_FLAG);
    	AddEditBox(x, 9, 28, (char*)&typ_charlist_3[0],apn[5],&apn_rtn[5], CHAR_FLAG);
    	AddEditBox(x, 10, 28, (char*)&typ_charlist_3[0],apn[6],&apn_rtn[6], CHAR_FLAG);
    	AddEditBox(x, 11, 28, (char*)&typ_charlist_3[0],apn[7],&apn_rtn[7], CHAR_FLAG);
    	AddEditBox(x, 12, 28, (char*)&typ_charlist_3[0],apn[8],&apn_rtn[8], CHAR_FLAG);
	x++;
	y = 0; 
	
    	DisplayNormal("��IP:",strlen("��IP:"),x*16-MENU_LINE_POS,0);
    	y = 5;
	
	int pls = y*8;
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], ParaTerm.svrip.ipmain[0]/100, &svrip_rtn[0], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipmain[0]%100)/10, &svrip_rtn[1], CHAR_FLAG);	
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipmain[0]%100)%10, &svrip_rtn[2], CHAR_FLAG);
	DisplayNormal(".",1,x*16-MENU_LINE_POS,pls+24);
	y++;
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], ParaTerm.svrip.ipmain[1]/100, &svrip_rtn[3], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipmain[1]%100)/10, &svrip_rtn[4], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipmain[1]%100)%10, &svrip_rtn[5], CHAR_FLAG);	
	DisplayNormal(".",1,x*16-MENU_LINE_POS,pls+56);
	y++;
    	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], ParaTerm.svrip.ipmain[2]/100, &svrip_rtn[6], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipmain[2]%100)/10, &svrip_rtn[7], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0],(ParaTerm.svrip.ipmain[2]%100)%10, &svrip_rtn[8], CHAR_FLAG);	
	DisplayNormal(".",1,x*16-MENU_LINE_POS,pls+88);
	y++;
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], ParaTerm.svrip.ipmain[3]/100,  &svrip_rtn[9], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipmain[3]%100)/10, &svrip_rtn[10], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipmain[3]%100)%10, &svrip_rtn[11], CHAR_FLAG);		
	x++;
    	DisplayNormal("��IP:",strlen("��IP:"),x*16-MENU_LINE_POS,0);
    	y = 5;
	
	int plsbak = y*8;
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], ParaTerm.svrip.ipbakup[0]/100, &svrbakip_rtn[0], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipbakup[0]%100)/10, &svrbakip_rtn[1], CHAR_FLAG);	
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipbakup[0]%100)%10, &svrbakip_rtn[2], CHAR_FLAG);
	DisplayNormal(".",1,x*16-MENU_LINE_POS,plsbak+24);
	y++;
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], ParaTerm.svrip.ipbakup[1]/100, &svrbakip_rtn[3], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipbakup[1]%100)/10, &svrbakip_rtn[4], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipbakup[1]%100)%10, &svrbakip_rtn[5], CHAR_FLAG);	
	DisplayNormal(".",1,x*16-MENU_LINE_POS,plsbak+56);
	y++;
    	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], ParaTerm.svrip.ipbakup[2]/100, &svrbakip_rtn[6], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipbakup[2]%100)/10, &svrbakip_rtn[7], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0],(ParaTerm.svrip.ipbakup[2]%100)%10, &svrbakip_rtn[8], CHAR_FLAG);	
	DisplayNormal(".",1,x*16-MENU_LINE_POS,plsbak+88);
	y++;
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], ParaTerm.svrip.ipbakup[3]/100,  &svrbakip_rtn[9], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipbakup[3]%100)/10, &svrbakip_rtn[10], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], (ParaTerm.svrip.ipbakup[3]%100)%10, &svrbakip_rtn[11], CHAR_FLAG);	
	x++;
    	DisplayNormal("��վ�˿�:",strlen("��վ�˿�:"),x*16-MENU_LINE_POS,0);
	
	y = 11;
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], port_ini[0], &port_rtn[0], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], port_ini[1], &port_rtn[1], CHAR_FLAG);	
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], port_ini[2], &port_rtn[2], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], port_ini[3], &port_rtn[3], CHAR_FLAG);
	AddEditBox(x, y++, 10, (char *)&typ_charlist_a[0], port_ini[4], &port_rtn[4], CHAR_FLAG);
	
    	x++;
	x++;

	//PrintLog(0, "x=%d",x);
	AddButton(8, 5, "ȷ��", DefOkClick);
	AddButton(8, 11, "ȡ��", DefEscClick);
   
	if(CONTROL_OK_EXIT == ShowControlPage()) {	

		PrintHexLog(0, svrip_rtn, 12);
		for(i=0; i<4; i++) ParaTerm.svrip.ipmain[i] = svrip_rtn[0+i*3]*100+svrip_rtn[1+i*3]*10+svrip_rtn[2+i*3];
		ParaTerm.svrip.portmain = port_rtn[0]*10000+port_rtn[1]*1000+port_rtn[2]*100+port_rtn[3]*10+port_rtn[4];
		term_addr = scr_term_addr_rtn[0]*10000+scr_term_addr_rtn[1]*1000+scr_term_addr_rtn[2]*100+scr_term_addr_rtn[3]*10+scr_term_addr_rtn[4];
		ParaUni.addr_area[1] = city_area_rtn[1]+city_area_rtn[0]*16;
		ParaUni.addr_area[0] = city_area_rtn[3]+city_area_rtn[2]*16;
		DEPART_SHORT(term_addr,ParaUni.addr_sn);


	for(i=0;i<strlen(ParaTerm.svrip.apn);i++)
	{
		if(ParaTerm.svrip.apn[i] != '.')
		{
			apn[i] = (ParaTerm.svrip.apn[i]-typ_charlist_3[0]);
		}
		else
		{
			apn[i] = (ParaTerm.svrip.apn[i]-'.'+26);
			PrintLog(0, "ParaTerm.svrip.apn i=%d\n",i);
		}
	}



		for(i=0;i<strlen(apn_rtn);i++)
		{
			if(apn_rtn[i] != 26)
			{
				apn_rtn[i] += (typ_charlist_3[0]);
			}
			else 
			{
				apn_rtn[i] += '.' - 26;
			}
			ParaTerm.svrip.apn[i] = apn_rtn[i];
		}
		//apn_len = i;
		//PrintLog(0, "apn_len=%d\n",apn_len);
		//strcpy((char *)ParaTerm.svrip.apn, "CMNET");
		
		//for(i=0;i<apn_len;i++)
			//ParaTerm.svrip.apn[i] = apn_rtn[i];
		//ParaUni.uplink = chn_rtn;
		SetSaveParamFlag(SAVEFLAG_TERM|SAVEFLAG_UNI);
		SaveParam();
		ClearLcdScreen();
		scrpbuf_main.line = 1;
		strcpy(scrpbuf_main.buf[0], " ���óɹ�!");
		//strcpy(scrpbuf_main.buf[1], " ϵͳ5���λ...");
		DisplayNormal((unsigned char*)&scrpbuf_main.buf[0],strlen((char *)&scrpbuf_main.buf[0]),16 * 3,0);
		//DisplayNormal((unsigned char*)&scrpbuf_main.buf[1],strlen((char *)&scrpbuf_main.buf[1]),16 * 4,0);
		DisplayLcdBuffer();
		//Sleep(500);
		//SysRestart();
		//SysAddCTimer(5, CTimerSysRestart, 0);
		return; 
	}
	else {
		scrpbuf_main.line = 1;
		strcpy(scrpbuf_main.buf[0], " �˳�,������!");
		return;
	}
	return;
	
}




