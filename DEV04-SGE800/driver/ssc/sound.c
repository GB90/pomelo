#if 0
/*                                                   
 * sound.c   
 * ��¼�Ƽ�������Ƶ���ݣ����������ڴ滺�����У�Ȼ���ٽ��лطţ�
 * �����еĹ��ܶ���ͨ����д/dev/dsp�豸�ļ������                                        
 */                                                  
#include <unistd.h>                                  
#include <fcntl.h>                                   
#include <sys/types.h>                               
#include <sys/ioctl.h>                               
#include <stdlib.h>                                  
#include <stdio.h>  
#include <string.h>
#include <linux/soundcard.h>                         
#define LENGTH 3    /* �洢���� */                   
#define RATE 44100   /* ����Ƶ�� */                   
#define SIZE 16      /* ����λ�� */                   
#define CHANNELS 1  /* ������Ŀ */                   
/* ���ڱ���������Ƶ���ݵ��ڴ滺���� */               
unsigned char sound_buf[LENGTH*RATE*SIZE*CHANNELS/8];      
int main()                                           
{                                                    
	int fd;	/* �����豸���ļ������� */                 
	int arg;	/* ����ioctl���õĲ��� */                
	int status;   /* ϵͳ���õķ���ֵ */   
	int i=0;
	memset(sound_buf,1,LENGTH*RATE*SIZE*CHANNELS/8);
	for(i=0;i<LENGTH*RATE*SIZE*CHANNELS/8;i++){

		if(i%2==0)
			sound_buf[i]=0x55;
		else
			sound_buf[i]=0xff;
	}
	for(i=0;i<5;i++){
		printf("%x	",sound_buf[i]);
	}	

	/* �������豸 */                                 
	fd = open("/dev/dsp", O_WRONLY);                     
	if (fd < 0) {                                      
		perror("open of /dev/dsp failed");               
	exit(1);                                         
	} 

#if 0	
	/* ���ò���ʱ������λ�� */      
	
	arg = SIZE;                                        
	status = ioctl(fd, SOUND_PCM_WRITE_BITS, &arg);    
	if (status == -1)                                  
		perror("SOUND_PCM_WRITE_BITS ioctl failed");     
	if (arg != SIZE)                                   
		perror("unable to set sample size");  
	
	/* ���ò���ʱ��������Ŀ */ 
	
	arg = CHANNELS;                                    
	status = ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &arg);
	if (status == -1)                                  
		perror("SOUND_PCM_WRITE_CHANNELS ioctl failed"); 
	if (arg != CHANNELS)                               
		perror("unable to set number of channels");  
	
	/* ���ò���ʱ�Ĳ���Ƶ�� */
	
	arg = RATE;                                        
	status = ioctl(fd, SOUND_PCM_WRITE_RATE, &arg);    
	if (status == -1)                                  
		perror("SOUND_PCM_WRITE_WRITE ioctl failed"); 
#endif	
	/* ѭ����ֱ������Control-C */                      
	                                      
	//printf("Say something:\n");                      
	//status = read(fd, buf, sizeof(buf)); /* ¼�� */  
	//if (status != sizeof(buf))                       
	 // perror("read wrong number of bytes");          
	printf("You said:%d = %d\n",sizeof(sound_buf),LENGTH*RATE*SIZE*CHANNELS/8);                           
	status = write(fd, sound_buf, sizeof(sound_buf)); /* �ط� */ 
	
	if (status != sizeof(sound_buf))                       
		perror("wrote wrong number of bytes");         
	/* �ڼ���¼��ǰ�ȴ��طŽ��� */                   
	//status = ioctl(fd, SOUND_PCM_SYNC, 0);           
	//if (status == -1)                                
	//  perror("SOUND_PCM_SYNC ioctl failed");   
	close(fd);
	return 0;
                                                  
}
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/soundcard.h>

