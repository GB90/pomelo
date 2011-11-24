/**
* route.h -- ·�ɱ����
* 
* 
* ����ʱ��: 2010-5-20
* ����޸�ʱ��: 2010-5-20
*/

#ifndef _PARAM_ROUTE_H
#define _PARAM_ROUTE_H

#include "include/param/capconf.h"

#define MAX_ROUTE_LEVEL		5  //���·�ɼ���
#define MAX_ROUTE_ONEMET	4  //һ���������·����

typedef struct {
	unsigned char level;  //����
	unsigned char unuse;
	unsigned char addr[MAX_ROUTE_LEVEL*6];
} cfg_route_t;

typedef struct {
	unsigned char num;
	unsigned char unuse[3];
	cfg_route_t route[MAX_ROUTE_ONEMET];
} cfg_route_met_t;

#ifndef DEFINE_PARAROUTE
extern const cfg_route_met_t ParaRoute[MAX_METER];
#endif

#endif /*_PARAM_ROUTE_H*/

