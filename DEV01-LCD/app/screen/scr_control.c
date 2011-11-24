/**
* scr_control.c -- ��ʾ��������
* 
* 
* ����ʱ��: 2010-5-20
* ����޸�ʱ��: 2010-7-10
*/

#include "include/debug.h"
#include "scr_input_api.h"
#include "scr_menu.h"
#include "scr_lcd.h"
///����������ַ��嵥
static unsigned int    input_port_num;
static unsigned char   task_num;
static unsigned short  sn_num;
static unsigned int    major_met_num;
static unsigned char  qur_day[3];

const char typ_charlist_a[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
};
const char typ_charlist_a1[] = {
'1', '2', '3', '4'
};
const char typ_charlist_a2[] = {
'1', '2', '3', '4', '5', '6', '7', '8'
};
const char typ_charlist_a3[] = {
 '0','1'
};
const char typ_charlist_a4[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};
const char typ_charlist_a5[] = {
	'0', '1', '2'
};
const char typ_charlist_b[] = {
	'Y', 'N',
};

/**
* @brief           ȷ�ϰ�������
*/
void DefOkClick(void)
{	
	InitControlPage();  ///���ǰ�����
	QuitControlPage(CONTROL_OK_EXIT);  ///ȷʵ֮���˳�������
}

/**
* @brief           ȡ����������
*/
void DefEscClick(void)
{
	InitControlPage();
	QuitControlPage(-1);
}


/**
* @brief           �ַ�ת��Ϊ����
* @param c         �ַ�
* @return          0ת��ʧ�ܣ���0����ת���ɹ�
*/
static UCHAR CharToNum(char c)
{
	if((c >= '0') && (c <= '9')) return(c-'0');
	else if((c >= 'a') && (c <= 'f')) return(c-'a');
	else if((c >= 'A') && (c <= 'F')) return(c-'A');
	else return 0;
}
/**
* @brief           �ַ�ת��ΪBCD��
* @param bcds      BCD���׵�ַ
* @param len       BCD��ĳ���
* @param chars     �ַ����׵�ַ
* @return          ���ص��ļ�ָ��
*/
static void CharToBcd(UCHAR *bcds, int len, unsigned char *chars)
{
	int i;

	for(i=0; i<len; i++) {
		bcds[i] = CharToNum(*chars++);
		bcds[i] |= CharToNum(*chars++) << 4;
	}
}

/**
* @brief       ����������ʾ������ 
* @return      0����ɹ���1����ʧ��
*/
int ScrPwdConfirm(void)
{
    ///char editbox_rtn[6];
	unsigned char editbox_rtn[6];
	UCHAR pwd[3];
	int i;	
	DisplayNormal("����������:",11,2*16-MENU_LINE_POS,0);
	AddEditBox(4, 7, 10, (char *)&typ_charlist_a[0], 9, &editbox_rtn[0], CHAR_FLAG);
	AddEditBox(4, 8, 10, (char *)&typ_charlist_a[0], 9, &editbox_rtn[1], CHAR_FLAG);	
	AddEditBox(4, 9, 10, (char *)&typ_charlist_a[0], 9, &editbox_rtn[2], CHAR_FLAG);
	AddEditBox(4, 10, 10, (char *)&typ_charlist_a[0], 9, &editbox_rtn[3], CHAR_FLAG);
	AddEditBox(4, 11, 10, (char *)&typ_charlist_a[0], 9, &editbox_rtn[4], CHAR_FLAG);
	AddEditBox(4, 12, 10, (char *)&typ_charlist_a[0], 9, &editbox_rtn[5], CHAR_FLAG);
	AddButton(6, 5, "ȷ��", DefOkClick);
	AddButton(6, 11, "ȡ��", DefEscClick);
	if(CONTROL_OK_EXIT == ShowControlPage())   //���ò˵��༭������������ȴ�����������
	{	
		for(i=0; i<6; i++)   ///���õ���������editbox_rtn[]�У�typ_charlist_a[0]��ֵΪ0
		editbox_rtn[i] += typ_charlist_a[0];
		CharToBcd(pwd, 3, editbox_rtn);   ///���ַ���ת��ΪBCD��
		///if(pwd[0]!=para_term.com_pwd[0])return 1;
		///if(pwd[1]!=para_term.com_pwd[1])return 1;
		///if(pwd[2]!=para_term.com_pwd[2])return 1;

		return 0;  ///����ֵΪ0������������ȷ
	}
		return 1;
}


