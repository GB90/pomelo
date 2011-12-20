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
    �ļ���         ��  c-tx-comport-setconfig-getconfig.c
    ����	       ��  ���ļ�����ƽ̨��setconfig��getconfig�����Ĳ���
    �汾              ��  0.1
    ����              ��  ����
    ��������    ��  2010.03
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>			//exit
#include <unistd.h>			//sleep
#include <db.h>
#include <string.h>
//ƽ̨��ͷ�ļ�
#include "../include/comport.h"
#include "../include/xj_assert.h"
#include "../include/error.h"

int main()
{
	int ret,i;
	//�����ĳ�ʼ��
	inittest();
	u8 port = 2;
	u8 mode = COMPORT_MODE_NORMAL;
	ret = comport_init(port, mode);
	if(ret < 0){
		printf("test1:init error!\n");
		goto error;
	}

	/******��������1(�������ܣ�����������)****************/
	u32 baud_set;
	comport_config_t cfg_get;
	comport_config_t cfg_set = {COMPORT_VERIFY_EVEN, 7,  1, 20, baud_set, COMPORT_RTSCTS_ENABLE};
	u8 flag = 0;
	for (i=0;i<12;i++){
		switch(i){
		case 0:
			baud_set = 50;
			break;
		case 1:
			baud_set = 110;
			break;
		case 2:
			baud_set = 150;
			break;
		case 3:
			baud_set = 300;
			break;
		case 4:
			baud_set = 600;
			break;
		case 5:
			baud_set = 1200;
			break;
		case 6:
			baud_set = 2400;
			break;
		case 7:
			baud_set = 4800;
			break;
		case 8:
			baud_set = 9600;
			break;
		case 9:
			baud_set = 115200;
			break;
		case 10:
			baud_set = 460800;
			break;
		case 11:
			baud_set = 4000000;
			break;
		default:
			goto error;
		}
		cfg_set.baud = baud_set;
		ret = comport_setconfig (port, &cfg_set);
		if(ret<0){
			assert(ret==0,"test1:setconfig error!");
			goto error;
		}
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set.baud == cfg_get.baud) &&
			   (cfg_set.verify == cfg_get.verify) &&
			   (cfg_set.ndata == cfg_get.ndata) &&
			   (cfg_set.nstop == cfg_get.nstop) &&
			   (cfg_set.rtscts == cfg_get.rtscts) &&
			   (cfg_set.verify == cfg_get.verify) &&
			   (cfg_set.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 12;
		}
	}
	assert(flag==0,"test1:comport setconfig error");

	/******��������2(�������ԣ���������λ)****************/
	u8 ndata_set = 7;
	comport_config_t cfg_set1 = {COMPORT_VERIFY_EVEN, ndata_set,  1, 20, 9600, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<2;i++){
		switch(i){
		case 0:
			ndata_set = 7;
			break;
		case 1:
			ndata_set = 8;
			break;
		case 2:
			ndata_set = 9;
			break;
		default:
			goto error;
		}
		cfg_set1.ndata = ndata_set;
		ret = comport_setconfig (port, &cfg_set1);
		if(ret<0){
			assert(ret==0,"test2:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set1.baud == cfg_get.baud) &&
			   (cfg_set1.verify == cfg_get.verify) &&
			   (cfg_set1.ndata == cfg_get.ndata) &&
			   (cfg_set1.nstop == cfg_get.nstop) &&
			   (cfg_set1.rtscts == cfg_get.rtscts) &&
			   (cfg_set1.verify == cfg_get.verify) &&
			   (cfg_set1.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test2:comport setconfig error");

	/******��������3(�������ԣ�����У�鷽ʽ)****************/
	u8 verify_set = 7;
	comport_config_t cfg_set2 = {verify_set, 7,  1, 20, 9600, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<2;i++){
		switch(i){
		case 0:
			verify_set = COMPORT_VERIFY_EVEN;
			break;
		case 1:
			verify_set = COMPORT_VERIFY_ODD;
			break;
		case 2:
			verify_set = COMPORT_VERIFY_NO;
			break;
		default:
			goto error;
		}
		cfg_set2.verify = verify_set;
		ret = comport_setconfig (port, &cfg_set2);
		if(ret<0){
			assert(ret==0,"test3:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set2.baud == cfg_get.baud) &&
			   (cfg_set2.verify == cfg_get.verify) &&
			   (cfg_set2.ndata == cfg_get.ndata) &&
			   (cfg_set2.nstop == cfg_get.nstop) &&
			   (cfg_set2.rtscts == cfg_get.rtscts) &&
			   (cfg_set2.verify == cfg_get.verify) &&
			   (cfg_set2.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test3:comport set-getconfig error");

	/******��������4���������ԣ�����ֹͣλ��****************/
	u8 nstop_set = 7;
	comport_config_t cfg_set3 = {COMPORT_VERIFY_NO, 7,  nstop_set, 20, 9600, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<1;i++){
		switch(i){
		case 0:
			nstop_set = 1;
			break;
		case 1:
			nstop_set = 2;
			break;
		default:
			goto error;
		}
		cfg_set3.nstop = nstop_set;
		ret = comport_setconfig (port, &cfg_set3);
		if(ret<0){
			assert(ret==0,"test4:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set3.baud == cfg_get.baud) &&
			   (cfg_set3.verify == cfg_get.verify) &&
			   (cfg_set3.ndata == cfg_get.ndata) &&
			   (cfg_set3.nstop == cfg_get.nstop) &&
			   (cfg_set3.rtscts == cfg_get.rtscts) &&
			   (cfg_set3.verify == cfg_get.verify) &&
			   (cfg_set3.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test4:comport set-getconfig error");

	/******��������5���������ԣ�������ʱ��****************/
	u8 timeout_set = 7;
	comport_config_t cfg_set4 = {COMPORT_VERIFY_NO, 7,  1, timeout_set, 9600, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<1;i++){
		switch(i){
		case 0:
			timeout_set = 0;
			break;
		case 1:
			timeout_set = 0xff;
			break;
		default:
			goto error;
		}
		cfg_set4.timeout = timeout_set;
		ret = comport_setconfig (port, &cfg_set4);
		if(ret<0){
			assert(ret==0,"test5:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set4.baud == cfg_get.baud) &&
			   (cfg_set4.verify == cfg_get.verify) &&
			   (cfg_set4.ndata == cfg_get.ndata) &&
			   (cfg_set4.nstop == cfg_get.nstop) &&
			   (cfg_set4.rtscts == cfg_get.rtscts) &&
			   (cfg_set4.verify == cfg_get.verify) &&
			   (cfg_set4.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test5:comport set-getconfig error");

	/******��������6���������ԣ�����Ӳ�����أ�****************/
	u8 rtscts_set = 7;
	comport_config_t cfg_set5 = {COMPORT_VERIFY_NO, 7,  1, 20, 9600, rtscts_set};;
	flag = 0;
	for (i=0;i<1;i++){
		switch(i){
		case 0:
			rtscts_set = COMPORT_RTSCTS_ENABLE;
			break;
		case 1:
			rtscts_set = COMPORT_RTSCTS_DISABLE;
			break;
		default:
			goto error;
		}
		cfg_set5.rtscts = rtscts_set;
		ret = comport_setconfig (port, &cfg_set5);
		if(ret<0){
			assert(ret==0,"test6:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set5.baud == cfg_get.baud) &&
			   (cfg_set5.verify == cfg_get.verify) &&
			   (cfg_set5.ndata == cfg_get.ndata) &&
			   (cfg_set5.nstop == cfg_get.nstop) &&
			   (cfg_set5.rtscts == cfg_get.rtscts) &&
			   (cfg_set5.verify == cfg_get.verify) &&
			   (cfg_set5.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test6:comport set-getconfig error");

	/******��������7���߽���ԣ�����0���������ʣ�****************/
	port = 0;
	mode = COMPORT_MODE_NORMAL;
	ret = comport_init(port, mode);
	if(ret < 0){
		printf("test7:init error!\n");
		goto error;
	}
	comport_config_t cfg_set6 = {COMPORT_VERIFY_EVEN, 7,  1, 20, baud_set, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<12;i++){
		switch(i){
		case 0:
			baud_set = 50;
			break;
		case 1:
			baud_set = 110;
			break;
		case 2:
			baud_set = 150;
			break;
		case 3:
			baud_set = 300;
			break;
		case 4:
			baud_set = 600;
			break;
		case 5:
			baud_set = 1200;
			break;
		case 6:
			baud_set = 2400;
			break;
		case 7:
			baud_set = 4800;
			break;
		case 8:
			baud_set = 9600;
			break;
		case 9:
			baud_set = 115200;
			break;
		case 10:
			baud_set = 460800;
			break;
		case 11:
			baud_set = 4000000;
			break;
		default:
			goto error;
		}
		cfg_set6.baud = baud_set;
		ret = comport_setconfig (port, &cfg_set6);
		if(ret<0){
			assert(ret==0,"test7:setconfig error!");
			goto error;
		}
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set6.baud == cfg_get.baud) &&
			   (cfg_set6.verify == cfg_get.verify) &&
			   (cfg_set6.ndata == cfg_get.ndata) &&
			   (cfg_set6.nstop == cfg_get.nstop) &&
			   (cfg_set6.rtscts == cfg_get.rtscts) &&
			   (cfg_set6.verify == cfg_get.verify) &&
			   (cfg_set6.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 12;
		}
	}
	assert(flag==0,"test7:comport setconfig error");

	/******��������8���߽���ԣ�����0��������λ��****************/
	port = 0;
	ndata_set = 7;
	comport_config_t cfg_set7 = {COMPORT_VERIFY_EVEN, ndata_set,  1, 20, 9600, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<2;i++){
		switch(i){
		case 0:
			ndata_set = 7;
			break;
		case 1:
			ndata_set = 8;
			break;
		case 2:
			ndata_set = 9;
			break;
		default:
			goto error;
		}
		cfg_set7.ndata = ndata_set;
		ret = comport_setconfig (port, &cfg_set7);
		if(ret<0){
			assert(ret==0,"test8:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set7.baud == cfg_get.baud) &&
			   (cfg_set7.verify == cfg_get.verify) &&
			   (cfg_set7.ndata == cfg_get.ndata) &&
			   (cfg_set7.nstop == cfg_get.nstop) &&
			   (cfg_set7.rtscts == cfg_get.rtscts) &&
			   (cfg_set7.verify == cfg_get.verify) &&
			   (cfg_set7.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test8:comport setconfig error");

	/******��������9���߽���ԣ�����0����У��λ��****************/
	verify_set = 7;
	comport_config_t cfg_set8 = {verify_set, 7,  1, 20, 9600, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<2;i++){
		switch(i){
		case 0:
			verify_set = COMPORT_VERIFY_EVEN;
			break;
		case 1:
			verify_set = COMPORT_VERIFY_ODD;
			break;
		case 2:
			verify_set = COMPORT_VERIFY_NO;
			break;
		default:
			goto error;
		}
		cfg_set8.verify = verify_set;
		ret = comport_setconfig (port, &cfg_set8);
		if(ret<0){
			assert(ret==0,"test9:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set8.baud == cfg_get.baud) &&
			   (cfg_set8.verify == cfg_get.verify) &&
			   (cfg_set8.ndata == cfg_get.ndata) &&
			   (cfg_set8.nstop == cfg_get.nstop) &&
			   (cfg_set8.rtscts == cfg_get.rtscts) &&
			   (cfg_set8.verify == cfg_get.verify) &&
			   (cfg_set8.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test9:comport set-getconfig error");

	/******��������10���߽���ԣ�����0����ֹͣλ��****************/
	port = 0;
	nstop_set = 7;
	comport_config_t cfg_set9 = {COMPORT_VERIFY_NO, 7,  nstop_set, 20, 9600, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<1;i++){
		switch(i){
		case 0:
			nstop_set = 1;
			break;
		case 1:
			nstop_set = 2;
			break;
		default:
			goto error;
		}
		cfg_set9.nstop = nstop_set;
		ret = comport_setconfig (port, &cfg_set9);
		if(ret<0){
			assert(ret==0,"test10:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set9.baud == cfg_get.baud) &&
			   (cfg_set9.verify == cfg_get.verify) &&
			   (cfg_set9.ndata == cfg_get.ndata) &&
			   (cfg_set9.nstop == cfg_get.nstop) &&
			   (cfg_set9.rtscts == cfg_get.rtscts) &&
			   (cfg_set9.verify == cfg_get.verify) &&
			   (cfg_set9.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test10:comport set-getconfig error");

	/******��������11���߽���ԣ�����0������ʱ��****************/
	port = 0;
	timeout_set = 7;
	comport_config_t cfg_set10 = {COMPORT_VERIFY_NO, 7,  1, timeout_set, 9600, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<1;i++){
		switch(i){
		case 0:
			timeout_set = 0;
			break;
		case 1:
			timeout_set = 0xff;
			break;
		default:
			goto error;
		}
		cfg_set10.timeout = timeout_set;
		ret = comport_setconfig (port, &cfg_set10);
		if(ret<0){
			assert(ret==0,"test11:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set10.baud == cfg_get.baud) &&
			   (cfg_set10.verify == cfg_get.verify) &&
			   (cfg_set10.ndata == cfg_get.ndata) &&
			   (cfg_set10.nstop == cfg_get.nstop) &&
			   (cfg_set10.rtscts == cfg_get.rtscts) &&
			   (cfg_set10.verify == cfg_get.verify) &&
			   (cfg_set10.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test11:comport set-getconfig error");

	/******��������12���߽���ԣ�����0����Ӳ�����أ�****************/
	port = 0;
	rtscts_set = 7;
	comport_config_t cfg_set11 = {COMPORT_VERIFY_NO, 7,  1, 20, 9600, rtscts_set};;
	flag = 0;
	for (i=0;i<1;i++){
		switch(i){
		case 0:
			rtscts_set = COMPORT_RTSCTS_ENABLE;
			break;
		case 1:
			rtscts_set = COMPORT_RTSCTS_DISABLE;
			break;
		default:
			goto error;
		}
		cfg_set11.rtscts = rtscts_set;
		ret = comport_setconfig (port, &cfg_set11);
		if(ret<0){
			assert(ret==0,"test12:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set11.baud == cfg_get.baud) &&
			   (cfg_set11.verify == cfg_get.verify) &&
			   (cfg_set11.ndata == cfg_get.ndata) &&
			   (cfg_set11.nstop == cfg_get.nstop) &&
			   (cfg_set11.rtscts == cfg_get.rtscts) &&
			   (cfg_set11.verify == cfg_get.verify) &&
			   (cfg_set11.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test12:comport set-getconfig error");

	/******��������13���߽���ԣ�����6���������ʣ�****************/
	port = 6;
	mode = COMPORT_MODE_NORMAL;
	ret = comport_init(port, mode);
	if(ret < 0){
		printf("test13:init error!\n");
		goto error;
	}
	comport_config_t cfg_set12 = {COMPORT_VERIFY_EVEN, 7,  1, 20, baud_set, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<12;i++){
		switch(i){
		case 0:
			baud_set = 50;
			break;
		case 1:
			baud_set = 110;
			break;
		case 2:
			baud_set = 150;
			break;
		case 3:
			baud_set = 300;
			break;
		case 4:
			baud_set = 600;
			break;
		case 5:
			baud_set = 1200;
			break;
		case 6:
			baud_set = 2400;
			break;
		case 7:
			baud_set = 4800;
			break;
		case 8:
			baud_set = 9600;
			break;
		case 9:
			baud_set = 115200;
			break;
		case 10:
			baud_set = 460800;
			break;
		case 11:
			baud_set = 4000000;
			break;
		default:
			goto error;
		}
		cfg_set12.baud = baud_set;
		ret = comport_setconfig (port, &cfg_set12);
		if(ret<0){
			assert(ret==0,"test13:setconfig error!");
			goto error;
		}
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set12.baud == cfg_get.baud) &&
			   (cfg_set12.verify == cfg_get.verify) &&
			   (cfg_set12.ndata == cfg_get.ndata) &&
			   (cfg_set12.nstop == cfg_get.nstop) &&
			   (cfg_set12.rtscts == cfg_get.rtscts) &&
			   (cfg_set12.verify == cfg_get.verify) &&
			   (cfg_set12.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 12;
		}
	}
	assert(flag==0,"test13:comport setconfig error");

	/******��������14���߽���ԣ�����6��������λ��****************/
	port =6;
	ndata_set = 7;
	comport_config_t cfg_set13 = {COMPORT_VERIFY_EVEN, ndata_set,  1, 20, 9600, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<2;i++){
		switch(i){
		case 0:
			ndata_set = 7;
			break;
		case 1:
			ndata_set = 8;
			break;
		case 2:
			ndata_set = 9;
			break;
		default:
			goto error;
		}
		cfg_set13.ndata = ndata_set;
		ret = comport_setconfig (port, &cfg_set13);
		if(ret<0){
			assert(ret==0,"test2:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set13.baud == cfg_get.baud) &&
			   (cfg_set13.verify == cfg_get.verify) &&
			   (cfg_set13.ndata == cfg_get.ndata) &&
			   (cfg_set13.nstop == cfg_get.nstop) &&
			   (cfg_set13.rtscts == cfg_get.rtscts) &&
			   (cfg_set13.verify == cfg_get.verify) &&
			   (cfg_set13.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test14:comport setconfig error");

	/******��������15���߽���ԣ�����6����У��λ��****************/
	port = 6;
	verify_set = 7;
	comport_config_t cfg_set14 = {verify_set, 7,  1, 20, 9600, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<2;i++){
		switch(i){
		case 0:
			verify_set = COMPORT_VERIFY_EVEN;
			break;
		case 1:
			verify_set = COMPORT_VERIFY_ODD;
			break;
		case 2:
			verify_set = COMPORT_VERIFY_NO;
			break;
		default:
			goto error;
		}
		cfg_set14.verify = verify_set;
		ret = comport_setconfig (port, &cfg_set14);
		if(ret<0){
			assert(ret==0,"test15:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set14.baud == cfg_get.baud) &&
			   (cfg_set14.verify == cfg_get.verify) &&
			   (cfg_set14.ndata == cfg_get.ndata) &&
			   (cfg_set14.nstop == cfg_get.nstop) &&
			   (cfg_set14.rtscts == cfg_get.rtscts) &&
			   (cfg_set14.verify == cfg_get.verify) &&
			   (cfg_set14.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test15:comport set-getconfig error");

	/******��������16���߽���ԣ�����6����ֹͣλ��****************/
	port = 6;
	nstop_set = 7;
	comport_config_t cfg_set15 = {COMPORT_VERIFY_NO, 7,  nstop_set, 20, 9600, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<1;i++){
		switch(i){
		case 0:
			nstop_set = 1;
			break;
		case 1:
			nstop_set = 2;
			break;
		default:
			goto error;
		}
		cfg_set15.nstop = nstop_set;
		ret = comport_setconfig (port, &cfg_set15);
		if(ret<0){
			assert(ret==0,"test16:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set15.baud == cfg_get.baud) &&
			   (cfg_set15.verify == cfg_get.verify) &&
			   (cfg_set15.ndata == cfg_get.ndata) &&
			   (cfg_set15.nstop == cfg_get.nstop) &&
			   (cfg_set15.rtscts == cfg_get.rtscts) &&
			   (cfg_set15.verify == cfg_get.verify) &&
			   (cfg_set15.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test16:comport set-getconfig error");

	/******��������17���߽���ԣ�����6������ʱ��****************/
	port = 6;
	timeout_set = 7;
	comport_config_t cfg_set16 = {COMPORT_VERIFY_NO, 7,  1, timeout_set, 9600, COMPORT_RTSCTS_ENABLE};;
	flag = 0;
	for (i=0;i<1;i++){
		switch(i){
		case 0:
			timeout_set = 0;
			break;
		case 1:
			timeout_set = 0xff;
			break;
		default:
			goto error;
		}
		cfg_set16.timeout = timeout_set;
		ret = comport_setconfig (port, &cfg_set16);
		if(ret<0){
			assert(ret==0,"test17:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set10.baud == cfg_get.baud) &&
			   (cfg_set16.verify == cfg_get.verify) &&
			   (cfg_set16.ndata == cfg_get.ndata) &&
			   (cfg_set16.nstop == cfg_get.nstop) &&
			   (cfg_set16.rtscts == cfg_get.rtscts) &&
			   (cfg_set16.verify == cfg_get.verify) &&
			   (cfg_set16.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test17:comport set-getconfig error");

	/******��������18���߽���ԣ�����6����Ӳ�����أ�****************/
	port = 6;
	rtscts_set = 7;
	comport_config_t cfg_set17 = {COMPORT_VERIFY_NO, 7,  1, 20, 9600, rtscts_set};;
	flag = 0;
	for (i=0;i<1;i++){
		switch(i){
		case 0:
			rtscts_set = COMPORT_RTSCTS_ENABLE;
			break;
		case 1:
			rtscts_set = COMPORT_RTSCTS_DISABLE;
			break;
		default:
			goto error;
		}
		cfg_set17.rtscts = rtscts_set;
		ret = comport_setconfig (port, &cfg_set17);
		if(ret<0){
			assert(ret==0,"test18:setconfig error!");
			goto error;
		}
		memset(&cfg_get, 0,sizeof(comport_config_t));
		ret = comport_getconfig(port, &cfg_get);
		if((cfg_set17.baud == cfg_get.baud) &&
			   (cfg_set17.verify == cfg_get.verify) &&
			   (cfg_set17.ndata == cfg_get.ndata) &&
			   (cfg_set17.nstop == cfg_get.nstop) &&
			   (cfg_set17.rtscts == cfg_get.rtscts) &&
			   (cfg_set17.verify == cfg_get.verify) &&
			   (cfg_set17.timeout == cfg_get.timeout) &&
			   (ret==0)){
			continue;
		}else{
			p_err(ret);
			flag = 1;
			i = 4;
		}
	}
	assert(flag==0,"test18:comport set-getconfig error");

	/*************��������19��������ԣ����ںŴ���***********************************/
	port = 10;
	int ret1;
	comport_config_t cfg_set18 = {COMPORT_VERIFY_EVEN, 7,  1, 20,9600, COMPORT_RTSCTS_ENABLE};
	ret = comport_setconfig (port, &cfg_set18);
	ret1 = comport_getconfig(port, &cfg_get);
	assert((ret == -ERR_INVAL)&&(ret1 == -ERR_INVAL),"test19:comport set-getconfig error");

	/*************��������20��������ԣ������ʴ���***********************************/
	port = 2;
	comport_config_t cfg_set19 = {COMPORT_VERIFY_EVEN, 7,  1, 20,100, COMPORT_RTSCTS_ENABLE};
	ret = comport_setconfig (port, &cfg_set19);
	assert(ret == -ERR_INVAL,"test20:comport set-getconfig error");

	/*************��������21��������ԣ�����λ����***********************************/
	port = 2;
	comport_config_t cfg_set20 = {COMPORT_VERIFY_EVEN, 1,  1, 20, 9600, COMPORT_RTSCTS_ENABLE};
	ret = comport_setconfig (port, &cfg_set20);
	assert(ret == -ERR_INVAL,"test21:comport set-getconfig error");

	/*************��������22��������ԣ�У�鷽ʽ����***********************************/
	port = 2;
	comport_config_t cfg_set21 = {12, 7,  1, 20, 9600, COMPORT_RTSCTS_ENABLE};
	ret = comport_setconfig (port, &cfg_set21);
	assert(ret == -ERR_INVAL,"test22:comport set-getconfig error");

	/*************��������23��������ԣ�ֹͣλ����***********************************/
	port = 2;
	comport_config_t cfg_set22 = {COMPORT_VERIFY_EVEN, 7,  3, 20, 9600, COMPORT_RTSCTS_ENABLE};
	ret = comport_setconfig (port, &cfg_set22);
	assert(ret == -ERR_INVAL,"test22:comport set-getconfig error");

	/*************��������24��������ԣ�Ӳ�����ش���***********************************/
	port = 2;
	comport_config_t cfg_set23 = {COMPORT_VERIFY_EVEN, 7,  1, 20, 9600, 9};
	ret = comport_setconfig (port, &cfg_set23);
	assert(ret == -ERR_INVAL,"test24:comport set-getconfig error");

	finaltest();
error:
	exit(0);
}

