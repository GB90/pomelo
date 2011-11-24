
/**
* scr_task.c -- �˵���ʾ����
* 
* 
* ����ʱ��: 2010-5-20
* ����޸�ʱ��: 2010-7-10
*/

#include <unistd.h>
#include <string.h>
#include <stdio.h> 

#include "include/debug.h"
#include "include/sys/task.h"
#include "include/sys/schedule.h"
#include "include/screen/scr_show.h"
#include "scr_menupage.h"
#include "scr_menu.h"
#include "scr_lcd.h"
#include "include/plcmet/pltask.h"
#include "scr_lcd.h"
#include "include/plcmet/pltask.h"

///MNU_ITEM�͵ı����Ǵ��ݸ�m_menu_sys��

static MNU_ITEM   m_menu_main;  
static MNU_ITEM   m_menu1;
static MNU_ITEM   m_menu2;
static MNU_ITEM   m_menu3;


extern scrpage_t scrpbuf_main;
extern unsigned short int scrhflag_alm;
extern unsigned char showlosflag;
static unsigned char lcd_back_light_conut;
//���˵����飬ÿ����ԱΪMNU_SUB_ATTR�ṹ��
static MNU_SUB_ATTR menu_main[] = {
        ///����     ���ɲ������ֽ���ĺ���    �����Ĳ���      �Ӳ˵�Ϊȫ�ֽṹ�����m_menu1��m_menu1�ں����make_menu�����б���ֵ
	{"   ������������ʾ  ",   MENU_NULL, {0,0,NULLKEY}, (unsigned char *)&m_menu1,SHOW_ATTR},  
	{"   ����������鿴  ",   MENU_NULL, {0,0,NULLKEY}, (unsigned char *)&m_menu2,SHOW_ATTR},  ///m_menu1�ĵ�һ����Ա�ĵ�һ��Ԫ��Ϊunsigned char����
	{"   �ն˹�����ά��  ",   MENU_NULL, {0,0,NULLKEY}, (unsigned char *)&m_menu3,SHOW_ATTR},  ///m_menu1�ĵ�һ����Ա�ĵ�һ��Ԫ��Ϊunsigned char����
	{MENU_NULL,MENU_NULL, {0,0,NULLKEY}, MENU_NULL,SHOW_ATTR}  ///��β��־�����������ж�
};


static MNU_ITEM   m_menu1_sub[2];   
///����������鿴����
static MNU_SUB_ATTR menu_1[] = { 
		{" ȫ������������", MENU_NULL, {0,0,NULLKEY}, (unsigned char *)&m_menu1_sub[0], SHOW_ATTR},
         //#if PARA_SET 
		//{" ̨���ܱ����", MENU_NULL, {0,0,OKKEY6}, (unsigned char *)&m_menu2_sub[1], SHOW_ATTR},
		{" ��������������", MENU_NULL, {0,0,OKKEY1}, (unsigned char *)&m_menu1_sub[1], SHOW_ATTR},
		{MENU_NULL,MENU_NULL,{0,0,NULLKEY},MENU_NULL,SHOW_ATTR}
};



#if 0
static MNU_ITEM   m_menu1_1_sub[2];  