/**
* @brief       ����ѡ��������
* @return      0����ɹ���1����ʧ��
*/
int SelTemSn(void)
{
	unsigned char editbox_rtn[6];

	//DisplayNormal("�������ز������:",17,2*16-MENU_LINE_POS,0);
	CleanPartOfScreen(2*16-MENU_LINE_POS,7);
	DisplayNormal("�����������:",17,2*16-MENU_LINE_POS,0);
	AddEditBox(4, 8, 3, (char *)&typ_charlist_a5[0], 0, &editbox_rtn[0], CHAR_FLAG);
	AddEditBox(4, 9, 10, (char *)&typ_charlist_a[0], 0, &editbox_rtn[1], CHAR_FLAG);	
	AddEditBox(4, 10, 10, (char *)&typ_charlist_a[0], 0, &editbox_rtn[2], CHAR_FLAG);
	AddEditBox(4, 11, 10, (char *)&typ_charlist_a[0], 0, &editbox_rtn[3], CHAR_FLAG);
	DisplayNormal("      (3-2040)",15,5*16-MENU_LINE_POS,0);

	AddButton(7, 5, "ȷ��", DefOkClick);
	AddButton(7, 11, "ȡ��", DefEscClick);

	if(CONTROL_OK_EXIT == ShowControlPage())   //���ò˵��༭������������ȴ�����������
	{	
		sn_num = 1000*editbox_rtn[0]+100*editbox_rtn[1]+10*editbox_rtn[2]+editbox_rtn[3];
		PrintLog(0, "sn_num= %d.\r\n", sn_num);
		if(sn_num<=2 || sn_num>2039)
		{
			//ClearLcdScreen();
			//scrpbuf_main.line = 1;
			//strcpy(scrpbuf_main.buf[0], " ���������Ŵ���!");
			//strcpy(scrpbuf_main.buf[1], " ϵͳ5���λ...");
			//DisplayNormal((unsigned char*)&scrpbuf_main.buf[0],strlen(&scrpbuf_main.buf[0]),16 * 3,0);
			//DisplayNormal((unsigned char*)&scrpbuf_main.buf[1],strlen(&scrpbuf_main.buf[1]),16 * 4,0);
			//DisplayLcdBuffer();
			return 1;
		}
		else
			return 0; 
	}
	return 1;
}

int input_date(void)
{
	unsigned char editbox_rtn[8];

	//DisplayNormal("�������ز������:",17,2*16-MENU_LINE_POS,0);
	CleanPartOfScreen(2*16-MENU_LINE_POS,7);
	DisplayNormal("�������ѯ����:",15,2*16-MENU_LINE_POS,0);
	AddEditBox(4, 5, 3, (char *)&typ_charlist_a[0], 2, &editbox_rtn[0], CHAR_FLAG);
	AddEditBox(4, 6, 10, (char *)&typ_charlist_a[0], 0, &editbox_rtn[1], CHAR_FLAG);	
	AddEditBox(4, 7, 10, (char *)&typ_charlist_a[0], 1, &editbox_rtn[2], CHAR_FLAG);
	AddEditBox(4, 8, 10, (char *)&typ_charlist_a[0], 1, &editbox_rtn[3], CHAR_FLAG);
	//DisplayNormal(".",1,4*16-MENU_LINE_POS,80+24);
	DisplayNormal("-",1,4*16-MENU_LINE_POS,8 * 8 + 8);
	AddEditBox(4, 10, 10, (char *)&typ_charlist_a[0], 0, &editbox_rtn[4], CHAR_FLAG);
	AddEditBox(4, 11, 10, (char *)&typ_charlist_a[0], 1, &editbox_rtn[5], CHAR_FLAG);
	DisplayNormal("-",1,4*16-MENU_LINE_POS,11 * 8 + 8);
	AddEditBox(4, 13, 10, (char *)&typ_charlist_a[0], 0, &editbox_rtn[6], CHAR_FLAG);
	AddEditBox(4, 14, 10, (char *)&typ_charlist_a[0], 1, &editbox_rtn[7], CHAR_FLAG);
	

	AddButton(7, 5, "ȷ��", DefOkClick);
	AddButton(7, 11, "ȡ��", DefEscClick);

	if(CONTROL_OK_EXIT == ShowControlPage())   //���ò˵��༭������������ȴ�����������
	{	
		qur_day[0] = 10*editbox_rtn[2]+editbox_rtn[3];
		qur_day[1] = 10*editbox_rtn[4]+editbox_rtn[5];
		qur_day[2] = 10*editbox_rtn[6]+editbox_rtn[7];
		//sn_num = 1000*editbox_rtn[0]+100*editbox_rtn[1]+10*editbox_rtn[2]+editbox_rtn[3];
		PrintLog(0, "sn_num= %d.\r\n", sn_num);
		if(qur_day[1]<1 || qur_day[1]>12 || qur_day[2]<1 || qur_day[2] >31)
		{
			//ClearLcdScreen();
			//scrpbuf_main.line = 1;
			//strcpy(scrpbuf_main.buf[0], " ���������Ŵ���!");
			//strcpy(scrpbuf_main.buf[1], " ϵͳ5���λ...");
			//DisplayNormal((unsigned char*)&scrpbuf_main.buf[0],strlen(&scrpbuf_main.buf[0]),16 * 3,0);
			//DisplayNormal((unsigned char*)&scrpbuf_main.buf[1],strlen(&scrpbuf_main.buf[1]),16 * 4,0);
			//DisplayLcdBuffer();
			return 1;
		}
		else
			return 0; 
	}
	return 1;
}




