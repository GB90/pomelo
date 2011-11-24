/***************************************************************************************
* 
* Copyright (c) 2008, �������з��Ƽ����޹�˾
* All rights reserved.
* �ļ����ƣ�neo_tcpip_demo.cpp
* 
* ժ    Ҫ������������û���Ϥ�����ͷ���TCP�������̽��вο��������в��ֺ���û�к�����
*           �����Ѿ�ע���ú�������Ӧ���ܣ����û������Լ�ʹ�õ�Ӳ������ӣ��û�Ҳ����
*			�����Լ�����Ҫ�Ա����̽��в��ֵ������Ż���
* ��ǰ�汾��1.0
* 
* ��    �ߣ�kevin
* 
* �ο����ϣ����з�GPRSģ��_GPRS_PPP_TCP_��ϸ����.pdf��
* 
* ��˾��ַ��http://www.neoway.com.cn
* 
* ģ��ϵ�У�GRPS: M580/M580i/M580z/M590 (ȫ���׿оƬGPRS��ҵģ��)
*
* �� �� ������ߣ������ʣ� Let's enjoy the wireless life!
* 
* �޸ļ�¼��
* ʱ�䣺20100416
* �汾��V1.1
* ���ݣ����Ӷ���·��Ӧ�ã�
****************************************************************************************/

unsigned char gSendBuffer[256];
unsigned char gRecieveBuffer[256];


