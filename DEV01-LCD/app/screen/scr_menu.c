/**
* scr_menu.c -- �˵���ʾ����
* 
* 
* ����ʱ��: 2010-5-20
* ����޸�ʱ��: 2010-7-10
*/

#include <string.h>
#include "scr_menu.h"
#include "scr_lcd.h"
#include "scr_input_api.h"
#include "include/debug.h"
#include "include/screen/scr_show.h"
#include "scr_lcd.h"

static MNU_ITEMP   m_menu_sys;  
scrpage_t scrpbuf_main;// ��ʾ�˵�������
unsigned char loopuseflag=0; 
static pscrpage_t pcurshow;   ///��ǰ��ʾ������ָ��
unsigned char loopshowfalg = 0 ;
static int curshowlineidx,curshowcolidx;  
static int loopfuncidx;
static MNU_LOOP_FUNC * mlpf;
static MNU_LOOP_FUNC *mfloop;


void PushKey(char key);
int  SelTemSn(void);
int input_date(void);
void InitControlPage(void);
int ScrPwdConfirm(void);
int SelTaskNum1(void);
int SelTaskNum2(void);
int SelMajorNum(void);
int SelPortNum(void);
char KeyKbHit(void);
void DisplayPageNumber(int number,unsigned char x);
char GetLoopState();
void SetLoopState(unsigned char LoopState);
/**
* @brief           �õ��˵�������λ��
* @param item      �˵����Խṹ�����
* @return          ���صİ���ֵ
*/
unsigned int GetMenuSelPosition(MNU_ITEMP item)
{
	unsigned char xkey,x=0,start,end,i=0;
	start = 0;  
	end   = 0; 
	
	if(item->item_num == 0) return (NULLKEY); 
	if(item->item_cur == NULLKEY)   
	{   
		item->item_cur = 0; 
		xkey = NULLKEY;
		goto mnushow;
	}
	if(item->item_action == DO_SHOW_CHG)
	{  
		xkey = NULLKEY;
		goto mnushow;
	}
	
	xkey = KeyKbHit();
	

	if(xkey==0)return (0);  
	if(m_menu_sys->item_action==DO_FUNC) return(xkey);
	if(xkey != DOWNKEY && xkey != UPKEY)return xkey;
	if(xkey == DOWNKEY&&loopshowfalg!=1) 
	{
		if(item->item_cur < item->item_num-1) 
		item->item_cur++; 
		else 
		item->item_cur = 0;
	}
	if(xkey == UPKEY&&loopshowfalg!=1)  
	{
		if(item->item_cur)
		item->item_cur--; 
		else 
		item->item_cur = item->item_num-1;
	}
	
mnushow:
	if(loopshowfalg!=1)
	{
		if(item->item_cur >= PG_MNU_MAX)  
		{
			start = item->item_cur+1-PG_MNU_MAX;  
			end   = item->item_cur+1;
			CleanPartOfScreen(16 * (i+2)-MENU_LINE_POS,7);
			for(i = 0; i < 7; i++)
		 	{
				DisplayNormal((unsigned char*)(item->sub_attr[start+i].title),strlen(item->sub_attr[start].title),16 * (i+2)-MENU_LINE_POS,0);
				SetLineInvs(8*16-MENU_LINE_POS);
		 	}
		}
		else 
		{
			start = 0;
			if(item->item_num<PG_MNU_MAX)
				end = item->item_num;
			else 
				end = PG_MNU_MAX;

		 	CleanPartOfScreen(16 * (x+2)-MENU_LINE_POS,7);
		 	for(x = start; x < end; x++)
		 	{
				DisplayNormal((unsigned char*)(item->sub_attr[x].title),strlen(item->sub_attr[x].title),16 * (x+2)-MENU_LINE_POS,0);
				if(x == item->item_cur) 
					SetLineInvs(16 * (x+2)-MENU_LINE_POS);
		 	}
		}
		CleanPageNum();
		CleanLoopMetNum();
		DisplayLcdBuffer();   
	}
	return (xkey);   
}

/**
* @brief           �ñ�ϵͳ�ĸ�������˵�����һ������
* @param m_menux   Ҫ����Ĳ˵�����
*/
void LoadAllMenu(PMNU_ATTR m_menux)
{	
	unsigned char  i;
	
	for(;m_menux->mnu != MENU_NULL;m_menux++)
	{
		m_menux->mnu->father = m_menux->root_mnu;
		m_menux->mnu->sub_attr = m_menux->mnu_item;		
		m_menux->mnu->item_cur=NULLKEY; 

		i = 0;
		while(m_menux->mnu->sub_attr[i].title != MENU_NULL)
		{
			i++;  
		}
		m_menux->mnu->item_num = i;  
		m_menux->mnu->item_action = DO_SHOW; 
	}
	
}