/**
* @brief       ����ѡ��1�������
* @return      0����ɹ���1����ʧ��
*/
int SelTaskNum1(void)
{

	unsigned char editbox_rtn[6];
	DisplayNormal("�����������:",15,2*16-MENU_LINE_POS,0);
	DisplayNormal("        (1-8)",14,5*16-MENU_LINE_POS,0);
	AddEditBox(4, 10, 8, (char *)&typ_charlist_a2[0], 0, &editbox_rtn[0], CHAR_FLAG);
	AddButton(6, 5, "ȷ��", DefOkClick);
	AddButton(6, 11, "ȡ��", DefEscClick);
	///ֻ�а���ȷ����ȡ�������˳�
	if(CONTROL_OK_EXIT == ShowControlPage())   //���ò˵��༭������������ȴ�����������
	{
		task_num = editbox_rtn[0] + 1;
		return 0; 
	}
		return 1;
}
/**
* @brief       ����ѡ��2�������
* @return      0����ɹ���1����ʧ��
*/
int SelTaskNum2(void)
{
	unsigned char editbox_rtn[6];	
	DisplayNormal("�����������:",15,2*16-MENU_LINE_POS,0);
	DisplayNormal("       (1-16)",13,5*16-MENU_LINE_POS,0);
	AddEditBox(4, 9, 2, (char *)&typ_charlist_a3[0], 0, &editbox_rtn[0], CHAR_FLAG);	
	AddEditBox(4, 10, 10, (char *)&typ_charlist_a4[0], 1, &editbox_rtn[1], CHAR_FLAG);
	AddButton(7, 5, "ȷ��", DefOkClick);
	AddButton(7, 11, "ȡ��", DefEscClick);
	///ֻ�а���ȷ����ȡ�������˳�
	if(CONTROL_OK_EXIT == ShowControlPage())   //���ò˵��༭������������ȴ�����������
	{	
		task_num = editbox_rtn[0]*10+editbox_rtn[1];
		return 0; 
	}
		return 1;
}
/**
* @brief       ����ѡ��˿ں�
* @return      0����ɹ���1����ʧ��
*/
int SelPortNum(void)
{
	unsigned char editbox_rtn[6];

	DisplayNormal("������˿ں�:",13,2*16-MENU_LINE_POS,0);	
	AddEditBox(4, 10, 4, (char *)&typ_charlist_a1[0], 0, &editbox_rtn[0], CHAR_FLAG);
    DisplayNormal("        (1-4)",13,5*16-MENU_LINE_POS,0);
	AddButton(7, 5, "ȷ��", DefOkClick);
	AddButton(7, 11, "ȡ��", DefEscClick);
	///ֻ�а���ȷ����ȡ�������˳�
	if(CONTROL_OK_EXIT == ShowControlPage())   //���ò˵��༭������������ȴ�����������
	{	
		input_port_num = editbox_rtn[0];
		return 0; 
	}
	return 1;
}
/**
* @brief       ����ѡ���ܱ��ƺ�
* @return      0����ɹ���1����ʧ��
*/
int SelMajorNum(void)
{
	unsigned char editbox_rtn[6];
	///unsigned char rtn[3];
	DisplayNormal("�������ܱ��:",13,2*16-MENU_LINE_POS,0);
	DisplayNormal("        (1-8)",13,5*16-MENU_LINE_POS,0);	
	AddEditBox(4, 10, 8, (char *)&typ_charlist_a2[0], 0, &editbox_rtn[0], CHAR_FLAG);
	AddButton(6, 5, "ȷ��", DefOkClick);
	AddButton(6, 11, "ȡ��", DefEscClick);
	if(CONTROL_OK_EXIT == ShowControlPage())   //���ò˵��༭������������ȴ�����������
	{	
		major_met_num = editbox_rtn[0] + 1;
		return 0; 
	}
	return 1;
}

/**
* @brief           �õ�����Ķ˿ں�
* @return          �ܱ��
*/
unsigned int GetPortNum()
{
	return input_port_num;
}
/**
* @brief           �õ�����������
* @return          �ܱ��
*/
unsigned char GetTaskNum()
{
	return task_num;
}
/**
* @brief           �õ�����ĵ���
* @return          �ܱ��
*/
unsigned short GetSnNum()
{
	return sn_num;
}
/**
* @brief           �õ�������ܱ��
* @return          �ܱ��
*/
unsigned int GetMajorMetNum()
{
	return major_met_num;
}


unsigned char *GetQurDay()
{
	return qur_day;
}



