/*****************************************************************************
	��̵����ɷ����޹�˾			��Ȩ��2008-2015

	��Դ���뼰������ĵ�Ϊ���������̵����ɷ����޹�˾�������У�δ��������
	�ɲ��������޸Ļ򷢲�������׷����صķ������Ρ�

						���������̹ɷ����޹�˾
						www.xjgc.com
						(0374) 321 2924
*****************************************************************************/


/*****************************************************************************
	��Ŀ����	��SGE800���������ն�ƽ̨
	�ļ�		��file.c
	����		�����ļ�ʵ�����ļ�����ģ���е�API����
	�汾		��0.1
	����		����迡
	��������	��2009.12
*****************************************************************************/
//������ͷ�ļ�
#include "private/config.h"

//ģ�����ÿ���
#ifdef CFG_FILE_MODULE

//����ͷ�ļ�
#include "private/debug.h"

//��������ͷ�ļ�

//C��ͷ�ļ�
#include <stdio.h>						//printf
#include <string.h>
#include <errno.h>
#include <fcntl.h>						//open
#include <unistd.h>						//write
#include <sys/stat.h>					//stat
#include <sys/types.h>

//�ṩ���û���ͷ�ļ�
#include "include/file.h"
#include "include/error.h"


/*************************************************
  ��̬ȫ�ֱ�������
*************************************************/
//���洢����·��
static const char *const path_ram = "/var/";
static const char *const path_flash_code = "/mnt/local/";
static const char *const path_flash_data = "/mnt/data0/";
static const char *const path_sd = "/mnt/sddisk/";
static const char *const path_u = "/mnt/udisk/";



/*************************************************
  API
*************************************************/
/******************************************************************************
*	����:	file_open
*	����:	���ļ�
*	����:	name			-	�ļ���
 			pos				-	�ļ����λ��
 			mode			-	�ļ���ģʽ
*	����:	>0				-	�ļ�������
 			-ERR_INVAL		-	��������
 			-ERR_BUSY		-	�ļ��Ѵ���
*	˵��:
 ******************************************************************************/
int file_open(char *name, u8 pos, u8 mode)
{
    int ret = 0;
    int i;
    int fd;
    int errnum;
    int oflag;
    int namesize;
    char path[32 + CFG_FILE_NAME_MAX] = "";			//32����·���洢�ռ�

//ȷ���ļ���ģʽ����
    if (FILE_MODE_OPEN == mode) {
        oflag = O_RDWR;
    }
    else if (FILE_MODE_CREAT == mode) {
        oflag = O_RDWR | O_CREAT | O_EXCL;
    }
    else if (FILE_MODE_RW == mode) {
        oflag = O_RDWR | O_CREAT;
    }
    else if (FILE_MODE_RD == mode) {
        oflag = O_RDONLY;
    }
    else {
        ret = -ERR_INVAL;
        goto error;
    }
    oflag = oflag | O_SYNC;
//ȷ���ļ�·��
    if (FILE_POS_RAM == pos) {
        strcat (path, path_ram);
    }
    else if (FILE_POS_FLASH_CODE == pos) {
        strcat (path, path_flash_code);
    }
    else if (FILE_POS_FLASH_DATA == pos) {
        strcat (path, path_flash_data);
    }
    else if (FILE_POS_SD == pos) {
        strcat (path, path_sd);
    }
    else if (FILE_POS_U == pos) {
        strcat (path, path_u);
    }
    else {
        ret = -ERR_INVAL;
        goto error;
    }
//����ļ���
    namesize = strlen (name);
    if ((0 == namesize) || (namesize > CFG_FILE_NAME_MAX)) {
        ret = -ERR_INVAL;
        goto error;
    }
    for (i=0; i<namesize; i++) {
    	//�ļ���ֻ�������֡���Сд��ĸ���»���
        if (((name[i] > 47) && (name[i] < 58)) || ((name[i] > 64) && (name[i] < 91))
        	|| ((name[i] > 96) && (name[i] < 123)) || (name[i] == 95)) {
            continue;
        }
        else {
            ret = -ERR_INVAL;
            goto error;
        }
    }
    strcat (path, name);
//���ļ�
    fd = open (path, oflag, S_IRWXU);
    if (fd < 0) {
        errnum = errno;
        switch (errnum) {
        case EEXIST:
            ret = -ERR_BUSY;
            break;
        case ENOENT:
            ret = -ERR_NOFILE;
            break;
        case ENOMEM:
            ret = -ERR_NOMEM;
            break;
        default:
            ret = -ERR_SYS;
        }
        goto error;
    }
    else {
        ret = fd;
    }
error:
    return ret;
}

