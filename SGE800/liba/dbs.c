/*****************************************************************************
	��̵����ɷ����޹�˾			��Ȩ��2008-2015

	��Դ���뼰������ĵ�Ϊ���������̵����ɷ����޹�˾�������У�δ��������
	�ɲ��������޸Ļ򷢲�������׷����صķ������Ρ�

						���������̹ɷ����޹�˾
						www.xjgc.com
						(0374) 321 2924
*****************************************************************************/


/******************************************************************************
	��Ŀ����	��SGE800���������ն�ƽ̨
	�ļ�		��dbs.c
	����		�����ļ�ʵ�������ݿ����ģ���е�API����
	�汾		��0.1
	����		������
	��������	��2009.12
******************************************************************************/

//������ͷ�ļ�
#include "private/config.h"

//ģ�����ÿ���
#ifdef CFG_DBS_MODULE

//����ͷ�ļ�
#include "private/debug.h"

//��������ͷ�ļ�

//C��ͷ�ļ�
#include <stdio.h>						//printf
#include <fcntl.h>						//open
#include <unistd.h>						//read,write
#include <string.h>						//bzero
#include <errno.h>
#include <db.h>


//�ṩ���û���ͷ�ļ�
#include "include/error.h"
#include "include/dbs.h"

/*************************************************
  ��̬ȫ�ֱ�������
*************************************************/
static DB_ENV *dbenv = NULL;                         //���ݿ⻷�����
static DB *dbp[CFG_DBS_UNM_MAX];                     //���ݿ���      
/*************************************************
  �ṹ���Ͷ���
*************************************************/
static struct {
	pthread_mutex_t dbp_mutex;  //������  
	u8 dbp_count;               //����
	u8 dbp_mode;                //�򿪷�ʽ
	u8 dbp_pos;                 //��λ��
}dbp_info[CFG_DBS_UNM_MAX];     //����

//���洢����·��
static const char *const path_ram = "/var/";
static const char *const path_flash_code = "/mnt/local/";
static const char *const path_flash_data = "/mnt/data0/";
static const char *const path_sd = "/mnt/sddisk/";
static const char *const path_u = "/mnt/udisk/";

/*******************************API����ʵ��***********************************/

/******************************************************************************
*	����:	dbs_init
*	����:	��ʼ�����ݿ�
*	����:	��
*	����:	0				-	�ɹ�
			-ERR_SYS		-	ϵͳ����
			-ERR_INVAL		-	������Ч
			-ERR_BUSY		-	�Ѿ���ʼ��

*	˵��:	��ʼ�����ݿ⻷�������ݿ������
******************************************************************************/
int dbs_init()
{
	int ret = 0;
	int ref;
	int i;
	 
	for (i = 0;i < CFG_DBS_UNM_MAX;i ++){            //��ʼ�����ݿ���
		dbp[i] = NULL;
	}
	
	for (i = 0;i < CFG_DBS_UNM_MAX;i ++){            //��ʼ�����ݿ�򿪹رռ�����
		dbp_info[i].dbp_count = 0;
	}
	
	for (i = 0;i < CFG_DBS_UNM_MAX;i ++){            //��ʼ�����ݿ�����������
		ref = pthread_mutex_init(&(dbp_info[i].dbp_mutex),NULL);
		if (ref){
			ret = -ERR_SYS;
			goto error;
		}
	}
 
	ref = db_env_create(&dbenv, 0);                       //�������ݿ⻷�����
	if (ref){
		DBSPRINTF("create db handle error!\n");
		ret = -ERR_SYS;
		goto error;
	}                    
	
	//���ù����ڴ�ؿռ��С��Ĭ��256K
	if (CFG_DBS_MPOOL_SIZE < MPOOL_SIZE_MIN){
		ret = -ERR_INVAL;
		goto error;
	}
	ref = dbenv->set_cachesize(dbenv, 0, CFG_DBS_MPOOL_SIZE, 0);   
	if (ref !=0){
		DBSPRINTF("set cachesize error!\n");
		ret = -ERR_SYS;
		goto error;
	}
	
    //������denv->set_timeout �������ĳ�ʱ
    
    //�򿪻���
	ref = dbenv->open (dbenv, CFG_DBS_ENV_HOME, CFG_DBS_ENV_FLAG, 0);    //��ʼ�����ݴ洢��ϵͳ
	if (ref == ENOSPC ){
		DBSPRINTF("env has opened error!ref =%d\n",ref);
		ret = -ERR_BUSY; 
		goto error;
	}else if (ref){
		DBSPRINTF("env initial fail!\n");
		ret = -ERR_SYS;
		goto error;
	}
	DBSPRINTF("env initial success!\n");
	
error:
	return(ret);	
}
/******************************************************************************
*	����:	dbs_open
*	����:	�����ݿ�
*	����:	id				-	���ݿ���
			pose			-	��λ��
			mode			-	�򿪷�ʽ
*	����:	0				-	�ɹ�
			-ERR_INVAL		-	��������
			-ERR_SYS		-	ϵͳ����
			-ERR_NOINIT		-	û��ʼ��
			-ERR_BUSY		-	�Ѵ�/�Ѵ���
			-ERR_NOFILE     -   ���ݿ��ļ�������
*	˵��:	��DBS_MODE_CREAT��ʽ�����ݿ⣬������ݿ��Ѿ�������ᱨ��-ERR_BUSY��
��DBS_MODE_RD��ʽ��ֻ�����ݿ⣬���ݿⲻ���ڱ���-ERR_NOFILE��
 ******************************************************************************/
