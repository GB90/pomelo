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
    ��Ŀ����    ��  SGE800�����ն�ҵ��ƽ̨
    �ļ���         ��  cyclicdb.c
    ��          ��    ��  ���ļ�����ҵ��ƽ̨��ѭ���洢���ݿ⹦�ܵ�ʵ��
    ��          ��    ��  0.1
    ��          ��    ��  ����
    ��������    ��  2010.04
******************************************************************************/

//C��ͷ�ļ�
#include <stdio.h>			//printf
#include <stdlib.h>		    //exit
#include <unistd.h>		    //sleep
#include <db.h>
#include <string.h>

//ƽ̨��ͷ�ļ�
#include "include/dbs.h"
#include "private/config.h"
#include "include/error.h"

/*************************************************
  �ṹ���Ͷ���
*************************************************/
static struct {
	u32 record_len;
	u8 count;            //��ʼ������
}cyclicdb_info[CFG_DBS_UNM_MAX];     //����

#pragma pack(1)
typedef struct {
	 u8 year;		//��: [0 - 255]�Դ�2000����������
	 u8 mon;		//��: [1 - 12]
	 u8 day;		//�գ�[1 - 31]
	 u8 hour;		//ʱ��[0 - 23]
	 u8 min;		//�֣�[0 - 59]
	 u8 sec;		//�룺[0 - 59]
} db_t;

#pragma pack()

/*******************************API����ʵ��***********************************/

/******************************************************************************
*	����:	cyclicdb_init
*	����:	��ʼ������ѭ�����ݿ�
*	����:	id				-	���ݿ���
			pose			-	��λ��
			mode			-	�򿪷�ʽ
			record_len      -   ѭ���洢����¼��
*	����:	0				-	�ɹ�
			-ERR_INVAL		-	��������
			-ERR_SYS		-	ϵͳ����
			-ERR_NOINIT		-	û��ʼ��
			-ERR_BUSY		-	�Ѵ�/�Ѵ���
			-ERR_NOFILE     -   ���ݿ��ļ�������
*	˵��:	��DBS_MODE_CREAT��ʽ�����ݿ⣬������ݿ��Ѿ�������ᱨ��-ERR_BUSY��
��DBS_MODE_RD��ʽ��ֻ�����ݿ⣬���ݿⲻ���ڱ���-ERR_NOFILE��
******************************************************************************/
int cyclicdb_init(u8 id, u8 pos, u8 mode, u8 record_len)
{
	int ret;

	ret = dbs_init();                          //���ݿ�ģ���ʼ��
	if ((ret != -ERR_BUSY) && (ret != 0)){
		goto error;
	}

	ret = dbs_open(id, pos, mode);            //�����ݿ�
	if ((ret != -ERR_BUSY) && (ret != 0)){
		goto error;
	}

	cyclicdb_info[id].record_len = record_len;
	cyclicdb_info[id].count = 1;

	ret = 0;
error:
	return(ret);
}

