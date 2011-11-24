/**
* scr_keyboard.c -- ��������
* 
* 
* ����ʱ��: 2010-5-20
* ����޸�ʱ��: 2010-7-10
*/

#include "scr_menu.h"
#include "scr_lcd.h"

///#define LOOP_TIMEOUT	300
#define LOOP_CYCLE		30 //3S
#define LOOP_TIMEOUT	300 //30s


static char key_pushed = 0;
static char loop_state = 0;

/**
* @brief           ��ȡ����ֵ
* @return          ����ֵΪ��ȡ���İ���ֵ
*/
char KeyKbHit(void)
{
	///static int count = LOOP_TIMEOUT;   ///��ʼֵΪLOOP_TIMEOUT��Ĭ�Ͽ��������Ѳ��
	static int count = 0;             ///�����ȴ�һ����ٽ�������
	static int state = 1;   ///״̬Ĭ��ֵΪ1
    ///�����İ���ֵ
	char ch = LcdGetKey();  ///0.1��Ŷ�һ��
	
	int count_max ;

	if(key_pushed) 
	{
		ch = key_pushed;   ///�������ⰴ��
		key_pushed = 0;
	}
    
	if(0 == ch)   ///û�г�ʱ�䰴������ʱ
	{
		if(0 == state) 
		{
			count_max = LOOP_CYCLE;   ///Ѳ�Է�ҳʱ����
			loop_state = 1;
		}
		else 
		{
			count_max = LOOP_TIMEOUT; ///����Ѳ��ʱ����
			loop_state = 2;
		}

		count++;
		if(count >= count_max)  ///�������ļ���
		{
			state = 0;
			count = 0;      ///������0���ֿ�ʼ����Ѳ��
			return LOOPKEY;   ///����ֵΪ��ҳ��ʱ������������Ϊ��ҳ��ʱ�������
		}
	}
	
	
	else  ///����м�����
	{
		state = 1;   ///״̬��Ϊ1
		count = 0;  ///������0
		loop_state = 0;
	}

	return ch;     ///���ض����İ���ֵ
}

void PushKey(char key)
{
	key_pushed = key;
}

char GetLoopState()
{
	return loop_state;
}

void SetLoopState(unsigned char LoopState)
{
	loop_state = LoopState ;
}


