/**************************************************************
** Copyight (C) 2002-2006 ��������������
** All right reserved
***************************************************************
**
** ��Ŀ���ƣ�XJDN ���������
** ���ܼ�飺XJ_ASSERT �򵥲��Կ��
**
** ԭʼ���ߣ��ܾ���  Email��zhoujingyu@xjgc.com zjy@cnpack.org
** ��֯���ţ���̼��� ��������������
** ��    ע��
** ����ʱ�䣺2006-11-30
** ���ʱ�䣺2006-11-30
** 20090919 ����printbuffer,��ӡ������������ݣ����ڵ��� ��ΰ
** ��    ����1.0
***************************************************************
** ��Ԫ��ʶ��$Id: xj_assert.h,v 1.1 2009/12/20 11:17:05 yjb Exp $
** �汾��ʷ��
** �� �� �ߣ�
** ����޸ģ�
**************************************************************/

#ifndef XJ_ASSERT_H
#define XJ_ASSERT_H

#ifdef XJ_ASSERT_DECLARE

/* ��ʼ�����Ի��� */
void inittest();

/* ������Ի��� */
void finaltest();

/* ���Զ��ԣ�exp ΪҪ���Եı��ʽ�����msg Ϊ���ʽ���Ϊ 0 ʱ�Ĵ�����Ϣ */
void assert( int exp, const char * msg );

//������������ݣ����ڵ��Բ�������
void printbuffer( unsigned char buffer[], int len, char split );

#else

#include <stdio.h>
//#include <string.h>

static int _TestCount  = 0;  /* ���Լ��� */
static int _ErrorCount = 0;  /* ������� */

/* ��ʼ�����Ի��� */
void inittest()
{
    _TestCount  = 0;
    _ErrorCount = 0;
}

//������������ݣ����ڵ��Բ�������
void printbuffer( unsigned char buffer[], int len, char split )
{
    int i;
    for ( i = 0; i < len; i ++)
    {
        fprintf( stdout, "%02x", buffer[i] );
        if( i > 0 &&(( i % 16 )== 15 )) fprintf( stdout, "\n" );//16�ֽ�һ��
        else if( i != len - 1 ) fprintf( stdout, "%c", split ); //������ַ����ָ���
        else fprintf( stdout, "\n" );                         //�����ַ�
    }
}

/* ������Ի��� */
void finaltest()
{
    if ( _ErrorCount > 0 )
        printf( "%d failed in %d tests.\n", _ErrorCount, _TestCount );
    else
        printf( "%d tests passed.\n", _TestCount );
}

/* ���Զ��ԣ�exp ΪҪ���Եı��ʽ�����msg Ϊ���ʽ���Ϊ 0 ʱ�Ĵ�����Ϣ */
void assert( int exp, const char * msg )
{
    _TestCount ++;
    if (exp == 0)
    {
        _ErrorCount ++; //asdfasdf
        printf( msg );
        //if ( msg[strlen( msg ) - 1] != '\n' )
        printf( "\n" );
    }
}

void p_err(int exp)
{
	//_TestCount ++;
	if (exp != 0)
	{
	//	_ErrorCount ++;
//		printf(msg);
		switch(exp)
		{
			case -1:
				printf(" error: ERR_SYS\n");
				break;
			case -2:
				printf(" error: ERR_NODEV/ERR_NOFILE\n");
				break;
			case -3:
				printf(" error: ERR_TIMEOUT\n");
				break;
			case -4:
				printf(" error: ERR_INVAL\n");
				break;
			case -5:
				printf(" error: ERR_NOFUN\n");
				break;
			case -6:
				printf(" error: ERR_BUSY\n");
				break;
			case -7:
				printf(" error: ERR_NOINIT\n");
				break;
			case -8:
				printf(" error: ERR_NOMEM\n");
				break; 
			case -9:
				printf(" error: ERR_NODISK\n");
				break; 
			case -10:
				printf(" error: ERR_NORECORD\n");
				break; 
			case -11:
				printf(" error: ERR_CFG\n");
				break; 
			case -12:
				printf(" error: ERR_NOCFG\n");
				break; 
			case -13:
				printf(" error: ERR_DEVUNSTAB\n");
				break; 
			case -14:
				printf(" error: ERR_DISCONNECT\n");
				break;
			case -80:
				printf(" error: ERR_OTHER\n");
				break;
			default:
				printf("\n");
				break;
		}
				
	}
}

#endif                  /* XJ_ASSERT_DECLARE */

#endif                  /* XJ_ASSERT_H */
