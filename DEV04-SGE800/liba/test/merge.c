/*****************************************************************************/
/*��̵����ɷ����޹�˾                                     ��Ȩ��2008-2015   */
/*****************************************************************************/
/* ��Դ���뼰������ĵ�Ϊ���������̵����ɷ����޹�˾�������У�δ��������    */
/* �ɲ��������޸Ļ򷢲�������׷����صķ������Ρ�                          */
/*                                                                           */
/*                      ���������̹ɷ����޹�˾                             */
/*                      www.xjgc.com                                         */
/*                      (0374) 321 2924                                      */
/*                                                                           */
/*****************************************************************************/


/******************************************************************************
    ��Ŀ����    ��  SGE800���������ն�ƽ̨
    �ļ���      ��  merge.c
    ����        ��  ���ļ����ڵ��ԺͲ���ƽ̨��adc
    �汾        ��  0.1
    ����        ��  ·ȽȽ
    ��������    ��  2010.05
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

int main(int argn, char **arg)
{
	char str[256];
	char command[20];
	FILE *fp;

	char *mainlib;
	char *addlib;

	if (argn != 3) {
		printf ("should be not 2 file\n");
		return 1;
	}
	mainlib = arg[1];
	fp = fopen (mainlib, "r");
	if (fp == NULL) {
		printf ("error file\n");
		return 1;
	}
	fclose (fp);

	addlib = arg[2];
	fp = fopen (addlib, "r");
	if (fp == NULL) {
		printf ("error file\n");
		return 1;
	}
	fclose (fp);
	printf ("start to expand addlib file\n");
	sprintf (command, "arm-linux-ar -x %s", addlib);
	system (command);
	printf ("start to check the addlib out to a.txt \n");
	  // ��ar -t��ʾģ�����б�,������a.txt�ļ���,����ܱ������ڴ���Ϳ���
	sprintf (command, "arm-linux-ar -t %s > a.txt", addlib);
	system (command);

	fp = fopen ("a.txt", "r");
	if (fp == NULL) {
		printf ("error\n");
		return 1;
	}
	printf ("start to add %s to %s \n",mainlib,addlib);
	while (-1 != fscanf (fp, "%s", str)) {
		sprintf (command, "arm-linux-ar -r %s %s", mainlib, str);
		//sprintf (command, "arm-linux-ar -d %s %s", mainlib, str);
		printf ("%s\n", command);
		system (command);
		sprintf (command, "rm -f %s ", str);
		system (command);
	}
	system ("rm -f a.txt");
	fclose (fp);
	printf ("done \n");
	return 0;
}