/**
* @brief           ��ʼ��ϵͳ�˵�
* @param xmenu     �����˵�
* @param xlpf      ���Բ˵�
*/
void InitSysMenu(MNU_ITEMP xmenu,MNU_LOOP_FUNC * lpf)
{
	m_menu_sys = xmenu;    
	mlpf = mfloop = lpf;       
	pcurshow = &scrpbuf_main;
	ClrShowBuf(&scrpbuf_main); 
	curshowcolidx = 0;  ///��
	curshowlineidx = 0; ///��
	loopshowfalg = 0;   ///Ѳ��
	loopfuncidx = 0;    ///Ѳ�Եĺ���
}

/**
* @brief           �����ʾ������
* @param pshowp    ������ָ��
*/
void ClrShowBuf(pscrpage_t pshowp)
{
	memset((UCHAR *)pshowp, 0, sizeof(scrpage_t));
	pshowp->line = 0;
}


#if 0
/**
* @brief           ��ʾ��ҳ�˵�����
* @param xkey      ��Ӧ�İ���
* @param loop_flag ���Է�ҳ��־
*/
void ShowScrPage(unsigned char xkey,unsigned char loop_flag)
{
	int start,end,i,j;
	int tint, tdot,num;
	int sum;

	ClearMenuBuffer();

	if(xkey == DOWNKEY)
		xkey = PAGE_DOWNKEY;
	if(xkey == UPKEY)
		xkey = PAGE_UPKEY;  
	
	if(xkey == PAGE_DOWNKEY)  
	{ 
		if(curshowlineidx+PG_MNU_MAX < pcurshow->line) 
			curshowlineidx += PG_MNU_MAX;  
		else 
			curshowlineidx = 0; 
	}
	else if(xkey == PAGE_UPKEY)
	{ 
		if(curshowlineidx >= PG_MNU_MAX) 
			curshowlineidx -= PG_MNU_MAX;
		else 
			curshowlineidx = ((pcurshow->line-1)/PG_MNU_MAX)*PG_MNU_MAX;
	}
	
	tint = curshowlineidx/PG_MNU_MAX;
	num = tint+1;                     
	tint = pcurshow->line/PG_MNU_MAX;  
	tdot = pcurshow->line%PG_MNU_MAX;
	if(tdot) sum = tint+1;   
	else sum = tint;
	if(num>sum) num = sum;
	
	start = curshowlineidx;        
	end = curshowlineidx+PG_MNU_MAX;		
	CleanPartOfScreen(2*16-MENU_LINE_POS,7);

    if(loop_flag==0)
    {
    		DisplayPageNumber(num,16);
		DisplayPageNumber(sum,18);
		DisplayPageNumber(10,17);
    }
	else
		CleanPageNum();

	if(num > 0)
	{
		for(j=0,i=(num-1)*PG_MNU_MAX; i<num*PG_MNU_MAX; i++)
		{
	    	DisplayNormal((unsigned char *)pcurshow->buf[i],strlen(pcurshow->buf[i]),16 * (++j+1)-MENU_LINE_POS ,0);
		}
	}
	DisplayLcdBuffer(); 
}
#endif


