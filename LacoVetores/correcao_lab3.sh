#!/bin/bash

TMP_FILE="tmp.hex"
IAS_OUT="ias.out"

# caminho para o executavel do simulador ias
IAS_EXEC_PATH="./ias"

STR_DIV="-------------------------------------------"

TEST[0]="tests/test0.hex"
TEST[1]="tests/test1.hex"
TEST[2]="tests/test2.hex"

if [ $# -lt 1 ]; then
    echo "ERRO!"
    echo "FORMA DE USO:"
    echo " ./correcao raXXXXXX.hex"

else

    for((I = 0; I < "${#TEST[@]}"; I++))
    do
        rm -f $TMP_FILE $IAS_OUT
        touch $TMP_FILE
        touch $IAS_OUT

        echo $STR_DIV
        echo "${TEST[$I]}"

        while read line           
        do             
            
            NEW_LINE=`echo "${line%%#*}" | tr -d ' '  `
            
            if [ "$NEW_LINE" != "" ]; then

                ADDRESS="${NEW_LINE:0:1}""${NEW_LINE:1:1}""${NEW_LINE:2:1}"

                ADDRESS=`echo "ibase=16; $ADDRESS" | bc`

                if [ "$ADDRESS" -le 64  ]; then
                    NEW_LINE="${NEW_LINE:0:1}${NEW_LINE:1:1}${NEW_LINE:2:1} ${NEW_LINE:3}"
                    echo $NEW_LINE  >> $TMP_FILE
                fi
            fi
        done < $1


        while read line           
        do             
            
            NEW_LINE=`echo "${line%%#*}" | tr -d ' '  `
            
            if [ "$NEW_LINE" != "" ]; then

                ADDRESS="${NEW_LINE:0:1}""${NEW_LINE:1:1}""${NEW_LINE:2:1}"

                if [ "$ADDRESS" = "400"  ]; then
                    RESULT="0x${NEW_LINE:3}"
                else
                    NEW_LINE="${NEW_LINE:0:1}${NEW_LINE:1:1}${NEW_LINE:2:1} ${NEW_LINE:3}"
                    echo $NEW_LINE  >> $TMP_FILE
                fi
            fi
        done < ${TEST[$I]}


        $IAS_EXEC_PATH --load=$TMP_FILE >& $IAS_OUT

        STR=`grep "+ AC: " $IAS_OUT | tail -1`


        STR_AC=`echo $STR | cut -d\  -f3`
        STR_AC=`echo $STR_AC | tr -d ' ' `

        STR_PC="${STR#*PC: }"
        
        STR_PC=`echo "ibase=16; ${STR_PC:2}" | bc`


        if [ "$STR_AC" == "$RESULT" ] \
            && [ "$STR_PC" -ge 1024 ]; then
            echo "CORRETO"        
        else
            echo "ERRO"
        fi

        echo $STR_DIV
    done
fi

rm -f $TMP_FILE $IAS_OUT
