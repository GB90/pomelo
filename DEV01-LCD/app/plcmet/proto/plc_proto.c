/**
* plc_proto.c -- �ز�Ӧ��Э��ӿ�
* 
* 
* ����ʱ��: 2010-5-21
* ����޸�ʱ��: 2010-5-21
*/

#include <stdio.h>
#include <string.h>

#include "include/debug.h"
#include "include/sys/timeal.h"
#include "plcmet/plcomm.h"
#include "plc_proto.h"
#include "dl645_1997.h"

static const plcmet_prot_t PlcProtoList[] = {
	{1, Dl1997MakeReadPkt, Dl1997MakeWritePkt, Dl1997MakeChkTimePkt, Dl1997CheckRead, Dl1997CheckWrite},
};
#define NUM_LIST	(sizeof(PlcProtoList)/sizeof(PlcProtoList[0]))

const plcmet_prot_t *GetPlcMetProto(unsigned char proto)
{
	int i;

	for(i=0; i<NUM_LIST; i++) {
		if(PlcProtoList[i].protoid == proto) return &PlcProtoList[i];
	}

	return NULL;
}
