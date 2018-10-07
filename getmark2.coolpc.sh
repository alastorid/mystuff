#!/bin/bash
m3dphp="http://www.coolpc.com.tw/dx.php"
m3dlist="http://coolpc.com.tw/dx12.php"

PostData=`curl -sv $m3dlist|& grep -o "id=[0-9]\+ "|tr '\n' ','|sed 's/id=//g;s/ ,/,/g;s/^/N=/;s/,$//;s/$/\&S=1/;'`

curl -sv --data "$PostData" "$m3dphp" 2>/dev/null | iconv --from big5 --to utf8|sed 's/<[^>]*>/,/g; s/,\{1,\}/,/g; s/^,\|,$//g; s/&#176;//g;s/, ,\$\([0-9]\+\),◆,★\(.*\)$/\2, \$\1/' |column -t -s ','

#curl -sv http://coolpc.com.tw/3d.php|& iconv --from big5 --to utf8| sed -n 's/^[^>]\+>\([^<]\+\)<.*$/\1/p'