#define OPEN_DSP_FAILED     0x00000001    /*open dsp failed!*/
#define SAMPLERATE_STATUS     0x00000002    /*samplerate status failed*/
#define SET_SAMPLERATE_FAILED  0x00000003       /*set samplerate failed*/
#define CHANNELS_STATUS       0x00000004    /*Channels status failed*/
#define SET_CHANNELS_FAILED    0x00000005    /*set channels failed*/
#define FMT_STATUS       0x00000006    /*FMT status failed*/
#define SET_FMT_FAILED     0x00000007    /*set fmt failed*/
#define OPEN_FILE_FAILED        0x00000008    /*opem filed failed*/
/*  RIFF WAVE Chunk
��'FIFF'��Ϊ��ʾ��Ȼ�������Ϊsize�ֶΣ���size������wav�ļ���С��ȥID
������Size��ռ�õ��ֽ�������FileLen - 8 = Size��Ȼ����Type�ֶΣ�Ϊ'WAVE'����
����ʾ��wav�ļ���
*/
struct RIFF_H
{
	char szRiffID[4]; // 'R','I','F','F'
	unsigned int dwRiffSize;
	char szRiffFormat[4]; // 'W','A','V','E'
};
/*
						Format Chunk
����====================================================================
����| | �ֽ��� | �������� |
����====================================================================
����| ID | 4 Bytes | 'fmt ' |
����--------------------------------------------------------------------
����| Size | 4 Bytes | ��ֵΪ16��18��18������ָ�����Ϣ |
����-------------------------------------------------------------------- ----
����| FormatTag | 2 Bytes | ���뷽ʽ��һ��Ϊ0x0001 | |
����-------------------------------------------------------------------- |
����| Channels | 2 Bytes | ������Ŀ��1--��������2--˫���� | |
����-------------------------------------------------------------------- |
����| SamplesPerSec | 4 Bytes | ����Ƶ�� | |
����-------------------------------------------------------------------- |
����| AvgBytesPerSec| 4 Bytes | ÿ�������ֽ��� | |===> WAVE_FORMAT
����-------------------------------------------------------------------- |
����| BlockAlign | 2 Bytes | ���ݿ���뵥λ(ÿ��������Ҫ���ֽ���) | |
����-------------------------------------------------------------------- |
����| BitsPerSample | 2 Bytes | ÿ��������Ҫ��bit�� | |
����-------------------------------------------------------------------- |
����| | 2 Bytes | ������Ϣ����ѡ��ͨ��Size���ж����ޣ� | |
����-------------------------------------------------------------------- ----
����					ͼ3 Format Chunk

������'fmt '��Ϊ��ʾ��һ�������SizeΪ16����ʱ��󸽼���Ϣû�У����Ϊ18
������������2���ֽڵĸ�����Ϣ����Ҫ��һЩ����Ƴɵ�wav��ʽ�к��и�2���ֽڵ�
����������Ϣ��
�����ṹ�������£�
*/
struct WAVE_FORMAT
{
	unsigned short wFormatTag;
	unsigned short wChannels;
	unsigned int dwSamplesPerSec;
	unsigned int dwAvgBytesPerSec;
	unsigned short wBlockAlign;
	unsigned short wBitsPerSample;
	//unsigned short accb;
};
struct FMT_BLOCK
{
	char szFmtID[4]; // 'f','m','t',' '
	unsigned int dwFmtSize;
	struct WAVE_FORMAT wavFormat;
};
/*
Fact Chunk
����==================================
����| |��ռ�ֽ���| �������� |
����==================================
����| ID | 4 Bytes | 'fact' |
����----------------------------------
����| Size | 4 Bytes | ��ֵΪ4 |
����----------------------------------
����| data | 4 Bytes | |
����----------------------------------
����ͼ4 Fact Chunk
����Fact Chunk�ǿ�ѡ�ֶΣ�һ�㵱wav�ļ���ĳЩ���ת�����ɣ��������Chunk��
�����ṹ�������£�
*/

struct FACT_BLOCK
{	
	char szFactID[4]; // 'f','a','c','t'
	unsigned int dwFactSize;
	unsigned int data;
};
/*
Data Chunk
����==================================
����| |��ռ�ֽ���| �������� |
����==================================
����| ID | 4 Bytes | 'data' |
����----------------------------------
����| Size | 4 Bytes | |
����----------------------------------
����| data | | |
����----------------------------------
����ͼ5 Data Chunk
����Data Chunk����������wav���ݵĵط�����'data'��Ϊ��Chunk�ı�ʾ��Ȼ����
�������ݵĴ�С�������ž���wav���ݡ�����Format Chunk�е��������Լ�����bit����
����wav���ݵ�bitλ�ÿ��Էֳ����¼�����ʽ��
����---------------------------------------------------------------------
����| ������ | ȡ��1 | ȡ��2 | ȡ��3 | ȡ��4 |
����| |--------------------------------------------------------
����| 8bit���� | ����0 | ����0 | ����0 | ����0 |
����---------------------------------------------------------------------
����| ˫���� | ȡ��1 | ȡ��2 |
����| |--------------------------------------------------------
����| 8bit���� | ����0(��) | ����1(��) | ����0(��) | ����1(��) |
����---------------------------------------------------------------------
����| | ȡ��1 | ȡ��2 |
����| ������ |--------------------------------------------------------
����| 16bit���� | ����0 | ����0 | ����0 | ����0 |
����| | (��λ�ֽ�) | (��λ�ֽ�) | (��λ�ֽ�) | (��λ�ֽ�) |
����---------------------------------------------------------------------
����| | ȡ��1 |
����| ˫���� |--------------------------------------------------------
����| 16bit���� | ����0(��) | ����0(��) | ����1(��) | ����1(��) |
����| | (��λ�ֽ�) | (��λ�ֽ�) | (��λ�ֽ�) | (��λ�ֽ�) |
����---------------------------------------------------------------------
����ͼ6 wav����bitλ�ð��ŷ�ʽ
����Data Chunkͷ�ṹ�������£�
*/
struct DATA_BLOCK
{
	char szDataID[4]; // 'd','a','t','a'
	unsigned int dwDataSize;
};