/******************************************************************************
 *	����:	cyclicdb_write
 *	����:	дѭ����¼���ݣ����µ�һ����¼��
 *	����:	id				-	���ݿ���
 			key_size		-	�ؼ��ֳ���
 			key             -   �ؼ���
            data_size       -   ���ݴ�С
            data            -   ����
 *	����:	0				-	�ɹ�
 			-ERR_INVA		-   �ӿڲ������ô���
 			-ERR_NOFILE		-	���ݿ�δ��
 			-ERR_NOFUN		-	���߱�дȨ��
 			-ERR_NODISK		-	���ݳ���Խ��
 			-ERR_TIMEOUT	-	��ʱ
 			-ERR_NORECORD	-	���ݼ�¼������
 *	˵��:	�����¼���Ѵﵽ����¼������ɾ������ļ�¼��������µĵ�һ����¼��
                               ����ùؼ����¼�¼�����ڣ���ֱ����ӵ�һ���¼�¼
******************************************************************************/
int cyclicdb_write(u8 id, u8 key_size, void *key, u16 data_size, void *data)
{
	int ret;
	u32 record_total;
	dbs_set_t set;
	u8 flag;

	if (cyclicdb_info[id].count != 1){
		ret = -ERR_NOINIT;
		goto error;
	}
//�ж��Ƿ񵽴�ѭ���洢�����ֵ
	ret = dbs_count(id, key_size, key, &record_total);
	if (ret == 0){
		if (record_total == cyclicdb_info[id].record_len){     //�Ѿ��ﵽ���ֵ
			set.offset = 0;
			set.whence = DBS_SEEK_SET;
			ret = dbs_delete(id, key_size, key, &set);
			if (ret != 0){
				goto error;
			}else{
				set.offset = 0;
				set.whence = DBS_SEEK_END;
				flag = DBS_SEEK_BACKWARD;
				ret = dbs_insert(id, key_size, key, &set, flag, data_size, data);
				if(ret !=0){
					goto error;
				}
			}
		}else if(record_total < cyclicdb_info[id].record_len){    //û�дﵽ���ֵ
			set.offset = 0;
			set.whence = DBS_SEEK_END;
			flag = DBS_SEEK_BACKWARD;
			ret = dbs_insert(id, key_size, key, &set, flag, data_size, data);
			if(ret !=0){
				goto error;
			}
		}else{
			ret = -ERR_SYS;
			goto error;
		}
	}else if(ret==-ERR_NORECORD){             //�޼�¼ʱֱ�Ӳ����¼�¼
		set.offset = 0;
		set.whence = DBS_SEEK_END;
		flag = DBS_SEEK_BACKWARD;
		ret = dbs_insert(id, key_size, key, &set, flag, data_size, data);
		if(ret !=0){
			goto error;
		}
	}else {
		goto error;
	}
	ret = 0;
error:
	return(ret);

}

/******************************************************************************
  *	����:	cyclicdb_read
  *	����:	��ȡѭ����¼����
  *	����:	id				-	���ݿ���
  			key_size		-	�ؼ��ֳ���
  			key             -   �ؼ���
            set             -	��¼��λ
            max_size        -	dataԪ��ָ���С��������data��������С��
 			data_size       -	ʵ�ʷ������ݵĳ���(���)
 			data            -	����
  *	����:	0				-	�ɹ�
  			-ERR_INVA		-   �ӿڲ������ô���
  			-ERR_NOINIT		-	���ݿ�ģ��δ��ʼ��
  			-ERR_NOFILE		-	���ݿ�δ��
  			-ERR_NOMEM		-	�ڴ�������ݳ��ȳ����ӿڲ�������󳤶�
  			-ERR_TIMEOUT	-	��ʱ
  			-ERR_NORECORD	-	���ݼ�¼������
  *	˵��:	��
 ******************************************************************************/
