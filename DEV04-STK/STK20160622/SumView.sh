#!/bin/sh

BASE_PATH=`pwd`
CTK_LIST_FILE=.README.list1
TODAY_DATA=.stk.data
ALL_STK_COLLECT=${BASE_PATH}/_stk.data_
ALL_TMP_COLLECT=${BASE_PATH}/_stk.data2_
ANALYSE_SCRIPT=${BASE_PATH}/SumView2.sh

STOP_2_T=1531
STOP_3_T=1535
HH_MM_NOW=`date +%H%M`
HH_MM_NOW=`date +%H%M`
SKIP_WAIT=0
echo "------------------------------$(date)--------------------------------"
SKIP_WAIT=1

CALC_CNT=2
for i in `seq 2 50`
do 
    STK_CODE=`cat ${CTK_LIST_FILE} | awk -F',' '{print $"'$i'"}'`

    if [ "${STK_CODE}" != "" ];then
        if [ ${HH_MM_NOW} -gt ${STOP_2_T} ];then
            TODAY_TRADE_SUM=""
            NOW_PRICE=""
            PER_PRICE=""
        else
            TODAY_TRADE_SUM=`cat ${TODAY_DATA} |grep "${STK_CODE}" | awk -F',' '{print $9}'`
            NOW_PRICE=`cat ${TODAY_DATA} |grep "${STK_CODE}" | awk -F',' '{print $4}'`
            PER_PRICE=`cat ${TODAY_DATA} |grep "${STK_CODE}" | awk -F',' '{print $3}'`
        fi
        TODAY_TRADE_SUM=`cat ${TODAY_DATA} |grep "${STK_CODE}" | awk -F',' '{print $9}'`
        NEED_ADD_STR=`cat .${STK_CODE}.SUM    | sed -e s/${STK_CODE}//g`   # Change by KEWELL  2016-03-21 17:27:00 
        NEED_ADD_STR=${NOW_PRICE}" "${PER_PRICE}" "${TODAY_TRADE_SUM}" "${NEED_ADD_STR}
        sh ${ANALYSE_SCRIPT} ${STK_CODE} ${CALC_CNT} ${NEED_ADD_STR}
    fi
done
    exit