int dbs_open(u8 id, u8 pos, u8 mode)
{
	int ret,ref;
	u32 flag;
	char path[22] = "";                       //���ݿ��·����
	char name[3] = "";						  //���ݿ���
	char index[4] = ".db";                    //���ݿ�����׺
	
	if (dbenv == NULL){                       //δ��ʼ�����ݿ�ģ�飺����δ����
		ret = -ERR_NOINIT;
		return ret;
	}
	
	//�в����ӿ�
	if ((id >= CFG_DBS_UNM_MAX) || (id < 0)){  
		ret = -ERR_INVAL;
		return ret;
	}
	
	//��ü�������
	ref = pthread_mutex_lock(&(dbp_info[id].dbp_mutex));
	if (ref ==  EINVAL){
		DBSPRINTF("mutex lock fail!\n");
		ret = -ERR_NOINIT;
		return ret;
	}else if (ref){                           //ref == EDEADLK
		ret = -ERR_BUSY;
		return ret;
	}else{
		//�򿪼���
		if (dbp_info[id].dbp_count > 0){               //�Ѵ�
			ret = -ERR_BUSY;
			DBSPRINTF("db has opened!\n");
			goto error;
		}else {         
			if ((dbp_info[id].dbp_pos != 0) && (dbp_info[id].dbp_pos != pos)){
				ret = -ERR_INVAL;
				goto error;
			}
			//�����ݿ�
    	    if ((id >= 0) && (id < 10)){             //���ݿ���Ϊһλ����
    	    	name[0] = '0' + id;
    	    }else if(id >9 && id < CFG_DBS_UNM_MAX){ //���ݿ���Ϊ��λ����
    	    	name[0] = '0' + id/10;				 //�����ݿ�����Ϊ���ݿ����ƣ�ȡʮλ����
    	    	name[1] = '0' + id%10;				 //�����ݿ�����Ϊ���ݿ����ƣ�ȡ��λ����
    	    }else{
    	    	ret = -ERR_INVAL;
    	    	goto error;
    	    }

    	    //���ô�λ�ýӿ�
        	switch (pos){
        		case DBS_POS_RAM:
        			strcat (path, path_ram);
        			strcat (path, name);
        			strcat (path, index);
        			break;
        		case DBS_POS_FLASH_CODE:
        			strcat (path, path_flash_code);
        			strcat (path, name);
        			strcat (path, index);
        			break;
    			case DBS_POS_FLASH_DATA:
        			strcat (path, path_flash_data);
        			strcat (path, name);
        			strcat (path, index);
        			break;
        		case DBS_POS_SD:
        			strcat (path, path_sd);
        			strcat (path, name);
        			strcat (path, index);
        			break;
        		case DBS_POS_U:
        			strcat (path, path_u);
        			strcat (path, name);
        			strcat (path, index);
        			break;
        		default:
        			ret = -ERR_INVAL;
        			goto error;
        			break;
        	}
        	
        	//�������ݿ��λ��
        	dbp_info[id].dbp_pos = pos;
            
        	//���ݿ�򿪷�ʽ
        	switch (mode){
        		case DBS_MODE_OPEN:                             //�Զ�д��ʽ�����ݿ⣬���ݿⲻ���ڲ������¿���Ǳ���
        			flag = DB_THREAD;
        			dbp_info[id].dbp_mode = 0;                  //����򿪷�ʽ����д
        			break;
        		case DBS_MODE_CREAT:                            //�����µ����ݿ⣬�˱�����ݿ��Ѵ���ʱ����
        			flag = DB_CREATE | DB_EXCL | DB_THREAD;
        			dbp_info[id].dbp_mode = 0;                  //����򿪷�ʽ����д
        			break;
        		case DBS_MODE_RW:                               //�Զ�д��ʽ�����ݿ⣬���ݿⲻ����ʱ�����¿�
        			flag = DB_CREATE| DB_THREAD;
        			dbp_info[id].dbp_mode = 0;                  //����򿪷�ʽ����д
        			break;
        		case DBS_MODE_RD:                               //��ֻ����ʽ��ֻ�����ݿ⣬���ݿⲻ����ʱ����
        			flag = DB_RDONLY | DB_THREAD;
        			dbp_info[id].dbp_mode = 1;                  //����򿪷�ʽ��ֻ��
        			break;
        		default:
        			ret = -ERR_INVAL;
        			goto error;
        			break;
        	}
		
			//�����������ݿ�
			if (db_create(&dbp[id], dbenv, 0)) {
				ret = -ERR_SYS;
				goto error; 
			}
			
			//�������ݿ�֧�ֶ��ؼ�¼
			if (dbp[id]->set_flags(dbp[id], DB_DUP)){
				ret = -ERR_SYS;
				goto error;
			}
		    
			//�����ݿ�
			ref = dbp[id]->open(dbp[id], NULL, path, NULL, CFG_DBS_ARITH, flag, 0);
			if (ref == ENOENT ){
				ret = -ERR_NOFILE;                  //�˵�ַû�����ݿ����
				goto error;
			}else if (ref == EEXIST){
				ret = -ERR_BUSY;                    //�����¿�ʱ�Ѵ���
				goto error;
			}else if (ref){                         //DB_LOCK_NOTGRANTED
				ret = -ERR_TIMEOUT;
				goto error;
			}
			DBSPRINTF("db open!id =%x\n",id);
		}
		ret = 0;
		dbp_info[id].dbp_count = 1;
		//����
error:
		ref = pthread_mutex_unlock(&(dbp_info[id].dbp_mutex)); //����
		if (ref){   
			DBSPRINTF("mutex unlock fail!\n");
			ret = -ERR_SYS;
		}
	}

	return(ret);        

}
/******************************************************************************
*	����:	dbs_insert
*	����:	д���ؼ�¼���ݣ��ڸ����ݹؼ��ֵ�ָ��λ�ò��뵥����¼��
*	����:	id				-	���ݿ���
			key_size		-	�ؼ��ֳ���
			key             -   �ؼ���
            set             -   ���ؼ�¼��λ
            flag            -   ���ؼ�¼�����־
            data_size       -   ���ݴ�С
            data            -   ����
*	����:	0				-	�ɹ�
			-ERR_INVA		-   �ӿڲ������ô���
			-ERR_NOFILE		-	���ݿ�δ��
			-ERR_NOFUN		-	���߱�дȨ��
			-ERR_NODISK		-	���ݳ���Խ��
			-ERR_TIMEOUT	-	��ʱ
			-ERR_NORECORD	-	��¼������
			-ERR_NOMEM		-   �ڴ����
*	˵��:	�����¼--����ü�¼�����ڣ�������������󽫷���-ERR_NORECORD����
 ******************************************************************************/