int cyclicdb_read(u8 id, u8 key_size, void *key, dbs_set_t *set, u16 max_size, u16 *data_size, void  *data)
{
	int ret;
	ret = dbs_read(id, key_size, key, set, max_size, data_size, data);
	if (ret != 0){
		goto error;
	}
	ret = 0;
error:
	return(ret);
}
/******************************************************************************
 *	����:	cyclicdb_write_t
 *	����:	д��ʱ���ʾ��ѭ����¼���ݣ����µ�һ����¼��
 *	����:	id				-	���ݿ���
 			key_size		-	�ؼ��ֳ���
 			key             -   �ؼ���
 			time			-   ָ��ʱ��
            data_size       -   ���ݴ�С
            data            -   ����
 *	����:	0				-	�ɹ�
 			-ERR_INVA		-   �ӿڲ������ô���
 			-ERR_NOFILE		-	���ݿ�δ��
 			-ERR_NOFUN		-	���߱�дȨ��
 			-ERR_NODISK		-	���ݳ���Խ��
 			-ERR_TIMEOUT	-	��ʱ
 			-ERR_NORECORD	-	���ݼ�¼������
 *	˵��:	�����¼���Ѵﵽ����¼������ɾ������ļ�¼��������µĵ�һ����¼��
                                ����ùؼ����¼�¼�����ڣ���ֱ����ӵ�һ���¼�¼
            ���ü�¼ʱ��Ӧ�ô�����һд���ݿ��¼��ʱ��
******************************************************************************/
int cyclicdb_write_t(u8 id, u8 key_size, void *key, db_t *time, u16 data_size, void  *data)
{
	int ret;
	u32 record_total;
	dbs_set_t set;
	u8 flag;
	struct{
		db_t record_time;
		u8 record_data[data_size];
	}record;

	if (cyclicdb_info[id].count != 1){
		ret = -ERR_NOINIT;
		goto error;
	}
	record.record_time.day = time->day;
	record.record_time.sec = time->sec;
	record.record_time.hour = time->hour;
	record.record_time.min = time->min;
	record.record_time.mon = time->mon;
	record.record_time.year = time->year;

	memcpy(&record.record_data, data, data_size);
	//�ж��Ƿ�ﵽѭ���洢�����ֵ
	ret = dbs_count(id, key_size, key, &record_total);
	if (ret == 0){
		if (record_total == cyclicdb_info[id].record_len){           //�ﵽ���ֵ
			set.offset = 0;
			set.whence = DBS_SEEK_SET;
			ret = dbs_delete(id, key_size, key, &set);
			if (ret != 0){
				goto error;
			}else{
				set.offset = 0;
				set.whence = DBS_SEEK_END;
				flag = DBS_SEEK_BACKWARD;
				ret = dbs_insert(id, key_size, key, &set, flag, (data_size + sizeof(db_t)), &record);
				if(ret !=0){
					goto error;
				}
			}
		}else if(record_total < cyclicdb_info[id].record_len){        //û�дﵽ���ֵ
			set.offset = 0;
			set.whence = DBS_SEEK_END;
			flag = DBS_SEEK_BACKWARD;
			ret = dbs_insert(id, key_size, key, &set, flag, (data_size + sizeof(db_t)), &record);
			if(ret !=0){
				goto error;
			}
		}else{
			ret = -ERR_SYS;
			goto error;
		}
	}else if (ret == -ERR_NORECORD){                  //�޼�¼�����¼�¼
		set.offset = 0;
		set.whence = DBS_SEEK_END;
		flag = DBS_SEEK_BACKWARD;
		ret = dbs_insert(id, key_size, key, &set, flag, (data_size + sizeof(db_t)), &record);
		if(ret !=0){
			goto error;
		}
	}else{
		goto error;
	}
	ret = 0;
error:
	return(ret);

}
/******************************************************************************
  *	����:	cyclicdb_read_t
  *	����:	��ָ��ʱ���ȡѭ����¼���ݿ��еļ�¼
  *	����:	id				-	���ݿ���
  			key_size		-	�ؼ��ֳ���
  			key             -   �ؼ���
  			time			-	ָ��ʱ��
            max_size        -	dataԪ��ָ���С��������data��������С��
 			data_size       -	ʵ�ʷ������ݵĳ���(���)
 			data            -	����
  *	����:	0				-	�ɹ�
  			-ERR_INVA		-   �ӿڲ������ô���
  			-ERR_NOINIT		-	���ݿ�ģ��δ��ʼ��
  			-ERR_NOFILE		-	���ݿ�δ��
  			-ERR_NOMEM		-	�ڴ�������ݳ��ȳ����ӿڲ�������󳤶�
  			-ERR_TIMEOUT	-	��ʱ
  			-ERR_NORECORD	-	���ݼ�¼������
  *	˵��:	��
 ******************************************************************************/