/**
* @brief           ��ʾ��ҳ�˵�����
* @param xkey      ��Ӧ�İ���
* @param loop_flag ���Է�ҳ��־
*/
void ShowScrPage(unsigned char xkey,unsigned char loop_flag)
{
	int start,end,i,j;
	int tint, tdot,num;
	int sum;

	ClearMenuBuffer();

	if(xkey == DOWNKEY)
		xkey = PAGE_DOWNKEY;
	if(xkey == UPKEY)
		xkey = PAGE_UPKEY;  
	
	if(xkey == PAGE_DOWNKEY)  
	{ 
		if(curshowlineidx+PG_MNU_MAX < pcurshow->line) 
			curshowlineidx += PG_MNU_MAX;  
		else 
			curshowlineidx = 0; 
	}
	else if(xkey == PAGE_UPKEY)
	{ 
		if(curshowlineidx >= PG_MNU_MAX) 
			curshowlineidx -= PG_MNU_MAX;
		else 
			curshowlineidx = ((pcurshow->line-1)/PG_MNU_MAX)*PG_MNU_MAX;
	}
	
	tint = curshowlineidx/PG_MNU_MAX;
	num = tint+1;                     
	tint = pcurshow->line/PG_MNU_MAX;  
	tdot = pcurshow->line%PG_MNU_MAX;
	if(tdot) sum = tint+1;   
	else sum = tint;
	if(num>sum) num = sum;
	
	start = curshowlineidx;        
	end = curshowlineidx+PG_MNU_MAX;		
	CleanPartOfScreen(2*16-MENU_LINE_POS,7);

	if(loop_flag==0)
	{
		
		if(sum >= 1 && sum  <= 9)
		{
			DisplayPageNumber(num,16);
			DisplayPageNumber(sum,18);
			DisplayPageNumber(10,17);
		}
		
		else if(sum >= 10 && sum <= 99)
		{
			if(num >= 10 && num <= 99)
			{
				DisplayPageNumber(num / 10,15);
				DisplayPageNumber(num % 10,16);
				DisplayPageNumber(10,17);
				DisplayPageNumber(sum / 10,18);
				DisplayPageNumber(sum % 10,19);
			}
			else if(num >= 1 && num <= 9)
			{
				DisplayPageNumber(num,16);
				DisplayPageNumber(10,17);
				DisplayPageNumber(sum / 10,18);
				DisplayPageNumber(sum % 10,19);
			}
		}
		
		else if(sum >= 100 && sum <= 999)
		{
			if(num >= 100 && num <= 999)
			{
				DisplayPageNumber(num /100,13);
				DisplayPageNumber((num %100)/10,14);
				DisplayPageNumber((num %100)%10,15);
				DisplayPageNumber(10,16);
				DisplayPageNumber(sum /100,17);
				DisplayPageNumber((sum %100)/10,18);
				DisplayPageNumber((sum %100)%10,19);
			}
			else if(num >= 10 && num <= 99)
			{
				DisplayPageNumber(num / 10,14);
				DisplayPageNumber(num % 10,15);
				DisplayPageNumber(10,16);
				DisplayPageNumber(sum /100,17);
				DisplayPageNumber((sum %100)/10,18);
				DisplayPageNumber((sum %100)%10,19);
			}
			else if(num >= 1 && num <= 9)
			{
				DisplayPageNumber(num,15);
				DisplayPageNumber(10,16);
				DisplayPageNumber(sum /100,17);
				DisplayPageNumber((sum %100)/10,18);
				DisplayPageNumber((sum %100)%10,19);
			}
		}
		
	}
	else
		CleanPageNum();

	if(num > 0)
	{
		for(j=0,i=(num-1)*PG_MNU_MAX; i<num*PG_MNU_MAX; i++)
		{
	    	DisplayNormal((unsigned char *)pcurshow->buf[i],strlen(pcurshow->buf[i]),16 * (++j+1)-MENU_LINE_POS ,0);
		}
	}
	DisplayLcdBuffer(); 
}




/**
* @brief		����������ʾ
*/
void ShowDspBufLoop(void)
{	
	
	Dsp_loop:		
    
	if(mlpf[loopfuncidx].mpmenu_do_func != MENU_NULL)   ///���Խ���
	{
		if(curshowlineidx == 0 && loopuseflag==0)
		{
		    pcurshow = &scrpbuf_main;
			ClrShowBuf(&scrpbuf_main);  ///�����µ���ʾ����
        	scrpbuf_main.line = 0;
			mlpf[loopfuncidx].mpmenu_do_func(mlpf[loopfuncidx].para.id,mlpf[loopfuncidx].para.para,mlpf[loopfuncidx].para.key);
			if(0 == scrpbuf_main.line) 
			{
				loopfuncidx++;   ///��һ����ʾ����
				loopuseflag=0; 
				goto Dsp_loop;
			}

			if(scrpbuf_main.line >7)  ///˵��ͬһ����ʾ������Ҫ��ҳ
			{
					ShowScrPage(OKKEY,1); 
					loopuseflag=1;   ///ͬһ����ʾ������Ҫ��ҳ��־
					return;			
			}
						
		}
	}
	else 
	{//NULL function
		if(loopfuncidx==0)
			DefMenuFuc(0,0,NULLKEY);
		else 
		{
			loopfuncidx = 0;  ///��ʾ�ص�����0�����¿�ʼѲ��
			loopuseflag=0;    
			return;
		}
	}

	ShowScrPage(PAGE_DOWNKEY,1);  ///��ʾ�����scrpbuf_main

	if(curshowlineidx != 0)
	{
		if(curshowlineidx+PG_MNU_MAX >=scrpbuf_main.line)
		{	
			curshowlineidx=0; ///��ǰ�б���0
			loopfuncidx++; ///ת����һ����ʾ����
			loopuseflag=0; ///��0
			return;
		}
	}

	
	if(curshowlineidx == 0)   ///�����ǰ��Ϊ0
	{
		loopfuncidx++;  ///ת����һ����ʾ����
		loopuseflag=0;
	}
}