/******************************************************************************
*	����:	file_write
*	����:	д���ļ�
*	����:	fd				-	�ļ�������
 			buf				-	�����������ݴ��룩
 			count			-	�����ֽ���
*	����:	>=0				-	д���ֽ���
 			-ERR_INVAL		-	��������
 			-ERR_NOFUN		-	�޴˹���
 			-ERR_NOMEM		-	����������
 			-ERR_NODISK		-	Ӳ�̿ռ䲻��
 			-ERR_SYS		-	ϵͳ�쳣
*	˵��:
 ******************************************************************************/
int file_write(int fd, void *buf, u32 count)
{
    int ret = 0;
    int errnum;

    if ((count > CFG_FILE_DATA_MAX) || (count <= 0)) {
        ret = -ERR_INVAL;
        goto error;
    }
    if (fd <= 0) {
        ret = -ERR_INVAL;
        goto error;
    }
    if (buf == NULL){
    	 ret = -ERR_INVAL;
    	 goto error;
    }
    ret = write (fd, buf, count);
    if (ret < 0) {
        errnum = errno;
        switch (errnum) {
        case EBADF:
        case EINVAL:
            ret = -ERR_NOFUN;
            break;
        case EFAULT:
            ret = -ERR_NOMEM;
            break;
        case ENOSPC:
            ret = -ERR_NODISK;
            break;
		default:
            ret = -ERR_SYS;
        }
    }
error:
    return ret;
}

/******************************************************************************
*	����:	file_read
*	����:	��ȡ�ļ�
*	����:	fd				-	�ļ�������
 			buf				-	�����������ݴ�����
 			count			-	�����ֽ���
*	����:	>=0				-	��ȡ�ֽ���
 			-ERR_INVAL		-	��������
 			-ERR_NOMEM		-	����������
 			-ERR_SYS		-	ϵͳ�쳣
*	˵��:
 ******************************************************************************/
int file_read(int fd, void *buf, u32 count)
{
    int ret = 0;
    int errnum;

    if ((count > CFG_FILE_DATA_MAX) || (count <= 0)) {
        ret = -ERR_INVAL;
        goto error;
    }
    if (fd <= 0) {
        ret = -ERR_INVAL;
        goto error;
    }
    ret = read (fd, buf, count);
    if (ret < 0) {
        errnum = errno;
        switch (errnum) {
        case EBADF:
            ret = -ERR_INVAL;
            break;
        case EFAULT:
            ret = -ERR_NOMEM;
            break;
        default:
            ret = -ERR_SYS;
        }
    }
error:
    return ret;
}

/******************************************************************************
*	����:	file_seek
*	����:	�ƶ��ļ�����ָ��
*	����:	fd				-	�ļ�������
 			offset			-	ƫ��ֵ
 			whence			-	ƫ�Ʋο�
*	����:	>0				-	�ļ���ǰ��ƫ��
 			-ERR_INVAL		-	��������
 			-ERR_SYS		-	ϵͳ�쳣
*	˵��:
 ******************************************************************************/
int file_seek(int fd, int offset, u8 whence)
{
    int ret = 0;
    int errnum;

    if ((fd <= 0) || (whence > FILE_SEEK_END)) {
        ret = -ERR_INVAL;
        goto error;
    }
    ret = lseek (fd, offset, whence);
    if (ret < 0) {
        errnum = errno;
        switch (errnum) {
        case EBADF:
        case EINVAL:
            ret = -ERR_INVAL;
            break;
        default:
            ret = -ERR_SYS;
        }
    }
error:
    return ret;
}

/******************************************************************************
*	����:	file_close
*	����:	�ر��ļ�
*	����:	fd				-	�ļ�������
*	����:	0				-	�ɹ�
 			-ERR_INVAL		-	��������
 			-ERR_SYS		-	ϵͳ�쳣
*	˵��:
 ******************************************************************************/
int file_close(int fd)
{
    int ret = 0;
    int errnum;
    ret = close (fd);
    if (ret < 0) {
        errnum = errno;
        switch (errnum) {
        case EBADF:
            ret = -ERR_INVAL;
            break;
        default:
            ret = -ERR_SYS;
        }
    }
    return ret;
}

/******************************************************************************
*	����:	file_delete
*	����:	ɾ���ļ�
*	����:	name			-	�ļ���
 			pos				-	�ļ����λ��
*	����:	0				-	�ɹ�
 			-ERR_NOFILE		-	�޴��ļ�
 			-ERR_NOMEM		-	�ڴ治��
 			-ERR_SYS		-	ϵͳ�쳣
*	˵��:
 ******************************************************************************/