int cyclicdb_read_t(u8 id, u8 key_size, void *key, db_t *time, u16 max_size, u16 *data_size, void  *data)
{
	int ret;
	dbs_set_t set;
	u32 record_total,mid,top,bot;
	u8 find;
	struct rec{
		db_t record_time;
		u8 record_data[max_size];
	}record;
	u16 record_len;

	if (cyclicdb_info[id].count != 1){
		ret = -ERR_NOINIT;
		goto error;
	}
	ret = dbs_count(id, key_size, key, &record_total);
	if(ret != 0){
		goto error;
	}
	top = 0;
	bot = record_total-1;
	find = 0;
	do              //ѭ�����ҷ���ָ��ʱ��ļ�¼
	{
		mid = (top + bot)/2;
		set.offset = mid;
		set.whence = DBS_SEEK_SET;
		ret = dbs_read(id, key_size, key, &set, sizeof(db_t)+max_size, &record_len, &record);
		if (ret == 0){
			if (record.record_time.year < time->year){
				top = mid + 1;
			}else if (record.record_time.year > time->year){
				bot = mid - 1;
			}else{
				if (record.record_time.mon < time->mon){
					top = mid + 1;
				}else if (record.record_time.mon > time->mon){
					bot = mid - 1;
				}else{
					if (record.record_time.day < time->day){
						top = mid + 1;
					}else if (record.record_time.day > time->day){
						bot = mid - 1;
					}else{
						if (record.record_time.hour < time->hour){
							top = mid + 1;
						}else if (record.record_time.hour > time->hour){
							bot = mid - 1;
						}else {
							if (record.record_time.min < time->min){
								top = mid + 1;
							}else if (record.record_time.min > time->min){
								bot = mid - 1;
							}else{
								if (record.record_time.sec < time->sec){
									top = mid + 1;
								}else if (record.record_time.sec > time->sec){
									bot = mid - 1;
								}else{
									find = 1;
									record_len = record_len - sizeof(db_t);
									memcpy(data_size, &record_len, sizeof(record_len));
									memcpy(data,&record.record_data, record_len);
								}
							}
						}
					}
				}
			}
		}else{
			goto error;
		}

	}
	while((find == 0) && (top <= bot));
	if (find != 1){
		ret = -ERR_NORECORD;
		goto error;
	}
	ret = 0;
error:
	return(ret);

}

