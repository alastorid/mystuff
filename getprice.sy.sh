#!/bin/bash
echo "---- $1 ----"
curl --data "sub_id=$1" https://www.sinya.com.tw/diy/show_option/ |& sed 's/<i>/###/g'|grep -o 'title=[^#]*###'|sed 's/<[^>]*>/ /g;s/^[^>]*>//;s/###//;s/[ ]\+/ /g;s/^ //'

#curl -vs https://www.sinya.com.tw/diy/|& sed '/OPTION/!d;s/[^>]*>//;/OPTION value/d;/OPTGROUP/d;'