#pragma pack(2)

 struct SOUND_FILE_HEAD{
	char szRiffID[4]; // 'R','I','F','F'	
	unsigned int dwRiffSize;
	char szRiffFormat[4]; // 'W','A','V','E'

	char szFmtID[4]; // 'f','m','t',' '
	unsigned int dwFmtSize;

	unsigned short wFormatTag;
	unsigned short wChannels;
	unsigned int dwSamplesPerSec;
	unsigned int dwAvgBytesPerSec;
	unsigned short wBlockAlign;
	unsigned short wBitsPerSample;
	unsigned short accb;

	char szFactID[4]; // 'f','a','c','t'
	unsigned int dwFactSize;
	unsigned int factdata;

	char szDataID[4]; // 'd','a','t','a'
	unsigned int dwDataSize;
	/*
	struct RIFF_H riff_h;	
	struct FMT_BLOCK fmt_blk;	
	struct FACT_BLOCK fact;
	struct DATA_BLOCK data_blk;
	*/
};
#pragma pack()


//struct SOUND_FILE_HEAD sound_file_h;

int Uda1330Audio_Play(char *pathname,int nSampleRate,int nChannels,int fmt)
{
	int dsp_fd,mix_fd,status,arg;
	dsp_fd = open("/dev/dsp" , O_WRONLY);   /*open dsp*/
	if(dsp_fd < 0) 
		return  OPEN_DSP_FAILED;
#if 1
	arg = nSampleRate;
	
	status = ioctl(dsp_fd,SOUND_PCM_WRITE_RATE,&arg); /*set samplerate*/
	if(status < 0)	{
		close(dsp_fd);
		return SAMPLERATE_STATUS;
	} 
	if(arg != nSampleRate){
		close(dsp_fd);
		return SET_SAMPLERATE_FAILED;
	} 
	arg = nChannels;  /*set channels*/   
	status = ioctl(dsp_fd, SOUND_PCM_WRITE_CHANNELS, &arg); 
	if(status < 0){
		close(dsp_fd);
		return CHANNELS_STATUS;
	} 
	if( arg != nChannels){
		close(dsp_fd);
		return SET_CHANNELS_FAILED;
	}
	arg = fmt; /*set bit fmt*/
	status = ioctl(dsp_fd, SOUND_PCM_WRITE_BITS, &arg); 
	if(status < 0)
	{
		close(dsp_fd);
		return FMT_STATUS;
	} 
	if(arg != fmt)
	{
		close(dsp_fd);
		return SET_FMT_FAILED;
	}/*�������ú���DSP�ĸ�������*/  
#endif
	
	struct SOUND_FILE_HEAD *sound_file_h;
	FILE *file_fd = fopen(pathname,"r");
	if(file_fd == NULL)
	{
		close(dsp_fd);
		return OPEN_FILE_FAILED;
	}
	int num = 2*nChannels*nSampleRate*fmt/8;
	int get_num; 
	char buf[num];
	
	char buf_temp[100];
	char *buf_t;
	buf_t = buf_temp;
	get_num = fread(buf_t, sizeof(struct SOUND_FILE_HEAD), 1, file_fd);
	//printf("get_num = %d,struct size=%d\n ",get_num,sizeof(struct SOUND_FILE_HEAD));
	sound_file_h = (struct SOUND_FILE_HEAD *)buf_t;
	/*
	printf("rffid  = %s,size = %d,format = %s\n ",sound_file_h->szRiffID,
												sound_file_h->dwRiffSize,
												sound_file_h->szRiffFormat);
	printf("fmtid  = %s,size = %d,channels = %d,fs = %d,datalign = %d\n ",sound_file_h->szFmtID,
												sound_file_h->dwFmtSize,
												sound_file_h->wChannels,
												sound_file_h->dwSamplesPerSec,
												sound_file_h->wBlockAlign);
	printf("factid = %s,size = %d\n ",sound_file_h->szFactID,
											sound_file_h->dwFactSize);
	printf("dataid = %s,size = %d\n ",sound_file_h->szDataID,
												sound_file_h->dwDataSize);
	*/
	printf("fileinfo:channels %d, Fs %d Hz, fmt %d bit, size %d B, %d Kbps,%3.1f s\n",
												
												sound_file_h->wChannels,
												sound_file_h->dwSamplesPerSec,
												sound_file_h->wBitsPerSample,
												sound_file_h->dwDataSize,
												sound_file_h->dwAvgBytesPerSec*8/1000,
												(double)(100*sound_file_h->dwDataSize/sound_file_h->dwAvgBytesPerSec)/100);
	while(feof(file_fd) == 0)
	{
		get_num = fread(buf, 1, num, file_fd);
		printf("get_num = %d, num = %d\n",get_num,num);
		write(dsp_fd,buf,get_num); 
		if((get_num != num) && (feof(file_fd) == 0))
		{			
			close(dsp_fd);
			fclose(file_fd);
			return -1;
		} 
	}
	sleep(5);
	close(dsp_fd);
	fclose(file_fd);
	return 0; 
}
/*
*test
*/
int main()
{	
	int value;
	value = Uda1330Audio_Play("test8km16.wav",8000,1,16);
	fprintf(stderr,"value is %d",value);
	
	return 0;
} 
#endif