/******************************************************************************
   *	����:	cyclicdb_read_p
   *	����:	��ָ��һ��ʱ����ѭ���洢���ݿ��еļ�¼����
   *	����:	id				-	���ݿ���
				key_size		-	�ؼ��ֳ���
				key             -   �ؼ���
				from            -	��ȡ��¼��ʼʱ��
				to              -	��ȡ��¼����ʱ��
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
   *	˵��:	���ݷ��ؽṹ�Ǽ�¼���ȣ�ʱ��+���ݣ�+ ʱ��+��¼���ݵ���ʽ��Ϊ���ֽ���ǰ
******************************************************************************/
int cyclicdb_read_p(u8 id, u8 key_size, void *key, db_t *from, db_t *to, u16 max_size, u16 *size, void *data)
{
	int ret;
	dbs_set_t set;
	u32 record_total,mid,top,bot;
	u8 find;
	u8 offset_f,offset_t;
	struct rec{
		db_t record_time;
		u8 record_data[max_size];
	}record;
	u16 record_len;

	if (cyclicdb_info[id].count != 1){
		ret = -ERR_NOINIT;
		goto error;
	}

	//�����жϿ�ʼʱ�����ڽ���ʱ��
	ret = dbs_count(id, key_size, key, &record_total);
	if(ret != 0){
		goto error;
	}
	top = 0;
	bot = record_total-1;
	find = 0;
	do             //ѭ�����ҷ���ָ����ʼʱ��ļ�¼
	{
		mid = (top + bot)/2;
		set.offset = mid;
		set.whence = DBS_SEEK_SET;
		ret = dbs_read(id, key_size, key, &set, sizeof(db_t)+max_size, &record_len, &record);
		if (ret == 0){
			if (record.record_time.year < from->year){
				top = mid + 1;
			}else if (record.record_time.year > from->year){
				bot = mid - 1;
			}else{
				if (record.record_time.mon < from->mon){
					top = mid + 1;
				}else if (record.record_time.mon > from->mon){
					bot = mid - 1;
				}else{
					if (record.record_time.day < from->day){
						top = mid + 1;
					}else if (record.record_time.day > from->day){
						bot = mid - 1;
					}else{
						if (record.record_time.hour < from->hour){
							top = mid + 1;
						}else if (record.record_time.hour > from->hour){
							bot = mid - 1;
						}else {
							if (record.record_time.min < from->min){
								top = mid + 1;
							}else if (record.record_time.min > from->min){
								bot = mid - 1;
							}else{
								if (record.record_time.sec < from->sec){
									top = mid + 1;
								}else if (record.record_time.sec > from->sec){
									bot = mid - 1;
								}else{
									find = 1;
									offset_f = mid;
								}
							}
						}
					}
				}
			}
		}else{
			goto error;
		}

	}
	while((find == 0) && (top <= bot));
	if (find != 1){
		ret = -ERR_NORECORD;
		goto error;
	}
	top = 0;
	bot = record_total-1;
	find = 0;
	do                     //ѭ�����ҷ��Ͻ���ʱ��ļ�¼
	{
		mid = (top + bot)/2;
		set.offset = mid;
		set.whence = DBS_SEEK_SET;
		ret = dbs_read(id, key_size, key, &set, sizeof(db_t)+max_size, &record_len, &record);
		if (ret == 0){
			if (record.record_time.year < to->year){
				top = mid + 1;
			}else if (record.record_time.year > to->year){
				bot = mid - 1;
			}else{
				if (record.record_time.mon < to->mon){
					top = mid + 1;
				}else if (record.record_time.mon > to->mon){
					bot = mid - 1;
				}else{
					if (record.record_time.day < to->day){
						top = mid + 1;
					}else if (record.record_time.day > to->day){
						bot = mid - 1;
					}else{
						if (record.record_time.hour < to->hour){
							top = mid + 1;
						}else if (record.record_time.hour > to->hour){
							bot = mid - 1;
						}else {
							if (record.record_time.min < to->min){
								top = mid + 1;
							}else if (record.record_time.min > to->min){
								bot = mid - 1;
							}else{
								if (record.record_time.sec < to->sec){
									top = mid + 1;
								}else if (record.record_time.sec > to->sec){
									bot = mid - 1;
								}else{
									find = 1;
									offset_t = mid;
								}
							}
						}
					}
				}
			}
		}else{
			goto error;
		}

	}
	while((find == 0) && (top <= bot));
	if (find != 1){
		ret = -ERR_NORECORD;
		goto error;
	}

	//������ȡ����
	dbs_set_t read_f;
	dbs_set_t read_t;
	read_f.whence = DBS_SEEK_SET;
	read_f.offset = 2;
	read_t.whence = DBS_SEEK_SET;
	read_t.offset = 5;
	ret = dbs_read_bulk(id, key_size, key, &read_f, &read_t, max_size, size, data);
	if(ret != 0){
		goto error;
	}
	ret = 0;
error:
	return(ret);
}

/******************************************************************************
    *	����:	cyclicdb_close
    *	����:	�ر�ѭ����¼���ݿ�
    *	����:	id				-	���ݿ���
    *	����:	0				-	�ɹ�
 				-ERR_INVA		-   �ӿڲ������ô���
 				-ERR_NOINIT		-	���ݿ�ģ��δ��ʼ��
 				-ERR_NOFILE		-	���ݿ�δ��
 				-ERR_BUSY		-	δ�ر�
				-ERR_SYS		-	ϵͳ����
    *	˵��:	��
******************************************************************************/
int  cyclicdb_close(u8 id)
{
	int ret;
	ret = dbs_close(id);
	if(ret != 0){
		goto error;
	}
	ret = 0;
error:
	return(ret);
}

/******************************************************************************
      *	����:	cyclicdb_remove
      *	����:	ɾ��ѭ����¼���ݿ�
      *	����:	id				-	���ݿ���
      *	����:	0				-	�ɹ�
   				-ERR_INVA		-   �ӿڲ������ô���
   				-ERR_NOINIT		-	���ݿ�ģ��δ��ʼ��
   				-ERR_NOFILE		-	���ݿ�δ��
   				-ERR_BUSY		-	δ�ر�
 				-ERR_SYS		-	ϵͳ����
      *	˵��:	��
 ******************************************************************************/
int  cyclicdb_remove(u8 id)
{
	int ret;
	ret = dbs_remove(id);
	if (ret != 0){
		goto error;
	}
	ret = 0;
error:
	return(ret);

}