int dbs_insert(u8 id, u8 key_size, void *key, dbs_set_t *set, u8 flag, u16 data_size, void  *data)
{
	int ret;
	int ref,i;
	
	DBC *cursorp_w;
	DBT key_w;                            //���ݿ��¼�ṹ
	DBT record_w;
	dbs_set_t set_w;
	u32 num_total;	
	u8 data_middle[CFG_DBS_RECORD_MAX];   //�м�����
	
	//�в����ӿ�
	if ((id >= CFG_DBS_UNM_MAX) || (id < 0)){  
		ret = -ERR_INVAL;
		goto error1;
	}
	
	if (dbp[id] == NULL){                 //���ݿ�δ��
		ret = -ERR_NOFILE;
		goto error1;
	}

	//�жϸñ�����ݿ��Ƿ���дȨ��
	if (dbp_info[id].dbp_mode == 1){      //��ֻ����ʽ��
		ret = -ERR_NOFUN;
		goto error1;
	}
	//�жϹؼ��ֲ����Ƿ�Խ��
	if (key_size > CFG_DBS_KEYSIZE_MAX){
		ret = -ERR_INVAL;
		goto error1;
	}

	//�ж�д�����Ƿ�Խ��
	if (data_size > CFG_DBS_RECORD_MAX){
		ret = -ERR_NODISK;
		goto error1;
	}
	//��ʼ��д�α�
	ref = dbp[id]->cursor(dbp[id], NULL, &cursorp_w, DB_WRITECURSOR);        //Ϊд�α�
	if (ref){                           
		ret = -ERR_TIMEOUT;
		goto error1;
	}
	memset(data_middle, 0, CFG_DBS_RECORD_MAX);
	memset(&key_w, 0, sizeof(DBT));
	memset(&record_w, 0, sizeof(DBT));
	key_w.data = key;
	key_w.size = key_size;

	//���루��ӣ������ؼ�¼��ͷ��
	if ((set->whence == DBS_SEEK_SET) && (set->offset == 0) && (flag == DBS_SEEK_FORWARD) ){
		memset(&key_w, 0, sizeof(DBT));
    	memset(&record_w, 0, sizeof(DBT));
    	key_w.data = key;
    	key_w.size = key_size;
    	record_w.data = data;
    	record_w.size = data_size;
    	ref = cursorp_w->put(cursorp_w, &key_w, &record_w, DB_KEYFIRST);
    	if (ref == EACCES){
    		ret = -ERR_NOFUN;
    		goto error;
    	}else if (ref){
    		ret = -ERR_TIMEOUT;
    		goto error;
    	}
	//���루��ӣ������ؼ�¼��β��
	}else if ((set->whence == DBS_SEEK_END) && (set->offset == 0) && (flag == DBS_SEEK_BACKWARD)){
		memset(&key_w, 0, sizeof(DBT));
    	memset(&record_w, 0, sizeof(DBT));
    	key_w.data = key;
    	key_w.size = key_size;
    	record_w.data = data;
    	record_w.size = data_size;
    	ref = cursorp_w->put(cursorp_w, &key_w, &record_w, DB_KEYLAST);
    	if (ref == EACCES){
    		ret = -ERR_NOFUN;
    		goto error;
    	}else if (ref){
    		ret = -ERR_TIMEOUT;
    		goto error;
    	}
	//�趨λ�µĲ���λ��
	}else{
    	//����ӿ�
    	if (set->whence == DBS_SEEK_END){
    		ref = dbs_count(id,key_size,key,&num_total);   //���ؼ�¼���ܼ�¼��
    		if (ref){
    			ret = -ERR_NORECORD;
    			goto error;
    		}
    		set_w.whence = DBS_SEEK_SET;
    		set_w.offset = num_total - 1 - set->offset;
    		if(set_w.offset < 0){
    			ret = -ERR_NORECORD;
    			goto error;
    		}
    	}else if (set->whence == DBS_SEEK_SET){
    		set_w.whence = DBS_SEEK_SET;
    		set_w.offset = set->offset;
    	}else{
    		ret = -ERR_INVAL;
    		goto error;
    	}
    	//��λ���µĶ�дλ��
		record_w.data = data_middle;
		record_w.ulen = CFG_DBS_RECORD_MAX;
		record_w.flags = DB_DBT_USERMEM;
		ref = cursorp_w->get(cursorp_w, &key_w, &record_w, DB_SET);
		if (ref == DB_NOTFOUND){                          //δ�ҵ����������ļ�¼
			ret = -ERR_NORECORD;
			goto error;
		}else if (ref){
			ret = -ERR_TIMEOUT;
			goto error;
		}
    	for (i = 0;i < set_w.offset;i++){                 //˳��ƫ�Ƶ��µĶ�дλ��
    		memset(data_middle,0,CFG_DBS_RECORD_MAX);
    		memset(&key_w, 0, sizeof(DBT));
    		memset(&record_w, 0, sizeof(DBT));
    		key_w.data = key;
    		key_w.size = key_size;
    		record_w.data = data_middle;
    		record_w.ulen = CFG_DBS_RECORD_MAX;
			record_w.flags = DB_DBT_USERMEM;
    		ref = cursorp_w->get(cursorp_w, &key_w, &record_w, DB_NEXT_DUP);
    		if (ref == DB_NOTFOUND){                      //δ�ҵ����������ļ�¼
				ret = -ERR_NORECORD;
				goto error;
    		}else if (ref){
    			ret = -ERR_TIMEOUT;
    			goto error;
    		}
    	}
		//ȷ�����µĶ�дλ�õ���ǰ����
    	if (flag == DBS_SEEK_FORWARD){                    //�µĶ�дλ����ǰ����
        	memset(&key_w, 0, sizeof(DBT));
        	memset(&record_w, 0, sizeof(DBT));
        	key_w.data = key;
        	key_w.size = key_size;
        	record_w.data = data;
        	record_w.size = data_size;
        	ref = cursorp_w->put(cursorp_w, &key_w, &record_w, DB_BEFORE);
        	if (ref == EACCES){
    			ret = -ERR_NOFUN;
    			goto error;
    		}else if (ref){
    			ret = -ERR_TIMEOUT;
    			goto error;
    		}
    	}else if (flag == DBS_SEEK_BACKWARD){             //�µĶ�дλ��������
    		memset(&key_w, 0, sizeof(DBT)); 
        	memset(&record_w, 0, sizeof(DBT));
        	key_w.data = key;
        	key_w.size = key_size;
        	record_w.data = data;
        	record_w.size = data_size;
        	ref = cursorp_w->put(cursorp_w, &key_w, &record_w, DB_AFTER);
        	if (ref == EACCES){
    			ret = -ERR_NOFUN;
    			goto error;
    		}else if (ref){
    			ret = -ERR_TIMEOUT;
    			goto error;
    		}
    	}else{
    		//�ӿڲ������ô���
    		ret = -ERR_INVAL;
    		goto error;
    	}
	}	
	ret = 0;
error:
	//�ر��α�
	ref = cursorp_w->close(cursorp_w);
	if (ref){
		ret = -ERR_SYS;
	}
error1:
	return (ret);
	
}
/******************************************************************************
 *	����:	dbs_write
 *	����:	д���ؼ�¼���ݣ��ڸ����ݹؼ��ֵ�ָ��λ���޸��滻������¼��
 *	����:	id				-	���ݿ���
 			key_size		-	�ؼ��ֳ���
 			key             -   �ؼ���
            set             -   ���ؼ�¼��λ
            data_size       -   ���ݴ�С
            data            -   ����
 *	����:	0				-	�ɹ�
 			-ERR_INVA		-   �ӿڲ������ô���
 			-ERR_NOFILE		-	���ݿ�δ��
 			-ERR_NOFUN		-	���߱�дȨ��
 			-ERR_NODISK		-	���ݳ���Խ��
 			-ERR_TIMEOUT	-	��ʱ
 			-ERR_NORECORD	-	���ݼ�¼������
 *	˵��:	�޸ļ�¼����������޸ĵ�һ����¼�����Ǽ�¼�����ڲ���ɴ���ֱ����ӵ�һ��
******************************************************************************/
int dbs_write(u8 id, u8 key_size, void *key, dbs_set_t *set,u16 data_size, void  *data)
{
	int ret;
	int ref,i;
	
	DBC *cursorp_w;
	DBT key_w;         
	DBT record_w;
	dbs_set_t set_w;
	u32 num_total;
	u8 data_middle[CFG_DBS_RECORD_MAX];
	
	//�в����ӿ�
	if ((id >= CFG_DBS_UNM_MAX) || (id < 0)){  
		ret = -ERR_INVAL;
		goto error1;
	}
	
	//�ж��Ƿ��
	if (dbp[id] == NULL){
		ret = -ERR_NOFILE;
		goto error1;
	}
	//�жϸñ�����ݿ��Ƿ���дȨ��
	if (dbp_info[id].dbp_mode == 1){             //��ֻ����ʽ��
		ret = -ERR_NOFUN;
		goto error1;
	}
	//�жϹؼ��ֲ����Ƿ�Խ��
	if (key_size > CFG_DBS_KEYSIZE_MAX){
		ret = -ERR_INVAL;
		goto error1;
	}

	//�ж�д�����Ƿ�Խ��
	if (data_size > CFG_DBS_RECORD_MAX){
		ret = -ERR_NODISK;
		goto error1;
	}
	//��ʼ��д�α�
	ref = dbp[id]->cursor(dbp[id], NULL, &cursorp_w, DB_WRITECURSOR);        //Ϊд�α�
	if (ref){                           
		ret = -ERR_TIMEOUT;
		goto error1;
	}
	
	memset(data_middle,0,CFG_DBS_RECORD_MAX);
	memset(&key_w, 0, sizeof(DBT));
	memset(&record_w, 0, sizeof(DBT));
	key_w.data = key;
	key_w.size = key_size;
	record_w.data = data_middle;
	record_w.ulen = CFG_DBS_RECORD_MAX;
	record_w.flags = DB_DBT_USERMEM;
	
	//����ӿ�
	if (set->whence == DBS_SEEK_SET){
		ref = dbs_count(id,key_size,key,&num_total);           //���ؼ�¼���ܼ�¼��
		if ((ref == -ERR_NORECORD) && (set->offset == 0)){     //��һ����¼������,�������
			memset(&key_w, 0, sizeof(DBT));
        	memset(&record_w, 0, sizeof(DBT));
        	key_w.data = key;
        	key_w.size = key_size;
        	record_w.data = data;
        	record_w.size = data_size;
        	ref = cursorp_w->put(cursorp_w, &key_w, &record_w, DB_KEYFIRST);
        	if (ref == EACCES){
        		ret = -ERR_NOFUN;
        		goto error;
        	}else if (ref){
        		ret = -ERR_NORECORD;
        		goto error;
        	}
        	//�ر��α�
        	ref = cursorp_w->close(cursorp_w);
        	if (ref){
        		ret = -ERR_SYS;
        	} 
        	ret = 0;
        	return(ret);
		}else if(ref){
			ret = -ERR_SYS;
			goto error;
		}
		if (num_total > 0){
			set_w.whence = DBS_SEEK_SET;
			set_w.offset = set->offset;
		}else{
			ret = -ERR_NORECORD;
			goto error;
		}
	}else if (set->whence == DBS_SEEK_END){
		ref = dbs_count(id,key_size,key,&num_total);          //���ؼ�¼���ܼ�¼��
		if (ref){
			ret = -ERR_NORECORD;
			goto error;
		}
		if (num_total > 0){
    		set_w.whence = DBS_SEEK_SET;
    		set_w.offset = num_total - 1 - set->offset;
    		if(set_w.offset < 0){
    			ret = -ERR_NORECORD;
    			goto error;
    		}
		}else{
			ret = -ERR_NORECORD;
    		goto error;
		}
	}else{
		ret = -ERR_INVAL;
		goto error;
	}
	//��λ�µĶ�дλ��
	if (set_w.whence == DBS_SEEK_SET){
		ref = cursorp_w->get(cursorp_w, &key_w, &record_w,DB_SET);
		if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
			ret = -ERR_NORECORD;
			goto error;
		}else if (ref){
			ret = -ERR_TIMEOUT;
			goto error;
		}
		if (set_w.offset == 0){          //ƫ����Ϊ0
			memset(&key_w, 0, sizeof(DBT));
			memset(&record_w,0,sizeof(DBT));
			key_w.data = key;
			key_w.size = key_size;
			record_w.data = data;
			record_w.size = data_size;
			ref = cursorp_w->put(cursorp_w, &key_w, &record_w, DB_CURRENT);
			if (ref == EACCES){
    			ret = -ERR_NOFUN;
    			goto error;
    		}else if (ref){
    			ret = -ERR_TIMEOUT;
    			goto error;
    		}
		}
		else if (set_w.offset > 0){
			//���α��Ƶ�ƫ������ָ�Ķ�дλ��
			for (i = 0;i < set_w.offset;i ++){
				memset(data_middle,0,CFG_DBS_RECORD_MAX);
				memset(&key_w, 0, sizeof(DBT));
				memset(&record_w, 0, sizeof(DBT));
				key_w.data = key;
				key_w.size = key_size;
				record_w.data = data_middle;
				record_w.ulen = CFG_DBS_RECORD_MAX;
				record_w.flags = DB_DBT_USERMEM;
				ref = cursorp_w->get(cursorp_w, &key_w, &record_w,DB_NEXT_DUP);
				if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
        			ret = -ERR_NORECORD;
        			goto error;
        		}else if (ref){
        			ret = -ERR_TIMEOUT;
        			goto error;
        		}
			}
			//������д���µĶ�дλ��
			memset(&key_w, 0, sizeof(DBT));
			memset(&record_w, 0, sizeof(DBT));
			key_w.data = key;
			key_w.size = key_size;
			record_w.data = data;
			record_w.size = data_size;
			ref = cursorp_w->put(cursorp_w, &key_w, &record_w, DB_CURRENT);
			if (ref == EACCES){
    			ret = -ERR_NOFUN;
    			goto error;
    		}else if (ref){
    			ret = -ERR_TIMEOUT;
    			goto error;
    		}
		}
		else{
			DBSPRINTF("interface fail!\n");
			ret = -ERR_INVAL;
			goto error;
		}
	}
	
	ret = 0;