//static MNU_ITEM   m_menu1_sub[3];  
///������������ʾ����
static MNU_SUB_ATTR menu1_1[] = { 
		{" ��ǰ��������", ScrpPlmMetDay, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{" �ն�������", ScrpPlmMetDayData, {0,0,OKKEY7}, MENU_NULL, SHOW_ATTR},
		{" �ص��û�����", ScrpImportantUserData, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		//{" �ز�������Ϣ", ScrpPlmInfo, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},

		{MENU_NULL,MENU_NULL,{0,0,NULLKEY},MENU_NULL,SHOW_ATTR}
};

static MNU_SUB_ATTR menu1_2[] = { 
		{" ��ǰ��������", ScrpPlmMetDayData, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{" �ն�������", ScrpPlmMetDayData, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{" �ص��û�����", ScrpImportantUserData, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		//{" �ز�������Ϣ", ScrpPlmInfo, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},

		{MENU_NULL,MENU_NULL,{0,0,NULLKEY},MENU_NULL,SHOW_ATTR}
};
#endif


#if 1
//static MNU_ITEM   m_menu1_1_sub[2];  

//static MNU_ITEM   m_menu1_sub[3];  
///������������ʾ����
static MNU_SUB_ATTR menu1_1[] = { 
		{" ��ǰ��������", MENU_NULL, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{" �ն�������", MENU_NULL, {0,0,OKKEY7}, MENU_NULL, SHOW_ATTR},
		{" �ص��û�����", MENU_NULL, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		//{" �ز�������Ϣ", ScrpPlmInfo, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},

		{MENU_NULL,MENU_NULL,{0,0,NULLKEY},MENU_NULL,SHOW_ATTR}
};

static MNU_SUB_ATTR menu1_2[] = { 
		{" ��ǰ��������", MENU_NULL, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{" �ն�������", MENU_NULL, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{" �ص��û�����", MENU_NULL, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		//{" �ز�������Ϣ", ScrpPlmInfo, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},

		{MENU_NULL,MENU_NULL,{0,0,NULLKEY},MENU_NULL,SHOW_ATTR}
};
#endif


static MNU_ITEM   m_menu2_sub[2];   
///����������鿴����
static MNU_SUB_ATTR menu_2[] = { 
		{" �ն˲���    ", MENU_NULL, {0,0,NULLKEY}, (unsigned char *)&m_menu2_sub[0], SHOW_ATTR},
         //#if PARA_SET 
		//{" ̨���ܱ����", MENU_NULL, {0,0,OKKEY6}, (unsigned char *)&m_menu2_sub[1], SHOW_ATTR},
		{" ̨���ܱ����", MENU_NULL, {0,0,NULLKEY}, (unsigned char *)&m_menu2_sub[1], SHOW_ATTR},
         //#endif  
		 #if PARA_SET 
		{" ����������    ",ScrpUserMetPara, {0,0,OKKEY2}, MENU_NULL, SHOW_ATTR},//û���Ӳ˵�
         #endif  
		 {" ͨ�Ų�������", ScrShowTermPara, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		//{" �ն�������Ϣ", MENU_NULL, {0,0,NULLKEY}, (unsigned char *)&m_menu3_sub[2], SHOW_ATTR},
		{MENU_NULL,MENU_NULL,{0,0,NULLKEY},MENU_NULL,SHOW_ATTR}
};

///�ն˲������Ӳ˵�
static MNU_ITEM   m_menu2_1_sub[1];  
static MNU_SUB_ATTR menu2_1[] = {
		{" ͨ�Ų���", ScrpTermParaCom, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{" �˿ڳ������", MENU_NULL, {0,0,NULLKEY}, (unsigned char *)&m_menu2_1_sub[0], SHOW_ATTR},
		///{" �������", ScrpReadMetPara, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		//{" ��ʱ�ϱ�����", MENU_NULL, {0,0,NULLKEY}, (unsigned char *)&m_menu2_1_sub[1], SHOW_ATTR},
		//{" �¼���¼����", ScrpEventReadCfg, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		///{" �˿ڲ���", MENU_NULL, {0,0,NULLKEY}, (unsigned char *)&m_menu3_1_sub[1], SHOW_ATTR},
		//{" ��������", ScrpCascadePara, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{MENU_NULL,MENU_NULL,{0,0,NULLKEY},MENU_NULL,SHOW_ATTR}
};

///�˿ڲ����Ӳ˵�
static MNU_SUB_ATTR menu2_1_2[] = {
		{" �ز��˿ڲ���", ScrpReadMetPara, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{" 485�˿ڲ���", ScrpReadMetPara, {0,1,NULLKEY}, MENU_NULL, SHOW_ATTR},
		//{" �˿�3", ScrpReadMetPara, {0,2,NULLKEY}, MENU_NULL, SHOW_ATTR},
		//{" �˿�4", ScrpReadMetPara, {0,3,NULLKEY}, MENU_NULL, SHOW_ATTR},

		{MENU_NULL,MENU_NULL,{0,0,NULLKEY},MENU_NULL,SHOW_ATTR}
};


///�ܱ����
static MNU_SUB_ATTR menu2_2[] = {
		{" ��������", ScrpMajorMetBasePara, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{" ��ֵ����", ScrpMajorMetLmtPara, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},

		{MENU_NULL,MENU_NULL,{0,0,NULLKEY},MENU_NULL,SHOW_ATTR}
};

static MNU_SUB_ATTR menu_3[] = { 
		{"      ������Ϣ	   ", ScrpRunState, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{"      �������      ", ScrpMeterCfg, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{"      ����״̬      ", ScrpReadMeterStat, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{"      �㳭���      ", ScrpVesionInfo, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{"      �����ն�      ", ScrpRestartTerm, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{"      �ն�����      ", ScrpOtherCfg, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{"      �汾��Ϣ      ", ScrpVesionInfo, {0,0,NULLKEY}, MENU_NULL, SHOW_ATTR},
		{MENU_NULL,MENU_NULL,{0,0,NULLKEY},MENU_NULL,SHOW_ATTR}
};

///�����еĲ˵����ɺ�����̬�Ĵ����menu_all[]�������
///���������е�Ŀ����Ϊ�˷���ʵ�ֶ�ϵͳ���в˵�ʵ������ʽ������
static MNU_ATTR menu_all[] = {
	{&m_menu_main,MENU_NULL,menu_main},  ///���˵�
	{&m_menu1,&m_menu_main,menu_1},      ///������������ʾ
	{&m_menu1_sub[0],&m_menu1,menu1_1},  
	{&m_menu1_sub[1],&m_menu1,menu1_2},  
#if PARA_SET
	{&m_menu2,&m_menu_main,menu_2},      ///����������鿴
#endif
	{&m_menu3,&m_menu_main,menu_3},      ///�ն˹�����ά��
	{&m_menu2_sub[0],&m_menu2,menu2_1},  
	{&m_menu2_sub[1],&m_menu2,menu2_2},  
	{&m_menu2_1_sub[0],&m_menu2_sub[0],menu2_1_2},

	{MENU_NULL,MENU_NULL,MENU_NULL}
};

//Ѳ�Բ˵�����
static MNU_LOOP_FUNC loop_shows[] = {
		{ScrpHead,{0,0,NULLKEY}},   ///��˾
		{ScrpMetDataEne,{0,0,NULLKEY}},  ///1���ܱ������
		{ScrpMetDataImm, {0,0,NULLKEY}}, ///1���ܱ�˲ʱ��

		{ScrpMetDataEne,{1,0,NULLKEY}},  
		{ScrpMetDataImm, {1,0,NULLKEY}}, 

		{ScrpMetDataEne,{2,0,NULLKEY}},  
		{ScrpMetDataImm, {2,0,NULLKEY}}, 

		{ScrpMetDataEne,{3,0,NULLKEY}},  
		{ScrpMetDataImm, {3,0,NULLKEY}}, 

		{ScrpMetDataEne,{4,0,NULLKEY}},  
		{ScrpMetDataImm, {4,0,NULLKEY}}, 

		{ScrpMetDataEne,{5,0,NULLKEY}},  
		{ScrpMetDataImm, {5,0,NULLKEY}}, 

		{ScrpMetDataEne,{6,0,NULLKEY}},  
		{ScrpMetDataImm, {6,0,NULLKEY}},

		{ScrpMetDataEne,{7,0,NULLKEY}},  
		{ScrpMetDataImm, {7,0,NULLKEY}}, 

		{MENU_NULL, {0,0,NULLKEY}}  ///ĩβ��־
};

/**
* @brief			lcd��ʾ����
* @param arg		����
*/
void ClearLcdBackLightConut(void)
{
	lcd_back_light_conut = 0;
}


void *screen_task(void *arg)
{
	Sleep(10);
	InitLcd();
	ShowTopFlag();
	LoadAllMenu(menu_all);  ///���ɸ�����˵�������
	InitSysMenu(&m_menu_main,loop_shows); 
   
	while(1)
	{
		MenuProcess();        //������ʾ���� 
		Sleep(10);
	}

	return 0;
}

int CheckCSQ1();
extern unsigned char GprsDevSigState;
void *screen_task1(void *arg)
{
	unsigned char one_second_conut = 0;
	unsigned char term_stat_check = 0;
	unsigned char botm_displ_buf[30];

	while(1)
	{
	    	one_second_conut++;
		//if(one_second_conut >= 80)
		if(one_second_conut >= 80 * 3)	
		{	
			one_second_conut = 0;
			//GprsDevSigState = CheckCSQ1();
            		ShowTopFlag();
		}
		if(GetLcdBakLightState())
		{
			lcd_back_light_conut++;
			if(lcd_back_light_conut >= 120)
			{
				lcd_back_light_conut = 0;
				LcdBakLight(0);
				CloseLcdBakLightState();
			}
		}

		#if 1
		if((meter_total_cnt - cen_meter_cnt))	
		{
			term_stat_check++;
			//if(term_stat_check >= 10 * 30)
			if(term_stat_check >= 100)	
			{	
				term_stat_check = 0;
				CleanPartOfScreen(16 * 9,1);

				memset(botm_displ_buf,'\0',30);
				if(!read_meter_finish)
				{
					sprintf(botm_displ_buf, "�ն����ڳ���... %d",meter_read_succ_cnt*100/(meter_total_cnt - cen_meter_cnt));
					strcat(botm_displ_buf,"%");	
				}
				else
				{
					sprintf(botm_displ_buf, "�ն˳������! %d",meter_read_succ_cnt*100/(meter_total_cnt - cen_meter_cnt));
					strcat(botm_displ_buf,"%");
				}
				
				DisplayNormal(botm_displ_buf,strlen((char *)botm_displ_buf),BTM_LINE_ROW+1,0);
				DisplayLcdBuffer();
			}
		}
		#endif
		Sleep(10);
	}

	return 0;
}



DECLARE_INIT_FUNC(ScreenInit);


/**
* @brief			lcd��ʾ�����ʼ��
* @return			����ֵΪ0���ʼ���ɹ�������ʧ��
*/
int ScreenInit(void)
{
	SysCreateTask(screen_task, NULL);
	SysCreateTask(screen_task1, NULL);
	SET_INIT_FLAG(ScreenInit);

	return 0;
}