/***************************************************************************************
*��������TCPIP_CreateProcess
 
*��  ��������TCPIP���ӵ��������̣���������ź�ǿ�ȣ�CCID,����ע��״̬�������ڲ�Э��ջ��
	     ����APN���ʽڵ㣬����PPP���ӣ�����TCP ���������ӣ�
         ��ע��һ����·���ֻ������һ��TCP���ӻ���UDP���ӣ��������Ӷ�����ӡ�
*****************************************************************************************/ 
BOOL CTCPIPDlg::TCPIP_CreateProcess()
{
	///////////////////////////////////////////////////////////////////////////////
	// (1) ���PBREADY;
	if( !CheckPBReady() ) 
	{// ģ���ϵ�󣬻���г�ʼ������ʼ����Ϻ󣬻��ͳ�+PBREADY;���û���յ�������ģ�鴮���Ƿ�ͨ��
		if( !CheckAT() ) 
			return FALSE;
		// ���ڲ�ͨ�����鲨�����Ƿ�һ�£�ģ��Ĭ����115200������ͨ��AT+IPR�����ò����ʣ�ֻ��Ҫ����һ�Σ�
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// (2) ���SIM����
	if( !CheckCCID() )
	{ 
		return FALSE;// ��ȡSIM��ʧ�ܣ�������û�в忨���߿��Ӵ�������
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// (3) ����ź�ǿ�ȣ�
	if( !CheckCSQ() )
	{
		return FALSE;// û���źţ�
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// (4) ��ѯ����ע��״̬���緵�� +CREG: 0,1��+CREG: 0,5 ������ѭ�����糬ʱ������;
	if( !CheckCreg() ) 
	{
		return FALSE;// ����ע��ʧ�ܣ�
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// (5) ���ó��ڲ�Э��ջ
	SendATCommand("at+xisp=0\r",10);//����"at+xisp=0
	
	TCPIP_CloseLink();// ��ܼ������������ʧ��,���Բ����жϷ���ֵ�����һ��Ҫ���ϣ���ע�⣡����
	
	///////////////////////////////////////////////////////////////////////////////
	// (6) ����APN
	SendATCommand("at+cgdcont=1,IP,CMNET\r",22);//����"at+cgdcont,��ע��APNҪ���ݸ��Ե�����������ã�
	
	///////////////////////////////////////////////////////////////////////////////
	// (7) ����һЩר���������Ҫ�û���ָ֤�����������������û���֤,	AT+XGAUTH=1,1,"GSM","1234";
	char buffer[30];
	memset(buffer,0,sizeof(buffer));
	strcpy(buffer,"AT+XGAUTH=1,1, GSM , 1234 ");
	buffer[14] = '"';
	buffer[18] = '"';
	buffer[20] = '"';
	buffer[25] = '"';
	buffer[26] = 0x0d;
	SendATCommand(buffer,27);
	
	///////////////////////////////////////////////////////////////////////////////
	// (8) ����PPP����
	if( !CreatePPP() )
	{
		return FALSE;// ����PPP����ʧ�ܣ�
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// (9) ��ѯPPP��·״̬
	if( !CheckPPPStatus() )
	{
		return FALSE;// ��ѯPPP����ʧ�ܣ�
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// (10) ����TCP���ӣ�ע������ķ�������ַ�ǿɱ䣬Ҫ���ݸ��Բ�Ʒ����������޸ģ�
	if( !CreateSingleTCPLink(0,"110.123.12.56","6800") )  
		return FALSE;
	
	return TRUE;
}

/****************************************************************************************
*��������SendATCommand

*��  ��������ָ��������ݸ��Ե�ƽ̨�������Ӧ���޸ģ�

*****************************************************************************************/ 
BOOL CTCPIPDlg::SendATCommand(char *strCmd,int len) 
{
	BOOL bRead;
	
	if( len<=0 )	return FALSE;
	
    // send to serial port
    bRead = csport.CSSend(gPortHandle,(unsigned char *)strCmd,len);

	return bRead;
}

/****************************************************************************************
*��������RecieveFromUart

*��  �����Ӵ����н������ݣ����ݸ��Ե�ƽ̨�������Ӧ���޸ģ�

*****************************************************************************************/ 
int CTCPIPDlg::RecieveFromUart(unsigned char *gRecieveBuffer) 
{
	int nLength;

	memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
	csport.CSReceive(gPortHandle,gRecieveBuffer,&nLength);

	return nLength;
}

/****************************************************************************************
*��������Delay_ms

*��  ������ʱ���������ݸ��Ե�ƽ̨�޸�һ������ĵ��ú����Ϳ����ˡ�

*****************************************************************************************/ 
void CTCPIPDlg::Delay_ms(int timelen) 
{
	Sleep(timelen);
}

/****************************************************************************************
*��������CheckCCID

*��  �������sim����

*****************************************************************************************/ 
BOOL CTCPIPDlg::CheckCCID()
{
    int timeout;  // ��ʱ����������

	timeout = 0;
	do
    {   
	    timeout++;

		SendATCommand("at+ccid\r",8); // ͨ�����ڷ���AT+CSQ�س�
		
		Delay_ms(200);                 // ��ʱ200ms��
		
		memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
		RecieveFromUart(gRecieveBuffer); //��÷���ֵ��Ϣ
		
		if( strstr((char *)gRecieveBuffer,"ERROR")==0 )
		{// ˵������ֵ����ERROR,��ʾ��ȡsim����CCID�ɹ����˳���
			break;
		}
	}while(timeout<50);	  
	
	if( timeout>=50 )
	{	// ����
		return FALSE;// ��ȡSIM��ʧ�ܣ�������û�в忨���߿��Ӵ�������
	}

	return TRUE; 
}


/****************************************************************************************
*��������CheckPBReady

*��  ��������Ƿ��յ�+PBREADY��

*****************************************************************************************/ 
BOOL CTCPIPDlg::CheckPBReady()
{
    int timeout;  // ��ʱ����������

	timeout = 0;
	do
    {   
    	timeout++;
    	
		memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
		RecieveFromUart(gRecieveBuffer); //��÷���ֵ��Ϣ
		
		if( strstr((char *)gRecieveBuffer,"+PBREADY")>0 )
		{// ˵���յ�+PBREADY���˳���
			break;
		}
		Delay_ms(300);  // ��ʱ300ms��
	}while(timeout<50);	  
	
	if( timeout>=50 )
	{	// ����
		return FALSE;// û���յ���
	}

	return TRUE; 
}

/****************************************************************************************
*��������CheckAT

*��  ��������ź�ǿ�ȣ�

*****************************************************************************************/ 
BOOL CTCPIPDlg::CheckAT()
{
    int timeout;  // ��ʱ����������

	timeout = 0;
	do
    {   
	    timeout++;

		SendATCommand("at\r",3); // ͨ�����ڷ���AT�س�
		
		Delay_ms(100);                 // ��ʱ100ms��
		
		memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
		RecieveFromUart(gRecieveBuffer); //��÷���ֵ��Ϣ
		
		if( strstr((char *)gRecieveBuffer,"OK")>0 )
		{// ˵������ͨ���˳���
			break;
		}
	}while(timeout<50);	  
	
	if( timeout>=50 )
	{	// ����
		return FALSE;
	}

	return TRUE; 
}

/***************************************************************************************
*��������CheckCSQ

*��  ��������ź�ǿ�ȣ�

*****************************************************************************************/ 
BOOL CTCPIPDlg::CheckCSQ()
{
    int timeout;  // ��ʱ����������

	timeout = 0;
	do
    {   
	    timeout++;

		SendATCommand("at+csq\r",7); // ͨ�����ڷ���AT+CSQ�س�
		
		Delay_ms(100);                 // ��ʱ100ms��
		
		memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
		RecieveFromUart(gRecieveBuffer); //��÷���ֵ��Ϣ
		
		if( strstr((char *)gRecieveBuffer,"+CSQ:99,99")==0 )
		{// ˵�����Ĳ���99��99�����źţ��˳���
			break;
		}
	}while(timeout<50);	  
	
	if( timeout>=50 )
	{	// ����
		return FALSE;// û���źţ�
	}

	return TRUE; 
}

/****************************************************************************************
*��������CheckCreg

*��  �����������ע��״̬��

*****************************************************************************************/ 
BOOL CTCPIPDlg::CheckCreg()
{
    int timeout;  // ��ʱ����������
    	
	timeout = 0;
	do
    {   
	    timeout++;

		SendATCommand("at+creg?\r",9); // ͨ�����ڷ���AT+CREG?�س�
		
		Delay_ms(200);                 // ��ʱ200ms��
		
		memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
		RecieveFromUart(gRecieveBuffer); //��÷���ֵ��Ϣ
		
		if( strstr((char *)gRecieveBuffer,"+CREG: 0,1")>0 ||  strstr((char *)gRecieveBuffer,"+CREG: 0,5")>0 )
		{// �������ֵΪ+CREG: 0,1��+CREG: 0,5��������ע��ɹ�
			break;
		}
	}while(timeout<50);	  
	
	if( timeout>=50 )
	{// ����˵������ע��ʧ�ܣ�
		return FALSE;// ����ע��ʧ�ܣ�
	}
	
	return TRUE;
}

/****************************************************************************************
*��������CreatePPP

*��  ��������PPP����

*****************************************************************************************/ 
BOOL CTCPIPDlg::CreatePPP()
{
    int timeout;  // ��ʱ����������

	timeout = 0;
	do
	{  
		timeout++;
		
    	SendATCommand("at+xiic=1\r",10);// ����"at+xiic=1
    	Delay_ms(200);                  // ��ʱ100ms

		memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
		RecieveFromUart(gRecieveBuffer); //��÷���ֵ��Ϣ
		
		if( strstr((char *)gRecieveBuffer,"OK")>0 )
		{// �緵��OK ����ѭ��
			break;
		}
	}while(timeout<50);	  
	
    if( timeout>=50 )
	{
		// ����˵��PPP����ʧ�ܣ�
		return FALSE;
	}
	return TRUE;
}

/****************************************************************************************
*��������CheckPPPStatus

*��  ������ѯPPP��·״̬

*****************************************************************************************/ 
BOOL CTCPIPDlg::CheckPPPStatus()
{
	BOOL result_ok=FALSE;
    int value,timeout;  // ��ʱ����������
    	
	timeout=0;
	value = 0;
	do
	{   
		value++;
		
    	SendATCommand("at+xiic?\r",9);//����"at+xiic?
    	
    	Delay_ms(500);   // ��ʱ500ms;
		
		do
		{
			timeout++; 
    		memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
    		RecieveFromUart(gRecieveBuffer); //��÷���ֵ��Ϣ
    		
    		if( strstr((char *)gRecieveBuffer,"+XIIC:    1")>0 )
    		{//�жϷ���ֵ �������+XIIC:    1, *.*.*.*,������ѭ��
    			result_ok = TRUE;
    			break;
    		}
		}while(timeout<50);
		if( result_ok==TRUE )
			break;
	}while(value<3);	  

	if( timeout>=50 )
	{
		// ����˵��PPP����ʧ�ܣ�
		return FALSE;
	}
	return TRUE;
}

/****************************************************************************************
*��������CreateTCPLink

*��  ��������TCP���ӣ�

*****************************************************************************************/ 
BOOL CTCPIPDlg::CreateTCPLink(char *linkStr,int size)
{
    int timeout;  // ��ʱ����������
	
	if( size<=0 ) 	return FALSE;
	
	timeout = 0;

	SendATCommand(linkStr,size);//����ATָ���TCP����
		
	Delay_ms(1000);   // ��ʱ1000ms;
	do
	{
		timeout++;
		Delay_ms(100);   // ��ʱ10ms;
		
		memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
		RecieveFromUart(gRecieveBuffer); //��÷���ֵ��Ϣ
		if( strstr((char *)gRecieveBuffer,"+TCPSETUP:0,OK")>0 )
		{//�жϷ���ֵ �������+TCPSETUP:0,OK,������ѭ��
			break;
		}
	}while(timeout<50);
 	
	if( timeout>=50 )
	{
		// ����˵��TCP����ʧ�ܣ�
		return FALSE;
	}
	
	return TRUE;
}


/****************************************************************************************
*��������TCPIP_CloseLink

*��  �����ر�TCP IP ��·��

*��  ����void
*****************************************************************************************/ 
void CTCPIPDlg::TCPIP_CloseLink()
{
	SendATCommand("at+tcpclose=0\r",14);// ����AT+TCPCLOSE=0�س�
	SendATCommand("at+tcpclose=1\r",14);// ����AT+TCPCLOSE=1�س�
}

/****************************************************************************************
*��������TCPIP_SendData

*��  ��������TCP����

*��  ����BOOL

*****************************************************************************************/
BOOL CTCPIPDlg::TCPIP_SendData()
{
	int timeout=0;

    while(1)
	{
		// ���TCP buffer�Ĵ�С�����ж��Ƿ������ݣ�
		do
		{
			timeout++;
			SendATCommand("at+ipstatus=0\r",14);//���� at+ipstatus=0�س���ѯTCP��·״̬
			Delay_ms(100);
	
    		memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
    		RecieveFromUart(gRecieveBuffer); //��÷���ֵ��Ϣ
    		if( strstr((char *)gRecieveBuffer,"+IPSTATUS:0,CONNECT,TCP")>0 )
    		{// �������+IPSTATUS:0,CONNECT,TCP,****��
    			break;
    		}
    		if( strstr((char *)gRecieveBuffer,"+IPSTATUS:0,DISCONNECT")>0 )
    		{
    			// ����˵����·û�н�����
    			return FALSE;
    		}
   		}while(timeout<50);
   		
		if( timeout>=50 )
		{
			// ����˵��TCP����ʧ�ܣ�
			return FALSE;
		}

		// ���TCP״̬OK ��ʼ����TCPָ�
		SendATCommand("at+tcpsend=0,50\r",16);	//���� at+tcpsend=0,50�س�������50���ֽڣ�

		// �жϽ��շ��ͷ���">"
		do
		{
			timeout++;
    		Delay_ms(100);   // ��ʱ100ms;
    		
    		memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
    		RecieveFromUart(gRecieveBuffer); //��÷���ֵ��Ϣ
    		if( strstr((char *)gRecieveBuffer,">")>0 )
    		{//�жϷ���ֵ �������>,������ѭ������ʾ���Է��������ˣ�
    			break;
    		}
   		}while(timeout<50);
   		
   		if( timeout>=50 )
		{
			// ����˵��û���յ� >,���ܷ������ݣ�
			return FALSE;
		}

		strcpy((char *)gSendBuffer,"12345678901234567890123456789012345678901234567890\r");
		// ��ʼ�������ݣ���������ʱ�����������ݰ���ĩβ����0x0d ��Ϊ�����������ý��������㵽���ݳ����
		SendATCommand((char *)gSendBuffer,50+1); //����50���ֽ����ݺͻس�����ס�������Ҫ�ӻس�����
		Delay_ms(100);   // ��ʱ100ms;
		
		memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
		RecieveFromUart(gRecieveBuffer); //��÷���ֵ��Ϣ
		if( strstr((char *)gRecieveBuffer,"+TCPSEND:0,50")>0 )
		{// �жϷ���ֵ �������+TCPSEND:0,50;��ʾ���ͳɹ���
			return TRUE;
		}
		else if( strstr((char *)gRecieveBuffer,"+TCPSEND:Error")>0 )
		{// �жϷ���ֵ �������+TCPSEND:Error;����·0 �Ϸ���50�ֽڵ����ݣ�����·��δ����������ʧ�ܡ�
			return FALSE;
		}
		else if( strstr((char *)gRecieveBuffer,"+TCPSEND:Buffer not enough")>0 )
		{// �жϷ���ֵ �������+TCPSEND:Buffer not enough;����·0 �Ϸ���50�ֽڵ����ݣ��ڲ�buffer���㣬����ʧ�ܡ�
			return FALSE;
		}
		
		// �������������Ӧ�Ĵ�����д���  ....
	}
}

/****************************************************************************************
*��������OnTcpipTest

*��  ����TCP-IP ��·���������ݷ��Ͳ��Ժ���

*****************************************************************************************/
void CTCPIPDlg::OnTcpipTest() 
{
	// TODO: Add your control notification handler code here
	OpenPortMode();  // �򿪴��ڣ�
	
	TCPIP_CreateProcess(); 
	TCPIP_SendData();
	TCPIP_CloseLink(); // �ر���·��
	
	ClosePortMode(); // �رմ��ڣ�
}

/****************************************************************************************
*��������TCPIP_SendData

*��  ��������TCP���ݣ���������ʱ��ͨ�������жϴ����������ݸ�ʽ�����жϣ�
	     �����+TCPRECV:0��ͷ�ģ���˵�����յ�TCP���ݣ�
		 �磺+TCPRECV:0,10,1234567890\r\n
	
		 ����ݸ���ƽ̨���ص�����ݽ��ս�����Ӧ�Ĵ�����������������ο���
	
		 ���жϴ���ʱ�������������TCPIP_RecieveData���н��գ�
*��  ����BOOL

*****************************************************************************************/
BOOL CTCPIPDlg::TCPIP_RecieveData()
{
	int timeout=0;

	do
	{
		timeout++;
		Delay_ms(100);

		memset(gRecieveBuffer,0x00,sizeof(gRecieveBuffer));
		RecieveFromUart(gRecieveBuffer); //��÷���ֵ��Ϣ
		if( strstr((char *)gRecieveBuffer,"+TCPRECV:0")>0 )
		{// �������+TCPRECV:0
			// ��������ĳ��Ⱥ����ݣ���������ϴ��룻
			// ...
			break;
		}
	}while(timeout<50);
	
	if( timeout>=50 )
	{
		// ����
		return FALSE;
	}

	return TRUE;
}

/****************************************************************************************
*��������CreateSingleTCPLink

*��  ����������TCP��·.
	
*��  ����BOOL

*****************************************************************************************/
BOOL CTCPIPDlg::CreateSingleTCPLink(unsigned char iLinkNum,char *strServerIP,char *strPort)
{
	int length;
	char buffer[50];
	
	memset(buffer,0x00,sizeof(buffer));
	sprintf(buffer,"AT+TCPSETUP=%d,%s,%s\r",iLinkNum,strServerIP,strPort);
	length = strlen((char *)buffer);
	if( !CreateTCPLink(buffer,length) )  
	{
		return FALSE;// ����TCP����ʧ�ܣ�
	}
	return TRUE;
}

typedef enum
{
	NEO_LINK0,
	NEO_LINK1,
	NEO_LINK2,
	NEO_LINK3
}neo_link_enum_type;
	
/****************************************************************************************
*��������CreateMultiTCPLink

*��  ����������TCP��·,������Ϻ�ÿ����·�������շ�Ҫ������·��������
	
	     ����·�İ汾��ATָ��û�б仯��ֻ����·��ŷ�Χ�����ˣ�0-4��
	     
	     ע�⣺�ر���·ʱ��һ��Ҫ�����д�������·���رա�
	     	 
*��  ����BOOL

*****************************************************************************************/
BOOL CTCPIPDlg::CreateMultiTCPLink()
{
	// ������һ·TCP��·����·����0��
	if( !CreateSingleTCPLink(NEO_LINK0,"110.123.12.56","6800") )  
		return FALSE;

	// �����ڶ�·TCP��·����·����1��
	if( !CreateSingleTCPLink(NEO_LINK1,"110.122.12.55","6800") )  
		return FALSE;

	// ��������·TCP��·����·����2��
	if( !CreateSingleTCPLink(NEO_LINK2,"110.121.12.54","6800") )  
		return FALSE;
	
	return TRUE;
}

/****************************************************************************************
*��������TCPIP_CloseMulitLink

*��  �����ر�TCP IP ��·��

*��  ����void
*****************************************************************************************/ 
void CTCPIPDlg::TCPIP_CloseMulitLink()
{
	SendATCommand("AT+TCPCLOSE=0\r",14);// ����AT+TCPCLOSE=0�س�,�ر���·0��
	SendATCommand("AT+TCPCLOSE=1\r",14);// ����AT+TCPCLOSE=1�س�,�ر���·1��
	SendATCommand("AT+TCPCLOSE=2\r",14);// ����AT+TCPCLOSE=1�س�,�ر���·2��
}


//////////////////////////////////////////////End//////////////////////////////////////////