error:
	//�ر��α�
	ref = cursorp_w->close(cursorp_w);
	if (ref){
		ret = -ERR_SYS;
	} 
error1:
	return (ret);
}
/******************************************************************************
  *	����:	dbs_multiple_count
  *	����:	ͳ�ƶ��ؼ�¼���ݵļ�¼����
  *	����:	id				-	���ݿ���
  			key_size		-	�ؼ��ֳ���
  			key             -   �ؼ���
            count_num       -	���ؼ�¼���������أ�
  *	����:	0				-	�ɹ�
  			-ERR_INVA		-   �ӿڲ������ô���
  			-ERR_NOFILE		-	���ݿ�δ��
  			-ERR_NODISK		-	���ݳ���Խ��
  			-ERR_TIMEOUT	-	��ʱ
  			-ERR_NORECORD	-	���ݼ�¼������
  *	˵��:	��
 ******************************************************************************/
int dbs_count(u8 id, u8 key_size, void *key,u32 *count_num)
{
	int ref;
	int ret;
	
	DBC *cursorp_c;
	DBT key_c;            //���ݿ��¼�ṹ
	DBT record_c;
	
	u8 data_middle[CFG_DBS_RECORD_MAX];
	db_recno_t *countp;   //���ؼ�¼����ָ��
    countp = count_num; 
    
	//�в����ӿ�
	if ((id >= CFG_DBS_UNM_MAX) || (id < 0)){  
		ret = -ERR_INVAL;
		goto error1;
	}
	
	//�ж��Ƿ��
	if (dbp[id] == NULL){
		ret = -ERR_NOFILE;
		goto error1;
	}
	
	//�жϹؼ��ֲ����Ƿ�Խ��
	if (key_size > CFG_DBS_KEYSIZE_MAX){
		ret = -ERR_INVAL;
		goto error1;
	}

	//��ʼ���α�
	if (dbp[id]->cursor(dbp[id], NULL, &cursorp_c, 0)){    //��ʼ���α�
		ret = -ERR_TIMEOUT;
		goto error1;
    }
	
	//���α궨λ����Ҫͳ�Ƽ�¼������KEY
	memset(&key_c, 0, sizeof(DBT));
	memset(&record_c, 0, sizeof(DBT));
	key_c.data = key;
	key_c.size = key_size;
	record_c.data = data_middle;
	record_c.ulen = CFG_DBS_RECORD_MAX;
	record_c.flags = DB_DBT_USERMEM;
	ref = cursorp_c->get(cursorp_c, &key_c, &record_c,DB_SET);
	if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
		ret = -ERR_NORECORD;
		goto error;
	}else if (ref){
		ret = -ERR_TIMEOUT;
		goto error;
	}
	
	//ͳ�Ƽ�¼����
	ref = cursorp_c->count(cursorp_c,countp, 0);
	if (ref) {
		ret = -ERR_TIMEOUT;
		goto error;
	}
	ret = 0;
error:
	//�ر��α�
	ref = cursorp_c->close(cursorp_c);
	if (ref){
		ret = -ERR_SYS;
	} 
error1:
	return (ret);
}

/******************************************************************************
  *	����:	dbs_read
  *	����:	�����ؼ�¼�������еĵ�����¼����
  *	����:	id				-	���ݿ���
  			key_size		-	�ؼ��ֳ���
  			key             -   �ؼ���
            set             -	���ؼ�¼��λ
            max_size        -	dataԪ��ָ���С��������data��������С��
 			data_size       -	ʵ�ʷ������ݵĳ���(���)
 			data            -	���ݣ�������¼�ĳ��Ⱥ����ݼ�¼��
  *	����:	0				-	�ɹ�
  			-ERR_INVA		-   �ӿڲ������ô���
  			-ERR_NOINIT		-	���ݿ�ģ��δ��ʼ��
  			-ERR_NOFILE		-	���ݿ�δ��
  			-ERR_NOMEM		-	�ڴ�������ݳ��ȳ����ӿڲ�������󳤶�
  			-ERR_TIMEOUT	-	��ʱ
  			-ERR_NORECORD	-	���ݼ�¼������
  *	˵��:	��
 ******************************************************************************/
int dbs_read(u8 id, u8 key_size, void *key, dbs_set_t *set, u16 max_size, u16 *data_size, void  *data)
{
	int ret;
	int ref,i;
	u32 num_total;
	
	DBC *cursorp_r;
	DBT key_r;         //���ݿ��¼�ṹ
	DBT record_r;
	dbs_set_t set_r;
	u8 data_middle[CFG_DBS_RECORD_MAX];
	
	//�в����ӿ�
	if ((id >= CFG_DBS_UNM_MAX) || (id < 0)){  
		ret = -ERR_INVAL;
		goto error1;
	}
	
	//�ж����ݿ��Ƿ��
	if (dbp[id] == NULL){
		ret = -ERR_NOFILE;
		goto error1;
	}
	
	//�жϹؼ��ֲ����Ƿ�Խ��
	if (key_size > CFG_DBS_KEYSIZE_MAX){
		ret = -ERR_INVAL;
		goto error1;
	}
	
	if ((max_size < 0) || (max_size > CFG_DBS_RECORD_MAX)){
		ret = -ERR_INVAL;
		goto error1;
	}
	
	//��ʼ���α�
	ref = dbp[id]->cursor(dbp[id], NULL, &cursorp_r, 0);
	if (ref){        
		ret = -ERR_TIMEOUT;
		goto error1;
    }
	//����ӿ�
	if (set->whence == DBS_SEEK_END){
		ref = dbs_count(id, key_size, key, &num_total);   //���ؼ�¼���ܼ�¼��
		if (ref){
			ret = -ERR_NORECORD;
			goto error;
		}
		set_r.whence = DBS_SEEK_SET;
		set_r.offset = num_total - 1 - set->offset;
		if(set_r.offset < 0){
			ret = -ERR_NORECORD;
			goto error;
		}
	}else if (set->whence == DBS_SEEK_SET){
		set_r.whence = DBS_SEEK_SET;
		set_r.offset = set->offset;
	}else{
		ret = -ERR_INVAL;
		goto error;
	}
	memset(data_middle, 0, CFG_DBS_RECORD_MAX);
	memset(&key_r, 0, sizeof(DBT));
	memset(&record_r, 0, sizeof(DBT));
	key_r.data = key;
	key_r.size = key_size;

	if (set_r.offset == 0){
		//ƫ����Ϊ0,����ʼ��¼
		record_r.data = data;
		record_r.ulen = max_size;
		record_r.flags = DB_DBT_USERMEM;
		ref = cursorp_r->get(cursorp_r, &key_r, &record_r, DB_SET);
		if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
    		ret = -ERR_NORECORD;
    		goto error;
    	}else if (ref == DB_BUFFER_SMALL){
    		ret = -ERR_NOMEM;
    		goto error;
    	}else if (ref){
    		ret = -ERR_TIMEOUT;
    		goto error;
		}else{
			*data_size = record_r.size;
		}
	}else if(set_r.offset > 0){
		//���α궨λ����Ҫ��д��KEY
		record_r.data = data_middle;
		record_r.ulen = CFG_DBS_RECORD_MAX;
		record_r.flags = DB_DBT_USERMEM;
		ref = cursorp_r->get(cursorp_r, &key_r, &record_r,DB_SET);
		if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
    		ret = -ERR_NORECORD;
    		goto error;
    	}else if (ref){
    		ret = -ERR_TIMEOUT;
    		goto error;
    	}
		for (i = 0;i <(set_r.offset - 1);i++){
			memset(data_middle, 0, CFG_DBS_RECORD_MAX);
			memset(&key_r, 0, sizeof(DBT));
			memset(&record_r, 0, sizeof(DBT));
			key_r.data = key;
			key_r.size = key_size;
			record_r.data = data_middle;
			record_r.ulen = CFG_DBS_RECORD_MAX;
			record_r.flags = DB_DBT_USERMEM;
			ref = cursorp_r->get(cursorp_r, &key_r, &record_r, DB_NEXT_DUP);
			if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
        		ret = -ERR_NORECORD;
        		goto error;
        	}else if (ref){
        		ret = -ERR_TIMEOUT;
        		goto error;
        	}
		}
		//������
		memset(&key_r, 0, sizeof(DBT));
		memset(&record_r, 0, sizeof(DBT));
		key_r.data = key;
		key_r.size = key_size;
		record_r.data = data;
		record_r.ulen = max_size;
		record_r.flags = DB_DBT_USERMEM;
		ref = cursorp_r->get(cursorp_r, &key_r, &record_r,DB_NEXT_DUP);
		if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
    		ret = -ERR_NORECORD;
    		goto error;
    	}else if (ref == DB_BUFFER_SMALL){
    		ret = -ERR_NOMEM;
    		goto error;
    	}else if (ref){
    		ret = -ERR_TIMEOUT;
    		goto error;
		}else{
			*data_size = record_r.size;
		}
    }
	ret = 0;
