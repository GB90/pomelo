/**
* cenmet_proto.c -- ���Э��
* 
* 
* ����ʱ��: 2010-5-17
* ����޸�ʱ��: 2010-5-17
*/

#include <stdio.h>
#include <dlfcn.h>

#define DEFINE_CENMETPROTO

#include "include/environment.h"
#include "include/debug.h"
#include "include/sys/rs485.h"
#include "include/sys/schedule.h"
#include "include/param/capconf.h"
#include "include/param/meter.h"
#include "mdb/mdbconf.h"
#include "mdb/mdbcur.h"
#include "cenmet_proto.h"

#define METPROTO_FILENAME		TEMP_PATH "metproto.so"

typedef struct {
	int (*rs485_send)(unsigned int port, const unsigned char *buf, int len);
	int (*rs485_recv)(unsigned int port, unsigned char *buf, int maxlen);
	void (*rs485_set)(unsigned int port, int speed, int databits, int stopbits, char parity);
	void (*print)(int level, const char *format, ...);
	void (*sleep)(unsigned long ticks);
	void (*updatedb)(unsigned short tnid, unsigned short itemid, const unsigned char *buf, int len, unsigned char flag);
	void (*setalm)(unsigned short alarmid, unsigned char tn, unsigned char flag);
	int (*get_logolvl)(void);
} proto_calls_t;


unsigned int CenMetProtoValid = 0;

static void *MProtoHandle = NULL;

static void MProtoSleep(unsigned long ticks)
{
	Sleep(ticks);
}

static void MProtoSetAlarm(unsigned short alarmid, unsigned char tn, unsigned char flag)
{
}

static const proto_calls_t mproto_calls = {
	Rs485Send,
	Rs485Recv,
	Rs485Set,
	PrintLog,
	MProtoSleep,
	UpdateMdbCurrent,
	MProtoSetAlarm,
	GetLogType,
};

static int (*fn_init)(const proto_calls_t *list);
static int (*fn_getfunc)(unsigned char protoid, unsigned char functype, metfunc_t **pfunc);
static int (*fn_getstdlen)(unsigned short itemid);
static int (*fn_getinfo)(unsigned char *buf);

/**
* @brief �����Э��
* @return �ɹ�0, ����ʧ��
*/
DECLARE_INIT_FUNC(LoadCenMetProto);
int LoadCenMetProto(void)
{
	const char *errmsg;

	CenMetProtoValid = 0;
	if(NULL != MProtoHandle) dlclose(MProtoHandle);

	MProtoHandle = dlopen(METPROTO_FILENAME, RTLD_NOW);
	if(NULL == MProtoHandle) {
		printf("open met proto file failed!\n");
		return 1;
	}

	fn_init = NULL;
	fn_getfunc = NULL;
	fn_getstdlen = NULL;
	fn_getinfo = NULL;

	fn_init = dlsym(MProtoHandle, "mproto_init");
	if((errmsg = dlerror()) != NULL) goto mark_fail;

	fn_init(&mproto_calls);

	fn_getfunc = dlsym(MProtoHandle, "get_metfunc");
	if((errmsg = dlerror()) != NULL) goto mark_fail;

	fn_getstdlen = dlsym(MProtoHandle, "get_stdlen");
	if((errmsg = dlerror()) != NULL) goto mark_fail;

	fn_getinfo = dlsym(MProtoHandle, "get_info");
	if((errmsg = dlerror()) != NULL) goto mark_fail;

	printf("load met proto OK.\r\n");
	CenMetProtoValid = 1;
	SET_INIT_FLAG(LoadCenMetProto);
	return 0;

mark_fail:
	dlclose(MProtoHandle);
	MProtoHandle = NULL;
	printf("init met proto dll failed!\r\n");
	return 1;
}

/**
* @brief ��ӡ��Э����Ϣ
* @param �����Э����Ϣ������ָ��
* @return �ɹ�����1,ʧ�ܷ���0
*/
int PrintCMetProtoInfo(unsigned char *buf)
{
	if(!CenMetProtoValid) return 0;

	return(fn_getinfo(buf));
}

/**
* @brief ��ñ�Ʒ��ʺ���ָ��
* @param metid ���ID
* @param functype ��������, 0-������, 1-д����
* @return ����ָ��
*/
metfunc_t *GetCMetFunc(unsigned char metid, unsigned char functype)
{
	metfunc_t *pfunc = NULL;

	if(!CenMetProtoValid) return NULL;

	if(fn_getfunc(ParaMeter[metid].proto, functype, &pfunc)) pfunc = NULL;

	return(pfunc);
}

/**
* @brief ����������׼����(DL645-1997)
* @param �������ʶ
* @return �������
*/
int GetCMetStdLen(unsigned short itemid)
{
	int len = 0;

	if(CenMetProtoValid) len = fn_getstdlen(itemid);

	return(len);
}

/**
* @brief ��ñ��Э������
* @param metid ���ID
* @param attr ���ص����Ա���ָ��
* @return ��ȡ��Ŀ��
*/
int GetCMetAttr(unsigned char metid, unsigned char *pattr)
{
	metfunc_t *pfunc = GetCMetFunc(metid, FUNCTYPE_READ);

	if(NULL == pfunc) {
		*pattr = METATTR_EMPTY;
		return 0;
	}

	*pattr = pfunc->attr;
	return(pfunc->read_num);
}

