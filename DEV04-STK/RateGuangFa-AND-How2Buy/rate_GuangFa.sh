#!/bin/bash
SCALE=4
PERIOD_DEFAULT_MONEY=38000

for INDEX in `seq 0 8`;do
    if [ 0 -eq $INDEX ];then
        RATE=0.364;PERIOD_MONTH=12;PERIOD_MONTH_PLUS=0 # carbank.cn
        PERIOD_DEFAULT_MONEY=80000
    elif [ 1 -eq $INDEX ];then
        RATE=0.410;PERIOD_MONTH=06;PERIOD_MONTH_PLUS=0 # ZX 0.82*.75    Expect 0.38%For12Month
        PERIOD_DEFAULT_MONEY=146000
    elif [ 2 -eq $INDEX ];then
        RATE=0.380;PERIOD_MONTH=12;PERIOD_MONTH_PLUS=0 # ZX 0.82*.75    Expect 0.38%For12Month
        PERIOD_DEFAULT_MONEY=146000
    elif [ 3 -eq $INDEX ];then
        RATE=0.500;PERIOD_MONTH=06;PERIOD_MONTH_PLUS=0 # GF
        PERIOD_DEFAULT_MONEY=38000
    elif [ 4 -eq $INDEX ];then
        RATE=0.440;PERIOD_MONTH=12;PERIOD_MONTH_PLUS=0 # GF
        PERIOD_DEFAULT_MONEY=38000
    elif [ 5 -eq $INDEX ];then
        RATE=0.345;PERIOD_MONTH=09;PERIOD_MONTH_PLUS=0 # PF_DD
        PERIOD_DEFAULT_MONEY=70000
    elif [ 6 -eq $INDEX ];then
        PERIOD_DEFAULT_MONEY=35000
        RATE=0.331;PERIOD_MONTH=12;PERIOD_MONTH_PLUS=0 # ALi
    elif [ 7 -eq $INDEX ];then
        PERIOD_DEFAULT_MONEY=18000
        RATE=0.580;PERIOD_MONTH=12;PERIOD_MONTH_PLUS=0 # ALi-ZJ
    elif [ 8 -eq $INDEX ];then
        PERIOD_DEFAULT_MONEY=47000                     # Expect4.7
        RATE=0.250;PERIOD_MONTH=12;PERIOD_MONTH_PLUS=0 # ALi
    else
        exit
        break;
    fi

    PERIOD_RATE_MONEY=$(echo "scale=${SCALE};$PERIOD_DEFAULT_MONEY * $RATE * 0.01 * $PERIOD_MONTH"|bc) 
    for i in `seq 1 $PERIOD_MONTH`;do PERIOD_MONTH_PLUS=`expr $PERIOD_MONTH_PLUS \+ $i`;done
    PAY_ALL_PER_MONTH1=$(echo "scale=${SCALE};$PERIOD_DEFAULT_MONEY * 0.01 * $RATE"|bc) 
    PAY_ALL_PER_MONTH2=$(echo "scale=${SCALE};$PERIOD_DEFAULT_MONEY / $PERIOD_MONTH"|bc) 
    PAY_ALL_PER_MONTH=$(echo "scale=${SCALE};$PAY_ALL_PER_MONTH1 + $PAY_ALL_PER_MONTH2"|bc) 
    USAGE_PER_MONTH=$(echo "scale=${SCALE};$PERIOD_DEFAULT_MONEY / $PERIOD_MONTH * $PERIOD_MONTH_PLUS / $PERIOD_MONTH"|bc) 
    RATE_PER_MONTH_1W=$(echo "scale=${SCALE};$PERIOD_DEFAULT_MONEY * 0.01 * $RATE / $USAGE_PER_MONTH * 10000"|bc)
    RATE_PER_DAY_1W=$(echo "scale=${SCALE};$RATE_PER_MONTH_1W * 12 / 365"|bc)
    LOOP=`expr 12 \/ $PERIOD_MONTH`;LOOP=12
    G_MONEY_YEAR=$(echo "scale=${SCALE};$LOOP * $USAGE_PER_MONTH"|bc)
    G_RATE_YEAR=$(echo "scale=${SCALE};365 * $RATE_PER_DAY_1W * $PERIOD_DEFAULT_MONEY / 10000"|bc)
    RATEs_RATE=$(echo "scale=${SCALE};$G_RATE_YEAR * $LOOP / $G_MONEY_YEAR * 100"|bc)  # Should be wrong here
    RATEs_RATE2=$(echo "scale=${SCALE};$RATE_PER_DAY_1W * 365 / 100"|bc)
    #echo Mon=$PERIOD_MONTH Rate=$RATE ￥/Per=$PERIOD_DEFAULT_MONEY ￥/Per.M=$USAGE_PER_MONTH ￥/Y=$G_MONEY_YEAR R/Per=$PERIOD_RATE_MONEY R/Y=$G_RATE_YEAR  R/W.Mon=$RATE_PER_MONTH_1W R/W.Day=$RATE_PER_DAY_1W R/Y%=${RATEs_RATE}%
    echo Mon=$PERIOD_MONTH R=$RATE PAY/M=$PAY_ALL_PER_MONTH ￥/P.M=$USAGE_PER_MONTH ￥/Y=$G_MONEY_YEAR R/P=$PERIOD_RATE_MONEY R/Y=$G_RATE_YEAR  R/W.Mon=$RATE_PER_MONTH_1W R/W.Day=$RATE_PER_DAY_1W R/Y%=$RATEs_RATE2%
done