error:
	//�ر��α�
	ref = cursorp_r->close(cursorp_r);
	if (ref){
		ret = -ERR_SYS;
	} 
error1:
	return (ret);
}

/******************************************************************************
   *	����:	dbs_read_bulk
   *	����:	�����ؼ�¼�������еĶ�����¼����
   *	����:	id				-	���ݿ���
				key_size		-	�ؼ��ֳ���
				key             -   �ؼ���
				from            -	��ȡ���ؼ�¼һ���¼��ʼ��λ
				to              -	������λ
				max_size        -	dataԪ��ָ���С��������data��������С��
				data_size       -	ʵ�ʷ������ݵĳ���(���)
				data            -	���ݣ�������¼�ĳ��Ⱥ����ݼ�¼��
   *	����:	0				-	�ɹ�
				-ERR_INVA		-   �ӿڲ������ô���
				-ERR_NOINIT		-	���ݿ�ģ��δ��ʼ��
				-ERR_NOFILE		-	���ݿ�δ��
				-ERR_NOMEM		-	�ڴ�������ݳ��ȳ����ӿڲ�������󳤶�
				-ERR_TIMEOUT	-	��ʱ
				-ERR_NORECORD	-	���ݼ�¼������
   *	˵��:	���ݷ��ؽṹ�ǳ���+���ݵ���ʽ
   				from.whence =DBS_SEEK_SET��ʾ˳���ȡ���ؼ�¼
   				from.whence =DBS_SEEK_END��ʾ�����ȡ���ؼ�¼
 ******************************************************************************/
int dbs_read_bulk
(u8 id, u8 key_size, void *key, dbs_set_t *from, dbs_set_t *to, u16 max_size, u16 *data_size, void *data)
{
	int ret;
	int ref,i;
	
	DBC *cursorp_r;
	DBT key_r;                   //���ݿ��¼�ṹ
	DBT record_r;
	u8 data_middle[CFG_DBS_RECORD_MAX];
	u8 flag;
	
	u16 data_length;             //������¼�����ݳ���
	u32 num_total = 0;           //key�¼�¼������
	u16 record_length = 0;       //���ݼ�¼�ܳ���
	dbs_set_t set;               //��λ����ʼλ��
	u16 record_total = 0;        //Ҫ��ȡ�ļ�¼����
	
	void *point;
	point = data;
	memset(&set,0,sizeof(dbs_set_t));
	//�в����ӿ�
	if ((id >= CFG_DBS_UNM_MAX) || (id < 0)){ 
		ret = -ERR_INVAL;
		goto error1;
	}
	//�ж����ݿ��Ƿ��
	if (dbp[id] == NULL){
		ret = -ERR_NOFILE;
		goto error1;
	}
	
	//�жϹؼ��ֲ����Ƿ�Խ��
	if (key_size > CFG_DBS_KEYSIZE_MAX){
		ret = -ERR_INVAL;
		goto error1;
	}
	
	if ((max_size < 0) || (max_size > CFG_DBS_RECORD_MAX)){
		ret = -ERR_INVAL;
		goto error1;
	}
	
	//��ʼ���α�
	ref = dbp[id]->cursor(dbp[id], NULL, &cursorp_r, 0);
	if (ref){        
		ret = -ERR_TIMEOUT;
		goto error1;
    }
	
	memset(&key_r, 0, sizeof(DBT));
	memset(&record_r, 0, sizeof(DBT));
	key_r.data = key;
	key_r.size = key_size;
	
	if (max_size < 0){
		ret = -ERR_INVAL;
		goto error;
	} 
	
	//�жϽӿ��Ƿ���ȷ---����ӿ�
	ref = dbs_count(id, key_size, key, &num_total);                      			//���ؼ�¼���ܼ�¼��
	if (ref){
		ret = -ERR_NORECORD;
		goto error;
	}
	
	if ((from->offset >= num_total) || (to->offset >= num_total)){      			//����¼λ�ó����ܼ�¼��
		ret = -ERR_NORECORD;
		goto error;
	}
	if ((from->whence == DBS_SEEK_SET) && (to->whence == DBS_SEEK_SET)){   
		if (from->offset > to->offset){								  			//��ʼλ�������λ�õߵ�
			ret = -ERR_INVAL;
			goto error;
		}
		set.whence = DBS_SEEK_SET;
		set.offset = from->offset;
		flag = DB_NEXT_DUP;
		record_total = to->offset - from->offset + 1;
	}
	else if ((from->whence == DBS_SEEK_END) && (to->whence == DBS_SEEK_END)){   
		if (from->offset > to->offset){                              			//��ʼλ�������λ�õߵ�
			ret = -ERR_INVAL;
			goto error;
		}
		set.whence = DBS_SEEK_SET;
		set.offset = num_total - 1 - from->offset;
		flag = DB_PREV_DUP;
		record_total = to->offset - from->offset + 1;
	}
	else if ((from->whence == DBS_SEEK_SET) && (to->whence == DBS_SEEK_END)){   //��ʼλ�������λ�õߵ�
		if ((from->offset + to->offset) >= num_total){
			ret = -ERR_INVAL;
			goto error;
		}
		set.whence = DBS_SEEK_SET;
		set.offset = from->offset;
		flag = DB_NEXT_DUP;
		record_total = num_total - to->offset - from->offset;
	}
	else if ((from->whence == DBS_SEEK_END) && (to->whence == DBS_SEEK_SET)){   //��ʼλ�������λ�õߵ�
		if ((from->offset + to->offset) >= num_total){
			ret = -ERR_INVAL;
			goto error;
		}
		set.whence = DBS_SEEK_SET;
		set.offset = num_total - 1 - from->offset;
		flag = DB_PREV_DUP;
		record_total = num_total - to->offset - from->offset;
	}
	else{
		ret = -ERR_INVAL;
		goto error;
	}
	//��������Ϊ��һ����¼
	if((set.offset == 0) && (record_total-1 == 0)){
		memset(&key_r, 0, sizeof(DBT));
		memset(&record_r, 0, sizeof(DBT));
		key_r.data = key;
		key_r.size = key_size;
		record_r.data = point + 2;               //���ݳ��������ֽ�
		record_r.ulen = max_size - record_length;
		record_r.flags = DB_DBT_USERMEM;
		ref = cursorp_r->get(cursorp_r, &key_r, &record_r,DB_SET);   //����һ��
		if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
			ret = -ERR_NORECORD;
			goto error;
		}else if (ref == DB_BUFFER_SMALL){
			ret = -ERR_NOMEM;
			goto error;
		}else if (ref){
			ret = -ERR_TIMEOUT;
			goto error;
		}else{
			data_length = record_r.size;
			memcpy(point, &data_length, 2);		//���ݳ��������ֽ�
			point += 2 + data_length;			//���ݳ��������ֽ�
			record_length += 2 + data_length;	//���ݳ��������ֽ�
		}
	}
	//����ʼλ�����ݽ��α궨λ����ʼλ��
	else{
		record_r.data = data_middle;
		record_r.ulen = CFG_DBS_RECORD_MAX;
		record_r.flags = DB_DBT_USERMEM;
		ref = cursorp_r->get(cursorp_r, &key_r, &record_r,DB_SET);
		if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
			ret = -ERR_NORECORD;
			goto error;
		}else if (ref){
			ret = -ERR_TIMEOUT;
			goto error;
		}
		if (set.offset > 0){
			for (i = 0;i < set.offset;i ++){
				memset(data_middle,0,CFG_DBS_RECORD_MAX);
				memset(&key_r, 0, sizeof(DBT));
				memset(&record_r, 0, sizeof(DBT));
				key_r.data = key;
				key_r.size = key_size;
				record_r.data = data_middle;
				record_r.ulen = CFG_DBS_RECORD_MAX;
				record_r.flags = DB_DBT_USERMEM;
				ref = cursorp_r->get(cursorp_r, &key_r, &record_r,DB_NEXT_DUP);
				if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
					ret = -ERR_NORECORD;
					goto error;
				}else if (ref){
					ret = -ERR_TIMEOUT;
					goto error;
				}
			}
		}
		//������
		memset(&key_r, 0, sizeof(DBT));
		memset(&record_r, 0, sizeof(DBT));
		key_r.data = key;
		key_r.size = key_size;
		record_r.data = point + 2;               //���ݳ��������ֽ�
		record_r.ulen = max_size - record_length;
		record_r.flags = DB_DBT_USERMEM;
		ref = cursorp_r->get(cursorp_r, &key_r, &record_r,DB_CURRENT);   //����һ��
		if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
			ret = -ERR_NORECORD;
			goto error;
		}else if (ref == DB_BUFFER_SMALL){
			ret = -ERR_NOMEM;
			goto error;
		}else if (ref){
			ret = -ERR_TIMEOUT;
			goto error;
		}else{
			data_length = record_r.size;
			memcpy(point, &data_length, 2);		//���ݳ��������ֽ�
			point += 2 + data_length;			//���ݳ��������ֽ�
			record_length += 2 + data_length;	//���ݳ��������ֽ�
		}
		if(record_total-1 > 0){
			for (i = 0;i< record_total-1;i ++){
				memset(&key_r, 0, sizeof(DBT));
				memset(&record_r, 0, sizeof(DBT));
				key_r.data = key;
				key_r.size = key_size;
				record_r.data = point + 2;               //���ݳ��������ֽ�
				record_r.ulen = max_size - record_length;
				record_r.flags = DB_DBT_USERMEM;
				ref = cursorp_r->get(cursorp_r, &key_r, &record_r,flag);
				if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
					ret = -ERR_NORECORD;
					goto error;
				}else if (ref == DB_BUFFER_SMALL){
					ret = -ERR_NOMEM;
					goto error;
				}else if (ref){
					ret = -ERR_TIMEOUT;
					goto error;
				}else{
					data_length = record_r.size;
					memcpy(point, &data_length, 2);		//���ݳ��������ֽ�
					point += 2 + data_length;			//���ݳ��������ֽ�
					record_length += 2 + data_length;	//���ݳ��������ֽ�
				}
			}
		}
	}
	*data_size = record_length;
	ret = 0;
