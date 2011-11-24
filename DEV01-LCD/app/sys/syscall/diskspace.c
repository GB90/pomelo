/**
* diskspace.c -- �����������ʣ��ռ�
* 
* 
* ����ʱ��: 2010-6-16
* ����޸�ʱ��: 2010-6-16
*/

#include <stdio.h>
#include <sys/vfs.h>

/**
* @brief ��������Ŀռ�������
* @param diskpath ������
* @return �ɹ����ؿռ�������(1%), ʧ�ܷ���-1
*/
int DiskUsage(const char *diskpath)
{
	struct statfs buffer;
	int rtn;

	if(statfs(diskpath, &buffer)) return -1;

	if(buffer.f_blocks <= 0) return -1;

	rtn = (buffer.f_blocks - buffer.f_bavail)*100/buffer.f_blocks;
	return rtn;
}