//���ص����˵�
///�ڽ�������֮ǰ����m_menu_sys���ص�����ĸ��˵��ڵ�
/**
* @brief           ���ص����˵�
*/
void ReturnRoot(void)
{
	while(m_menu_sys->father != MENU_NULL)
	{
		m_menu_sys = m_menu_sys->father;  ///һֱ���ص����ϲ�˵�
	}
	m_menu_sys->item_action = DO_SHOW; 
	m_menu_sys->item_cur = NULLKEY;
}


/**
* @brief           ���ص����Խ���
*/
void ReturnToLoop(void)
{
	while(m_menu_sys->father != MENU_NULL)
	{
		m_menu_sys = m_menu_sys->father;  ///һֱ���ص����ϲ�˵�
	}
}

/**
* @brief           �˵���ʾ����
* @return          ����ֵ
*/
unsigned char MenuProcess(void)
{
	unsigned char ch ;
	unsigned char id = 0;
	unsigned long int para = 0;
	
	ch = GetMenuSelPosition(m_menu_sys);	
		///���Դ���
    if(ch == LOOPKEY&&GetLoopState()==1)   ///�������Ժ�ҳ
	{
		loopshowfalg = 1;
		ShowDspBufLoop();    ///��ʾ����
     }
	 else if(ch == LOOPKEY&&GetLoopState()==2)  ///������������
	{
		LcdBakLight(0);
		CloseLcdBakLightState();
		loopshowfalg = 1;
		ReturnRoot();
		CleanPartOfScreen(2*16-MENU_LINE_POS,7);
		DisplayLcdBuffer(); 
		curshowcolidx = 0;  ///��
		curshowlineidx = 0; ///��
		loopfuncidx = 0;    ///�ص�Ѳ�Եĵ�һ������
		pcurshow = &scrpbuf_main;
		ClrShowBuf(&scrpbuf_main); 
		mlpf[loopfuncidx].mpmenu_do_func(mlpf[loopfuncidx].para.id,mlpf[loopfuncidx].para.para,mlpf[loopfuncidx].para.key);
		ShowDspBufLoop();    ///��ʾ����
     }
	 ///�����˳�����
	 else if((ch == ESCKEY||ch == OKKEY||ch == UPKEY||ch == DOWNKEY||ch == LEFTKEY||ch == RIGHTKEY)&&loopshowfalg == 1)
	 {
	  	ch = 0;
		loopshowfalg = 0;
	 	pcurshow = &scrpbuf_main;
		ClrShowBuf(&scrpbuf_main); 
		CleanPartOfScreen(2*16-MENU_LINE_POS,7);
		CleanLoopMetNum();
		curshowcolidx = 0;  ///��
		curshowlineidx = 0; ///��
		loopshowfalg = 0;   ///Ѳ��
		loopfuncidx = 0;    ///�ص�Ѳ�Եĵ�һ������
	 	ReturnRoot();
	 }

	if(m_menu_sys->item_action == DO_SHOW_CHG)
		m_menu_sys->item_action = DO_SHOW; 
    ///��ײ㷭ҳ�˵���ʾ
	if(m_menu_sys->item_action == DO_FUNC)  
	{ 
		ShowScrPage(ch,0);  	
		switch(ch)  
		{
		case OKKEY: 
			break;
		case ESCKEY: 
			m_menu_sys->item_action = DO_SHOW_CHG;   ///�뿪DO_FUNC����
			break;
		case EXITKEY://�˳��˵����ڵ��ȷ��ʱ�����˰���
			m_menu_sys->item_action = DO_SHOW_CHG;
          	break;
		default:	break;
		}
	}
	///�Ƿ�ҳ�����ֲ˵���ʾ
	else 
	{
		switch(ch)
		{
		case OKKEY:    ///���뵽��һ��˵�
			///���û���Ӳ˵�
			if(m_menu_sys->sub_attr[m_menu_sys->item_cur].mpmenu_do_func != MENU_NULL)
			{
				curshowlineidx = 0;
			  	#if PARA_SET    
			    if(m_menu_sys->sub_attr[m_menu_sys->item_cur].para.key == OKKEY2) 
				{
					ClrShowBuf(&scrpbuf_main);
					InitControlPage();
					if(SelTemSn())  
					{
						m_menu_sys->item_action = DO_SHOW_CHG; 
						break;
					}
				}
				
				///else if(m_menu_sys->sub_attr[m_menu_sys->item_cur].para.key == OKKEY5) 
				///{
				///	ClrShowBuf(&scrpbuf_main);
				///	InitControlPage();
				///	if(SelPortNum())  
				///	{
				///		m_menu_sys->item_action = DO_SHOW_CHG; 
				///		break;
				///	}
				///}

				else if(m_menu_sys->sub_attr[m_menu_sys->item_cur].para.key == OKKEY3) 
				{
					ClrShowBuf(&scrpbuf_main);
					InitControlPage();
					if(SelTaskNum1())  
					{
						m_menu_sys->item_action = DO_SHOW_CHG;  
						break;
					}
				}
				
				else if(m_menu_sys->sub_attr[m_menu_sys->item_cur].para.key == OKKEY4) 
				{
					ClrShowBuf(&scrpbuf_main);
					InitControlPage();
					if(SelTaskNum2())  
					{
						m_menu_sys->item_action = DO_SHOW_CHG;  
						break;
					}
				}
				else if(m_menu_sys->sub_attr[m_menu_sys->item_cur].para.key == OKKEY7) 
				{
					ClrShowBuf(&scrpbuf_main);
					InitControlPage();
					if(input_date()) 
					{
						m_menu_sys->item_action = DO_SHOW_CHG; 
						break;
					}
				}

				#endif
			   	id = m_menu_sys->sub_attr[m_menu_sys->item_cur].para.id;
			   	para =  m_menu_sys->sub_attr[m_menu_sys->item_cur].para.para;
			   	ClrShowBuf(&scrpbuf_main);
			   	m_menu_sys->sub_attr[m_menu_sys->item_cur].mpmenu_do_func(id,para,NULLKEY);				
               	PushKey(OKKEY);
			   	m_menu_sys->item_action = DO_FUNC; ///����DO_FUNC�ж�ȥִ��
			}
			///������Ӳ˵�
			else if(m_menu_sys->sub_attr[m_menu_sys->item_cur].son != MENU_NULL)
			{
                curshowlineidx = 0;
				if(m_menu_sys->sub_attr[m_menu_sys->item_cur].para.key == OKKEY) 
				{
					ClrShowBuf(&scrpbuf_main);
					InitControlPage();
					if(ScrPwdConfirm())  
					{
						m_menu_sys->item_action = DO_SHOW_CHG;  
						break;
					}
				}
    			else if(m_menu_sys->sub_attr[m_menu_sys->item_cur].para.key == OKKEY1) 
    			{
    				ClrShowBuf(&scrpbuf_main);
    				InitControlPage();
    				if(SelTemSn())  
    				{
    					m_menu_sys->item_action = DO_SHOW_CHG;  
    					break;
    				}
    			}
				
				
	            else if(m_menu_sys->sub_attr[m_menu_sys->item_cur].para.key == OKKEY6) 
				{
					ClrShowBuf(&scrpbuf_main);
					InitControlPage();
					if(SelMajorNum()) 
					{
						m_menu_sys->item_action = DO_SHOW_CHG; 
						break;
					}
				}


				
	     		m_menu_sys = (MNU_ITEMP)m_menu_sys->sub_attr[m_menu_sys->item_cur].son;
	            m_menu_sys->item_action = DO_SHOW_CHG; 
				m_menu_sys->item_cur = NULLKEY;
			}
            ///δ����˵�����
			else  
			{
				curshowlineidx = 0;
				DefMenuFuc(0,0,NULLKEY);  
				m_menu_sys->item_action = DO_FUNC;
			}
			break;
		case ESCKEY:   ///�˵���һ��˵�
			if(m_menu_sys->father!=MENU_NULL)
			{   
				m_menu_sys = m_menu_sys->father;  
				m_menu_sys->item_action = DO_SHOW_CHG; 
			}
			break;
		default:	break;
		}
	}

     
	return ch;   
}
/**
* @brief			Ĭ�ϲ˵�����
* @param mid		���
* @param arg		��ʾ����
* @param key		����ֵ
*/
void DefMenuFuc(unsigned char id,unsigned long int x,unsigned char key)
{
	//strcpy(scrpbuf_main.buf[0],"δ����˵���" );
	//strcpy(scrpbuf_main.buf[1],"δ����˵���" );
	//strcpy(scrpbuf_main.buf[2],"δ����˵���" );              
	//scrpbuf_main.line = 3;              
	strcpy(scrpbuf_main.buf[0],"         " );           
	scrpbuf_main.line = 1;       
}                    