error:
	//�ر��α�
	ref = cursorp_r -> close(cursorp_r);
	if (ref){
		ret = -ERR_SYS;
	} 
error1:
	return (ret);
}

/******************************************************************************
   *	����:	dbs_delete
   *	����:	ɾ�����ؼ�¼���ݵĵ�����¼
   *	����:	id				-	���ݿ���
				key_size		-	�ؼ��ֳ���
				key             -   �ؼ���
				set             -	���ؼ�¼��λ
   *	����:	0				-	�ɹ�
				-ERR_INVA		-   �ӿڲ������ô���
				-ERR_NOINIT		-	���ݿ�ģ��δ��ʼ��
				-ERR_NOFILE		-	���ݿ�δ�򿪣����ѹرգ�
				-ERR_NOFUN		-	û�д˹��ܣ�����ɾ����
				-ERR_TIMEOUT	-	��ʱ
				-ERR_NORECORD	-	���ݼ�¼������
   *	˵��:	��
 ******************************************************************************/
int  dbs_delete(u8 id, u8 key_size, void *key, dbs_set_t *set)
{
	int ret;
	int ref,i;
	
	DBC *cursorp_d;
	DBT key_d;         //���ݿ��¼�ṹ
	DBT record_d;
	dbs_set_t set_d;
	u8 data_middle[CFG_DBS_RECORD_MAX];
	u32 num_total;
	
	//�в����ӿ�
	if ((id >= CFG_DBS_UNM_MAX) || (id < 0)){  
		ret = -ERR_INVAL;
		goto error1;
	}
	//�ж����ݿ��Ƿ��
	if (dbp[id] == NULL){
		ret = -ERR_NOFILE;
		goto error1;
	}

	//�жϹؼ��ֲ����Ƿ�Խ��
	if (key_size > CFG_DBS_KEYSIZE_MAX){
		ret = -ERR_INVAL;
		goto error1;
	}
	//��ʼ���α�
	ref = dbp[id]->cursor(dbp[id], NULL, &cursorp_d, DB_WRITECURSOR);
	if (ref){        
		ret = -ERR_TIMEOUT;
		goto error1;
    }
	
	//����ӿ�
	if (set->whence == DBS_SEEK_END){
		ref = dbs_count(id,key_size,key,&num_total);   //���ؼ�¼���ܼ�¼��
		if (ref){
			ret = -ERR_NORECORD;
			goto error;
		}
		set_d.whence = DBS_SEEK_SET;
		set_d.offset = num_total - 1 - set->offset;
		if(set_d.offset < 0){
			ret = -ERR_NORECORD;
			goto error;
		}
	}else if (set->whence == DBS_SEEK_SET){
		set_d.whence = DBS_SEEK_SET;
		set_d.offset = set->offset;
	}else{
		ret = -ERR_INVAL;
		goto error;
	}
	memset(&key_d, 0, sizeof(DBT));
	memset(&record_d, 0, sizeof(DBT));
	key_d.data = key;
	key_d.size = key_size;
	
	//���α��Ƶ�ƫ������ָ�Ķ�дλ��
	record_d.data = data_middle;
	record_d.ulen = CFG_DBS_RECORD_MAX;
	record_d.flags = DB_DBT_USERMEM;
	ref = cursorp_d->get(cursorp_d, &key_d, &record_d,DB_SET);
	if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
		ret = -ERR_NORECORD;
		goto error;
	}else if (ref){
		ret = -ERR_TIMEOUT;
		goto error;
	}
	for (i = 0;i < set_d.offset;i ++){
		memset(data_middle,0,CFG_DBS_RECORD_MAX);
		memset(&key_d, 0, sizeof(DBT));
		memset(&record_d, 0, sizeof(DBT));
		key_d.data = key;
		key_d.size = key_size;
		record_d.data = data_middle;
		record_d.ulen = CFG_DBS_RECORD_MAX;
		record_d.flags = DB_DBT_USERMEM;
		ref = cursorp_d->get(cursorp_d, &key_d, &record_d,DB_NEXT_DUP);
		if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
    		ret = -ERR_NORECORD;
    		goto error;
    	}else if (ref){
    		ret = -ERR_TIMEOUT;
    		goto error;
    	}
	}
	//ɾ������
	memset(&key_d, 0, sizeof(DBT));
	memset(&record_d, 0, sizeof(DBT));
	key_d.data = key;
	key_d.size = key_size;
	ref = cursorp_d->del(cursorp_d, 0);
	if (ref == DB_KEYEMPTY ){
		ret = -ERR_NORECORD;
		goto error;
	}
	else if (ref){
		ret = -ERR_SYS;
		goto error;
	}
	
	ret = 0;
	DBSPRINTF("delete record success!\n");
error:
	//�ر��α�
	ref = cursorp_d->close(cursorp_d);
	if (ref){
		ret = -ERR_SYS;
	}
error1:
	return (ret);
}

/******************************************************************************
    *	����:	dbs_delete_bulk
    *	����:	ɾ�����ؼ�¼���ݵĶ�����¼
    *	����:	id				-	���ݿ���
 				key_size		-	�ؼ��ֳ���
 				key             -   �ؼ���
 				from            -	��ȡ���ؼ�¼һ���¼��ʼ��λ
 				to              -	������λ
    *	����:	0				-	�ɹ�
 				-ERR_INVA		-   �ӿڲ������ô���
 				-ERR_NOINIT		-	���ݿ�ģ��δ��ʼ��
 				-ERR_NOFILE		-	���ݿ�δ��
 				-ERR_NOFUN      -   û�д˹��ܣ�����ɾ����
 				-ERR_TIMEOUT	-	��ʱ
 				-ERR_NORECORD	-	���ݼ�¼������
    *	˵��:	��
 ******************************************************************************/