int file_delete(char *name, u8 pos)
{
    int ret = 0;
    int i;
    int errnum;
    int namesize;
    char path[32 + CFG_FILE_NAME_MAX] = "";			//32����·���洢�ռ�
//ȷ���ļ�·��
    if (FILE_POS_RAM == pos) {
        strcat (path, path_ram);
    }
    else if (FILE_POS_FLASH_CODE == pos) {
        strcat (path, path_flash_code);
    }
    else if (FILE_POS_FLASH_DATA == pos) {
        strcat (path, path_flash_data);
    }
    else if (FILE_POS_SD == pos) {
        strcat (path, path_sd);
    }
    else if (FILE_POS_U == pos) {
        strcat (path, path_u);
    }
    else {
        ret = -ERR_INVAL;
        goto error;
    }
//����ļ���
    namesize = strlen (name);
    if ((0 == namesize) || (namesize > CFG_FILE_NAME_MAX)) {
        ret = -ERR_INVAL;
        goto error;
    }
    for (i=0; i<namesize; i++) {
    	//�ļ���ֻ�������֡���Сд��ĸ���»���
        if (((name[i] > 47) && (name[i] < 58)) || ((name[i] > 64) && (name[i] < 91))
        	|| ((name[i] > 96) && (name[i] < 123)) || (name[i] == 95)) {
            continue;
        }
        else {
            ret = -ERR_INVAL;
            goto error;
        }
    }
    strcat (path, name);
//ɾ���ļ�
    ret = unlink (path);
    if (ret < 0) {
        errnum = errno;
        switch (errnum) {
        case ENOENT:
            ret = -ERR_NOFILE;
            break;
        case ENOMEM:
            ret = -ERR_NOMEM;
            break;
        default:
            ret = -ERR_SYS;
        }
    }
error:
    return ret;
}

/******************************************************************************
*	����:	file_info
*	����:	��ȡ�ļ���Ϣ
*	����:	name			-	�ļ���
 			pos				-	�ļ����λ��
 			size			-	�ļ���С�����ݴ�����
 			time			-	�޸�ʱ�䣨���ݴ�����
*	����:	0				-	�ɹ�
 			-ERR_NOFILE		-	�޴��ļ�
 			-ERR_NOMEM		-	�ڴ治��
 			-ERR_SYS		-	ϵͳ�쳣
*	˵��:
 ******************************************************************************/
int file_info(char *name, u8 pos, u32 *size, file_time_t *time)
{
    int ret = 0;
    int i;
    int errnum;
    int namesize;
    char path[32 + CFG_FILE_NAME_MAX] = "";			//32����·���洢�ռ�
    struct stat st;
    struct tm t;
//ȷ���ļ�·��
    if (FILE_POS_RAM == pos) {
        strcat (path, path_ram);
    }
    else if (FILE_POS_FLASH_CODE == pos) {
        strcat (path, path_flash_code);
    }
    else if (FILE_POS_FLASH_DATA == pos) {
        strcat (path, path_flash_data);
    }
    else if (FILE_POS_SD == pos) {
        strcat (path, path_sd);
    }
    else if (FILE_POS_U == pos) {
        strcat (path, path_u);
    }
    else {
        ret = -ERR_INVAL;
        goto error;
    }
//����ļ���
    namesize = strlen (name);
    if ((0 == namesize) || (namesize > CFG_FILE_NAME_MAX)) {
        ret = -ERR_INVAL;
        goto error;
    }
    for (i=0; i<namesize; i++) {
    	//�ļ���ֻ�������֡���Сд��ĸ���»���
        if (((name[i] > 47) && (name[i] < 58)) || ((name[i] > 64) && (name[i] < 91))
        	|| ((name[i] > 96) && (name[i] < 123)) || (name[i] == 95)) {
            continue;
        }
        else {
            ret = -ERR_INVAL;
            goto error;
        }
    }
    strcat (path, name);
//��ȡ�ļ���Ϣ
    ret = stat (path, &st);
    if (ret < 0) {
        errnum = errno;
        switch (errnum) {
		case ENOENT:
            ret = -ERR_NOFILE;
            break;
		case ENOMEM:
            ret = -ERR_NOMEM;
            break;
		default:
            ret = -ERR_SYS;
        }
    }
    else {
        ret = (int)(gmtime_r(&st.st_mtime, &t));
        if (0 == ret) {
            ret = -ERR_SYS;
            goto error;
        }
        *size = st.st_size;
        time->year = t.tm_year - 100;		//ϵͳ�����������Դ�1900����������
        time->mon = t.tm_mon + 1;
        time->day = t.tm_mday;
        time->hour = t.tm_hour;
        time->min = t.tm_min;
        time->sec = t.tm_sec;
        ret = 0;
    }
error:
    return ret;
}

#endif /* CFG_FILE_MODULE */