int dbs_delete_bulk(u8 id, u8 key_size, void *key, dbs_set_t *from, dbs_set_t *to)
{
	int ret;
	int ref,i;
	
	DBC *cursorp_d;
	DBT key_d;           //���ݿ��¼�ṹ
	DBT record_d;
	u8 data_middle[CFG_DBS_RECORD_MAX];
	
	u32 num_total = 0;           //key�¼�¼������
	dbs_set_t set;                    //��λ����ʼλ��
	u8 flag;
	u16 record_total = 0;        //Ҫ��ȡ�ļ�¼����
	memset(&set,0,sizeof(dbs_set_t));
	
	//�в����ӿ�
	if ((id >= CFG_DBS_UNM_MAX) || (id < 0)){ 
		ret = -ERR_INVAL;
		goto error1;
	}
	
	//�ж����ݿ��Ƿ��
	if (dbp[id] == NULL){
		ret = -ERR_NOFILE;
		goto error1;
	}
	
	//�жϹؼ��ֲ����Ƿ�Խ��
	if (key_size > CFG_DBS_KEYSIZE_MAX){
		ret = -ERR_INVAL;
		goto error1;
	}

	//��ʼ���α�
	ref = dbp[id]->cursor(dbp[id], NULL, &cursorp_d, DB_WRITECURSOR);
	if (ref){        
		ret = -ERR_SYS;
		goto error1;
    }
	
	memset(&key_d, 0, sizeof(DBT));
	memset(&record_d, 0, sizeof(DBT));
	key_d.data = key;
	key_d.size = key_size;
	
	
	//�жϽӿ��Ƿ���ȷ---����ӿ�
	ref = dbs_count(id, key_size,key,&num_total);             //���ؼ�¼���ܼ�¼��
	if (ref == -ERR_NORECORD){
		ret = -ERR_NORECORD;
		goto error;
	}
	else if (ref){
		ret = -ERR_SYS;
		goto error;
	}
	if ((from->offset >= num_total) || (to->offset >= num_total)){            //����¼λ�ó����ܼ�¼��
		ret = -ERR_NORECORD;
		goto error;
	}
	if ((from->whence == DBS_SEEK_SET) && (to->whence == DBS_SEEK_SET)){   
		if (from->offset > to->offset){                                    //��ʼλ�������λ�õߵ�
			ret = -ERR_INVAL;
			goto error;
		}
		set.whence = DBS_SEEK_SET;
		set.offset = from->offset;
		flag = DB_NEXT_DUP;
		record_total = to->offset - from->offset + 1;
	}
	else if ((from->whence == DBS_SEEK_END) && (to->whence == DBS_SEEK_END)){   //��ʼλ�������λ�õߵ�
		if (from->offset > to->offset){
			ret = -ERR_INVAL;
			goto error;
		}
		set.whence = DBS_SEEK_SET;
		set.offset = num_total - to->offset - 1;
		flag = DB_PREV_DUP;
		record_total = to->offset - from->offset + 1;
	}
	else if ((from->whence == DBS_SEEK_SET) && (to->whence == DBS_SEEK_END)){   //��ʼλ�������λ�õߵ�
		if ((from->offset + to->offset) >= num_total){
			ret = -ERR_INVAL;
			goto error;
		}
		set.whence = DBS_SEEK_SET;
		set.offset = from->offset;
		flag = DB_NEXT_DUP;
		record_total = num_total - to->offset - from->offset;
	}
	else if ((from->whence == DBS_SEEK_END) && (to->whence == DBS_SEEK_SET)){   //��ʼλ�������λ�õߵ�
		if ((from->offset + to->offset) >= num_total){
			ret = -ERR_INVAL;
			goto error;
		}
		set.whence = DBS_SEEK_SET;
		set.offset = to->offset;
		flag = DB_PREV_DUP;
		record_total = num_total - to->offset - from->offset;
	}
	else{
		ret = -ERR_INVAL;
		goto error;
	}
	
	//���α궨λ����ʼλ��
	if(set.offset == 0){                                //ƫ����Ϊ0
		record_d.data = data_middle;
		record_d.ulen = CFG_DBS_RECORD_MAX;
		record_d.flags = DB_DBT_USERMEM;
		ref = cursorp_d->get(cursorp_d, &key_d, &record_d,DB_SET);
		if (ref == DB_NOTFOUND){                        //δ�ҵ����������ļ�¼
    		ret = -ERR_NORECORD; 
    		goto error;
    	}else if (ref){
    		ret = -ERR_TIMEOUT;
    		goto error;
    	}else{
			record_total --;
			//ɾ����¼
			ref = cursorp_d->del(cursorp_d, 0);        //ɾ�����α���ָ�ļ�¼
    		if (ref == EPERM ){                        //��ֻ����ʽ�򿪵����ݿ�
    			DBSPRINTF("del has no right!\n");
    			ret = -ERR_NOFUN;
    			goto error;
    		}
    		else if (ref){
       			DBSPRINTF("del record fail!\n");
       			ret = -ERR_SYS;
       			goto error;
    		}else{
    			DBSPRINTF("del success!\n");
    			//goto hull;
			}
		}
	}else if (set.offset > 0){
		//���α��Ƶ�ƫ������ָ�Ķ�дλ��
		record_d.data = data_middle;
		record_d.ulen = CFG_DBS_RECORD_MAX;
		record_d.flags = DB_DBT_USERMEM;
		ref = cursorp_d->get(cursorp_d, &key_d, &record_d,DB_SET);
		if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
    		ret = -ERR_NORECORD;
    		goto error;
    	}else if (ref){
    		ret = -ERR_TIMEOUT;
    		goto error;
    	}
		for (i = 0;i < (set.offset - 1);i ++){
			memset(data_middle, 0, CFG_DBS_RECORD_MAX);
			memset(&key_d, 0, sizeof(DBT));
			memset(&record_d, 0, sizeof(DBT));
			key_d.data = key;
			key_d.size = key_size;
			record_d.data = data_middle;
			record_d.ulen = CFG_DBS_RECORD_MAX;
			record_d.flags = DB_DBT_USERMEM;
			ref = cursorp_d->get(cursorp_d, &key_d, &record_d, DB_NEXT_DUP);
			if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
        		ret = -ERR_NORECORD;
        		goto error;
        	}else if (ref){
        		ret = -ERR_TIMEOUT;
        		goto error;
        	}
		}
	}else{
		ret = -ERR_INVAL;
		goto error;
	}
	//ɾ������
	for (i = 0;i< record_total;i ++){
		memset(&key_d, 0, sizeof(DBT));
		memset(&record_d, 0, sizeof(DBT));
		memset(data_middle, 0, CFG_DBS_RECORD_MAX);
		key_d.data = key;
		key_d.size = key_size;
		record_d.data = data_middle;
		record_d.ulen = CFG_DBS_RECORD_MAX;
		record_d.flags = DB_DBT_USERMEM;
		ref = cursorp_d->get(cursorp_d, &key_d, &record_d, DB_NEXT_DUP);
		if (ref == DB_NOTFOUND){                 //δ�ҵ����������ļ�¼
    		ret = -ERR_NORECORD;
    		goto error;
    	}else if (ref){
    		ret = -ERR_TIMEOUT;
    		goto error;
    	}else{
			//ɾ����¼
			ref = cursorp_d->del(cursorp_d, 0);  //ɾ�����α���ָ�ļ�¼
    		if (ref == EPERM ){                        //��ֻ����ʽ�򿪵����ݿ�
    			DBSPRINTF("del has no right!\n");
    			ret = -ERR_NOFUN;
    			goto error;
    		}
    		else if (ref){
       			DBSPRINTF("del record fail!\n");
       			ret = -ERR_SYS;
       			goto error;
    		}else{
    			DBSPRINTF("del success!\n");
    			//goto hull;
			}
		}
	}
	ret = 0;
error:
	//�ر��α�
	ref = cursorp_d -> close(cursorp_d);
	if (ref){
		ret = -ERR_SYS;
		goto error;
	} 
error1:
	return (ret);
	
}
/******************************************************************************
    *	����:	dbs_close
    *	����:	�ر����ݿ�
    *	����:	id				-	���ݿ���
    *	����:	0				-	�ɹ�
 				-ERR_INVAL		-   �ӿڲ������ô���
 				-ERR_NOINIT		-	���ݿ�ģ��δ��ʼ��
 				-ERR_NOFILE		-	���ݿ�δ��
 				-ERR_BUSY		-	δ�ر�
				-ERR_SYS		-	ϵͳ����
    *	˵��:	��
******************************************************************************/
int  dbs_close(u8 id)
{
	int ret,ref;
	if (dbenv == NULL){         //δ��ʼ�����ݿ�ģ�飺����δ����
		ret = -ERR_NOINIT;
		goto error;
	}
	//�в����ӿ�
	if ((id >= CFG_DBS_UNM_MAX) || (id < 0)){ 
		ret = -ERR_INVAL;
		goto error;
	}
	//�ж����ݿ��Ƿ��
	if (dbp[id] == NULL){
		ret = -ERR_NOFILE;
		goto error;
	}
	
	//��ô򿪼�������
	ref = pthread_mutex_lock(&(dbp_info[id].dbp_mutex));
	if (ref ==  EINVAL){
		DBSPRINTF("mutex lock fail!\n");
		ret = -ERR_NOINIT;
		goto error;
	}else if (ref){                           //ref == EDEADLK
		ret = -ERR_BUSY;
		goto error;
	}else{
		//�򿪼���
		if (dbp_info[id].dbp_count > 0){      //�Ѵ�
			dbp_info[id].dbp_count = 0;       
		}else{
			ret = -ERR_NOFILE;                //���ݿ�δ��
			goto error1;
		}
		//�ر����ݿ�
		ref = dbp[id]->close(dbp[id], 0);
		if (ref){
			DBSPRINTF("close db fail!\n");
			dbp_info[id].dbp_count = 1;
			ret = -ERR_SYS;
			goto error1;
		}
		dbp[id] = NULL;
		DBSPRINTF("close db success!\n");
		ret = 0;
error1:
	//����
	ref = pthread_mutex_unlock(&(dbp_info[id].dbp_mutex)); //����
	if (ref){   
		DBSPRINTF("mutex unlock fail!\n");
		ret = -ERR_SYS;
	}
	}
error:
	return(ret);
}
/******************************************************************************
      *	����:	dbs_remove
      *	����:	ɾ�����ݿ�
      *	����:	id				-	���ݿ���
      *	����:	0				-	�ɹ�
   				-ERR_INVA		-   �ӿڲ������ô���
   				-ERR_NOINIT		-	���ݿ�ģ��δ��ʼ��
   				-ERR_NOFILE		-	���ݿ�δ��
   				-ERR_BUSY		-	δ�ر�
 				-ERR_SYS		-	ϵͳ����
      *	˵��:	��
 ******************************************************************************/
int  dbs_remove(u8 id)
{
	int ret,ref;
	char path[22] = "";         //���ݿ�·��
	char name[3]= "";			//���ݿ���
	char index[4] = ".db";      //���ݿ�����׺
	
	if (dbenv == NULL){         //δ��ʼ�����ݿ�ģ�飺����δ����
		ret = -ERR_NOINIT;
		goto error;
	}
	//�в����ӿ�
	if ((id >= CFG_DBS_UNM_MAX) || (id < 0)){ 
		ret = -ERR_INVAL;
		goto error;
	}
	
	//��ô򿪼�������
	if (pthread_mutex_lock(&(dbp_info[id].dbp_mutex))){
		DBSPRINTF("mutex lock fail!\n");
		ret = -ERR_SYS;
		goto error;
	}
	else{
		if (dbp_info[id].dbp_count > 0){    //�Ѵ�,����ɾ�����ݿ�
			DBSPRINTF("db has opened!\n");
			ret = -ERR_BUSY;
			goto error1;
		}
		else{         //�����ݿ⣨�ȴ򿪿����ɾ���� 
        	
        	//���ݿ���
        	if (id >= 0 && id < 10){              //���ݿ�����λ��
    	    	name[0] = '0' + id;
    	    }else if(id >9 && id < CFG_DBS_UNM_MAX){			//���ݿ�����λ��
    	    	name[0] = '0' + id/10;          //�����ݿ�����Ϊ���ݿ����ƣ�ȡʮλ����
    	    	name[1] = '0' + id%10;			//�����ݿ�����Ϊ���ݿ����ƣ�ȡ��λ����
    	    }else{
    	    	ret = -ERR_INVAL;
    	    	goto error1;
    	    }
    	    //��λ��		
			switch(dbp_info[id].dbp_pos){
        		case DBS_POS_RAM:
        			strcat (path, path_ram);
        			strcat (path, name);
        			strcat (path, index);
        			break;
        		case DBS_POS_FLASH_CODE:
        			strcat (path, path_flash_code);
        			strcat (path, name);
        			strcat (path, index);
        			break;
        		case DBS_POS_FLASH_DATA:
        			strcat (path, path_flash_data);
        			strcat (path, name);
        			strcat (path, index);
        			break;
        		case DBS_POS_SD:
        			strcat (path, path_sd);
        			strcat (path, name);
        			strcat (path, index);
        			break;
        		case DBS_POS_U:
        			strcat (path, path_u);
        			strcat (path, name);
        			strcat (path, index);
        			break;
        		default:
        			ret = -ERR_INVAL;
        			goto error1;
        			break;
        	}
			//�������ݿ�
			if (db_create(&dbp[id], dbenv, 0)) {
				ret = -ERR_SYS;
				goto error1; 
			}
			//ɾ�����Ƴ������ݿ�
			ref = dbp[id]->remove(dbp[id],path,NULL,0);
			if (ref == EINVAL){
				ret = -ERR_BUSY;
				goto error1;
			}else if (ref){
				DBSPRINTF("remove db fail!ref = %d\n",ref);
				ret = -ERR_SYS;
				goto error1;
			}
			dbp[id] = NULL;
			memset(&dbp_info[id], 0, sizeof(dbp_info));
		}
		ret = 0;
		//����
error1:
		if (pthread_mutex_unlock(&(dbp_info[id].dbp_mutex))){   //����
			DBSPRINTF("mutex unlock fail!\n");
			ret = -ERR_SYS;
			goto error;
		}
	}
error:
	return(ret);        //ϵͳ����
}
/******************************************************************************
       *	����:	dbs_reset
       *	����:	��λ���ݿ⣨��գ�
       *	����:	id				-	���ݿ���
       *	����:	0				-	�ɹ�
    				-ERR_INVA		-   �ӿڲ������ô���
    				-ERR_NOFILE		-	���ݿ�δ��
    				-ERR_BUSY		-	δ�ر�
					-ERR_SYS		-	ϵͳ����
       *	˵��:	��
 ******************************************************************************/
int  dbs_reset(u8 id)
{
	int ret,ref;
	u_int32_t mount;
	
	//�в����ӿ�
	if ((id >= CFG_DBS_UNM_MAX) || (id < 0)){  
		ret = -ERR_INVAL;
		goto error;
	}
	//�ж����ݿ��Ƿ��
	if (dbp[id] == NULL){
		ret = -ERR_NOFILE;
		goto error;
	}
	
	ref = dbp[id]->truncate(dbp[id], NULL, &mount, 0);
	if (ref == EINVAL ){  //�д򿪵��α�
		ret = -ERR_BUSY;
		goto error;
	}else if(ref){
		DBSPRINTF("sync failed!\n");
		ret = -ERR_SYS;
		goto error;
	}
	ret = 0;
error:
	return (ret);
}
/******************************************************************************
        *	����:	dbs_sync
        *	����:	ͬ�����ݿ�
        *	����:	id				-	���ݿ���
        *	����:	0				-	�ɹ�
     				-ERR_INVA		-   �ӿڲ������ô���
     				-ERR_NOFILE		-	���ݿ�δ��
 					-ERR_SYS		-	ϵͳ����
        *	˵��:	��
******************************************************************************/
int  dbs_sync(u8 id)
{
	int ret,ref;
	
	//�в����ӿ�
	if ((id >= CFG_DBS_UNM_MAX) || (id < 0)){ 
		ret = -ERR_INVAL;
		goto error;
	}
	
	//�ж����ݿ��Ƿ��
	if (dbp[id] == NULL){
		ret = -ERR_NOFILE;
		goto error;
	}
	
	ref = dbp[id]->sync(dbp[id],0);
	if (ref){
		DBSPRINTF("sync failed!\n");
		ret = -ERR_SYS;
		goto error;
	}
	ret = 0;
error:
	return (ret);
}


#endif    /* CFG_DBS_MODULE */
